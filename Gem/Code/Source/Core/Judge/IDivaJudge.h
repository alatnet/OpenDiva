#ifndef _H_IDIVAJUDGE
#define _H_IDIVAJUDGE

#pragma once

//#include "../CryMovie/Node/IDivaSequence.h"
#include "../Files/NoteFileEntries.h"
#include "../Bus/DivaJudgeBus.h"
#include "../Bus/DivaSequenceBus.h"
#include "../Bus/DivaHudBus.h"

/*
enum EHitScore {
eHS_Cool,
eHS_Fine,
eHS_Safe,
eHS_Sad,
eHS_Worst,
eHS_None
};
*/

/*
#define RANGE_WORST 0.80f
#define RANGE_SAD 0.85f
#define RANGE_SAFE 0.90f
#define RANGE_FINE 0.95f
#define RANGE_COOL 1.0f
*/

//TO ADD:
//- Big Star Fill Gauge

namespace OpenDiva {

	struct IDivaJudgeTechZone {
		unsigned int totalNotes;
		unsigned int currNotes;
		bool active;

		IDivaJudgeTechZone() : totalNotes(0), currNotes(0), active(true) {};
	};

	class IDivaJudge : public DivaJudgeBus::Handler {
	public:
		IDivaJudge();
		~IDivaJudge();
	public:
		// DivaJudgeBus
		virtual unsigned int GetScore() = 0;
		virtual unsigned int GetHealth() = 0;
		virtual unsigned int GetMaxHP() = 0;
		virtual unsigned int GetCombo() = 0;
		virtual EDivaJudgeCompletion GetCompletion() = 0;
		virtual unsigned int GetNumHits(EHitScore score) = 0;

		virtual unsigned int TechZoneNotesCount() = 0;
		virtual bool IsTechZoneActive() = 0;

		void SetTotalNotes(unsigned int totalNotes) { this->m_totalNotes = totalNotes; }
		void SetScoreRanges(float cool, float fine, float safe, float sad, float worst);
		virtual void SetTechZoneNotes(AZStd::vector<unsigned int> techZoneNotes);

		virtual void OnJudge(IDivaJudgeParams params) = 0;
		virtual EHitScore GetHitScore(float time, EHitScore ret);
		virtual EHitScore GetWorstRange(float time, EHitScore ret);
		// ~DivaJudgeBus
	public:
		Range m_Range_Cool, m_Range_Fine, m_Range_Safe, m_Range_Sad, m_Range_Worst;
		float m_Range_CoolF, m_Range_WorstF;
	protected:
		unsigned int m_totalNotes;
		AZStd::vector<IDivaJudgeTechZone *> m_techZoneNotes;
		SectionType m_currSection;
	};
}

#endif