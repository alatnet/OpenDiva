#ifndef _H_DIVAJUDGE_BUS
#define _H_DIVAJUDGE_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace OpenDiva {
	struct IDivaJudgeParams {
		ENoteType nType;
		ENoteHitType hType;
		EHitScore hitscore;
		SectionType sType;
		bool hold;
		bool holdRelease;
		bool wrong;
		AZ::Vector2 notePos;
		unsigned int holdMult;

		IDivaJudgeParams() :
			nType(eNT_Count),
			hType(eNHT_INVALID),
			hitscore(eHS_None),
			sType(eST_Norm),
			hold(false),
			holdRelease(false),
			wrong(false),
			notePos(AZ::Vector2(0.0f, 0.0f)),
			holdMult(0) {
		};
	};

	//communication bus for diva judge
	class DivaJudgeGroup : public AZ::EBusTraits {
	public: //there will be only one judge to recieve events
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
	public:
		virtual unsigned int GetScore() = 0;
		virtual unsigned int GetHealth() = 0;
		virtual unsigned int GetMaxHP() = 0;
		virtual unsigned int GetCombo() = 0;
		virtual EDivaJudgeCompletion GetCompletion() = 0;
		virtual unsigned int GetNumHits(EHitScore score) = 0;

		virtual unsigned int TechZoneNotesCount() = 0;
		virtual bool IsTechZoneActive() = 0;

		virtual void SetTotalNotes(unsigned int totalNotes) = 0;
		virtual void SetTechZoneNotes(AZStd::vector<unsigned int> techZoneNotes) = 0;
		virtual void SetScoreRanges(float cool, float fine, float safe, float sad, float worst) = 0;

		virtual void OnJudge(IDivaJudgeParams params) = 0;
		virtual EHitScore GetHitScore(float time, EHitScore ret) = 0;
		virtual EHitScore GetWorstRange(float time, EHitScore ret) = 0;
	};

	using DivaJudgeBus = AZ::EBus<DivaJudgeGroup>;
}

#endif