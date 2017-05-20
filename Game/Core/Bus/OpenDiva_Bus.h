#ifndef _H_OPENDIVA_BUS
#define _H_OPENDIVA_BUS

#pragma once

#include <AzCore/EBus/EBus.h>
#include "../Input/InputSystem.h"
#include "../CommonEnums.h"

namespace LYGame {
	namespace OpenDivaBus {
		/*class BaseOpenDivaEventGroup : public AZ::EBusTraits {
		public:
			virtual void Reset() = 0;
		};*/

		class OpenDivaSongEventGroup : public AZ::EBusTraits {
		public:
			virtual void OnStart() = 0;
			virtual void OnEnd() = 0;
		};

		class OpenDivaHPEventGroup : public AZ::EBusTraits {
		public:
			virtual void SetHP(int value) = 0; //constant update
			virtual void SetMaxHP(int value) = 0; //set once and forget.
		};

		class OpenDivaScoreEventGroup : public AZ::EBusTraits {
		public:
			virtual void SetScore(int value) = 0; //constant update
		};

		class OpenDivaCompletionEventGroup : public AZ::EBusTraits {
		public:
			virtual void SetNotes(int value) = 0; //constant update
			virtual void SetTotalNotes(int value) = 0; //set once and forget.
			virtual void SetRating(EDivaJudgeCompletion rating) = 0;
		};

		class OpenDivaZoneEventsGroup : public AZ::EBusTraits {
		public:
			virtual void OnChanceEnter() = 0;
			virtual void OnChanceExit() = 0;
			virtual void OnTechEnter() = 0;
			virtual void OnTechExit() = 0;
		};

		class OpenDivaTechnicalZoneInfoGroup : public AZ::EBusTraits {
		public:
			virtual void SetNumNotes(int value) = 0;
			virtual void SetActive(bool value) = 0;
		};

		class OpenDivaHitScoreEventGroup : public AZ::EBusTraits {
		public:
			virtual void SetHitScore(EHitScore hitScore, bool wrong) = 0; //constant update
		};

		class OpenDivaUiInputEventGroup : public AZ::EBusTraits, public IUIInputSystemListener {
		public:
			virtual void OnUi_Up(int mode, float value) = 0;
			virtual void OnUi_Down(int mode, float value) = 0;
			virtual void OnUi_Left(int mode, float value) = 0;
			virtual void OnUi_Right(int mode, float value) = 0;
			virtual void OnUi_Select(int mode, float value) = 0;
			virtual void OnUi_Cancel(int mode, float value) = 0;
		};

		//Redesign
		class OpenDivaLyricsEventGroup : public AZ::EBusTraits {
		public:
			virtual void SetLyrics(string lyrics, string romaji, string translation) = 0;
			//virtual void SetLyrics(string lyrics, string font, ColorF color) = 0;
		};

		class OpenDivaRomajiEventGroup : public AZ::EBusTraits {
		public:
			virtual void SetRomaji(string romaji, string font, ColorF color) = 0;
		};

		class OpenDivaTranslationEventGroup : public AZ::EBusTraits {
		public:
			virtual void SetTranslation(string romaji, string font, ColorF color) = 0;
		};

		//--------------------------------------------------
		using OpenDivaSongBus = AZ::EBus<OpenDivaSongEventGroup>;
		using OpenDivaHPBus = AZ::EBus<OpenDivaHPEventGroup>;
		using OpenDivaScoreBus = AZ::EBus<OpenDivaScoreEventGroup>;
		using OpenDivaCompletionBus = AZ::EBus<OpenDivaCompletionEventGroup>;
		using OpenDivaZoneEventsBus = AZ::EBus<OpenDivaZoneEventsGroup>;
		using OpenDivaTechnicalZoneInfoBus = AZ::EBus<OpenDivaTechnicalZoneInfoGroup>;
		using OpenDivaHitScoreBus = AZ::EBus<OpenDivaHitScoreEventGroup>;
		using OpenDivaUiInputBus = AZ::EBus<OpenDivaUiInputEventGroup>;

		//Redesign
		using OpenDivaLyricsBus = AZ::EBus<OpenDivaLyricsEventGroup>;
		using OpenDivaRomajiBus = AZ::EBus<OpenDivaRomajiEventGroup>;
		using OpenDivaTranslationBus = AZ::EBus<OpenDivaTranslationEventGroup>;
	}
}

#endif