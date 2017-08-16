#ifndef _H_DIVAHUD_BUS
#define _H_DIVAHUD_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>

#include <AzCore\std\string\string.h>
//#include <AzCore\Math\Color.h>

namespace LYGame {
	class DivaHudGroup : public AZ::EBusTraits {
	public:
		virtual void OnChanceEnter() = 0;
		virtual void OnChanceExit() = 0;
		virtual void OnTechEnter() = 0;
		virtual void OnTechExit() = 0;
	public:
		virtual void SetHP(unsigned int value) = 0;
		virtual void SetMaxHP(unsigned int value) = 0; //dont use?
	public:
		virtual void SetScore(unsigned int value) = 0;
	public:
		virtual void SetCompletion(unsigned int value, unsigned int count) = 0;
		virtual void SetRating(EDivaJudgeCompletion rating) = 0;
	public:
		virtual void SetTechZoneInfo(unsigned int value, bool active) = 0;
	public:
		virtual void SetHitScore(EHitScore hitScore, bool wrong) = 0;
	public:
		//virtual void SetLyrics(AZStd::string lyrics, AZStd::string romaji, AZStd::string translation) = 0;
		virtual void SetLyrics(AZStd::string lyrics) = 0;
		virtual void SetRomaji(AZStd::string romaji) = 0;
		virtual void SetTranslation(AZStd::string translation) = 0; //, AZStd::string font, AZ::Color color
	};

	using DivaHudBus = AZ::EBus<DivaHudGroup>;
}

#endif