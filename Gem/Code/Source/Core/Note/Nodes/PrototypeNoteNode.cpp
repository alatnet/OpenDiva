#include <StdAfx.h>
#include "PrototypeNoteNode.h"

namespace OpenDiva {
	//////////////////////////////////////////////////////////////////////////
	IDraw2d * PrototypeNoteNode::iDraw2d = NULL;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Constructor/Deconstructor
	//////////////////////////////////////////////////////////////////////////
	PrototypeNoteNode::PrototypeNoteNode(const int id, ENoteType type, ResourceCollection * rc) :
		CAnimNode(id),
		m_pnImage(rc->p_NoteResource->getNote(type)),
		m_pTail(rc->p_TailResource->getCTail(type)),
		m_pHTail(rc->p_TailResource->getHTail(type)),
		//m_pER(rc->p_EffectResource),
		//m_pRR(rc->p_RatingResource),
		m_pFR(rc->p_FontResource),
		m_Pos(0, 0) {
		CryLog("PrototypeNoteNode Constructor Called.");
		if (PrototypeNoteNode::iDraw2d == NULL) PrototypeNoteNode::iDraw2d = Draw2dHelper::GetDraw2d();
		/*this->posTrack =*/ this->scaleTrack = this->timeArrowTrack = /*this->opacityTrack =*/ this->beizerTrack = NULL;

		this->bTime = this->startTime = this->endTime = this->timeInRange = 0.0f;

		this->rippleTrack = NULL;
	}

	PrototypeNoteNode::~PrototypeNoteNode() {
		CryLog("PrototypeNoteNode Deconstructor Called.");
	}
	//////////////////////////////////////////////////////////////////////////

	/*void PrototypeNoteNode::SetPos(float time, const Vec3 &pos) {
		if (this->posTrack != NULL) this->posTrack->SetValue(time, pos);
		}*/

	/*void PrototypeNoteNode::SetRot(float time, const float rot) {
		if (this->rotTrack != NULL) this->rotTrack->SetValue(time, rot);
		}*/

	/*void PrototypeNoteNode::SetScale(float time, const float scale) {
		if (this->scaleTrack != NULL) this->scaleTrack->SetValue(time, scale);
		}*/
	/*
	void PrototypeNoteNode::SetOpacity(float time, const float opacity) {
	if (this->opacityTrack != NULL) this->opacityTrack->SetValue(time, opacity);
	}*/

	void PrototypeNoteNode::CreateDefaultTracks() {
		/*this->posTrack = this->CreateTrackInternal(CAnimParamType(eAnimParamType_Position), EAnimCurveType::eAnimCurveType_BezierFloat, EAnimValue::eAnimValue_Vector);
		this->AddTrack(this->posTrack);
		this->posTrack->SetValue(0, Vec3(0, 0, 0), true);

		this->scaleTrack = this->CreateTrackInternal(CAnimParamType(eAnimParamType_Scale), EAnimCurveType::eAnimCurveType_BezierFloat, EAnimValue::eAnimValue_Vector);
		this->AddTrack(this->scaleTrack);
		this->scaleTrack->SetValue(0, Vec3(1, 1, 1), true);*/

		this->timeArrowTrack = this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_TCBFloat,
			EAnimValue::eAnimValue_Float
		);
		this->AddTrack(this->timeArrowTrack);
		this->timeArrowTrack->SetValue(0, 0.0f, true);

		this->scaleTrack = this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_BezierFloat,
			EAnimValue::eAnimValue_Float
		);
		this->AddTrack(this->scaleTrack);
		this->scaleTrack->SetValue(0, 1.0f, true);

		this->rippleTrack = this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Position),
			EAnimCurveType::eAnimCurveType_TCBVector,
			EAnimValue::eAnimValue_Vector
		);
		this->AddTrack(this->rippleTrack);
		this->rippleTrack->SetValue(0, { 0.0f, 0.0f, 1.0f }, true);

		this->beizerTrack = this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_TCBFloat,
			EAnimValue::eAnimValue_Float
		);
		this->AddTrack(this->beizerTrack);
		this->beizerTrack->SetValue(0, 0.0f, true);

		/*this->opacityTrack = this->CreateTrackInternal(CAnimParamType(eAnimParamType_Float), EAnimCurveType::eAnimCurveType_BezierFloat, EAnimValue::eAnimValue_Float);
		this->AddTrack(this->opacityTrack);
		this->opacityTrack->SetValue(0, 1.0f, true);*/
	}

	//////////////////////////////////////////////////////////////////////////
	//GENTELMEN! BEHOLD! OUR ANIMATION!
	//////////////////////////////////////////////////////////////////////////
	void PrototypeNoteNode::Animate(SAnimContext &ec) {
		//this->posTrack->GetValue(ec.time, m_Pos);
		this->scaleTrack->GetValue(ec.time, m_Scale);
		this->timeArrowTrack->GetValue(ec.time, m_Rot);
		//this->opacityTrack->GetValue(ec.time, m_Opacity);

		this->beizerTrack->GetValue(ec.time, this->bTime);

		this->m_Pos = Util::BCurve(this->bTime, this->bCurve[0], this->bCurve[1], this->bCurve[2]);

		this->timeInRange = ec.time;

		this->rippleTrack->GetValue(ec.time, ripple);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Render to Screen.
	//////////////////////////////////////////////////////////////////////////
	void PrototypeNoteNode::Render() {
		if (this->timeInRange <= this->startTime || this->timeInRange >= this->endTime) return;

		//startTime <= timeInRange <= endTime;

		std::shared_ptr<OD_Draw2d> draw2dptr = OD_Draw2d::GetPtr();

		draw2dptr->BeginDraw2d(1280, 720);
		//draw2dptr->BeginDraw2d(1920, 1080);
		//draw2dptr->BeginDraw2d(800, 600);

		//this->m_pER->Draw(eET_Ripple, this->bCurve[2], this->ripple.x, this->ripple.y, this->ripple.z);

		/*eET_Cross; //grows then shrinks.
		eET_Flare; //fades into a halo effect?
		eET_Loop;
		eET_Note; //or eET_NoNote;*/

		//endpoint:
		//scale of end at the beginning of note animation:
		//scale = 1 : time = start time - 0.5
		//scale = 1.25 : time = start time - 0.25
		//scale = 1 : time = start time
		this->m_pnImage->DrawNoteE(this->bCurve[2], this->m_Rot, this->m_Scale, 0.0f, 1.0f);

		//this->m_pTail->Draw(this->bCurve[0], this->bCurve[1], this->bCurve[2], this->bTime - 0.25, this->bTime, 20);
		CometTail::DrawRainbow(this->bCurve[0], this->bCurve[1], this->bCurve[2], this->bTime - 0.25f, this->bTime, 40);
		//this->m_pHTail->Draw(this->bCurve[0], /*this->bCurve[1],*/ this->bCurve[2], this->bTime - 0.25, this->bTime, 20);
		this->m_pnImage->DrawNote(this->m_Pos);

		//draw2dptr->DrawLineStrip(&line[0], line.size());


		/*draw2dptr->DrawLine(Vec2(1280, 0), Vec2(1280, 720), ColorF(0.0, 1.0f, 0.0f, 1.0f));
		draw2dptr->DrawLine(Vec2(0, 720), Vec2(1280, 720), ColorF(0.0, 1.0f, 0.0f, 1.0f));

		for (int i = 0; i < 3; i++) {
		draw2dptr->DrawLine(Vec2(this->bCurve[i].x - 10, this->bCurve[i].y), Vec2(this->bCurve[i].x + 10, this->bCurve[i].y), ColorF(1.0f, 0.0f, 0.0f, 1.0f));
		draw2dptr->DrawLine(Vec2(this->bCurve[i].x, this->bCurve[i].y - 10), Vec2(this->bCurve[i].x, this->bCurve[i].y + 10), ColorF(1.0f, 0.0f, 0.0f, 1.0f));
		}*/

		//this->iDraw2d->DrawLine(Vec2(1280, 0), Vec2(1280, 720), ColorF(0.0, 1.0f, 1.0f, 1.0f));
		//this->iDraw2d->DrawLine(Vec2(0, 720), Vec2(1280, 720), ColorF(0.0, 1.0f, 1.0f, 1.0f));

		for (int i = 0; i < 3; i++) {
			this->iDraw2d->DrawLine(Vec2(this->bCurve[i].x - 5, this->bCurve[i].y), Vec2(this->bCurve[i].x + 5, this->bCurve[i].y), ColorF(0.0f, 0.0f, 1.0f, 1.0f));
			this->iDraw2d->DrawLine(Vec2(this->bCurve[i].x, this->bCurve[i].y - 5), Vec2(this->bCurve[i].x, this->bCurve[i].y + 5), ColorF(0.0f, 0.0f, 1.0f, 1.0f));
		}
		draw2dptr->EndDraw2d();

		/*PrototypeNoteNode::iDraw2d->BeginDraw2d(false);
			this->iDraw2d->DrawLine(Vec2(1280, 0), Vec2(1280, 720), ColorF(0.0, 1.0f, 1.0f, 1.0f));
			this->iDraw2d->DrawLine(Vec2(0, 720), Vec2(1280, 720), ColorF(0.0, 1.0f, 1.0f, 1.0f));

			for (int i = 0; i < 3; i++) {
			this->iDraw2d->DrawLine(Vec2(this->bCurve[i].x - 5, this->bCurve[i].y), Vec2(this->bCurve[i].x + 5, this->bCurve[i].y), ColorF(0.0f, 0.0f, 1.0f, 1.0f));
			this->iDraw2d->DrawLine(Vec2(this->bCurve[i].x, this->bCurve[i].y - 5), Vec2(this->bCurve[i].x, this->bCurve[i].y + 5), ColorF(0.0f, 0.0f, 1.0f, 1.0f));
			}
			PrototypeNoteNode::iDraw2d->EndDraw2d();*/
	}
	//////////////////////////////////////////////////////////////////////////

	void PrototypeNoteNode::SetBCurve(Vec2 p1, Vec2 p2, Vec2 p3, float start, float end) {
		this->bCurve[0] = p1;
		this->bCurve[1] = p2;
		this->bCurve[2] = p3;

		//only for line strip to show path
		/*
		if (LINE_2)
			for (float i = 0.0f; i <= 1.0f; i += LINE_SEP) {
				Vec2 bc = Util::BCurve(i, p1, p2, p3);

				SVF_P3F_C4B_T2F temp;
				temp.xyz = Vec3(bc.x, bc.y, 1.0f);
				temp.color.dcolor = ColorF(1.0f, 1.0f, 1.0f, 1.0f).pack_argb8888();
				temp.st = Vec2(0.0f, 0.0f);
				line.push_back(temp);
				}
				else
				for (float i = 0.0f; i <= 1.0f + LINE_SEP; i += LINE_SEP) {
				Vec2 bc = Util::BCurve(i, p1, p2, p3);

				SVF_P3F_C4B_T2F temp;
				temp.xyz = Vec3(bc.x, bc.y, 1.0f);
				temp.color.dcolor = ColorF(1.0f, 1.0f, 1.0f, 1.0f).pack_argb8888();
				temp.st = Vec2(0.0f, 0.0f);
				line.push_back(temp);
			}

		CryLog("Num Points: %i", line.size());
		*/

		float tackon_time = ((end - start)*BIEZER_TACKON) + end;

		this->SetTimeRange(Range(start - 0.5f, tackon_time));

		this->beizerTrack->SetTimeRange(Range(start, tackon_time));
		this->beizerTrack->SetValue(start, 0.0f);
		this->beizerTrack->SetValue(tackon_time, 1.0f + BIEZER_TACKON);

		this->timeArrowTrack->SetValue(start, 0.0f);
		this->timeArrowTrack->SetValue(end, 360.0f);

		this->startTime = start - 0.50f;
		this->endTime = tackon_time;

		configureEffects(start, end);
	}

	void PrototypeNoteNode::configureEffects(float start, float end) {
		//note entrance
		this->scaleTrack->SetValue(start - 0.5f, 1.0f);
		this->scaleTrack->SetValue(start - 0.25f, 1.25f);
		this->scaleTrack->SetValue(start, 1.0f);

		//ripple
		/*unsigned int numRippleAnims = this->m_pER->getNumAnims(eET_Ripple);
		if (numRippleAnims > 0) {
			for (unsigned int i = 0; i < numRippleAnims; i++) {
				EffectAnim anim = this->m_pER->getAnimData(eET_Ripple, i);
				this->rippleTrack->SetValue(start + anim.time, anim.data);
			}
		}*/
	}
}