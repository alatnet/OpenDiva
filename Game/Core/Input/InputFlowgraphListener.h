#ifndef _H_INPUTFLOWGRAPHLISTENER
#define _H_INPUTFLOWGRAPHLISTENER

#pragma once

#include "../Bus/OpenDiva_Bus.h"

namespace LYGame {

	class InputFlowgraphListener : public IUIInputSystemListener {
	public:
		void OnUi_Up(int mode, float value) { EBUS_EVENT(OpenDivaBus::OpenDivaUiInputBus, OnUi_Up, mode, value); }
		void OnUi_Down(int mode, float value) { EBUS_EVENT(OpenDivaBus::OpenDivaUiInputBus, OnUi_Down, mode, value); }
		void OnUi_Left(int mode, float value) { EBUS_EVENT(OpenDivaBus::OpenDivaUiInputBus, OnUi_Left, mode, value); }
		void OnUi_Right(int mode, float value) { EBUS_EVENT(OpenDivaBus::OpenDivaUiInputBus, OnUi_Right, mode, value); }
		void OnUi_Select(int mode, float value) { EBUS_EVENT(OpenDivaBus::OpenDivaUiInputBus, OnUi_Select, mode, value); }
		void OnUi_Cancel(int mode, float value) { EBUS_EVENT(OpenDivaBus::OpenDivaUiInputBus, OnUi_Cancel, mode, value); }
	};
}

#endif