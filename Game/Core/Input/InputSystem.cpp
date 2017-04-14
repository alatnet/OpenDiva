#include <StdAfx.h>
#include "InputSystem.h"

namespace LYGame {
	//////////////////////////////////////////////////////////////////////////////////////////
	//Default Input System
	//////////////////////////////////////////////////////////////////////////////////////////
	TActionHandler<CInputSystem> CInputSystem::s_actionHandler;

	CInputSystem::CInputSystem() {
		IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();

		pActionMapManager->AddExtraActionListener(this);

		#define ADD_HANDLER(action,function) ActionId action##Input; action##Input = #action; CInputSystem::s_actionHandler.AddHandler(action##Input, function);
		if (CInputSystem::s_actionHandler.GetNumHandlers() == 0) {
			ADD_HANDLER(cross, &CInputSystem::CrossAction);
			ADD_HANDLER(circle, &CInputSystem::CircleAction);
			ADD_HANDLER(square, &CInputSystem::SquareAction);
			ADD_HANDLER(triangle, &CInputSystem::TriangleAction);

			ADD_HANDLER(left, &CInputSystem::LeftAction);
			ADD_HANDLER(right, &CInputSystem::RightAction);
			ADD_HANDLER(up, &CInputSystem::UpAction);
			ADD_HANDLER(down, &CInputSystem::DownAction);

			ADD_HANDLER(star, &CInputSystem::StarAction);

			ADD_HANDLER(swipeL, &CInputSystem::SwipeLAction);
			ADD_HANDLER(swipeR, &CInputSystem::SwipeRAction);

			ADD_HANDLER(start, &CInputSystem::StartAction);
		}
		#undef ADD_HANDLER
	}

	CInputSystem::~CInputSystem() {
		gEnv->pGame->GetIGameFramework()->GetIActionMapManager()->RemoveExtraActionListener(this);
		this->listeners.clear();
	}

	void CInputSystem::RemoveListener(IInputSystemListener *l) {
		auto it = std::find(this->listeners.begin(), this->listeners.end(), l);

		if (it != this->listeners.end()) {
			std::swap(*it, this->listeners.back());
			this->listeners.pop_back();
		}
	}

	void CInputSystem::OnAction(const ActionId& action, int activationMode, float value) {
		CInputSystem::s_actionHandler.Dispatch(this, 0, action, activationMode, value);
	}

	/*switch (activationMode) {
	case eAAM_OnPress:
	break;
	case eAAM_OnRelease:
	break;
	}*/

	#define ACTION_FUNC(action) \
		bool CInputSystem::##action##Action(EntityId entityId, const ActionId& actionId, int activationMode, float value){ \
			if (!this->listeners.empty()){ \
			for (IInputSystemListener * l : this->listeners) { \
					l->On##action##(activationMode, value); \
					l->OnInputEvent(eISE_##action##, activationMode, value); \
				} \
			} \
			return false; \
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
	TActionHandler<CUIInputSystem> CUIInputSystem::s_actionHandler;

	CUIInputSystem::CUIInputSystem() {
		IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();

		pActionMapManager->AddExtraActionListener(this);

		#define ADD_HANDLER(action,function) ActionId action##Input; action##Input = #action; CUIInputSystem::s_actionHandler.AddHandler(action##Input, function);
		if (CUIInputSystem::s_actionHandler.GetNumHandlers() == 0) {
			ADD_HANDLER(ui_up, &CUIInputSystem::Ui_UpAction);
			ADD_HANDLER(ui_down, &CUIInputSystem::Ui_DownAction);
			ADD_HANDLER(ui_left, &CUIInputSystem::Ui_LeftAction);
			ADD_HANDLER(ui_right, &CUIInputSystem::Ui_RightAction);
			ADD_HANDLER(ui_select, &CUIInputSystem::Ui_SelectAction);
			ADD_HANDLER(ui_cancel, &CUIInputSystem::Ui_CancelAction);
		}
		#undef ADD_HANDLER
	}

	CUIInputSystem::~CUIInputSystem() {
		gEnv->pGame->GetIGameFramework()->GetIActionMapManager()->RemoveExtraActionListener(this);
		this->listeners.clear();
	}

	void CUIInputSystem::RemoveListener(IUIInputSystemListener *l) {
		auto it = std::find(this->listeners.begin(), this->listeners.end(), l);

		if (it != this->listeners.end()) {
			std::swap(*it, this->listeners.back());
			this->listeners.pop_back();
		}
	}

	void CUIInputSystem::OnAction(const ActionId& action, int activationMode, float value) {
		CUIInputSystem::s_actionHandler.Dispatch(this, 0, action, activationMode, value);
	}

	#define ACTION_FUNC(action) \
		bool CUIInputSystem::Ui_##action##Action(EntityId entityId, const ActionId& actionId, int activationMode, float value){ \
			if (!this->listeners.empty()) { \
				for (IUIInputSystemListener * l : this->listeners) { \
					l->OnUi_##action##(activationMode, value); \
					l->OnUIInputEvent(eUIISE_##action##, activationMode, value); \
				} \
			} \
			return false; \
		}

		ACTION_FUNC(Up)
		ACTION_FUNC(Down)
		ACTION_FUNC(Left)
		ACTION_FUNC(Right)
		ACTION_FUNC(Select)
		ACTION_FUNC(Cancel)
	#undef ACTION_FUNC
}