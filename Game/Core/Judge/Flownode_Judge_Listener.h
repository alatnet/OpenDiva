#ifndef _H_FLOWNODE_JUDGE_LISTENER
#define _H_FLOWNODE_JUDGE_LISTENER

#pragma once

#include "IDivaJudge.h"
#include "../Flowgraph/Bus/Flownodes_OpenDiva_Bus.h"

namespace LYGame {

	class FlownodeJudgeListener : public IDivaJudgeListener {
	public:
		void OnJudgeEvent(IDivaJudgeListenerParams params);
		//void OnMaxHP(unsigned int maxHP);
		void OnTechZoneScore(unsigned int numNotes, bool active);
	};

}

#endif