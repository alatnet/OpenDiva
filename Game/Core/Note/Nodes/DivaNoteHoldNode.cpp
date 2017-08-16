#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "DivaNoteBaseNode.h"
#include "DivaNoteHoldNode.h"

namespace LYGame {
	DivaNoteHoldNode::DivaNoteHoldNode(ResourceCollection *rc, NoteEntryHold* entry/*, NoteEntryBPM bpm*/) : DivaNoteBaseNode(rc), m_NoteEntry(entry) {
		m_ptimeArrowTrack2 = /*m_pNoteEScaleTrack2 = m_pRippleTrack2 =*/ nullptr;
		m_timeArrow2 = 0.0f;
		//m_noteScale2 = 1.0f;
		//m_Ripple2 = Vec3(0.0f,0.0f,0.0f);

		this->m_pBeizerTrack1 = this->m_pBeizerTrack2 = NULL;
		this->m_currPos1 = this->m_currPos2 = AZ::Vector2(0, 0);
		this->m_bTime1 = this->m_bTime2 = 0.0f;
		this->m_renderOut = false;
		this->m_holdMult = 0;
		this->m_holdTick = 0.0f;
		this->m_holdEnabled = true;
		this->m_faceHeld = false;
		this->m_arrowHeld = false;
		this->m_drawENote2 = false;
		this->m_hitScore = eHS_None;
		this->m_hitscoreWrong = false;
	}

	DivaNoteHoldNode::~DivaNoteHoldNode() {
		this->m_pENoteTracks2.clear();
		DivaNoteBaseNode::~DivaNoteBaseNode();
	}

	void DivaNoteHoldNode::Init(NoteEntryBPM bpm) {
		this->CreateDefaultTracks();

		float start_time1 = this->m_NoteEntry->hold1 - bpm.getDelay();
		float start_time2 = this->m_NoteEntry->hold2 - bpm.getDelay();

		float tackon_time1 = ((this->m_NoteEntry->hold1 - start_time1) * BIEZER_TACKON) + this->m_NoteEntry->hold1;
		float tackon_time2 = ((this->m_NoteEntry->hold2 - start_time2) * BIEZER_TACKON) + this->m_NoteEntry->hold2;

		this->m_posStart = Util::GetEdgePoint(
			this->m_NoteEntry->pos,
			this->m_NoteEntry->angle,
			0 - this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.GetX() - 2,
			1280 + this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.GetX() + 2,
			0 - this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.GetY() - 2,
			720 + this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.GetY() + 2
		);

		this->m_currPos1 = this->m_currPos2 = this->m_posStart;

		if (this->m_NoteEntry->pType == ePT_BCurve) {
			this->m_posCtrl = Util::PerpPointDist(
				this->m_posStart,
				this->m_NoteEntry->pos,
				this->m_NoteEntry->ctrlDist1,
				this->m_NoteEntry->ctrlDist2
				);
		}

		this->m_timeRange = Range(start_time1 - 0.5f, tackon_time2);
		//this->SetTimeRange(this->m_timeRange);

		this->m_pBeizerTrack1->SetTimeRange(Range(start_time1, tackon_time1));
		this->m_pBeizerTrack1->SetValue(start_time1, 0.0f);
		this->m_pBeizerTrack1->SetValue(tackon_time1, 1.0f + BIEZER_TACKON);

		this->m_pBeizerTrack2->SetTimeRange(Range(start_time2, tackon_time2));
		this->m_pBeizerTrack2->SetValue(start_time2, 0.0f);
		this->m_pBeizerTrack2->SetValue(tackon_time2, 1.0f + BIEZER_TACKON);

		this->ConfigureEffects(start_time1, start_time2, this->m_NoteEntry->hold1, this->m_NoteEntry->hold2);
	}

	void DivaNoteHoldNode::Reset() {
		this->m_holdEnabled = true;
		this->m_faceHeld = false;
		this->m_arrowHeld = false;
		this->m_renderOut = false;
		this->m_holdMult = 0;
		this->m_holdTick = 0.0f;
		this->m_drawENote2 = false;
		this->m_hitScore = eHS_None;
		this->m_hitscoreWrong = false;
	}

	void DivaNoteHoldNode::CreateDefaultTracks() {
		DivaNoteBaseNode::CreateDefaultTracks();

		IMovieSystem *iMovie = gEnv->pMovieSystem;

		this->m_ptimeArrowTrack2 = iMovie->CreateTrack(eAnimCurveType_TCBFloat); /*this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_TCBFloat,
			EAnimValue::eAnimValue_Float
			);
		this->AddTrack(this->m_ptimeArrowTrack2);*/
		this->m_ptimeArrowTrack2->SetValue(0, 0.0f, true);

		/*this->m_pNoteEScaleTrack2 = this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_BezierFloat,
			EAnimValue::eAnimValue_Float
			);
		this->AddTrack(this->m_pNoteEScaleTrack2);
		this->m_pNoteEScaleTrack2->SetValue(0, 1.0f, true);

		this->m_pRippleTrack2 = this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Position),
			EAnimCurveType::eAnimCurveType_TCBVector,
			EAnimValue::eAnimValue_Vector
			);
		this->AddTrack(this->m_pRippleTrack2);
		this->m_pRippleTrack2->SetValue(0, { 0.0f, 0.0f, 1.0f }, true);*/

		this->m_pBeizerTrack1 = iMovie->CreateTrack(eAnimCurveType_TCBFloat); /*this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_TCBFloat,
			EAnimValue::eAnimValue_Float
			);
		this->AddTrack(this->m_pBeizerTrack1);*/
		this->m_pBeizerTrack1->SetValue(0, 0.0f, true);

		this->m_pBeizerTrack2 = iMovie->CreateTrack(eAnimCurveType_TCBFloat); /*this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_TCBFloat,
			EAnimValue::eAnimValue_Float
			);
		this->AddTrack(this->m_pBeizerTrack2);*/
		this->m_pBeizerTrack2->SetValue(0, 0.0f, true);
	}


	void DivaNoteHoldNode::ConfigureEffects(float start1, float start2, float end1, float end2) {
		DivaNoteBaseNode::ConfigureEffects(start1, end1);

		//note time arrow
		this->m_ptimeArrowTrack2->SetValue(start2, 0.0f);
		this->m_ptimeArrowTrack2->SetValue(end2, 360.0f);

		//note entrance
		/*this->m_pNoteEScaleTrack2->SetValue(start2 - 0.5f, 1.0f);
		this->m_pNoteEScaleTrack2->SetValue(start2 - 0.25f, 1.25f);
		this->m_pNoteEScaleTrack2->SetValue(start2, 1.0f);

		//ripple
		unsigned int numRippleAnims = this->m_pRC->p_EffectResource->getNumAnims(eET_Ripple);
		if (numRippleAnims > 0) {
			for (unsigned int i = 0; i < numRippleAnims; i++) {
				EffectAnim anim = this->m_pRC->p_EffectResource->getAnimData(eET_Ripple, i);
				this->m_pRippleTrack2->SetValue(start2 + anim.time, anim.data);
			}
		}*/

		EffectDef * enote = this->m_pRC->p_EffectResource->getENoteEffect(); //get the enote animation

		if (enote->effects.size() > 0) { //if we have effects
			unsigned int numENoteEffects = enote->effects.size(); //get how many effects

			float starttime = start2;
			float endtime = end2;

			IMovieSystem *iMovie = gEnv->pMovieSystem;

			for (int i = 0; i < numENoteEffects; i++) { //for each effect
				ENoteTrack * track = new ENoteTrack(); //create a track for the effect

				EffectEntry entry = enote->effects.at(i); //get the effect entry

				track->img = entry.img; //assign the image

				//create the pos track
				track->posTrack = iMovie->CreateTrack(eAnimCurveType_TCBVector);/*this->CreateTrackInternal(
					CAnimParamType(eAnimParamType_Position),
					EAnimCurveType::eAnimCurveType_TCBVector,
					EAnimValue::eAnimValue_Vector
					);*/
					
				//create the sro track
				track->sroTrack = iMovie->CreateTrack(eAnimCurveType_TCBVector);/*this->CreateTrackInternal(
					CAnimParamType(eAnimParamType_Position),
					EAnimCurveType::eAnimCurveType_TCBVector,
					EAnimValue::eAnimValue_Vector
					);*/

				for (int aI = 0; aI < entry.anim.size(); aI++) { //for each animation
					EffectAnim anim = entry.anim.at(aI); //get the animation entry
					AZ::Vector3 pos = AZ::Vector3(anim.pos.GetX(), anim.pos.GetY(), 0.0f);
					track->posTrack->SetValue(start2 + anim.time, pos); //set the position based on time
					track->sroTrack->SetValue(start2 + anim.time, anim.sro); //set the sro based on time

					if (starttime > start2 + anim.time) starttime = start2 + anim.time;
					if (endtime < start2 + anim.time) endtime = start2 + anim.time;
				}

				m_pENoteTracks2.push_back(track); //pushback the track.
			}

			m_eNote2Range = Range(starttime, endtime);
		}
	}

	const bool DivaNoteHoldNode::NeedToRender() {
		return true;
	}

	const bool DivaNoteHoldNode::NeedToRender(float time) {
		return this->m_timeRange.IsInside(time);
	}

	void DivaNoteHoldNode::Animate(SAnimContext &ec) {
		DivaNoteBaseNode::Animate(ec);

		m_drawENote2 = m_eNote2Range.IsInside(ec.time);

		//this->m_pNoteEScaleTrack2->GetValue(ec.time, m_noteScale2);
		this->m_ptimeArrowTrack2->GetValue(ec.time, m_timeArrow2);
		//this->m_pRippleTrack2->GetValue(ec.time, m_Ripple2);

		if (this->m_pENoteTracks2.size() > 0) { //if we have effects
			for (ENoteTrack * track : this->m_pENoteTracks2) { //update each effect's position and sro.
				AZ::Vector3 pos;
				track->posTrack->GetValue(ec.time, pos);
				track->pos.Set(pos.GetX(), pos.GetY());
				track->sroTrack->GetValue(ec.time, track->sro);
			}
		}

		//if note is held, bTime1 is equal to 1.0f.
		if ((this->m_faceHeld || this->m_arrowHeld) && this->m_holdEnabled) {
			this->m_bTime1 = 1.0f;
		} else {
			this->m_pBeizerTrack1->GetValue(ec.time, this->m_bTime1);
		}
		this->m_pBeizerTrack2->GetValue(ec.time, this->m_bTime2);

		if (this->m_bTime1 == 1.0f + BIEZER_TACKON && this->m_holdEnabled) { //hold was missed
			this->m_holdEnabled = false; //kill hold

			IDivaJudgeParams params;
			params.hold = true;
			params.holdRelease = false;
			params.holdMult = 0;
			params.nType = this->m_NoteEntry->type;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			params.hitscore = eHS_Worst;
			params.hType = eNHT_Hold;

			//if (this->m_pDivaJudge != nullptr) this->m_pDivaJudge->DivaJudgeCallback(params);
			EBUS_EVENT(DivaJudgeBus, OnJudge, params);
			this->pushEffects(params);
		}

		//if hold is completed but not released
		//update judge and kill the hold
		if (this->m_bTime2 > 1.0f && this->m_holdEnabled) {
			this->m_holdEnabled = false;

			IDivaJudgeParams params;
			params.hold = true;
			params.holdRelease = true;
			params.holdMult = this->m_holdMult;
			params.nType = this->m_NoteEntry->type;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			params.hitscore = eHS_Worst;
			params.hType = eNHT_Hold;

			//if (this->m_pDivaJudge != nullptr) this->m_pDivaJudge->DivaJudgeCallback(params);
			EBUS_EVENT(DivaJudgeBus, OnJudge, params);
			this->pushEffects(params);
		}

		//if note is held, tick hold multiplier.
		if ((this->m_faceHeld || this->m_arrowHeld) && this->m_holdEnabled) {
			this->m_holdTick += ec.dt;
			if (this->m_holdTick >= 0.25 /*second*/) {
				this->m_holdTick = 0; //reset hold tick
				this->m_holdMult++; //up hold multiplier
			}
		}

		if (this->m_NoteEntry->pType == ePT_BCurve)
			this->m_currPos1 = Util::BCurve(this->m_bTime1, this->m_posStart, this->m_posCtrl, this->m_NoteEntry->pos);
		else
			this->m_currPos1 = Util::BCurve(this->m_bTime1, this->m_posStart, this->m_NoteEntry->pos);

		if (this->m_NoteEntry->pType == ePT_BCurve)
			this->m_currPos2 = Util::BCurve(this->m_bTime2, this->m_posStart, this->m_posCtrl, this->m_NoteEntry->pos);
		else
			this->m_currPos2 = Util::BCurve(this->m_bTime2, this->m_posStart, this->m_NoteEntry->pos);

		if (this->m_timeRange.IsInside(ec.time)) this->m_renderOut = true;
		else this->m_renderOut = false;
	}

	void DivaNoteHoldNode::Render() {
		if (this->m_renderOut && this->m_holdEnabled) {
			//render hold multiplier
			if (this->m_holdMult > 0) {
				float ypos = this->m_NoteEntry->pos.GetY() - this->m_pRC->p_FontResource->getMultiSize().GetY();

				if (m_hitscoreWrong) ypos -= (this->m_pRC->p_FontResource->getSize(MISSRATING(m_hitScore)).GetY());
				else  ypos -= (this->m_pRC->p_FontResource->getSize(m_hitScore).GetY());

				AZ::Vector2 pos = AZ::Vector2(this->m_NoteEntry->pos.GetX(), ypos);
				this->m_pRC->p_FontResource->DrawMult(pos, 10 * this->m_holdMult);
			}
				
			//hide first end note after button press
			if (!this->m_faceHeld && !this->m_arrowHeld) DivaNoteBaseNode::DrawNoteE(this->m_NoteEntry->pos, this->m_NoteEntry->type);
			//show second end note when ready
			if (m_drawENote2) this->DrawNoteE2(this->m_NoteEntry->pos, this->m_NoteEntry->type);

			if (this->m_NoteEntry->pType == ePT_BCurve)
				this->m_pRC->p_TailResource->getHTail(this->m_NoteEntry->type)->Draw(this->m_posStart,this->m_posCtrl, this->m_NoteEntry->pos,this->m_bTime2, this->m_bTime1,20);
			else
				this->m_pRC->p_TailResource->getHTail(this->m_NoteEntry->type)->Draw(this->m_posStart, this->m_NoteEntry->pos, this->m_bTime2, this->m_bTime1, 20);

			this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->DrawNote(this->m_currPos2);
			this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->DrawNote(this->m_currPos1);
		}
	}

	//                 notetype, held to check, held to ignore
	#define CheckRange(noteType,held,held2) \
		if (mode == eIET_Pressed) { \
			if (!this->m_##held2##Held && !this->m_##held##Held && this->m_holdEnabled) { \
				IDivaJudgeParams params; \
				params.hold = true; \
				params.nType = this->m_NoteEntry->type; \
				params.sType = this->m_NoteEntry->sType; \
				params.notePos = this->m_NoteEntry->pos; \
				params.hType = eNHT_Hold; \
				DivaJudgeBus::BroadcastResult(params.hitscore, &DivaJudgeBus::Events::GetHitScore, this->m_bTime1, eHS_None); \
				if (params.hitscore != eHS_None) { \
					if (!(this->m_NoteEntry->type == noteType)) { \
						params.wrong = true; \
						m_hitscoreWrong = true; \
						this->m_holdEnabled = false; \
					} \
					this->m_##held##Held = true; \
					this->m_hitScore = params.hitscore; \
					EBUS_EVENT(DivaJudgeBus, OnJudge, params); \
					this->pushEffects(params); \
				} \
			} \
		} else if (mode == eIET_Released) { \
			if (this->m_##held##Held && this->m_holdEnabled && this->m_NoteEntry->type == noteType) { \
				this->m_##held##Held = false; \
				IDivaJudgeParams params; \
				params.hold = true; \
				params.holdRelease = true; \
				params.nType = this->m_NoteEntry->type; \
				params.sType = this->m_NoteEntry->sType; \
				params.notePos = this->m_NoteEntry->pos; \
				params.holdMult = this->m_holdMult; \
				params.hType = eNHT_Hold; \
				DivaJudgeBus::BroadcastResult(params.hitscore, &DivaJudgeBus::Events::GetHitScore, this->m_bTime2, eHS_Worst); \
				EBUS_EVENT(DivaJudgeBus, OnJudge, params); \
				this->pushEffects(params); \
				this->m_holdEnabled = false; \
			} \
		}

	//Input
	void DivaNoteHoldNode::OnCross(LyInputEventType mode, float value) {
		CheckRange(eNT_Cross, face, arrow)
		/*if (this->m_pDivaJudge != nullptr) {
			if (mode == eAAM_OnPress) {
				if (!this->m_arrowHeld && !this->m_faceHeld && this->m_holdEnabled) {
					IDivaJudgeParams params;
					params.hold = true;
					params.nType = this->m_NoteEntry->type;
					params.sType = this->m_NoteEntry->sType;
					params.notePos = this->m_NoteEntry->pos;
					params.hType = eNHT_Hold;

					if (this->m_bTime1 > m_pDivaJudge->m_Range_CoolF) params.hitscore = eHS_Worst;
					else if (m_pDivaJudge->m_Range_Cool.IsInside(this->m_bTime1)) params.hitscore = eHS_Cool;
					else if (m_pDivaJudge->m_Range_Fine.IsInside(this->m_bTime1)) params.hitscore = eHS_Fine;
					else if (m_pDivaJudge->m_Range_Safe.IsInside(this->m_bTime1)) params.hitscore = eHS_Safe;
					else if (m_pDivaJudge->m_Range_Sad.IsInside(this->m_bTime1)) params.hitscore = eHS_Sad;
					else if (m_pDivaJudge->m_Range_Worst.IsInside(this->m_bTime1)) params.hitscore = eHS_Worst;

					this->m_hitScore = params.hitscore;

					if (params.hitscore != eHS_None) {
						if (!(this->m_NoteEntry->type == eNT_Cross)) {
							params.wrong = true;
							m_hitscoreWrong = true;
							this->m_holdEnabled = false; //kill hold
						}
						this->m_faceHeld = true;
						this->m_hitScore = params.hitscore;
						this->m_pDivaJudge->DivaJudgeCallback(params);
					}
				}
			} else if (mode == eAAM_OnRelease) {
				if (this->m_faceHeld && this->m_holdEnabled) {
					this->m_faceHeld = false;
					IDivaJudgeParams params;
					params.hold = true;
					params.holdRelease = true;
					params.nType = this->m_NoteEntry->type;
					params.sType = this->m_NoteEntry->sType;
					params.notePos = this->m_NoteEntry->pos;
					params.holdMult = this->m_holdMult;
					params.hType = eNHT_Hold;
					params.hitscore = eHS_Worst;

					if (this->m_bTime2 > m_pDivaJudge->m_Range_CoolF) params.hitscore = eHS_Worst;
					else if (m_pDivaJudge->m_Range_Cool.IsInside(this->m_bTime2)) params.hitscore = eHS_Cool;
					else if (m_pDivaJudge->m_Range_Fine.IsInside(this->m_bTime2)) params.hitscore = eHS_Fine;
					else if (m_pDivaJudge->m_Range_Safe.IsInside(this->m_bTime2)) params.hitscore = eHS_Safe;
					else if (m_pDivaJudge->m_Range_Sad.IsInside(this->m_bTime2)) params.hitscore = eHS_Sad;
					else if (m_pDivaJudge->m_Range_Worst.IsInside(this->m_bTime2)) params.hitscore = eHS_Worst;

					this->m_pDivaJudge->DivaJudgeCallback(params);

					this->m_holdEnabled = false; //kill hold
				}
			}
		}*/
	}

	void DivaNoteHoldNode::OnCircle(LyInputEventType mode, float value) {
		CheckRange(eNT_Circle, face, arrow)
	}

	void DivaNoteHoldNode::OnSquare(LyInputEventType mode, float value) {
		CheckRange(eNT_Square, face, arrow)
	}

	void DivaNoteHoldNode::OnTriangle(LyInputEventType mode, float value) {
		CheckRange(eNT_Triangle, face, arrow)
	}

	//fix this shit?
	void DivaNoteHoldNode::OnLeft(LyInputEventType mode, float value) {
		CheckRange(eNT_Square, arrow, face)
	}

	void DivaNoteHoldNode::OnRight(LyInputEventType mode, float value) {
		CheckRange(eNT_Circle, arrow, face)
	}

	void DivaNoteHoldNode::OnUp(LyInputEventType mode, float value) {
		CheckRange(eNT_Triangle, arrow, face)
	}

	void DivaNoteHoldNode::OnDown(LyInputEventType mode, float value) {
		CheckRange(eNT_Cross, arrow, face)
	}

	void DivaNoteHoldNode::OnSwipeL(LyInputEventType mode, float value) {
		if (!this->m_faceHeld && !this->m_arrowHeld && this->m_holdEnabled) {
			IDivaJudgeParams params;
			params.hold = true;
			params.holdRelease = true;
			params.nType = this->m_NoteEntry->type;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			params.holdMult = this->m_holdMult;
			params.hType = eNHT_Hold;
			params.hitscore = eHS_None;
			params.wrong = true;

			/*float coolF, worstF;
			DivaJudgeEventBus::BroadcastResult(coolF, &DivaJudgeEventBus::Events::GetCoolFRange);
			DivaJudgeEventBus::BroadcastResult(worstF, &DivaJudgeEventBus::Events::GetWorstFRange);

			if (this->m_bTime1 >= coolF) params.hitscore = eHS_Worst;
			if (this->m_bTime1 <= coolF && this->m_bTime1 > worstF) params.hitscore = eHS_Worst;*/

			DivaJudgeBus::BroadcastResult(params.hitscore, &DivaJudgeBus::Events::GetWorstRange, this->m_bTime1, eHS_None);

			if (params.hitscore != eHS_None) {
				EBUS_EVENT(DivaJudgeBus, OnJudge, params);
				this->pushEffects(params);

				this->m_holdEnabled = false; //kill hold
			}
		}
	}

	void DivaNoteHoldNode::OnSwipeR(LyInputEventType mode, float value) {
		if (!this->m_faceHeld && !this->m_arrowHeld && this->m_holdEnabled) {
			IDivaJudgeParams params;
			params.hold = true;
			params.holdRelease = true;
			params.nType = this->m_NoteEntry->type;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			params.holdMult = this->m_holdMult;
			params.hType = eNHT_Hold;
			params.hitscore = eHS_None;
			params.wrong = true;

			/*float coolF, worstF;
			DivaJudgeEventBus::BroadcastResult(coolF, &DivaJudgeEventBus::Events::GetCoolFRange);
			DivaJudgeEventBus::BroadcastResult(worstF, &DivaJudgeEventBus::Events::GetWorstFRange);

			if (this->m_bTime1 >= coolF) params.hitscore = eHS_Worst;
			if (this->m_bTime1 <= coolF && this->m_bTime1 > worstF) params.hitscore = eHS_Worst;*/

			DivaJudgeBus::BroadcastResult(params.hitscore, &DivaJudgeBus::Events::GetWorstRange, this->m_bTime1, eHS_None);

			if (params.hitscore != eHS_None) {
				EBUS_EVENT(DivaJudgeBus, OnJudge, params);
				this->pushEffects(params);

				this->m_holdEnabled = false; //kill hold
			}
		}
	}

	void DivaNoteHoldNode::OnStar(LyInputEventType mode, float value) {
		if (!this->m_faceHeld && !this->m_arrowHeld && this->m_holdEnabled) {
			IDivaJudgeParams params;
			params.hold = true;
			params.holdRelease = true;
			params.nType = this->m_NoteEntry->type;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			params.holdMult = this->m_holdMult;
			params.hType = eNHT_Hold;
			params.hitscore = eHS_None;
			params.wrong = true;

			/*float coolF, worstF;
			DivaJudgeEventBus::BroadcastResult(coolF, &DivaJudgeEventBus::Events::GetCoolFRange);
			DivaJudgeEventBus::BroadcastResult(worstF, &DivaJudgeEventBus::Events::GetWorstFRange);

			if (this->m_bTime1 >= coolF) params.hitscore = eHS_Worst;
			if (this->m_bTime1 <= coolF && this->m_bTime1 > worstF) params.hitscore = eHS_Worst;*/

			DivaJudgeBus::BroadcastResult(params.hitscore, &DivaJudgeBus::Events::GetWorstRange, this->m_bTime1, eHS_None);

			if (params.hitscore != eHS_None) {
				EBUS_EVENT(DivaJudgeBus, OnJudge, params);
				this->pushEffects(params);

				this->m_holdEnabled = false; //kill hold
			}
		}
	}

	#undef CheckRange

	void DivaNoteHoldNode::DrawNoteE2(AZ::Vector2 pos, ENoteType type) {
		if (this->m_pENoteTracks2.size() > 0) { //if we have effects
			for (ENoteTrack * track : this->m_pENoteTracks2) { //for each effect
				if (track->img == nullptr) //we are the enote data
					this->m_pRC->p_NoteResource->getNote(type)->DrawNoteE(pos + track->pos, this->m_timeArrow2, track->sro.GetX(), track->sro.GetY(), track->sro.GetZ()); //draw the enote
				else //we are an effect
					track->img->Draw(pos + track->pos, track->sro.GetX(), track->sro.GetY(), track->sro.GetZ()); //draw the effect
			}
		}
		else { //fallback for no effects
			this->m_pRC->p_NoteResource->getNote(type)->DrawNoteE(pos, this->m_timeArrow2, 1.0f, 0.0f, 1.0f); //draw the enote
		}
	}



	void DivaNoteHoldNode::GetMemoryUsage(ICrySizer *pSizer) const {
		DivaNoteBaseNode::GetMemoryUsage(pSizer);
		pSizer->AddObject(m_pBeizerTrack1);
		pSizer->AddObject(m_pBeizerTrack2);
		pSizer->AddObject(m_ptimeArrowTrack2);
		//pSizer->AddObject(m_pENoteTracks2);
		for (ENoteTrack * t : this->m_pENoteTracks2) pSizer->AddObject(t);
	}
}