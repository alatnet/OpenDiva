#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "DivaNoteBaseNode.h"

namespace OpenDiva {
	//IDraw2d * DivaNoteBaseNode::iDraw2d = NULL;

	DivaNoteBaseNode::DivaNoteBaseNode(ResourceCollection *rc) : m_pRC(rc) {
		//if (DivaNoteBaseNode::iDraw2d == NULL) DivaNoteBaseNode::iDraw2d = Draw2dHelper::GetDraw2d();

		m_ptimeArrowTrack = /*m_pNoteEScaleTrack = m_pRippleTrack =*/ NULL;
	}

	DivaNoteBaseNode::~DivaNoteBaseNode() {
		this->m_ptimeArrowTrack->release();

		for (ENoteTrack * track : this->m_pENoteTracks) {
			track->posTrack->release();
			track->sroTrack->release();
		}
		this->m_pENoteTracks.clear();
	}

	void DivaNoteBaseNode::CreateDefaultTracks() {
		this->m_ptimeArrowTrack = IMovieExt::CreateTrack(eAnimCurveType_TCBFloat);
		//this->m_ptimeArrowTrack = gEnv->pMovieSystem->CreateTrack(eAnimCurveType_TCBFloat);
		/*this->CreateTrackInternal(
			CAnimParamType(eAnimParamType_Float),
			EAnimCurveType::eAnimCurveType_TCBFloat,
			EAnimValue::eAnimValue_Float
			);*/
		//this->AddTrack(this->m_ptimeArrowTrack);
		this->m_ptimeArrowTrack->SetValue(0, 0.0f, true);
	}

	//convert to using effect resource v2
	void DivaNoteBaseNode::ConfigureEffects(float start, float end) {
		//note time arrow
		this->m_ptimeArrowTrack->SetValue(start, 0.0f);
		this->m_ptimeArrowTrack->SetValue(end, 360.0f);

		EffectDef * enote = this->m_pRC->p_EffectResource->getENoteEffect(); //get the enote animation

		IMovieSystem* iMovie = gEnv->pMovieSystem;

		if (enote->effects.size() > 0) { //if we have effects
			unsigned int numENoteEffects = enote->effects.size(); //get how many effects

			for (int i = 0; i < numENoteEffects; i++) { //for each effect
				ENoteTrack * track = new ENoteTrack(); //create a track for the effect

				EffectEntry entry = enote->effects.at(i); //get the effect entry

				track->img = entry.img; //assign the image

				//create the pos track
				track->posTrack = IMovieExt::CreateTrack(EAnimParamType::eAnimParamType_Position);
				//track->posTrack = iMovie->CreateTrack(eAnimCurveType_TCBVector);
				/*this->CreateTrackInternal(
					CAnimParamType(eAnimParamType_Position),
					EAnimCurveType::eAnimCurveType_TCBVector,
					EAnimValue::eAnimValue_Vector
					);*/

				//create the sro track
				track->sroTrack = IMovieExt::CreateTrack(EAnimParamType::eAnimParamType_Position);
				//track->sroTrack = iMovie->CreateTrack(eAnimCurveType_TCBVector);
				/*this->CreateTrackInternal(
					CAnimParamType(eAnimParamType_Position),
					EAnimCurveType::eAnimCurveType_TCBVector,
					EAnimValue::eAnimValue_Vector
					);*/

				for (int aI = 0; aI < entry.anim.size(); aI++) { //for each animation
					EffectAnim anim = entry.anim.at(aI); //get the animation entry
					AZ::Vector3 pos(anim.pos.GetX(), anim.pos.GetY(), 0.0f);
					track->posTrack->SetValue(start + anim.time, pos); //set the position based on time
					track->sroTrack->SetValue(start + anim.time, anim.sro); //set the sro based on time
				}

				m_pENoteTracks.push_back(track); //pushback the track.
			}
		}
	}

	void DivaNoteBaseNode::Animate(SAnimContext &ec) {
		//this->m_pNoteEScaleTrack->GetValue(ec.time, m_noteScale);
		this->m_ptimeArrowTrack->GetValue(ec.time, m_timeArrow); //update time arrow rotation
		//this->m_pRippleTrack->GetValue(ec.time, m_Ripple);

		if (this->m_pENoteTracks.size() > 0) { //if we have effects
			for (ENoteTrack * track : this->m_pENoteTracks) { //update each effect's position and sro.
				AZ::Vector3 pos;
				track->posTrack->GetValue(ec.time, pos);
				track->pos.Set(pos.GetX(), pos.GetY());
				track->sroTrack->GetValue(ec.time, track->sro);
			}
		}
	}

	void DivaNoteBaseNode::DrawNoteE(AZ::Vector2 pos, ENoteType type) {
		if (this->m_pENoteTracks.size() > 0) { //if we have effects
			for (ENoteTrack * track : this->m_pENoteTracks) { //for each effect
				if (track->img == nullptr) //we are the enote data
					this->m_pRC->p_NoteResource->getNote(type)->DrawNoteE(pos+track->pos, this->m_timeArrow, track->sro.GetX(), track->sro.GetY(), track->sro.GetZ()); //draw the enote
				else //we are an effect
					track->img->Draw(pos+track->pos, track->sro.GetX(), track->sro.GetY(), track->sro.GetZ()); //draw the effect
			}
		} else { //fallback for no effects
			this->m_pRC->p_NoteResource->getNote(type)->DrawNoteE(pos, this->m_timeArrow, 1.0f, 0.0f, 1.0f); //draw the enote
		}
	}

	void DivaNoteBaseNode::pushEffects(IDivaJudgeParams params) {
		//pushback rating particle
		unsigned int combo = 0;
		EBUS_EVENT_RESULT(combo, DivaJudgeBus, GetCombo);
		EBUS_EVENT(DivaSequenceEffectsBus, PushbackRating, params.notePos, params.nType, params.hitscore, combo, params.wrong);

		//pushback ending effect particle
		EEffectList eff = eEL_Count;
		switch (params.hitscore) {
		case eHS_Cool:
			eff = eEL_Cool;
			break;
		case eHS_Fine:
			eff = eEL_Fine;
			break;
		case eHS_Safe:
			eff = eEL_Safe;
			break;
		case eHS_Sad:
			eff = eEL_Sad;
			break;
		case eHS_Worst:
			eff = eEL_Worse;
		}

		EBUS_EVENT(DivaSequenceEffectsBus, PushbackEffect, params.notePos, eff);

		if (params.nType == eNT_SwipeL){
			EBUS_EVENT(DivaSequenceEffectsBus, PushbackEffect, params.notePos, eEL_SwipeL);
		} else if (params.nType == eNT_SwipeR) {
			EBUS_EVENT(DivaSequenceEffectsBus, PushbackEffect, params.notePos, eEL_SwipeR);
		}

		if (params.holdMult > 0) {
			EBUS_EVENT(DivaSequenceEffectsBus, PushbackHoldMulti, params.notePos, params.holdMult, params.hitscore, params.wrong);
		}
	}

	void DivaNoteBaseNode::GetMemoryUsage(ICrySizer *pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(m_ptimeArrowTrack);
		//pSizer->AddObject(m_pENoteTracks);
	}
}