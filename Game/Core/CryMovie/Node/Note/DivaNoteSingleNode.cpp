#include <StdAfx.h>
#include "DivaNoteBaseNode.h"
#include "DivaNoteSingleNode.h"

//TODO:
//-Ratings
//-Hit Effects
//-Hit Timings
//-Diva Node callback

namespace LYGame {
	DivaNoteSingleNode::DivaNoteSingleNode(ResourceCollection *rc, IDivaJudge *divaJudge, NoteEntrySingle* entry/*, NoteEntryBPM bpm*/) : DivaNoteBaseNode(rc, divaJudge), m_NoteEntry(entry) {
		this->m_pBeizerTrack = NULL;
		this->m_currPos = Vec2(0, 0);
		this->m_bTime = 0.0f;
		this->m_renderOut = false;
		this->m_noteHit = false;
	}

	void DivaNoteSingleNode::Init(NoteEntryBPM bpm) {
		this->CreateDefaultTracks();
		float start_time = this->m_NoteEntry->time - bpm.getDelay();

		float tackon_time = ((this->m_NoteEntry->time - start_time) * BIEZER_TACKON) + this->m_NoteEntry->time;

		//float end_time = effect_time - tackon_time;

		//static Vec2 GetEdgePoint(Vec2 point, float angle, float x_min, float x_max, float y_min, float y_max);
		//static Vec2 PerpPointDist(Vec2 start, Vec2 end, float dist1 = 0.5f, float dist2 = 1.0f);

		this->m_posStart = Util::GetEdgePoint(
			this->m_NoteEntry->pos,
			this->m_NoteEntry->angle,
			0 - this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.x - 2,
			1280 + this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.x + 2,
			0 - this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.y - 2,
			720 + this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->getSizes().note.y + 2
			);

		this->m_currPos = this->m_posStart;

		if (this->m_NoteEntry->pType == ePT_BCurve) {
			this->m_posCtrl = Util::PerpPointDist(
				this->m_posStart,
				this->m_NoteEntry->pos,
				this->m_NoteEntry->ctrlDist1,
				this->m_NoteEntry->ctrlDist2
				);
		}

		this->m_timeRange = Range(start_time - 0.5f, tackon_time);
		//this->SetTimeRange(this->m_timeRange);

		this->m_pBeizerTrack->SetTimeRange(Range(start_time, tackon_time));
		this->m_pBeizerTrack->SetValue(start_time, 0.0f);
		this->m_pBeizerTrack->SetValue(tackon_time, 1.0f + BIEZER_TACKON);

		DivaNoteBaseNode::ConfigureEffects(start_time, this->m_NoteEntry->time);
	}

	void DivaNoteSingleNode::Reset() {
		this->m_renderOut = false;
		this->m_noteHit = false;
	}

	void DivaNoteSingleNode::CreateDefaultTracks() {
		DivaNoteBaseNode::CreateDefaultTracks();

		this->m_pBeizerTrack = gEnv->pMovieSystem->CreateTrack(EAnimCurveType::eAnimCurveType_TCBFloat); /* this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_TCBFloat,
			EAnimValue::eAnimValue_Float
			);*/
		//this->AddTrack(this->m_pBeizerTrack);
		this->m_pBeizerTrack->SetValue(0, 0.0f, true);
	}

	const bool DivaNoteSingleNode::NeedToRender(float time) {
		return this->m_timeRange.IsInside(time);
	}
	
	void DivaNoteSingleNode::Animate(SAnimContext &ec) {
		//animate base node
		DivaNoteBaseNode::Animate(ec);

		//get our time between 0 and 1.0f + BIEZER_TACKON
		this->m_pBeizerTrack->GetValue(ec.time, this->m_bTime);

		//get our note position along the path
		if (this->m_NoteEntry->pType == ePT_BCurve)
			this->m_currPos = Util::BCurve(this->m_bTime, this->m_posStart, this->m_posCtrl, this->m_NoteEntry->pos);
		else
			this->m_currPos = Util::BCurve(this->m_bTime, this->m_posStart, this->m_NoteEntry->pos);

		//are we suppose to render?
		if (this->m_timeRange.IsInside(ec.time)) this->m_renderOut = true;
		else this->m_renderOut = false;

		//hit was missed
		//notify the judge
		//needs fix...
		if (this->m_bTime >= 1.0f + BIEZER_TACKON && !this->m_noteHit) {
			//update judge
			IDivaJudgeParams params;
			params.nType = this->m_NoteEntry->type;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			params.hitscore = eHS_Worst;
			params.hType = eNHT_Single;

			if (this->m_pDivaJudge != nullptr) this->m_pDivaJudge->DivaJudgeCallback(params);
			this->pushEffects(params);
			this->m_noteHit = true;
		}
	}

	void DivaNoteSingleNode::Render() {
		if (this->m_renderOut && !this->m_noteHit) {
			DivaNoteBaseNode::DrawNoteE(this->m_NoteEntry->pos, this->m_NoteEntry->type);

			if (this->m_NoteEntry->sType == eST_Chance || this->m_NoteEntry->type == eNT_BigStar) { //render rainbow if we are a chancetime node or a big star
				if (this->m_NoteEntry->pType == ePT_BCurve)
					CometTail::DrawRainbow(this->m_posStart, this->m_posCtrl, this->m_NoteEntry->pos, this->m_bTime - 0.25f, this->m_bTime, 20);
				else
					CometTail::DrawRainbow(this->m_posStart, this->m_NoteEntry->pos, this->m_bTime - 0.25f, this->m_bTime, 20);
			} else { //render comet tail
				if (this->m_NoteEntry->pType == ePT_BCurve)
					this->m_pRC->p_TailResource->getCTail(this->m_NoteEntry->type)->Draw(this->m_posStart, this->m_posCtrl, this->m_NoteEntry->pos, this->m_bTime - 0.25f, this->m_bTime, 20);
				else
					this->m_pRC->p_TailResource->getCTail(this->m_NoteEntry->type)->Draw(this->m_posStart, this->m_NoteEntry->pos, this->m_bTime - 0.25f, this->m_bTime, 20);
			}

			//draw the all mighty note!
			this->m_pRC->p_NoteResource->getNote(this->m_NoteEntry->type)->DrawNote(this->m_currPos);
		}
	}

	#define CheckRange(input) \
	if (mode == eAAM_OnPress && !this->m_noteHit) { \
		if (this->m_pDivaJudge != nullptr) { \
			IDivaJudgeParams params; \
			params.nType = this->m_NoteEntry->type; \
			params.sType = this->m_NoteEntry->sType; \
			params.notePos = this->m_NoteEntry->pos; \
			params.hType = eNHT_Single; \
			if (this->m_bTime > m_pDivaJudge->m_Range_CoolF) params.hitscore = eHS_Worst; \
			else if (m_pDivaJudge->m_Range_Cool.IsInside(this->m_bTime)) params.hitscore = eHS_Cool; \
			else if (m_pDivaJudge->m_Range_Fine.IsInside(this->m_bTime)) params.hitscore = eHS_Fine; \
			else if (m_pDivaJudge->m_Range_Safe.IsInside(this->m_bTime)) params.hitscore = eHS_Safe; \
			else if (m_pDivaJudge->m_Range_Sad.IsInside(this->m_bTime)) params.hitscore = eHS_Sad; \
			else if (m_pDivaJudge->m_Range_Worst.IsInside(this->m_bTime)) params.hitscore = eHS_Worst; \
			if (params.hitscore != eHS_None) { \
				if (!(this->m_NoteEntry->type == input##)) params.wrong = true; \
				this->m_pDivaJudge->DivaJudgeCallback(params); \
				this->pushEffects(params); \
				this->m_noteHit=true; \
			} \
		} \
	}

	//Input
	void DivaNoteSingleNode::OnCross(int mode, float value) {
		CheckRange(eNT_Cross)

		/*IDivaNodeCallbackParams params;
		params.sType = this->m_NoteEntry->sType;
		params.notePos = this->m_NoteEntry->pos;
		if (this->m_bTime > DivaNodeShared::sm_Range_CoolF) params.hitscore = eHS_Worst;
		else if (DivaNodeShared::sm_Range_Cool.IsInside(this->m_bTime)) params.hitscore = eHS_Cool;
		else if (DivaNodeShared::sm_Range_Fine.IsInside(this->m_bTime)) params.hitscore = eHS_Fine;
		else if (DivaNodeShared::sm_Range_Safe.IsInside(this->m_bTime)) params.hitscore = eHS_Safe;
		else if (DivaNodeShared::sm_Range_Sad.IsInside(this->m_bTime)) params.hitscore = eHS_Sad;
		else if (DivaNodeShared::sm_Range_Worst.IsInside(this->m_bTime)) params.hitscore = eHS_Worst;
		if (params.hitscore != eHS_None) {
			if (!(this->m_NoteEntry->type == eNT_Cross)) params.wrong = true;
			this->m_pDivaNodeCallback->DivaNodeCallback(params);
		}*/
	}

	void DivaNoteSingleNode::OnCircle(int mode, float value) {
		CheckRange(eNT_Circle)
	}

	void DivaNoteSingleNode::OnSquare(int mode, float value) {
		CheckRange(eNT_Square)
	}

	void DivaNoteSingleNode::OnTriangle(int mode, float value) {
		CheckRange(eNT_Triangle)
	}

	void DivaNoteSingleNode::OnLeft(int mode, float value) {
		this->OnSquare(mode, value);
	}

	void DivaNoteSingleNode::OnRight(int mode, float value) {
		this->OnCircle(mode, value);
	}

	void DivaNoteSingleNode::OnUp(int mode, float value) {
		this->OnTriangle(mode, value);
	}

	void DivaNoteSingleNode::OnDown(int mode, float value) {
		this->OnCross(mode, value);
	}

	void DivaNoteSingleNode::OnSwipeL(int mode, float value) {
		CheckRange(eNT_SwipeL)
		/*if (mode == eAAM_OnPress) {
			IDivaJudgeCallbackParams params;
			params.nType = eNT_SwipeL;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			if (this->m_bTime > DivaJudgeShared::sm_Range_CoolF) params.hitscore = eHS_Worst;
			else if (DivaJudgeShared::sm_Range_Cool.IsInside(this->m_bTime)) params.hitscore = eHS_Cool;
			else if (DivaJudgeShared::sm_Range_Fine.IsInside(this->m_bTime)) params.hitscore = eHS_Fine;
			else if (DivaJudgeShared::sm_Range_Safe.IsInside(this->m_bTime)) params.hitscore = eHS_Safe;
			else if (DivaJudgeShared::sm_Range_Sad.IsInside(this->m_bTime)) params.hitscore = eHS_Sad;
			else if (DivaJudgeShared::sm_Range_Worst.IsInside(this->m_bTime)) params.hitscore = eHS_Worst;
			params.wrong = true;
			if (params.hitscore != eHS_None) this->m_pDivaJudgeCallback->DivaJudgeCallback(params);
		}*/
}

	void DivaNoteSingleNode::OnSwipeR(int mode, float value) {
		CheckRange(eNT_SwipeR)
		/*if (mode == eAAM_OnPress) {
			IDivaJudgeCallbackParams params;
			params.nType = eNT_SwipeR;
			params.sType = this->m_NoteEntry->sType;
			params.notePos = this->m_NoteEntry->pos;
			if (this->m_bTime > DivaJudgeShared::sm_Range_CoolF) params.hitscore = eHS_Worst;
			else if (DivaJudgeShared::sm_Range_Cool.IsInside(this->m_bTime)) params.hitscore = eHS_Cool;
			else if (DivaJudgeShared::sm_Range_Fine.IsInside(this->m_bTime)) params.hitscore = eHS_Fine;
			else if (DivaJudgeShared::sm_Range_Safe.IsInside(this->m_bTime)) params.hitscore = eHS_Safe;
			else if (DivaJudgeShared::sm_Range_Sad.IsInside(this->m_bTime)) params.hitscore = eHS_Sad;
			else if (DivaJudgeShared::sm_Range_Worst.IsInside(this->m_bTime)) params.hitscore = eHS_Worst;
			params.wrong = true;
			if (params.hitscore != eHS_None) this->m_pDivaJudgeCallback->DivaJudgeCallback(params);
		}*/
	}

	void DivaNoteSingleNode::OnStar(int mode, float value) {
		//CheckRange(eNT_Star)
		if (mode == eAAM_OnPress && !this->m_noteHit) {
			if (this->m_pDivaJudge != nullptr) {
				IDivaJudgeParams params;
				params.nType = this->m_NoteEntry->type;
				params.sType = this->m_NoteEntry->sType;
				params.notePos = this->m_NoteEntry->pos;
				params.hType = eNHT_Single; \
				if (this->m_bTime > m_pDivaJudge->m_Range_CoolF) params.hitscore = eHS_Worst;
				else if (m_pDivaJudge->m_Range_Cool.IsInside(this->m_bTime)) params.hitscore = eHS_Cool;
				else if (m_pDivaJudge->m_Range_Fine.IsInside(this->m_bTime)) params.hitscore = eHS_Fine;
				else if (m_pDivaJudge->m_Range_Safe.IsInside(this->m_bTime)) params.hitscore = eHS_Safe;
				else if (m_pDivaJudge->m_Range_Sad.IsInside(this->m_bTime)) params.hitscore = eHS_Sad;
				else if (m_pDivaJudge->m_Range_Worst.IsInside(this->m_bTime)) params.hitscore = eHS_Worst;
				if (params.hitscore != eHS_None) {
					if (!(this->m_NoteEntry->type == eNT_Star)) params.wrong = true;
					else if (!(this->m_NoteEntry->type == eNT_BigStar)) params.wrong = true;
					this->m_pDivaJudge->DivaJudgeCallback(params);
					this->pushEffects(params);
					this->m_noteHit = true;
				}
			}
		}
	}

	void DivaNoteSingleNode::GetMemoryUsage(ICrySizer *pSizer) const {
		DivaNoteBaseNode::GetMemoryUsage(pSizer);
		pSizer->AddObject(m_pBeizerTrack);
	}

	#undef CheckRange
}