#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "InputSystem.h"

namespace OpenDiva {
	/*
	//------------------------------------------------------------------------------------------------
	//LyInputSystem
	//------------------------------------------------------------------------------------------------
	template <class C>
	LyInputSystem<C>::~LyInputSystem() {
		for (AZStd::pair<AZ::InputEventNotificationId, AZStd::pair<LyInputEvent<C> *, FunctionPtr>> e : this->m_eventFuncs)
			delete e.second.first;
		this->m_eventFuncs.clear();
	}

	template <class C>
	LyInputSystem<C>* LyInputSystem<C>::AddInput(Input::ProfileId& profileId, Input::ProcessedEventName actionNameCrc, FunctionPtr func) {
		LyInputEvent<C> * ievent = new LyInputEvent<C>(profileId, actionNameCrc);
		ievent->SetInputSystem(this);
		this->m_eventFuncs.insert(AZStd::make_pair(ievent->GetID(), AZStd::make_pair(ievent, func)));
		return this;
	}
	template <class C>
	LyInputSystem<C>* LyInputSystem<C>::AddInput(AZ::Crc32& entityChannel, AZStd::string actionName, FunctionPtr func) {
		LyInputEvent<C> * ievent = new LyInputEvent<C>(entityChannel, actionName);
		ievent->SetInputSystem(this);
		this->m_eventFuncs.insert(AZStd::make_pair(ievent->GetID(), AZStd::make_pair(ievent, func)));
		return this;
	}
	template <class C>
	LyInputSystem<C>* LyInputSystem<C>::AddInput(Input::ProcessedEventName& actionName, FunctionPtr func) {
		LyInputEvent<C> * ievent = new LyInputEvent<C>(actionName);
		ievent->SetInputSystem(this);
		this->m_eventFuncs.insert(AZStd::make_pair(ievent->GetID(), AZStd::make_pair(ievent, func)));
		return this;
	}
	template <class C>
	LyInputSystem<C>* LyInputSystem<C>::AddInput(AZStd::string actionName, FunctionPtr func) {
		LyInputEvent<C> * ievent = new LyInputEvent<C>(actionName);
		ievent->SetInputSystem(this);
		this->m_eventFuncs.insert(AZStd::make_pair(ievent->GetID(), AZStd::make_pair(ievent, func)));
		return this;
	}

	template <class C>
	void LyInputSystem<C>::OnInputEvent(LyInputEventType type, AZ::InputEventNotificationId actionId, float value) {
		//(reinterpret_cast<C*>(this->m_instance)->*this->m_eventFuncs[actionId].second)(type, value);
		(this->m_instance->*this->m_eventFuncs[actionId].second)(type, value);
	}

	//------------------------------------------------------------------------------------------------
	//LyInputEvent
	//------------------------------------------------------------------------------------------------
	template <class C>
	LyInputEvent<C>::LyInputEvent(Input::ProfileId& profileId, Input::ProcessedEventName actionNameCrc) {
		this->m_id = AZ::InputEventNotificationId(profileId, actionNameCrc);
		AZ::InputEventNotificationBus::Handler::BusConnect(this->m_id);
	}
	template <class C>
	LyInputEvent<C>::LyInputEvent(AZ::Crc32& entityChannel, AZStd::string actionName) {
		this->m_id = AZ::InputEventNotificationId(entityChannel, actionName.c_str());
		AZ::InputEventNotificationBus::Handler::BusConnect(this->m_id);
	}
	template <class C>
	LyInputEvent<C>::LyInputEvent(Input::ProcessedEventName& actionName) {
		this->m_id = AZ::InputEventNotificationId(actionName);
		AZ::InputEventNotificationBus::Handler::BusConnect(this->m_id);
	}
	template <class C>
	LyInputEvent<C>::LyInputEvent(AZStd::string actionName) {
		this->m_id = AZ::InputEventNotificationId(actionName.c_str());
		AZ::InputEventNotificationBus::Handler::BusConnect(this->m_id);
	}

	template <class C>
	LyInputEvent<C>::~LyInputEvent() { AZ::InputEventNotificationBus::Handler::BusDisconnect(); }

	template <class C>
	void LyInputEvent<C>::SetInputSystem(LyInputSystem<C> * system) { this->m_sys = system; }
	template <class C>
	AZ::InputEventNotificationId LyInputEvent<C>::GetID() { return this->m_id; }

	template <class C>
	void LyInputEvent<C>::OnPressed(float value) { this->m_sys->OnInputEvent(eIET_Pressed, this->m_id, value); }
	template <class C>
	void LyInputEvent<C>::OnHeld(float value) { this->m_sys->OnInputEvent(eIET_Held, this->m_id, value); }
	template <class C>
	void LyInputEvent<C>::OnReleased(float value) { this->m_sys->OnInputEvent(eIET_Released, this->m_id, value); }
	//------------------------------------------------------------------------------------------------
	*/

	//////////////////////////////////////////////////////////////////////////////////////////
	//Default Input System
	//////////////////////////////////////////////////////////////////////////////////////////
	CInputSystem::CInputSystem() {
		/*this->m_system = new LyInputSystem<CInputSystem>(this);
		this->m_system
			->AddInput("cross", &CInputSystem::CrossAction)
			->AddInput("circle", &CInputSystem::CircleAction)
			->AddInput("square", &CInputSystem::SquareAction)
			->AddInput("triangle", &CInputSystem::TriangleAction)
			->AddInput("left", &CInputSystem::LeftAction)
			->AddInput("right", &CInputSystem::RightAction)
			->AddInput("down", &CInputSystem::DownAction)
			->AddInput("up", &CInputSystem::UpAction)
			->AddInput("swipeL", &CInputSystem::SwipeLAction)
			->AddInput("swipeR", &CInputSystem::SwipeRAction)
			->AddInput("star", &CInputSystem::StarAction)
			->AddInput("start", &CInputSystem::StartAction);*/

		InputChannelEventListener::Connect();
	}

	CInputSystem::~CInputSystem() {
		InputChannelEventListener::Disconnect();

		//delete this->m_system;
		this->listeners.clear();
	}

	void CInputSystem::RemoveListener(IInputSystemListener *l) {
		auto it = std::find(this->listeners.begin(), this->listeners.end(), l);
		if (it != this->listeners.end()) this->listeners.erase(it);
	}

	bool CInputSystem::OnInputChannelEventFiltered(const AzFramework::InputChannel & inputChannel) {
		auto device_id = inputChannel.GetInputDevice().GetInputDeviceId();
		float value = inputChannel.GetValue();
		auto input_type = inputChannel.GetInputChannelId();

		LyInputEventType eventType;
		switch (inputChannel.GetState()) {
		case AzFramework::InputChannel::State::Began:
			eventType = LyInputEventType::eIET_Pressed;
			break;
		case AzFramework::InputChannel::State::Updated:
			eventType = LyInputEventType::eIET_Held;
			break;
		case AzFramework::InputChannel::State::Ended:
			eventType = LyInputEventType::eIET_Released;
			break;
		}

		if (device_id == AzFramework::InputDeviceKeyboard::Id) { //keyboard input
			if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericW) this->TriangleAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericS) this->CrossAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericA) this->SquareAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericD) this->CircleAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericI) this->UpAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericK) this->DownAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericJ) this->LeftAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericL) this->RightAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::NavigationArrowUp) this->UpAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::NavigationArrowDown) this->DownAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::NavigationArrowLeft) this->LeftAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::NavigationArrowRight) this->RightAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericQ) this->SwipeLAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericE) this->SwipeRAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericU) this->SwipeLAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::AlphanumericO) this->SwipeRAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::EditSpace) this->StarAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceKeyboard::Key::Escape) this->StartAction(eventType, value);
		} else if (device_id == AzFramework::InputDeviceGamepad::IdForIndex0) { //first gamepad input
			if (input_type == AzFramework::InputDeviceGamepad::Button::A) this->CrossAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::B) this->CircleAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::X) this->SquareAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::Y) this->TriangleAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::DU) this->UpAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::DD) this->DownAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::DL) this->LeftAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::DR) this->RightAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::L1) this->SwipeLAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Trigger::L2) this->SwipeLAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::R1) this->SwipeRAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Trigger::R2) this->SwipeRAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::Button::Start) this->StartAction(eventType, value);
			else if (input_type == AzFramework::InputDeviceGamepad::ThumbStickAxis2D::L) this->StarAction(eventType, 1.0f);
			else if (input_type == AzFramework::InputDeviceGamepad::ThumbStickAxis2D::R) this->StarAction(eventType, 1.0f);
		}

		return false;
	}

	#define ACTION_FUNC(action) \
		void CInputSystem::##action##Action(LyInputEventType type, float value){ \
			if (!this->listeners.empty()){ \
			for (IInputSystemListener * l : this->listeners) { \
					l->On##action##(type, value); \
					l->OnInputEvent(eISE_##action##, type, value); \
				} \
			} \
		}

		ACTION_FUNC(Cross)
		ACTION_FUNC(Circle)
		ACTION_FUNC(Square)
		ACTION_FUNC(Triangle)

		ACTION_FUNC(Left)
		ACTION_FUNC(Right)
		ACTION_FUNC(Up)
		ACTION_FUNC(Down)

		ACTION_FUNC(Star)

		ACTION_FUNC(SwipeL)
		ACTION_FUNC(SwipeR)

		ACTION_FUNC(Start)
	#undef ACTION_FUNC
}