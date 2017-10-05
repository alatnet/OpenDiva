#ifndef _H_DIVAHUD_BUS
#define _H_DIVAHUD_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore\std\string\string.h>
//#include <AzCore\Math\Color.h>

#include <AzCore/RTTI/BehaviorContext.h>

namespace OpenDiva {
	class DivaHudChanceEventsGroup : public AZ::EBusTraits {
	public:
		virtual void OnChanceEnter() = 0;
		virtual void OnChanceExit() = 0;
	};

	class DivaHudTechEventsGroup : public AZ::EBusTraits {
	public:
		virtual void OnTechEnter() = 0;
		virtual void OnTechExit() = 0;
	public:
		virtual void SetTechZoneInfo(unsigned int value, bool active) = 0;
	};

	class DivaHudHpGroup : public AZ::EBusTraits {
	public:
		virtual void SetHP(unsigned int value) = 0;
		virtual void SetMaxHP(unsigned int value) = 0; //dont use?
	};

	class DivaHudScoreGroup : public AZ::EBusTraits {
	public:
		virtual void SetScore(unsigned int value) = 0;
	};

	class DivaHudCompletionGroup : public AZ::EBusTraits {
	public:
		virtual void SetCompletion(unsigned int value, unsigned int count) = 0;
		virtual void SetRating(EDivaJudgeCompletion rating) = 0;
	};

	class DivaHudHitScoreGroup : public AZ::EBusTraits {
	public:
		virtual void SetHitScore(ENoteType noteType, EHitScore hitScore, bool wrong) = 0;
	};

	class DivaHudLyricsGroup : public AZ::EBusTraits {
	public:
		virtual void SetLyrics(AZStd::string lyrics) = 0;
	};

	/*class DivaHudRomajiGroup : public AZ::EBusTraits {
	public:
		virtual void SetRomaji(AZStd::string romaji) = 0;
	};

	class DivaHudTranslationGroup : public AZ::EBusTraits {
	public:
		virtual void SetTranslation(AZStd::string translation) = 0;
	};*/

	using DivaHudChanceEventsBus = AZ::EBus<DivaHudChanceEventsGroup>;
	using DivaHudTechEventsBus = AZ::EBus<DivaHudTechEventsGroup>;
	using DivaHudHpBus = AZ::EBus<DivaHudHpGroup>;
	using DivaHudScoreBus = AZ::EBus<DivaHudScoreGroup>;
	using DivaHudCompletionBus = AZ::EBus<DivaHudCompletionGroup>;
	using DivaHudHitScoreBus = AZ::EBus<DivaHudHitScoreGroup>;
	using DivaHudLyricsBus = AZ::EBus<DivaHudLyricsGroup>;
	//using DivaHudRomajiBus = AZ::EBus<DivaHudRomajiGroup>;
	//using DivaHudTranslationBus = AZ::EBus<DivaHudTranslationGroup>;

	//Lua Handlers
	class DivaHudChanceEventsHandler : public DivaHudChanceEventsBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaHudChanceEventsHandler,
			"{621267FF-3AD4-4071-98C3-11DD62933AB2}",
			AZ::SystemAllocator,
			OnChanceEnter,
			OnChanceExit
		);

	public:
		void OnChanceEnter() { Call(FN_OnChanceEnter); }
		void OnChanceExit() { Call(FN_OnChanceExit); }
	};
	class DivaHudTechEventsHandler : public DivaHudTechEventsBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaHudTechEventsHandler,
			"{376B79C8-F5C6-4D3E-8498-E5441452EF44}",
			AZ::SystemAllocator,
			OnTechEnter,
			OnTechExit,
			SetTechZoneInfo
		);

	public:
		void OnTechEnter() { Call(FN_OnTechEnter); }
		void OnTechExit() { Call(FN_OnTechEnter); }
		void SetTechZoneInfo(unsigned int value, bool active) { Call(FN_SetTechZoneInfo, value, active); }
	};
	class DivaHudHpHandler : public DivaHudHpBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaHudHpHandler,
			"{B627CBEC-B8F9-462A-971D-00CE35F82E8C}",
			AZ::SystemAllocator,
			SetHP,
			SetMaxHP
		);

	public:
		void SetHP(unsigned int value) { Call(FN_SetHP, value); }
		void SetMaxHP(unsigned int value) { Call(FN_SetMaxHP, value); }
	};
	class DivaHudScoreHandler : public DivaHudScoreBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaHudScoreHandler,
			"{8CD7F341-CC35-453D-AAB9-9FB1646C40BC}",
			AZ::SystemAllocator,
			SetScore
		);

	public:
		void SetScore(unsigned int value) { Call(FN_SetScore, value); }
	};
	class DivaHudCompletionHandler : public DivaHudCompletionBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaHudCompletionHandler,
			"{1BE096F2-4E0C-43CE-86F8-472F9D34E96C}",
			AZ::SystemAllocator,
			SetCompletion,
			SetRating
		);

	public:
		void SetCompletion(unsigned int value, unsigned int count) { Call(FN_SetCompletion, value, count); }
		void SetRating(EDivaJudgeCompletion rating) { Call(FN_SetRating, rating); }
	};
	class DivaHudHitScoreHandler : public DivaHudHitScoreBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaHudHitScoreHandler,
			"{CE7DF352-10BD-4AF8-AED8-DC26DCA44CB0}",
			AZ::SystemAllocator,
			SetHitScore
		);

	public:
		void SetHitScore(ENoteType noteType, EHitScore hitScore, bool wrong) { Call(FN_SetHitScore, noteType, hitScore, wrong); }
	};
	class DivaHudLyricsHandler : public DivaHudLyricsBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaHudLyricsHandler,
			"{9E04A2D0-31B1-403A-B1FA-75AE34ADC2A9}",
			AZ::SystemAllocator,
			SetLyrics
		);

	public:
		void SetLyrics(AZStd::string lyrics) { Call(FN_SetLyrics, lyrics); }
	};
}

#endif