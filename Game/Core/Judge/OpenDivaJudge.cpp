#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "IDivaJudge.h"
#include "OpenDivaJudge.h"
#include "../../../Graphics/Particles.h"

namespace LYGame {
	OpenDivaJudge::OpenDivaJudge(void * userdata) {
		/*
		#define RANGE_WORST 0.80f
		#define RANGE_SAD 0.85f
		#define RANGE_SAFE 0.90f
		#define RANGE_FINE 0.95f
		#define RANGE_COOL 1.0f
		*/
		//SetScoreRanges(COOL,FINE,SAFE,SAD,WORST);
		SetScoreRanges(1.005f, 0.995f, 0.99f, 0.985f, 0.98f);
		m_score = m_combo = m_ChanceMult = m_maxCombo = m_completion = 0;
		m_health = 100;
		for (int i = 0; i < eHS_None; i++) m_numHits[i] = 0;
		//m_divaSeq = nullptr;
		m_currTechZone = 0;
	}

	void OpenDivaJudge::SetTechZoneNotes(AZStd::vector<unsigned int> techZoneNotes) {
		IDivaJudge::SetTechZoneNotes(techZoneNotes);

		for (unsigned int numNotes : techZoneNotes) m_totalNotes += numNotes;
	}

	void OpenDivaJudge::OnJudge(IDivaJudgeParams params) {
		CryLog("[OnJudge] Begin");

		//tech zone section update
		if (this->m_currSection != params.sType && m_currTechZone < this->m_techZoneNotes.size()) {
			IDivaJudgeTechZone * currTechZone = this->m_techZoneNotes.at(this->m_currTechZone);
			//if (this->m_currSection == eST_Norm && params.sType == eST_Tech) { //entering techzone from normal
			/*} else*/ if (this->m_currSection == eST_Tech && params.sType == eST_Norm) { //exiting techzone to normal
				if (currTechZone->active) m_completion += currTechZone->totalNotes;
				m_currTechZone++; //update the current tech zone to update
			//} else if (this->m_currSection == eST_Chance && params.sType == eST_Tech) { //entering techzone from chance
			} else if (this->m_currSection == eST_Tech && params.sType == eST_Chance) { //exiting techzone to chance
				if (currTechZone->active) m_completion += currTechZone->totalNotes;
				m_currTechZone++; //update the current tech zone to update
			}

			this->m_currSection = params.sType;
		}

		//tech zone scoring
		if (this->m_currSection == eST_Tech && m_currTechZone < this->m_techZoneNotes.size()) {
			IDivaJudgeTechZone * currTechZone = this->m_techZoneNotes.at(this->m_currTechZone); //get the current tech zone

			if (currTechZone->active) { //if it is active
				switch (params.hitscore) { //based on the hitscore
				case eHS_Cool:
					if (params.wrong) currTechZone->active = false; //if it was wrong, the zone is inactive
					else {
						currTechZone->currNotes--; //decrease the number of notes to hit
						if (currTechZone->currNotes <= 0) { //when we are out of notes to hit.
							currTechZone->currNotes = 0; //make sure the number of notes to hit is zero.
							currTechZone->active = false; //and that it is inactive.
							//score tech zone
							this->m_score += 0;
							//i have no idea how this is scored...
						}
					}
					break;
				case eHS_Fine:
					if (params.wrong) currTechZone->active = false;
					else {
						currTechZone->currNotes--; //decrease the number of notes to hit
						if (currTechZone->currNotes <= 0) { //when we are out of notes to hit.
							currTechZone->currNotes = 0; //make sure the number of notes to hit is zero.
							currTechZone->active = false; //and that it is inactive.
							//score tech zone
							this->m_score += 0;
							//i have no idea how this is scored...
						}
					}
					break;
				case eHS_Safe:
				case eHS_Sad:
				case eHS_Worst:
					currTechZone->active = false;
					break;
				}

				//if (this->m_listeners.size() > 0) for (IDivaJudgeListener * listeners : this->m_listeners) listeners->OnTechZoneScore(currTechZone->currNotes, currTechZone->active);
				//EBUS_EVENT(DivaJudgeEventBus, OnTechZoneScore, currTechZone->currNotes, currTechZone->active);
				EBUS_EVENT(DivaHudBus, SetTechZoneInfo, currTechZone->currNotes, currTechZone->active);
			}
		}

		switch (params.hitscore) {
		case eHS_Cool:
			if (params.wrong) {
				this->m_score += 250;
				m_numHits[eHS_Worst]++;
			} else {
				this->m_score += 500;
				m_numHits[eHS_Cool]++;
				m_completion++;
			}
			this->m_combo++;
			this->m_health++;
			if (params.sType == eST_Chance) this->m_ChanceMult++;
			break;
		case eHS_Fine:
			if (params.wrong) {
				this->m_score += 150;
				m_numHits[eHS_Worst]++;
			} else {
				this->m_score += 300;
				m_numHits[eHS_Fine]++;
				m_completion++;
			}
			this->m_combo++;
			this->m_health++;
			if (params.sType == eST_Chance) this->m_ChanceMult++;
			break;
		case eHS_Safe:
			if (params.wrong) {
				this->m_score += 50;
				m_numHits[eHS_Worst]++;
			} else {
				this->m_score += 100;
				m_numHits[eHS_Safe]++;
			}
			if (this->m_maxCombo < this->m_combo) this->m_maxCombo = this->m_combo;
			this->m_combo = 0;
			if (params.sType == eST_Chance) this->m_ChanceMult = 1;
			break;
		case eHS_Sad:
			if (params.wrong) {
				this->m_score += 25;
				m_numHits[eHS_Worst]++;
			} else {
				this->m_score += 50;
				m_numHits[eHS_Sad]++;
			}
			if (params.sType != eST_Chance) this->m_health--;
			if (this->m_maxCombo < this->m_combo) this->m_maxCombo = this->m_combo;
			this->m_combo = 0;
			if (params.sType == eST_Chance) this->m_ChanceMult = 1;
			break;
		case eHS_Worst:
			if (params.sType != eST_Chance) this->m_health--;
			if (this->m_maxCombo < this->m_combo) this->m_maxCombo = this->m_combo;
			this->m_combo = 0;
			if (params.sType == eST_Chance) this->m_ChanceMult = 1;
			m_numHits[eHS_Worst]++;
			break;
		}

		//break the combo
		if (params.wrong) this->m_combo = 0;

		//combo scoring
		if (this->m_combo >= 10) this->m_score += 50;
		if (this->m_combo >= 20) this->m_score += 100;
		if (this->m_combo >= 30) this->m_score += 150;
		if (this->m_combo >= 40) this->m_score += 200;
		if (this->m_combo >= 50) this->m_score += 250;

		//chancetime scoring
		if (params.sType == eST_Chance) {
			if (this->m_ChanceMult > 50) this->m_ChanceMult = 50;
			this->m_score += 100 * this->m_ChanceMult;
		}

		//hold scoring
		if (params.hold && params.holdRelease && params.holdMult > 0) {
			this->m_score += 10 * params.holdMult;
		}

		if (this->m_health <= 0) this->m_health = 0;
		if (this->m_health >= 200) this->m_health = 200;

		/*IDivaJudgeListenerParams jparams;

		jparams.score = this->m_score;
		jparams.health = this->m_health;
		jparams.hitScore = params.hitscore;
		jparams.wrong = params.wrong;
		jparams.completion = this->GetCompletion();
		jparams.numNotesComplete = this->m_completion;*/

		//EBUS_EVENT(DivaJudgeEventBus, OnJudgeEvent, jparams);

		//move this to IDivaJudge?
		EBUS_EVENT(DivaHudBus, SetHP, this->m_health);
		EBUS_EVENT(DivaHudBus, SetScore, this->m_score);
		EBUS_EVENT(DivaHudBus, SetCompletion, this->m_completion, this->m_totalNotes);
		EBUS_EVENT(DivaHudBus, SetRating, this->GetCompletion());
		EBUS_EVENT(DivaHudBus, SetHitScore, params.hitscore, params.wrong);

		//we are done with the note, update which note to judge
		if (params.hitscore != eHS_None && !params.hold) {
			EBUS_EVENT(DivaSequenceJudgeBus, UpdateNoteHit);
		} else if (params.hitscore != eHS_None && params.hold && params.holdRelease) {
			EBUS_EVENT(DivaSequenceJudgeBus, UpdateNoteHit);
		}
		CryLog("[OnJudge] End");
	}

	//WX_DISABLE_(4244)

	EDivaJudgeCompletion OpenDivaJudge::GetCompletion() {
		float completion_ratio = (((float)this->m_completion)/((float)this->m_totalNotes))*100;

		if (completion_ratio >= 100.0f) return eDJC_Perfect;
		else if (completion_ratio < 100.0f && completion_ratio >= 97.0f) return eDJC_Excellent;
		else if (completion_ratio < 97.0f && completion_ratio >= 95.0f) return eDJC_Great;
		else if (completion_ratio < 95.0f && completion_ratio >= 85.0f) return eDJC_Standard;

		return eDJC_Cheap;
	}

	unsigned int OpenDivaJudge::TechZoneNotesCount() {
		if (this->m_currSection != eST_Tech) return 0;
		return this->m_techZoneNotes.at(this->m_currTechZone)->currNotes;
	}

	bool OpenDivaJudge::IsTechZoneActive() {
		return this->m_currSection == eST_Tech && this->m_techZoneNotes.at(this->m_currTechZone)->active;
	}

	//WX_ENABLE_(4244)
}
