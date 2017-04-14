#ifndef _H_IDIVAJUDGE
#define _H_IDIVAJUDGE

#pragma once

#include "../CryMovie/Node/IDivaSequence.h"
#include "../Files/NoteFileEntries.h"
#include "../Flowgraph/Bus/Flownodes_OpenDiva_Bus.h"

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

namespace LYGame {
	struct IDivaJudgeParams {
		ENoteType nType;
		ENoteHitType hType;
		EHitScore hitscore;
		SectionType sType;
		bool hold;
		bool holdRelease;
		bool wrong;
		Vec2 notePos;
		unsigned int holdMult;

		IDivaJudgeParams() :
			nType(eNT_Count),
			hType(eNHT_INVALID),
			hitscore(eHS_None),
			sType(eST_Norm),
			hold(false),
			holdRelease(false),
			wrong(false),
			notePos(Vec2(0.0f, 0.0f)),
			holdMult(0)
		{};
	};

	struct IDivaJudgeListenerParams {
		unsigned int score, health;
		EHitScore hitScore;
		bool wrong;
		EDivaJudgeCompletion completion;
		int numNotesComplete;

		IDivaJudgeListenerParams() : score(0), health(0), hitScore(eHS_None), wrong(false), completion(eDJC_Cheap), numNotesComplete(0) {};
	};

	struct IDivaJudgeListener {
		virtual void OnJudgeEvent(IDivaJudgeListenerParams params) = 0;
		//virtual void OnMaxHP(unsigned int maxHP) = 0;
		virtual void OnTechZoneScore(unsigned int numNotes, bool active) = 0;
	};

	struct IDivaJudgeTechZone {
		unsigned int totalNotes;
		unsigned int currNotes;
		bool active;

		IDivaJudgeTechZone() : totalNotes(0), currNotes(0), active(true) {};
	};

	//communication bus for flownodes
	class DivaJudgeEventGroup : public AZ::EBusTraits {
	public:
		virtual unsigned int getScore() = 0;
		virtual unsigned int getHealth() = 0;
		virtual unsigned int getMaxHP() = 0;
		virtual unsigned int getCombo() = 0;
		virtual EDivaJudgeCompletion getCompletion() = 0;
		virtual unsigned int getNumHits(EHitScore score) = 0;

		virtual unsigned int numTechZoneNotes() = 0;
		virtual bool isTechZoneActive() = 0;
	};

	typedef AZ::EBus<DivaJudgeEventGroup> DivaJudgeBus;

	class IDivaJudge : public DivaJudgeBus::Handler {
	public:
		IDivaJudge();
		~IDivaJudge();
	public:
		virtual void DivaJudgeCallback(IDivaJudgeParams params) = 0;
	public:
		void setDivaSequence(IDivaSequenceJudge * divaSeq) { this->m_divaSeq = divaSeq; }
		void setTotalNotes(unsigned int totalNotes) { this->m_totalNotes = totalNotes; }
		void SetScoreRanges(float cool, float fine, float safe, float sad, float worst);
		virtual void SetTechZoneNotes(std::vector<unsigned int> techZoneNotes);
	public:
		virtual void addListener(IDivaJudgeListener * listener) { if (listener != nullptr) this->m_listeners.push_back(listener); }
		void removeListener(IDivaJudgeListener * listener) { if (listener != nullptr) this->m_listeners.erase(std::remove(this->m_listeners.begin(), this->m_listeners.end(), listener), this->m_listeners.end()); }
	public:
		virtual unsigned int getScore() = 0;
		virtual unsigned int getHealth() = 0;
		virtual unsigned int getMaxHP() = 0;
		virtual unsigned int getCombo() = 0;
		virtual EDivaJudgeCompletion getCompletion() = 0;
		virtual unsigned int getNumHits(EHitScore score) = 0;

		virtual unsigned int numTechZoneNotes() = 0;
		virtual bool isTechZoneActive() = 0;
	public:
		Range m_Range_Cool, m_Range_Fine, m_Range_Safe, m_Range_Sad, m_Range_Worst;
		float m_Range_CoolF, m_Range_WorstF;
	protected:
		IDivaSequenceJudge * m_divaSeq;
		unsigned int m_totalNotes;
		std::vector<IDivaJudgeTechZone *> m_techZoneNotes;
		SectionType m_currSection;
	protected:
		std::vector<IDivaJudgeListener *> m_listeners;
	private:
		bool m_busConnected;
	};

	/*class DivaJudgeShared {
	public:
		static void SetScoreRanges(float cool, float fine, float safe, float sad, float worst);
		static Range sm_Range_Cool, sm_Range_Fine, sm_Range_Safe, sm_Range_Sad, sm_Range_Worst;
		static float sm_Range_CoolF, sm_Range_WorstF;
	};*/
}

#endif