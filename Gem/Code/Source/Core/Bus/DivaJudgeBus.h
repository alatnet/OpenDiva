#ifndef _H_DIVAJUDGE_BUS
#define _H_DIVAJUDGE_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "../Files/NoteFileEntries.h"

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

		AZ_RTTI(IDivaJudgeParams, "{242C7D45-4F59-4DC5-B4DD-C5B0979DBD2A}");

		ENoteType GetNoteType() { return this->nType; }
		ENoteHitType GetNoteHitType() { return this->hType; }
		EHitScore GetHitScore() { return this->hitscore; }
		SectionType GetSectionType() { return this->sType; }
		bool GetHold() { return this->hold; }
		bool GetHoldRelease() { return this->holdRelease; }
		bool GetWrong() { return this->wrong; }
		AZ::Vector2 GetNotePos() { return this->notePos; }
		unsigned int GetHoldMulti() { return this->holdMult; }

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

		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<IDivaJudgeParams>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<IDivaJudgeParams>("IDivaJudgeParams")
				->Method("GetNoteType", &IDivaJudgeParams::GetNoteType)
				->Method("GetNoteHitType", &IDivaJudgeParams::GetNoteHitType)
				->Method("GetHitScore", &IDivaJudgeParams::GetHitScore)
				->Method("GetSectionType", &IDivaJudgeParams::GetSectionType)
				->Method("GetHold", &IDivaJudgeParams::GetHold)
				->Method("GetHoldRelease", &IDivaJudgeParams::GetHoldRelease)
				->Method("GetWrong", &IDivaJudgeParams::GetWrong)
				->Method("GetNotePos", &IDivaJudgeParams::GetNotePos)
				->Method("GetHoldMulti", &IDivaJudgeParams::GetHoldMulti)

				->Constant("nType", &IDivaJudgeParams::GetNoteType)
				->Constant("hType", &IDivaJudgeParams::GetNoteHitType)
				->Constant("hitscore", &IDivaJudgeParams::GetHitScore)
				->Constant("sType", &IDivaJudgeParams::GetSectionType)
				->Constant("hold", &IDivaJudgeParams::GetHold)
				->Constant("holdRelease", &IDivaJudgeParams::GetHoldRelease)
				->Constant("wrong", &IDivaJudgeParams::GetWrong)
				->Constant("notePos", &IDivaJudgeParams::GetNotePos)
				->Constant("holdMulti", &IDivaJudgeParams::GetHoldMulti);
		}
	};

	//communication bus for diva judge
	class DivaJudgeGroup : public AZ::EBusTraits {
	public: //there will be only one judge to recieve events
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
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


	class DivaJudgeHandler : public DivaJudgeBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaJudgeHandler,
			"{ADDB4E58-BC32-4AA9-AB15-8511C04AE618}",
			AZ::SystemAllocator,
			GetScore,
			GetHealth,
			GetMaxHP,
			GetCombo,
			GetCompletion,
			GetNumHits,
			TechZoneNotesCount,
			IsTechZoneActive,
			SetTotalNotes,
			SetTechZoneNotes,
			SetScoreRanges,
			OnJudge,
			GetHitScore,
			GetWorstRange
		);

	public:
		unsigned int GetScore() {
			unsigned int ret = 0;
			CallResult(ret, FN_GetScore);
			return ret;
		}
		unsigned int GetHealth() {
			unsigned int ret = 0;
			CallResult(ret, FN_GetHealth);
			return ret;
		}
		unsigned int GetMaxHP() {
			unsigned int ret = 0;
			CallResult(ret, FN_GetMaxHP);
			return ret;
		}
		unsigned int GetCombo() {
			unsigned int ret = 0;
			CallResult(ret, FN_GetCombo);
			return ret;
		}
		EDivaJudgeCompletion GetCompletion() {
			EDivaJudgeCompletion ret = eDJC_Cheap;
			CallResult(ret, FN_GetCompletion);
			return ret;
		}
		unsigned int GetNumHits(EHitScore score) {
			unsigned int ret = 0;
			CallResult(ret, FN_GetNumHits, score);
			return ret;
		}
		unsigned int TechZoneNotesCount() {
			unsigned int ret = 0;
			CallResult(ret, FN_TechZoneNotesCount);
			return ret;
		}
		bool IsTechZoneActive() {
			bool ret = false;
			CallResult(ret, FN_IsTechZoneActive);
			return ret;
		}
		void SetTotalNotes(unsigned int totalNotes) { Call(FN_SetTotalNotes, totalNotes); }
		void SetTechZoneNotes(AZStd::vector<unsigned int> techZoneNotes) { Call(FN_SetTechZoneNotes, techZoneNotes); }
		void SetScoreRanges(float cool, float fine, float safe, float sad, float worst) { Call(FN_SetScoreRanges, cool, fine, safe, sad, worst); }
		void OnJudge(IDivaJudgeParams params) { Call(FN_OnJudge, params); }
		EHitScore GetHitScore(float time, EHitScore ret) {
			EHitScore ret1 = ret;
			CallResult(ret1, FN_GetHitScore, time, ret);
			return ret1;
		}
		EHitScore GetWorstRange(float time, EHitScore ret) {
			EHitScore ret1 = ret;
			CallResult(ret1, FN_GetWorstRange, time, ret);
			return ret1;
		}
	};
}

#endif