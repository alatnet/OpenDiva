#ifndef _H_OPENDIVAJUDGE
#define _H_OPENDIVAJUDGE

#pragma once

#include "DivaJudgeFactory.h"

namespace OpenDiva {
	class OpenDivaJudge : public IDivaJudge {
	REGISTER_DIVA_JUDGE("OpenDiva", OpenDivaJudge)

	public:
		OpenDivaJudge(void * userdata);
	public:
		unsigned int GetScore() { return this->m_score; }
		unsigned int GetHealth() { return this->m_health; }
		unsigned int GetMaxHP() { return 200; }
		unsigned int GetCombo() { return this->m_combo; }
		unsigned int GetNumHits(EHitScore score) { return this->m_numHits[score]; }
		EDivaJudgeCompletion GetCompletion();

		void SetTechZoneNotes(AZStd::vector<unsigned int> techZoneNotes);
		unsigned int TechZoneNotesCount();
		bool IsTechZoneActive();

		void OnJudge(IDivaJudgeParams params);
	private:
		unsigned int m_score, m_combo, m_maxCombo, m_ChanceMult, m_health;
		unsigned int m_numHits[eHS_None];
		unsigned int m_completion;
		unsigned int m_currTechZone;
	};
}

#endif