#ifndef _H_INPUTSYS_
#define _H_INPUTSYS_
#pragma once

#include <AzFramework/Input/Events/InputChannelEventListener.h>
#include <AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzFramework/Input/Devices/Gamepad/InputDeviceGamepad.h>
#include <AzFramework/Input/Devices/Touch/InputDeviceTouch.h>
//#include <InputEventBus.h>

namespace OpenDiva {
	//------------------------------------------------------------------------------------------------
	//template <class C> class LyInputEvent;

	enum LyInputEventType {
		eIET_Pressed = 0,
		eIET_Held,
		eIET_Released
	};
	//------------------------------------------------------------------------------------------------

	/*
	//------------------------------------------------------------------------------------------------
	//LyInputSystem
	//------------------------------------------------------------------------------------------------
	template<class C>
	class LyInputSystem {
	public:
		friend class LyInputEvent<C>;
	public:
		typedef void(C::* FunctionPtr)(LyInputEventType, float);
	public:
		LyInputSystem(C * instance) : m_instance(instance) {}
		~LyInputSystem();

	public:
		//LyInputSystem<C>* AddInput(AZStd::string entityChannel, AZStd::string actionNameCrc, FunctionPtr func);
		//LyInputSystem<C>* AddInput(const AZ::Crc32& entityChannel, Input::ProcessedEventName actionNameCrc, FunctionPtr func);
		//LyInputSystem<C>* AddInput(AZStd::string actionNameCrc, FunctionPtr func);
		//LyInputSystem<C>* AddInput(Input::ProcessedEventName actionNameCrc, FunctionPtr func);

		LyInputSystem<C>* AddInput(Input::ProfileId& profileId, Input::ProcessedEventName actionNameCrc, FunctionPtr func);
		LyInputSystem<C>* AddInput(AZ::Crc32&  entityChannel, AZStd::string actionName, FunctionPtr func);
		LyInputSystem<C>* AddInput(Input::ProcessedEventName& actionName, FunctionPtr func);
		LyInputSystem<C>* AddInput(AZStd::string actionName, FunctionPtr func);
		

	private:
		void OnInputEvent(LyInputEventType type, AZ::InputEventNotificationId actionId, float value);

	private:
		AZStd::unordered_map<AZ::InputEventNotificationId, AZStd::pair<LyInputEvent<C> *, FunctionPtr>> m_eventFuncs;
		C * m_instance;
	};

	//------------------------------------------------------------------------------------------------
	//LyInputEvent
	//------------------------------------------------------------------------------------------------
	template <class C>
	class LyInputEvent : public AZ::InputEventNotificationBus::Handler {
	public:
	public:
		//LyInputEvent(const AZ::Crc32& entityChannel, Input::ProcessedEventName actionNameCrc);
		//LyInputEvent(Input::ProcessedEventName actionNameCrc);

		LyInputEvent(Input::ProfileId& profileId, Input::ProcessedEventName actionNameCrc);
		LyInputEvent(AZ::Crc32&  entityChannel, AZStd::string actionName);
		LyInputEvent(Input::ProcessedEventName& actionName);
		LyInputEvent(AZStd::string actionName);

		~LyInputEvent();
	public:
		void SetInputSystem(LyInputSystem<C> * system);
		AZ::InputEventNotificationId GetID();
	public:
		void OnPressed(float value);
		void OnHeld(float value);
		void OnReleased(float value);
	private:
		AZ::InputEventNotificationId m_id;
		LyInputSystem<C> * m_sys;
	};
	//------------------------------------------------------------------------------------------------
	*/

	enum EInputSystemEvent {
		eISE_Cross,
		eISE_Circle,
		eISE_Square,
		eISE_Triangle,
		eISE_Left,
		eISE_Right,
		eISE_Up,
		eISE_Down,
		eISE_Star,
		eISE_SwipeL,
		eISE_SwipeR,
		eISE_Start
	};

	struct IInputSystemListener {
		#define ON_FUNC(action) virtual void On##action##(LyInputEventType mode, float value) {};
			ON_FUNC(Cross)
			ON_FUNC(Circle)
			ON_FUNC(Square)
			ON_FUNC(Triangle)

			ON_FUNC(Left)
			ON_FUNC(Right)
			ON_FUNC(Up)
			ON_FUNC(Down)

			ON_FUNC(Star)

			ON_FUNC(SwipeL)
			ON_FUNC(SwipeR)

			ON_FUNC(Start)
		#undef ON_FUNC
		virtual void OnInputEvent(EInputSystemEvent event, int mode, float value) {};
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	//Default Input System
	//////////////////////////////////////////////////////////////////////////////////////////
	class CInputSystem
		: public AzFramework::InputChannelEventListener
	{
	public:
		CInputSystem();
		~CInputSystem();
	public:
	public:
		void AddListener(IInputSystemListener *l) { listeners.push_back(l); }
		void RemoveListener(IInputSystemListener *l);
	public:
		#define ACTION_FUNC(action) void action##Action(LyInputEventType type, float value);
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
	private:
		AZStd::vector<IInputSystemListener*> listeners;
	private:
		//LyInputSystem<CInputSystem> *m_system;

		// Inherited via InputChannelEventListener
		bool OnInputChannelEventFiltered(const AzFramework::InputChannel & inputChannel) override;
	};
}

#endif //_H_INPUTSYS_