#ifndef _H_PARTICLES
#define _H_PARTICLES

#pragma once

#include "../../CryEngine/CryMovie/AnimSequence.h"
#include "Resources\ResourceCollection.h"

namespace LYGame {
	class ParticleBase {
	public:
		virtual void Render() = 0;
		virtual void Tick(float dt) = 0;
		virtual bool Alive() = 0;
		virtual void GetMemoryUsage(ICrySizer* pSizer) const = 0;
	};

	//used to display ratings
	//also displays combo
	#define RATINGPARTICLE_LIFE 0.5 //in seconds not milliseconds!
	class RatingParticle : public ParticleBase {
	public:
		RatingParticle(ResourceCollection * rc, AZ::Vector2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong, float life);
	public:
		void Render();
		void Tick(float dt);
		bool Alive() { return m_life > 0; }
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	private:
		float m_life;
		unsigned int m_combo;
		EHitScore m_hitscore;
		bool m_wrong;
		ResourceCollection * m_pRC;
		AZ::Vector2 m_pos;
	};

	//used to display ending effects
	struct EffectParticleTrack {
		EffectImage * img; //effect image
		IAnimTrack * posTrack; //position track
		IAnimTrack * sroTrack; //scale, rotation, and opacity track
		AZ::Vector2 pos;
		AZ::Vector3 sro;

		EffectParticleTrack() : img(nullptr), posTrack(nullptr), sroTrack(nullptr), pos(0,0), sro(0,0,0) {}

		void GetMemoryUsage(ICrySizer* pSizer) const {
			pSizer->AddObject(posTrack);
			pSizer->AddObject(sroTrack);
			//pSizer->AddObject(pos);
			//pSizer->AddObject(sro);
		}
	};

	class EffectParticle : public ParticleBase {
	public:
		EffectParticle(/*int animID,*/ ResourceCollection * rc, AZ::Vector2 pos, EEffectList eff);
		~EffectParticle();
	public:
		void Render();
		void Tick(float dt);
		bool Alive();
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	private:
		AZStd::vector<EffectParticleTrack*> tracks;
		AZ::Vector2 m_pos;
		float m_time;
		Range m_timeRange;
	};

	//use to display hold multiplier particle
	#define HOLDMULTITIMELENGTH 1 //in seconds!
	class HoldMultiParticle : public ParticleBase {
	public:
		HoldMultiParticle(ResourceCollection * rc, AZ::Vector2 pos, unsigned int score, EHitScore hitscore, bool wrong);
		~HoldMultiParticle();
	public:
		void Render();
		void Tick(float dt);
		bool Alive();
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	private:
		IAnimTrack* posTrack;
		AZ::Vector2 m_pos;
		float m_time;
		Range m_timeRange;
		unsigned int m_score;
		ResourceCollection * m_pRC;
	};
}

#endif