#ifndef _H_INPUTEBUSLISTENER
#define _H_INPUTEBUSLISTENER

#pragma once

#include "../Bus/OpenDiva_Bus.h"

namespace LYGame {

	class InputEBusListener : public IUIInputSystemListener {
	public:
		void OnUi_Up(int mode, float value) { OpenDivaBus::OpenDivaUiInputBus::Broadcast(&OpenDivaBus::OpenDivaUiInputEventGroup::OnUi_Up, mode, value); }
		void OnUi_Down(int mode, float value) { OpenDivaBus::OpenDivaUiInputBus::Broadcast(&OpenDivaBus::OpenDivaUiInputEventGroup::OnUi_Down, mode, value); }
		void OnUi_Left(int mode, float value) { OpenDivaBus::OpenDivaUiInputBus::Broadcast(&OpenDivaBus::OpenDivaUiInputEventGroup::OnUi_Left, mode, value); }
		void OnUi_Right(int mode, float value) { OpenDivaBus::OpenDivaUiInputBus::Broadcast(&OpenDivaBus::OpenDivaUiInputEventGroup::OnUi_Right, mode, value); }
		void OnUi_Select(int mode, float value) { OpenDivaBus::OpenDivaUiInputBus::Broadcast(&OpenDivaBus::OpenDivaUiInputEventGroup::OnUi_Select, mode, value); }
		void OnUi_Cancel(int mode, float value) { OpenDivaBus::OpenDivaUiInputBus::Broadcast(&OpenDivaBus::OpenDivaUiInputEventGroup::OnUi_Cancel, mode, value); }
	};
}

#endif