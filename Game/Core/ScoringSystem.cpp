#include <StdAfx.h>
#include "ScoringSystem.h"

namespace LYGame {

	//TODO: Hold notes... >.<
	ScoringSystem::ScoringSystem(/*NoteFile nFile*/) {
	}

	ScoringSystem::~ScoringSystem() {
	}

	void ScoringSystem::Init() {
		#pragma omp parallel for
		for (int i = 0; i < ENoteType::eNT_Count; i++) {
			this->m_Notes[i].clear();
			this->m_CurrNote[i] = 0;
		}
	}

	void ScoringSystem::OnCross(int mode, float value) {
		//we're checking which note has the earliest timing and executing it's OnCross;
		ENoteType checkType = this->CompareTimings();
		if (checkType == eNT_Count) return;
		//m_Notes[checkType][this->m_CurrNote[checkType]]->ArrowHeld(this->m_DownHeld);
		m_Notes[checkType][this->m_CurrNote[checkType]]->OnCross(mode, value);
	}

	void ScoringSystem::OnCircle(int mode, float value) {
		ENoteType checkType = this->CompareTimings();
		if (checkType == eNT_Count) return;
		//m_Notes[checkType][this->m_CurrNote[checkType]]->ArrowHeld(this->m_RightHeld);
		m_Notes[checkType][this->m_CurrNote[checkType]]->OnCircle(mode, value);
	}

	void ScoringSystem::OnTriangle(int mode, float value) {
		ENoteType checkType = this->CompareTimings();
		if (checkType == eNT_Count) return;
		//m_Notes[checkType][this->m_CurrNote[checkType]]->ArrowHeld(this->m_UpHeld);
		m_Notes[checkType][this->m_CurrNote[checkType]]->OnTriangle(mode, value);
	}

	void ScoringSystem::OnSquare(int mode, float value) {
		ENoteType checkType = this->CompareTimings();
		if (checkType == eNT_Count) return;
		//m_Notes[checkType][this->m_CurrNote[checkType]]->ArrowHeld(this->m_LeftHeld);
		m_Notes[checkType][this->m_CurrNote[checkType]]->OnSquare(mode, value);
	}

	void ScoringSystem::OnLeft(int mode, float value) {
		/*switch (mode) {
		case eAAM_OnHold:
			m_LeftHeld = true;
			break;
		case eAAM_OnRelease:
			m_LeftHeld = false;
			break;
		}*/
		this->OnSquare(mode, value);
	}

	void ScoringSystem::OnRight(int mode, float value) {
		/*switch (mode) {
		case eAAM_OnHold:
			m_RightHeld = true;
			break;
		case eAAM_OnRelease:
			m_RightHeld = false;
			break;
		}*/
		this->OnCircle(mode, value);
	}

	void ScoringSystem::OnUp(int mode, float value) {
		/*switch (mode) {
		case eAAM_OnHold:
			m_UpHeld = true;
			break;
		case eAAM_OnRelease:
			m_UpHeld = false;
			break;
		}*/
		this->OnTriangle(mode, value);
	}
	
	void ScoringSystem::OnDown(int mode, float value) {
		/*switch (mode) {
		case eAAM_OnHold:
			m_DownHeld = true;
			break;
		case eAAM_OnRelease:
			m_DownHeld = false;
			break;
		}*/
		this->OnCross(mode, value);
	}

	void ScoringSystem::ScoreCallback(IScoringCallbackParams params) {
		if (params.hitScore == eHS_None || params.noteType != eNT_Count) return;
		if (params.noteType != eNT_Count) this->m_CurrNote[params.noteType]++;

		//scoring
		switch (params.hitScore) {
		case eHS_Cool:
			if (params.incorrect) {
				this->m_Score += 250;
				m_NumHits[eHS_Worst]++;
			} else {
				this->m_Score += 500;
				m_NumHits[eHS_Cool]++;
			}
			this->m_ComboCount++;
			if (params.chanceTime) this->m_CtMultiplier++;
			break;
		case eHS_Fine:
			if (params.incorrect) {
				this->m_Score += 150;
				m_NumHits[eHS_Worst]++;
			} else {
				this->m_Score += 300;
				m_NumHits[eHS_Fine]++;
			}
			this->m_ComboCount++;
			if (params.chanceTime) this->m_CtMultiplier++;
			break;
		case eHS_Safe:
			if (params.incorrect) {
				this->m_Score += 50;
				m_NumHits[eHS_Worst]++;
			} else {
				this->m_Score += 100;
				m_NumHits[eHS_Safe]++;
			}
			if (this->m_MaxCombo < this->m_ComboCount) this->m_MaxCombo = this->m_ComboCount;
			this->m_ComboCount = 0;
			if (params.chanceTime) this->m_CtMultiplier = 1;
			break;
		case eHS_Sad:
			if (params.incorrect) {
				this->m_Score += 25;
				m_NumHits[eHS_Worst]++;
			} else {
				this->m_Score += 50;
				m_NumHits[eHS_Sad]++;
			}
			if (!params.chanceTime) this->m_Health--;
			if (this->m_MaxCombo < this->m_ComboCount) this->m_MaxCombo = this->m_ComboCount;
			this->m_ComboCount = 0;
			if (params.chanceTime) this->m_CtMultiplier = 1;
			break;
		case eHS_Worst:
			if (!params.chanceTime) this->m_Health--;
			if (this->m_MaxCombo < this->m_ComboCount) this->m_MaxCombo = this->m_ComboCount;
			this->m_ComboCount = 0;
			if (params.chanceTime) this->m_CtMultiplier = 1;
			m_NumHits[eHS_Worst]++;
			break;
		}

		if (params.incorrect) this->m_ComboCount = 0;

		//combo scoring
		if (this->m_ComboCount >= 10) this->m_Score += 50;
		if (this->m_ComboCount >= 20) this->m_Score += 100;
		if (this->m_ComboCount >= 30) this->m_Score += 150;
		if (this->m_ComboCount >= 40) this->m_Score += 200;
		if (this->m_ComboCount >= 50) this->m_Score += 250;

		//chancetime scoring
		if (params.chanceTime) {
			if (this->m_CtMultiplier > 50) this->m_CtMultiplier = 50;
			this->m_Score += 100 * this->m_CtMultiplier;
		}

		if (this->m_Health <= 0) this->m_Health = 0;
	}

	//adjust this to add in more note types.
	ENoteType ScoringSystem::CompareTimings() {
		/*int ret = eNT_Count;
		float timingTemp = 0.0;

		for (int i = 0; i < eNT_Count; i++) {
			timingTemp =
			for (int i2 = 0; i2 < eNT_Count; i2++) {

			}
		}*/

		float crossTiming = m_Notes[eNT_Cross][this->m_CurrNote[eNT_Cross]]->GetTiming();
		float triangleTiming = m_Notes[eNT_Triangle][this->m_CurrNote[eNT_Triangle]]->GetTiming();
		float circleTiming = m_Notes[eNT_Circle][this->m_CurrNote[eNT_Circle]]->GetTiming();
		float squareTiming = m_Notes[eNT_Square][this->m_CurrNote[eNT_Square]]->GetTiming();

		if (
			crossTiming < triangleTiming &&
			crossTiming < circleTiming &&
			crossTiming < squareTiming
		) return eNT_Cross;

		if (
			triangleTiming < crossTiming &&
			triangleTiming < circleTiming &&
			triangleTiming < squareTiming
		) return eNT_Triangle;

		if (
			circleTiming < crossTiming &&
			circleTiming < triangleTiming &&
			circleTiming < squareTiming
		) return eNT_Circle;

		if (
			squareTiming < crossTiming &&
			squareTiming < circleTiming &&
			squareTiming < triangleTiming
		) return eNT_Square;

		return eNT_Count;
	}
}