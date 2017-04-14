#include <StdAfx.h>

#include "Flownode_Judge_Listener.h"

namespace LYGame {
	void FlownodeJudgeListener::OnJudgeEvent(IDivaJudgeListenerParams params) {
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaHPBus, SetHP, params.health);
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaScoreBus, SetScore, params.score);
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaCompletionBus, SetRating, params.completion);
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaCompletionBus, SetNotes, params.numNotesComplete);
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaHitScoreBus, SetHitScore, params.hitScore, params.wrong);
	}

	/*void FlownodeJudgeListener::OnMaxHP(unsigned int maxHP) {
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaHPBus, SetMaxHP, maxHP);
	}*/

	void FlownodeJudgeListener::OnTechZoneScore(unsigned int numNotes, bool active) {
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaTechnicalZoneInfoBus, SetNumNotes, numNotes);
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaTechnicalZoneInfoBus, SetActive, active);
	}
}