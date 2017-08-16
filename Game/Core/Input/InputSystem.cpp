#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "InputSystem.h"

namespace LYGame {
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
	LyInputSystem<C>* LyInputSystem<C>::AddInput(AZStd::string entityChannel, AZStd::string actionNameCrc, FunctionPtr func) { return this->AddInput(Input::ChannelId(entityChannel.c_str()), Input::ProcessedEventName(actionNameCrc.c_str()), func); }

	template <class C>
	LyInputSystem<C>* LyInputSystem<C>::AddInput(const Input::ChannelId& entityChannel, Input::ProcessedEventName actionNameCrc, FunctionPtr func) {
		LyInputEvent<C> * ievent = new LyInputEvent<C>(entityChannel, actionNameCrc);
		ievent->SetInputSystem(this);
		this->m_eventFuncs.insert(AZStd::make_pair(ievent->GetID(), AZStd::make_pair(ievent, func)));
		return this;
	}
	template <class C>
	LyInputSystem<C>* LyInputSystem<C>::AddInput(AZStd::string actionNameCrc, FunctionPtr func) { return this->AddInput(Input::ProcessedEventName(actionNameCrc.c_str()), func); }
	template <class C>
	LyInputSystem<C>* LyInputSystem<C>::AddInput(Input::ProcessedEventName actionNameCrc, FunctionPtr func) {
		LyInputEvent<C> * ievent = new LyInputEvent<C>(actionNameCrc);
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
	LyInputEvent<C>::LyInputEvent(const Input::ChannelId& entityChannel, Input::ProcessedEventName actionNameCrc) {
		this->m_id = AZ::InputEventNotificationId(entityChannel, actionNameCrc);
		AZ::InputEventNotificationBus::Handler::BusConnect(this->m_id);
	}
	template <class C>
	LyInputEvent<C>::LyInputEvent(Input::ProcessedEventName actionNameCrc) {
		this->m_id = AZ::InputEventNotificationId(actionNameCrc);
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

	//////////////////////////////////////////////////////////////////////////////////////////
	//Default Input System
	//////////////////////////////////////////////////////////////////////////////////////////
	CInputSystem::CInputSystem() {
		this->m_system = new LyInputSystem<CInputSystem>(this);
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
			->AddInput("start", &CInputSystem::StartAction);
	}

	CInputSystem::~CInputSystem() {
		delete this->m_system;
		this->listeners.clear();
	}

	void CInputSystem::RemoveListener(IInputSystemListener *l) {
		auto it = std::find(this->listeners.begin(), this->listeners.end(), l);
		if (it != this->listeners.end()) this->listeners.erase(it);
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

	//////////////////////////////////////////////////////////////////////////////////////////
	//UI Input System
	//////////////////////////////////////////////////////////////////////////////////////////
	//TActionHandler<CUIInputSystem> CUIInputSystem::s_actionHandler;

	//CUIInputSystem::CUIInputSystem() {
	//	/*IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();

	//	pActionMapManager->AddExtraActionListener(this);

	//	#define ADD_HANDLER(action,function) ActionId action##Input; action##Input = #action; CUIInputSystem::s_actionHandler.AddHandler(action##Input, function);
	//	if (CUIInputSystem::s_actionHandler.GetNumHandlers() == 0) {
	//		ADD_HANDLER(ui_up, &CUIInputSystem::Ui_UpAction);
	//		ADD_HANDLER(ui_down, &CUIInputSystem::Ui_DownAction);
	//		ADD_HANDLER(ui_left, &CUIInputSystem::Ui_LeftAction);
	//		ADD_HANDLER(ui_right, &CUIInputSystem::Ui_RightAction);
	//		ADD_HANDLER(ui_select, &CUIInputSystem::Ui_SelectAction);
	//		ADD_HANDLER(ui_cancel, &CUIInputSystem::Ui_CancelAction);
	//	}
	//	#undef ADD_HANDLER*/
	//}

	//CUIInputSystem::~CUIInputSystem() {
	//	//gEnv->pGame->GetIGameFramework()->GetIActionMapManager()->RemoveExtraActionListener(this);
	//	this->listeners.clear();
	//}

	//void CUIInputSystem::RemoveListener(IUIInputSystemListener *l) {
	//	auto it = std::find(this->listeners.begin(), this->listeners.end(), l);

	//	if (it != this->listeners.end()) {
	//		std::swap(*it, this->listeners.back());
	//		this->listeners.pop_back();
	//	}
	//}

	///*void CUIInputSystem::OnAction(const ActionId& action, int activationMode, float value) {
	//	CUIInputSystem::s_actionHandler.Dispatch(this, 0, action, activationMode, value);
	//}*/

	//#define ACTION_FUNC(action) \
	//	bool CUIInputSystem::Ui_##action##Action(ODInputEventType type, float value){ \
	//		if (!this->listeners.empty()) { \
	//			for (IUIInputSystemListener * l : this->listeners) { \
	//				l->OnUi_##action##(type, value); \
	//				l->OnUIInputEvent(eUIISE_##action##, type, value); \
	//			} \
	//		} \
	//		return false; \
	//	}

	//	ACTION_FUNC(Up)
	//	ACTION_FUNC(Down)
	//	ACTION_FUNC(Left)
	//	ACTION_FUNC(Right)
	//	ACTION_FUNC(Select)
	//	ACTION_FUNC(Cancel)
	//#undef ACTION_FUNC
}