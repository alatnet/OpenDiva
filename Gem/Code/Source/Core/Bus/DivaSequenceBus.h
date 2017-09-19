#ifndef _H_DIVASEQUENCE_BUS
#define _H_DIVASEQUENCE_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>

#include "../Files/LyricsFile.h"

namespace OpenDiva {
	class DivaSequenceJudgeGroup : public AZ::EBusTraits {
	public: //there will only be one sequence that recieves judge events
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
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
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
	public:
		virtual void PushbackRating(AZ::Vector2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong) = 0;
		virtual void PushbackEffect(AZ::Vector2 pos, EEffectList eff) = 0;
		virtual void PushbackHoldMulti(AZ::Vector2 pos, unsigned int score, EHitScore hitscore, bool wrong) = 0;
		virtual void PushbackSubtext(LyricsFile::SubtextEntry* entry) = 0;
	};

	using DivaSequenceJudgeBus = AZ::EBus<DivaSequenceJudgeGroup>;
	using DivaSequenceEffectsBus = AZ::EBus<DivaSequenceEffectsGroup>;

	class DivaAudioEventsGroup : public AZ::EBusTraits {
	public: //there will be only one sequence to recieve events
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
	public:
		virtual void OnAudioStart() = 0; //before fade in starts
		virtual void OnAudioPause() = 0;
		virtual void OnAudioResume() = 0;
		virtual void OnAudioEnd() = 0; //after fade in ends
	};
	using DivaAudioEventsBus = AZ::EBus<DivaAudioEventsGroup>;
}

#endif