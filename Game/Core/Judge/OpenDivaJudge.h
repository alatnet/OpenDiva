#ifndef _H_OPENDIVAJUDGE
#define _H_OPENDIVAJUDGE

#pragma once

#include "DivaJudgeFactory.h"

namespace LYGame {
	class OpenDivaJudge : public IDivaJudge {
	REGISTER_DIVA_JUDGE("OpenDiva", OpenDivaJudge)

	public:
		OpenDivaJudge(void * userdata);
	public:
		void DivaJudgeCallback(IDivaJudgeParams params);
	public:
		void addListener(IDivaJudgeListener * listener);
	public:
		unsigned int getScore() { return this->m_score; }
		unsigned int getHealth() { return this->m_health; }
		unsigned int getMaxHP() { return 200; }
		unsigned int getCombo() { return this->m_combo; }
		unsigned int getNumHits(EHitScore score) { return this->m_numHits[score]; }
		EDivaJudgeCompletion getCompletion();

		void SetTechZoneNotes(std::vector<unsigned int> techZoneNotes);
		unsigned int numTechZoneNotes();
		bool isTechZoneActive();
	private:
		unsigned int m_score, m_combo, m_maxCombo, m_ChanceMult, m_health;
		unsigned int m_numHits[eHS_None];
		unsigned int m_completion;
		unsigned int m_currTechZone;
	};
}

#endif