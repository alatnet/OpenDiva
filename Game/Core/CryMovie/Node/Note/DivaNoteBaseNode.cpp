#include <StdAfx.h>
#include "DivaNoteBaseNode.h"

namespace LYGame {
	//IDraw2d * DivaNoteBaseNode::iDraw2d = NULL;

	DivaNoteBaseNode::DivaNoteBaseNode(ResourceCollection *rc, IDivaJudge *divaJudge) : m_pRC(rc), m_pDivaJudge(divaJudge) {
		//if (DivaNoteBaseNode::iDraw2d == NULL) DivaNoteBaseNode::iDraw2d = Draw2dHelper::GetDraw2d();

		m_ptimeArrowTrack = /*m_pNoteEScaleTrack = m_pRippleTrack =*/ NULL;
	}

	DivaNoteBaseNode::~DivaNoteBaseNode() {
		this->m_pENoteTracks.clear();
	}

	void DivaNoteBaseNode::CreateDefaultTracks() {
		this->m_ptimeArrowTrack = gEnv->pMovieSystem->CreateTrack(eAnimCurveType_TCBFloat);/*this->CreateTrackInternal(
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
					track->posTrack->SetValue(start + anim.time, { anim.pos.x, anim.pos.y, 0 }); //set the position based on time
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
				track->posTrack->GetValue(ec.time, track->pos);
				track->sroTrack->GetValue(ec.time, track->sro);
			}
		}
	}

	void DivaNoteBaseNode::DrawNoteE(Vec2 pos, ENoteType type) {
		if (this->m_pENoteTracks.size() > 0) { //if we have effects
			for (ENoteTrack * track : this->m_pENoteTracks) { //for each effect
				if (track->img == nullptr) //we are the enote data
					this->m_pRC->p_NoteResource->getNote(type)->DrawNoteE({ pos.x+track->pos.x, pos.y+track->pos.y }, this->m_timeArrow, track->sro.x, track->sro.y, track->sro.z); //draw the enote
				else //we are an effect
					track->img->Draw({ pos.x+track->pos.x, pos.y+track->pos.y }, track->sro.x, track->sro.y, track->sro.z); //draw the effect
			}
		} else { //fallback for no effects
			this->m_pRC->p_NoteResource->getNote(type)->DrawNoteE(pos, this->m_timeArrow, 1.0f, 0.0f, 1.0f); //draw the enote
		}
	}

	#define M_divaSeqEff if (m_pDivaSeqEff != nullptr) m_pDivaSeqEff
	void DivaNoteBaseNode::pushEffects(IDivaJudgeParams params) {
		//pushback rating particle
		M_divaSeqEff->pushbackRating(params.notePos, params.nType, params.hitscore, m_pDivaJudge->getCombo(), params.wrong);

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
		M_divaSeqEff->pushbackEffect(params.notePos, eff);

		if (params.nType == eNT_SwipeL) M_divaSeqEff->pushbackEffect(params.notePos, eEL_SwipeL);
		else if (params.nType == eNT_SwipeR) M_divaSeqEff->pushbackEffect(params.notePos, eEL_SwipeR);

		if (params.holdMult > 0) M_divaSeqEff->pushbackHoldMulti(params.notePos, params.holdMult, params.hitscore, params.wrong);
	}

	void DivaNoteBaseNode::GetMemoryUsage(ICrySizer *pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(m_ptimeArrowTrack);
		pSizer->AddObject(m_pENoteTracks);
	}
	#undef M_divaSeq
}