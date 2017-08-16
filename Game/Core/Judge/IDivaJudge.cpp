#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "IDivaJudge.h"

namespace LYGame {
	IDivaJudge::IDivaJudge() {
		m_currSection = eST_Norm;
		DivaJudgeBus::Handler::BusConnect();
	}

	IDivaJudge::~IDivaJudge() {
		this->m_techZoneNotes.clear();
		DivaJudgeBus::Handler::BusDisconnect();
	}

	EHitScore IDivaJudge::GetHitScore(float time, EHitScore ret) {
		if (time > this->m_Range_CoolF) return eHS_Worst;
		else if (this->m_Range_Cool.IsInside(time)) return eHS_Cool;
		else if (this->m_Range_Fine.IsInside(time)) return eHS_Fine;
		else if (this->m_Range_Safe.IsInside(time)) return eHS_Safe;
		else if (this->m_Range_Sad.IsInside(time)) return eHS_Sad;
		else if (this->m_Range_Worst.IsInside(time)) return eHS_Worst;
		return ret;
	}

	EHitScore IDivaJudge::GetWorstRange(float time, EHitScore ret) {
		if (time >= this->m_Range_CoolF) return eHS_Worst;
		if (time <= this->m_Range_CoolF && time > this->m_Range_WorstF) return eHS_Worst;
		return ret;
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

	void IDivaJudge::SetTechZoneNotes(AZStd::vector<unsigned int> techZoneNotes) {
		for (unsigned int numNotes : techZoneNotes) {
			IDivaJudgeTechZone * notes = new IDivaJudgeTechZone();
			notes->currNotes = notes->totalNotes = numNotes;
			this->m_techZoneNotes.push_back(notes);
		}
	}
}