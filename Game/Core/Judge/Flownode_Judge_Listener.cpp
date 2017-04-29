#include <StdAfx.h>

#include "Flownode_Judge_Listener.h"

namespace LYGame {
	void FlownodeJudgeListener::OnJudgeEvent(IDivaJudgeListenerParams params) {
		EBUS_EVENT(OpenDivaBus::OpenDivaHPBus, SetHP, params.health);
		EBUS_EVENT(OpenDivaBus::OpenDivaScoreBus, SetScore, params.score);
		EBUS_EVENT(OpenDivaBus::OpenDivaCompletionBus, SetRating, params.completion);
		EBUS_EVENT(OpenDivaBus::OpenDivaCompletionBus, SetNotes, params.numNotesComplete);
		EBUS_EVENT(OpenDivaBus::OpenDivaHitScoreBus, SetHitScore, params.hitScore, params.wrong);
	}

	/*void FlownodeJudgeListener::OnMaxHP(unsigned int maxHP) {
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaHPBus, SetMaxHP, maxHP);
	}*/

	void FlownodeJudgeListener::OnTechZoneScore(unsigned int numNotes, bool active) {
		EBUS_EVENT(OpenDivaBus::OpenDivaTechnicalZoneInfoBus, SetNumNotes, numNotes);
		EBUS_EVENT(OpenDivaBus::OpenDivaTechnicalZoneInfoBus, SetActive, active);
	}
}