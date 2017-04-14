#ifndef _H_FLOWNODES_OPENDIVA_BUS
#define _H_FLOWNODES_OPENDIVA_BUS

#pragma once

#include <AzCore/EBus/EBus.h>
#include "../../Input/InputSystem.h"

namespace LYGame {
	namespace OpenDivaFlowgraphBus {		
		class BaseOpenDivaEventGroup : public AZ::EBusTraits {
		public:
			virtual void Reset() = 0;
		};

		class OpenDivaSongEventGroup : public AZ::EBusTraits {
		public:
			virtual void OnStart() = 0;
			virtual void OnEnd() = 0;
		};

		class OpenDivaLyricsEventGroup : public BaseOpenDivaEventGroup {
		public:
			virtual void SetLyrics(string lyrics, string romaji, string translation) = 0;
			//virtual void SetLyrics(string lyrics, string font, ColorF color) = 0;
		};

		class OpenDivaRomajiEventGroup : public BaseOpenDivaEventGroup {
		public:
			virtual void SetRomaji(string romaji, string font, ColorF color) = 0;
		};

		class OpenDivaTranslationEventGroup : public BaseOpenDivaEventGroup {
		public:
			virtual void SetTranslation(string romaji, string font, ColorF color) = 0;
		};

		class OpenDivaHPEventGroup : public BaseOpenDivaEventGroup {
		public:
			virtual void SetHP(int value) = 0; //constant update
			virtual void SetMaxHP(int value) = 0; //set once and forget.
		};

		class OpenDivaScoreEventGroup : public BaseOpenDivaEventGroup {
		public:
			virtual void SetScore(int value) = 0; //constant update
		};

		class OpenDivaCompletionEventGroup : public BaseOpenDivaEventGroup {
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

		class OpenDivaTechnicalZoneInfoGroup : public BaseOpenDivaEventGroup {
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

		//--------------------------------------------------
		typedef AZ::EBus<OpenDivaSongEventGroup> OpenDivaSongBus;

		typedef AZ::EBus<OpenDivaLyricsEventGroup> OpenDivaLyricsBus;

		typedef AZ::EBus<OpenDivaRomajiEventGroup> OpenDivaRomajiBus;

		typedef AZ::EBus<OpenDivaTranslationEventGroup> OpenDivaTranslationBus;

		typedef AZ::EBus<OpenDivaHPEventGroup> OpenDivaHPBus;

		typedef AZ::EBus<OpenDivaScoreEventGroup> OpenDivaScoreBus;

		typedef AZ::EBus<OpenDivaCompletionEventGroup> OpenDivaCompletionBus;

		typedef AZ::EBus<OpenDivaZoneEventsGroup> OpenDivaZoneEventsBus;

		typedef AZ::EBus<OpenDivaTechnicalZoneInfoGroup> OpenDivaTechnicalZoneInfoBus;

		typedef AZ::EBus<OpenDivaHitScoreEventGroup> OpenDivaHitScoreBus;

		typedef AZ::EBus<OpenDivaUiInputEventGroup> OpenDivaUiInputBus;
	}
}

#endif