#ifndef _H_SCORINGSYSTEM_
#define _H_SCORINGSYSTEM_
#pragma once

#include "Files\NoteFile.h"
#include "Input\InputSystem.h"

namespace LYGame {
	struct IScoringCallbackParams {
		EHitScore hitScore; //what kind of hit timing was achived.
		ENoteType noteType; //what note type is it.
		ENoteHitHold hold; //hold note information.
		unsigned int holdHits; //number of hold note "hits". (occur during hold)
		ENoteHitArrow arrow; //arrow note information. //future expansion
		bool chanceTime; //are we in chance time?
		bool incorrect; //is the button press incorrect?

		IScoringCallbackParams() :
			hitScore(eHS_None),
			noteType(eNT_Count),
			hold(eNHH_None),
			holdHits(0),
			arrow(eNHA_None), //future expansion
			chanceTime(false),
			incorrect(false)
		{};
	};

	struct IScoringCallback {
		virtual void ScoreCallback(IScoringCallbackParams params) {};
	};

	struct IScoringListener : public IInputSystemListener {
		virtual void SetScoreCallback(IScoringCallback * callback) = 0;
		virtual float GetTiming() = 0; //used to check which note to work on.
		virtual bool IsHold() { return false; }; //are we a hold note?
		virtual bool WaitingForHoldEnd() { return false; } //are we waiting for a button press release?
		virtual bool IsArrow() { return false; }; //are we an arrow note?
		virtual bool WaitingForArrowPress() { return false; } //are we waiting for the second button press?
		virtual void ArrowHeld(bool held); //used to tell that the second button press is held.
	};

	class ScoringSystem : public IInputSystemListener, public IScoringCallback {
	public:
		ScoringSystem(/*NoteFile noteFile*/);
		~ScoringSystem();
	public:
		void Init();
		unsigned int GetScore() { return this->m_Score; }
		unsigned int GetCombo() { return this->m_ComboCount; }
		int GetHealth() { return this->m_Health; }
	public: //input stuff
		void OnCross(int mode, float value) override;
		void OnCircle(int mode, float value) override;
		void OnTriangle(int mode, float value) override;
		void OnSquare(int mode, float value) override;

		void OnLeft(int mode, float value) override;
		void OnRight(int mode, float value) override;
		void OnUp(int mode, float value) override;
		void OnDown(int mode, float value) override;
	public:
		void ScoreCallback(IScoringCallbackParams params);
	private:
		ENoteType CompareTimings(); //adjust this to add in more note types.
	private: //how we keep track of what notes are to be checked with. (keeps cpu usage down a bit.)
		std::vector<IScoringListener*> m_Notes[ENoteType::eNT_Count]; //separate notes based on type.
		int m_CurrNote[ENoteType::eNT_Count]; //keep track of the current note that is to be checked.
	private:
		unsigned int m_ComboCount; //our combo count!
		unsigned int m_Score; //our score!
		unsigned int m_CtMultiplier; //chance time multiplier
		int m_Health; //our health!
		//int m_Completion; //our completion!
		unsigned int m_NumHits[eHS_None];
		unsigned int m_MaxCombo;
	private:
		bool m_LeftHeld, m_RightHeld, m_UpHeld, m_DownHeld;
	};

}

#endif //_H_SCORINGSYSTEM_