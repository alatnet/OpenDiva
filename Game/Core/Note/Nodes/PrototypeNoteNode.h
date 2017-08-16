/*
Animation node to deal with all the button input animations.
*/

#ifndef PROTOTYPENOTENODE_H
#define PROTOTYPENOTENODE_H

#pragma once

#include "../../../CryEngine/CryMovie/AnimNode.h"
#include "../../../Graphics/OD_Draw2d.h"
#include "../../../Graphics/Resources/ResourceCollection.h"

#define BIEZER_TACKON 0.01f //both time and percentage, so a 0.05 is 0.05 seconds added to the time with a 5% additional overhang.

namespace LYGame {
	class PrototypeNoteNode : public CAnimNode {
	public:
		PrototypeNoteNode(const int id, ENoteType type, ResourceCollection * rc);
		~PrototypeNoteNode();
	public:
		//void SetPos(float time, const Vec3 &pos);
		//void SetRot(float time, const float rot);
		//void SetScale(float time, const float scale);
		//void SetOpacity(float time, const float opacity);

		CAnimParamType GetParamType(unsigned int nIndex) const { return NULL; }
		EAnimNodeType GetType() const { return eAnimNodeType_Entity; }
		bool NeedToRender() const { return true; } //render only when time is in range?

		void CreateDefaultTracks();

		void Animate(SAnimContext &ec);
		void Render();

		void SetBCurve(Vec2 p1, Vec2 p2, Vec2 p3, float start, float end);
	private:
		Vec2 m_Pos/*, m_Size*/;
		float /*m_Opacity = 1.0f,*/ m_Rot = 0.0f, m_Scale = 1.0f;
	private:
		IAnimTrack /**posTrack,*/ *timeArrowTrack, *scaleTrack,/* *opacityTrack,*/ *beizerTrack;
		Vec2 bCurve[3];
		float bTime;
		//std::vector<SVF_P3F_C4B_T2F> line;
		float startTime, endTime, timeInRange;
	private:
		NoteImage* m_pnImage;
		CometTail *m_pTail;
		HoldTail *m_pHTail;
		//EffectResource * m_pER;
		FontResource * m_pFR;
		//RatingResource * m_pRR;
		//ITexture * testHoldText;
	private:
		void configureEffects(float start, float end);
		Vec3 ripple; //X = Scale, Y = Rot, Z = Opacity,
		IAnimTrack *rippleTrack;
	private:
		static IDraw2d * iDraw2d;
	};
}

#endif