#ifndef _H_INPUTSYS_
#define _H_INPUTSYS_
#pragma once

#include <IActionMapManager.h>
#include <IGame.h>
#include <IGameFramework.h>

namespace LYGame {
	//////////////////////////////////////////////////////////////////////////////////////////
	//Default Input System
	//////////////////////////////////////////////////////////////////////////////////////////
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
		#define ON_FUNC(action) virtual void On##action##(int mode, float value) {};
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

	class CInputSystem : public IActionListener {
	public:
		CInputSystem();
		~CInputSystem();
	public:
		void OnAction(const ActionId& action, int activationMode, float value);
	public:
		void AddListener(IInputSystemListener *l) { listeners.push_back(l); }
		void RemoveListener(IInputSystemListener *l);
	public:
		#define ACTION_FUNC(action) bool action##Action(EntityId entityId, const ActionId& actionId, int activationMode, float value);
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
		std::vector<IInputSystemListener*> listeners;
	private:
		static TActionHandler<CInputSystem> s_actionHandler;
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	//UI Input System
	//////////////////////////////////////////////////////////////////////////////////////////

	enum EUIInputSystemEvent {
		eUIISE_Up,
		eUIISE_Down,
		eUIISE_Left,
		eUIISE_Right,
		eUIISE_Select,
		eUIISE_Cancel
	};

	struct IUIInputSystemListener {
		#define ON_FUNC(action) virtual void On##action##(int mode, float value) {};
			ON_FUNC(Ui_Up)
			ON_FUNC(Ui_Down)
			ON_FUNC(Ui_Left)
			ON_FUNC(Ui_Right)
			ON_FUNC(Ui_Select)
			ON_FUNC(Ui_Cancel)
		#undef ON_FUNC
		virtual void OnUIInputEvent(EUIInputSystemEvent event, int mode, float value) {};
	};

	class CUIInputSystem : public IActionListener {
	public:
		CUIInputSystem();
		~CUIInputSystem();
	public:
		void OnAction(const ActionId& action, int activationMode, float value);
	public:
		void AddListener(IUIInputSystemListener *l) { listeners.push_back(l); }
		void RemoveListener(IUIInputSystemListener *l);
	public:
		#define ACTION_FUNC(action) bool action##Action(EntityId entityId, const ActionId& actionId, int activationMode, float value);
			ACTION_FUNC(Ui_Up)
			ACTION_FUNC(Ui_Down)
			ACTION_FUNC(Ui_Left)
			ACTION_FUNC(Ui_Right)
			ACTION_FUNC(Ui_Select)
			ACTION_FUNC(Ui_Cancel)
		#undef ACTION_FUNC
	private:
		std::vector<IUIInputSystemListener*> listeners;
	private:
		static TActionHandler<CUIInputSystem> s_actionHandler;
	};
}

#endif //_H_INPUTSYS_