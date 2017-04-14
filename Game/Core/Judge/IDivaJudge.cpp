#include <StdAfx.h>
#include "IDivaJudge.h"

namespace LYGame {

	#define BusConnectC() if(!this->m_busConnected) { BusConnect(); this->m_busConnected = true; }
	#define BusDisconnectC() if(this->m_busConnected) { BusDisconnect(); this->m_busConnected = false; }

	IDivaJudge::IDivaJudge() {
		m_currSection = eST_Norm;
		BusConnectC();
	}

	IDivaJudge::~IDivaJudge() {
		this->m_techZoneNotes.clear();
		BusDisconnectC();
	}

	void IDivaJudge::SetScoreRanges(float cool, float fine, float safe, float sad, float worst) {
		m_Range_Cool.Set(fine, cool);
		m_Range_Fine.Set(safe, fine);
		m_Range_Safe.Set(sad, safe);
		m_Range_Sad.Set(worst, sad);
		m_Range_Worst.Set(worst - 0.05f, worst);
		m_Range_CoolF = cool;
		m_Range_WorstF = worst;
	}

	void IDivaJudge::SetTechZoneNotes(std::vector<unsigned int> techZoneNotes) {
		for (unsigned int numNotes : techZoneNotes) {
			IDivaJudgeTechZone * notes = new IDivaJudgeTechZone();
			notes->currNotes = notes->totalNotes = numNotes;
			this->m_techZoneNotes.push_back(notes);
		}
	}

	#undef BusConnectC
	#undef BusDisconnectC
}