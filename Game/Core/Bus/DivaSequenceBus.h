#ifndef _H_DIVASEQUENCE_BUS
#define _H_DIVASEQUENCE_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>

namespace LYGame {
	class DivaSequenceJudgeGroup : public AZ::EBusTraits {
	public: //there will only be one sequence that recieves judge events
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
	public:
		virtual void UpdateNoteHit() = 0;
	};

	enum EEffectList {
		//eEL_ENote, //special effect
		eEL_Cool,
		eEL_Fine,
		eEL_Safe,
		eEL_Sad,
		eEL_Worse,
		eEL_SwipeL,
		eEL_SwipeR,
		eEL_SwipeLEXTick,
		eEL_SwipeREXTick,
		eEL_Count
	};

	class DivaSequenceEffectsGroup : public AZ::EBusTraits {
	public: //there will be only one sequence that recieves effect events
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
	public:
		virtual void PushbackRating(AZ::Vector2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong) = 0;
		virtual void PushbackEffect(AZ::Vector2 pos, EEffectList eff) = 0;
		virtual void PushbackHoldMulti(AZ::Vector2 pos, unsigned int score, EHitScore hitscore, bool wrong) = 0;
	};

	using DivaSequenceJudgeBus = AZ::EBus<DivaSequenceJudgeGroup>;
	using DivaSequenceEffectsBus = AZ::EBus<DivaSequenceEffectsGroup>;
}

#endif