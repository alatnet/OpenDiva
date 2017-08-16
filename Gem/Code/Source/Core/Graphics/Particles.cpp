#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "Particles.h"

namespace OpenDiva {

	//-----------------------------------------------------------------------
	//Rating Particle Stuff
	//-----------------------------------------------------------------------
	RatingParticle::RatingParticle(ResourceCollection * rc, AZ::Vector2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong, float life) : m_pRC(rc), m_hitscore(hitscore), m_combo(combo), m_wrong(wrong), m_life(life) {
		NoteImageSizes sizes = m_pRC->p_NoteResource->getNoteSizes(nType);
		AZ::Vector2 textSize = m_pRC->p_FontResource->getSize(this->m_wrong ? MISSRATING(m_hitscore) : m_hitscore);

		/*float note_height = sizes.note.y / 2;
		float rating_height = float(this->m_pRC->p_RatingResource->getHeight(m_hitscore))/2;
		float y_pos = pos.y - note_height - rating_height;
		m_pos = Vec2(pos.x, y_pos);*/

		m_pos = AZ::Vector2(pos.GetX(), pos.GetY() - (sizes.note.GetY() / 2) - (textSize.GetY() / 2));
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
	EffectParticle::EffectParticle(/*int animID, */ResourceCollection * rc, AZ::Vector2 pos, EEffectList eff) : /*CAnimNode(animID), m_pRC(rc),*/ m_pos(pos), m_time(0) {
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
					AZ::Vector3 pos = AZ::Vector3(anim.pos.GetX(), anim.pos.GetY(), 0.0f);
					track->posTrack->SetValue(anim.time, pos); //set the track data for position.
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
				AZ::Vector2 pos = this->m_pos + entry->pos;
				entry->img->Draw(pos, entry->sro.GetX(), entry->sro.GetY(), entry->sro.GetZ()); //draw the entry's image based on data.
			}
	}

	void EffectParticle::Tick(float dt) {
		if (!Alive()) return;
		this->m_time += dt;

		for (EffectParticleTrack* entry : tracks) { //for each track entry
			AZ::Vector3 pos;
			entry->posTrack->GetValue(this->m_time, pos); //get the entry's position based on time.
			entry->pos.Set(pos.GetX(), pos.GetY());
			entry->sroTrack->GetValue(this->m_time, entry->sro); //get the entry's scale, rotation, and opacity based on time.
		}
	}

	bool EffectParticle::Alive() {
		return m_timeRange.IsInside(m_time);
	}

	void EffectParticle::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		//pSizer->AddObject(this->tracks);
		for (EffectParticleTrack* t : this->tracks) pSizer->AddObject(t);
	}

	//-----------------------------------------------------------------------
	//Hold Multiplier Particle Stuff
	//-----------------------------------------------------------------------
	HoldMultiParticle::HoldMultiParticle(ResourceCollection * rc, AZ::Vector2 pos, unsigned int score, EHitScore hitscore, bool wrong) : m_pRC(rc), m_pos(pos), m_score(score){
		IMovieSystem * iMovie = gEnv->pSystem->GetIMovieSystem();
		this->posTrack = iMovie->CreateTrack(EAnimCurveType::eAnimCurveType_TCBFloat);

		float ypos = pos.GetY() - rc->p_FontResource->getMultiSize().GetY();

		if (wrong){
			ypos -= (rc->p_FontResource->getSize(MISSRATING(hitscore)).GetY());
		} else {
			ypos -= (rc->p_FontResource->getSize(hitscore).GetY());
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
		if (this->Alive()) this->m_pRC->p_FontResource->DrawMult(this->m_pos, this->m_score*10);
	}

	void HoldMultiParticle::Tick(float dt) {
		if (!Alive()) return;
		this->m_time += dt;

		float ypos;
		this->posTrack->GetValue(this->m_time, ypos);
		this->m_pos.SetY(ypos);
	}

	bool HoldMultiParticle::Alive() {
		return m_timeRange.IsInside(m_time);
	}

	void HoldMultiParticle::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(this->posTrack);
		//pSizer->AddObject(this->m_pos);
	}

	//-----------------------------------------------------------------------
	//Subtext Particle Stuff
	//-----------------------------------------------------------------------
	SubtextParticle::SubtextParticle(
		AZStd::string text = "",
		AZ::Vector2 pos = AZ::Vector2(),
		float pointSize = 16.0f,
		float rot = 0.0f,
		IFFont * font = nullptr,
		unsigned int effectIndex = 0,
		AZ::Color color = AZ::Color(1.0f, 1.0f, 1.0f, 1.0f),
		float life = 0
	) {
		this->m_draw = Draw2dHelper::GetDraw2d();
		
		AZ::Vector2 scale = AZ::Vector2(gEnv->pRenderer->GetWidth() / 1280.0f, gEnv->pRenderer->GetHeight() / 720.0f);

		this->m_text = text;
		this->m_pos = pos * scale;
		this->m_pointSize = pointSize;
		this->m_life = life;
		this->m_color = color;

		this->m_options.font = font; //IFFont
		this->m_options.effectIndex = effectIndex;
		this->m_options.color = AZ::Vector3(color.GetR(), color.GetG(), color.GetB());
		this->m_options.rotation = rot;
	}

	void SubtextParticle::Render() {
		this->m_draw->DrawText(this->m_text.c_str(), this->m_pos, this->m_pointSize, this->m_color.GetA(), &this->m_options);
	}

	void SubtextParticle::Tick(float dt) {
		if (this->Alive()) {
			this->m_life -= dt;
			if (this->m_life < 0) this->m_life = 0;
		}
	}

	void SubtextParticle::GetMemoryUsage(ICrySizer * pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
	}
}