#ifndef _H_HOLDTAIL_
#define _H_HOLDTAIL_
#pragma once
#include "../../Graphics/OD_Draw2d.h"

namespace LYGame {
	//old version (more flexible)
	//apparently will cause renderer to crash...
	/*struct HoldTailGeom {
		float d1,d2;
		ColorF color1, color2;

		HoldTailGeom() :
			d1(0.0f),
			d2(0.0f),
			color1(0.0f, 0.0f, 0.0f, 0.0f),
			color2(0.0f, 0.0f, 0.0f, 0.0f)
		{};
	};

	class HoldTail {
	public:
		HoldTail(HoldTailGeom * geoms, const unsigned int numGeoms);
		~HoldTail();

		//bcurve
		void Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		void Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, int numPoints, bool isolatedDraw = false);

		//line
		void Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		void Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, int numPoints, bool isolatedDraw = false);

		void setSize(float size = 1.0f);
		float getSize() { return this->m_size; }
	private:
		unsigned int m_NumGeoms;
		uint32 * m_pCols;
		float * m_pDistances;
		float * m_pAdjDists;
		float m_size;

		//std::vector<SVF_P3F_C4B_T2F> *line; //individualized or shared vector space?
	};*/

	//new version (less flexible but uses images)
	class HoldTail {
	public:
		HoldTail(ITexture* holdImg, float width) : m_pTex(holdImg), m_width(width), m_size(1.0f) {}
		~HoldTail();

		//bcurve
		void Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		void Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, int numPoints, bool isolatedDraw = false);

		//line
		void Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		void Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, int numPoints, bool isolatedDraw = false);

		//width of hold tail (scale)
		void setSize(float size = 1.0f) { this->m_size = size; }
		float getSize() { return this->m_size; }
	private:
		ITexture* m_pTex;
		float m_width, m_size;
	};
}

#endif //_H_HOLDTAIL_