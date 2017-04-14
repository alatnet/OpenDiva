#include <StdAfx.h>
#include "Particles.h"

namespace LYGame {

	//-----------------------------------------------------------------------
	//Rating Particle Stuff
	//-----------------------------------------------------------------------
	RatingParticle::RatingParticle(ResourceCollection * rc, Vec2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong, float life) : m_pRC(rc), m_hitscore(hitscore), m_combo(combo), m_wrong(wrong), m_life(life) {
		NoteImageSizes sizes = m_pRC->p_NoteResource->getNoteSizes(nType);
		Vec2 textSize = m_pRC->p_FontResource->getSize(this->m_wrong ? MISSRATING(m_hitscore) : m_hitscore);

		/*float note_height = sizes.note.y / 2;
		float rating_height = float(this->m_pRC->p_RatingResource->getHeight(m_hitscore))/2;
		float y_pos = pos.y - note_height - rating_height;
		m_pos = Vec2(pos.x, y_pos);*/

		m_pos = Vec2(pos.x, pos.y - (sizes.note.y / 2) - (textSize.y / 2));
	}

	void RatingParticle::Render() {
		if (this->Alive()) this->m_pRC->p_FontResource->DrawRating(this->m_wrong ? MISSRATING(m_hitscore) : m_hitscore, m_pos, (m_combo >= 5), m_combo);
	}

	void RatingParticle::Tick(float dt) {
		if (this->Alive()) {
			this->m_life -= dt;
			if (this->m_life < 0) this->m_life = 0;
		}
	}

	void RatingParticle::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
	}

	//-----------------------------------------------------------------------
	//Ending Effect Particle Stuff
	//-----------------------------------------------------------------------
	EffectParticle::EffectParticle(/*int animID, */ResourceCollection * rc, Vec2 pos, EEffectList eff) : /*CAnimNode(animID), m_pRC(rc),*/ m_pos(pos), m_time(0) {
		IMovieSystem * iMovie = gEnv->pSystem->GetIMovieSystem();

		EffectDef * effect = rc->p_EffectResource->getEffect(eff); //get the effect.

		float endtime = 0.0f;

		//possibility: Redesign to move the tracks to effect resource as since all effects start a time 0 to whichever is biggest as a way to lower memory usage and processing time.

		//if we have effects
		if (effect->effects.size() > 0) {
			for (int i = 0; i < effect->effects.size(); i++) { //for each effect.
				EffectParticleTrack * track = new EffectParticleTrack(); //create a new effect track.
				track->posTrack = iMovie->CreateTrack(EAnimCurveType::eAnimCurveType_TCBVector); //create a new effect position track.
				track->sroTrack = iMovie->CreateTrack(EAnimCurveType::eAnimCurveType_TCBVector); //create a new effect scale, rotation, and opacity track.
				track->img = effect->effects.at(i).img; //assign the effect image.

				for (int i2 = 0; i2 < effect->effects.at(i).anim.size(); i2++) { //for each animation for this track.
					EffectAnim anim = effect->effects.at(i).anim.at(i2); //get the animation data.
					track->posTrack->SetValue(anim.time, { anim.pos.x,anim.pos.y,0.0f }); //set the track data for position.
					track->sroTrack->SetValue(anim.time, anim.sro); //set the track data for scale, rotation, and opacity.

					if (anim.time > endtime) endtime = anim.time;
				}


				EffectAnim anim = effect->effects.at(i).anim.at(0); //get the animation data.
				//set initial data
				//track->posTrack->GetValue(0, track->pos);
				//track->sroTrack->GetValue(0, track->sro);
				track->pos = anim.pos;
				track->sro = anim.sro;

				tracks.push_back(track); //push the effect track to the list.
			}
		}

		m_timeRange.Set(0, endtime);
	}

	EffectParticle::~EffectParticle() {
		/*while (!tracks.empty()) {
			EffectParticleTrack* entry = tracks.at(tracks.size() - 1);
			tracks.pop_back();
			delete entry;
		}*/
		tracks.clear(); //calls deconstructor.
	}

	void EffectParticle::Render() {
		if (this->Alive())
			for (EffectParticleTrack* entry : tracks) { //for each track entry
				entry->img->Draw({ this->m_pos.x + entry->pos.x, this->m_pos.y + entry->pos.y }, entry->sro.x, entry->sro.y, entry->sro.z); //draw the entry's image based on data.
			}
	}

	void EffectParticle::Tick(float dt) {
		if (!Alive()) return;
		this->m_time += dt;

		for (EffectParticleTrack* entry : tracks) { //for each track entry
			entry->posTrack->GetValue(this->m_time, entry->pos); //get the entry's position based on time.
			entry->sroTrack->GetValue(this->m_time, entry->sro); //get the entry's scale, rotation, and opacity based on time.
		}
	}

	bool EffectParticle::Alive() {
		return m_timeRange.IsInside(m_time);
	}

	void EffectParticle::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(this->tracks);
	}

	//-----------------------------------------------------------------------
	//Hold Multiplier Particle Stuff
	//-----------------------------------------------------------------------
	HoldMultiParticle::HoldMultiParticle(ResourceCollection * rc, Vec2 pos, unsigned int score, EHitScore hitscore, bool wrong) : m_pRC(rc), m_pos(pos), m_score(score){
		IMovieSystem * iMovie = gEnv->pSystem->GetIMovieSystem();
		this->posTrack = iMovie->CreateTrack(EAnimCurveType::eAnimCurveType_TCBFloat);

		float ypos = pos.y - rc->p_FontResource->getMultiSize().y;

		if (wrong){
			ypos -= (rc->p_FontResource->getSize(MISSRATING(hitscore)).y);
		} else {
			ypos -= (rc->p_FontResource->getSize(hitscore).y);
		}

		this->posTrack->SetValue(0, ypos);
		this->posTrack->SetValue(HOLDMULTITIMELENGTH, ypos - 20);
		this->m_timeRange = Range(0, HOLDMULTITIMELENGTH);
		this->m_time = 0;
	}

	HoldMultiParticle::~HoldMultiParticle() {
		this->posTrack->Release();
	}

	void HoldMultiParticle::Render() {
		if (this->Alive()) this->m_pRC->p_FontResource->DrawMult({ this->m_pos.x, this->m_pos.y }, this->m_score*10);
	}

	void HoldMultiParticle::Tick(float dt) {
		if (!Alive()) return;
		this->m_time += dt;

		this->posTrack->GetValue(this->m_time, this->m_pos.y);
	}

	bool HoldMultiParticle::Alive() {
		return m_timeRange.IsInside(m_time);
	}

	void HoldMultiParticle::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(this->posTrack);
		pSizer->AddObject(this->m_pos);
	}
}