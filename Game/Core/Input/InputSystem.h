#ifndef _H_INPUTSYS_
#define _H_INPUTSYS_
#pragma once

#include <InputEventBus.h>

namespace LYGame {
	//------------------------------------------------------------------------------------------------
	template <class C> class LyInputEvent;

	enum LyInputEventType {
		eIET_Pressed = 0,
		eIET_Held,
		eIET_Released
	};
	//------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------
	//LyInputSystem
	//------------------------------------------------------------------------------------------------
	template<class C>
	class LyInputSystem {
		typedef void(C::* FunctionPtr)(LyInputEventType, float);
	public:
		LyInputSystem(C * instance) : m_instance(instance) {}
		~LyInputSystem();

	public:
		LyInputSystem<C>* AddInput(AZStd::string entityChannel, AZStd::string actionNameCrc, FunctionPtr func);
		LyInputSystem<C>* AddInput(const Input::ChannelId& entityChannel, Input::ProcessedEventName actionNameCrc, FunctionPtr func);

		LyInputSystem<C>* AddInput(AZStd::string actionNameCrc, FunctionPtr func);
		LyInputSystem<C>* AddInput(Input::ProcessedEventName actionNameCrc, FunctionPtr func);

	public:
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
		LyInputEvent(const Input::ChannelId& entityChannel, Input::ProcessedEventName actionNameCrc);
		LyInputEvent(Input::ProcessedEventName actionNameCrc);

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
	class CInputSystem {
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
		LyInputSystem<CInputSystem> *m_system;
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	//UI Input System
	//////////////////////////////////////////////////////////////////////////////////////////

	//enum EUIInputSystemEvent {
	//	eUIISE_Up,
	//	eUIISE_Down,
	//	eUIISE_Left,
	//	eUIISE_Right,
	//	eUIISE_Select,
	//	eUIISE_Cancel
	//};

	//struct IUIInputSystemListener {
	//	#define ON_FUNC(action) virtual void On##action##(int mode, float value) {};
	//		ON_FUNC(Ui_Up)
	//		ON_FUNC(Ui_Down)
	//		ON_FUNC(Ui_Left)
	//		ON_FUNC(Ui_Right)
	//		ON_FUNC(Ui_Select)
	//		ON_FUNC(Ui_Cancel)
	//	#undef ON_FUNC
	//	virtual void OnUIInputEvent(EUIInputSystemEvent event, int mode, float value) {};
	//};

	//class CUIInputSystem /*: public IActionListener*/ {
	//public:
	//	CUIInputSystem();
	//	~CUIInputSystem();
	//public:
	//	//void OnAction(const ActionId& action, int activationMode, float value);
	//public:
	//	void AddListener(IUIInputSystemListener *l) { listeners.push_back(l); }
	//	void RemoveListener(IUIInputSystemListener *l);
	//public:
	//	#define ACTION_FUNC(action) bool action##Action(ODInputEventType type, float value);
	//		ACTION_FUNC(Ui_Up)
	//		ACTION_FUNC(Ui_Down)
	//		ACTION_FUNC(Ui_Left)
	//		ACTION_FUNC(Ui_Right)
	//		ACTION_FUNC(Ui_Select)
	//		ACTION_FUNC(Ui_Cancel)
	//	#undef ACTION_FUNC
	//private:
	//	std::vector<IUIInputSystemListener*> listeners;
	//private:
	//	//static TActionHandler<CUIInputSystem> s_actionHandler;
	//};
}

#endif //_H_INPUTSYS_