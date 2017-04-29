#include <StdAfx.h>
#include "CometTail.h"

namespace LYGame {
	float CometTail::m_rsize = 1.0f;
	Vec2 CometTail::m_rscale = Vec2(1.0f, 1.0f);

	void CometTail::Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
		if (tStart >= tEnd) return;
		this->Draw(p1, p2, p3, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	}

	void CometTail::Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
		if (tStart >= tEnd) return;

		float distances[] = { COMET_SIZE*this->m_size, -(COMET_SIZE*this->m_size), 0, 0 };

		std::vector<SVF_P3F_C4B_T2F> line1, line2; //individualized or shared vector space?

		float iS = tStart * 360; //starting orbit for orbit comet

		for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
			//calculate orbiting comet distances
			iS += pointsSpecified ? COMET_ROT : COMET_ROT * 2;
			if (iS >= 360) iS -= 360;
			distances[2] = (COMET_SEP * sin(DEG2RAD(iS))) + COMET_SIZE;
			distances[3] = (COMET_SEP * sin(DEG2RAD(iS))) - COMET_SIZE;

			//get the points based on distances
			Vec2 * points = Util::BCurvePerpPoint(i, p1, p2, p3, distances, 4);

			SVF_P3F_C4B_T2F tmp;

			//push back center comet vectors
			tmp.xyz = Vec3(points[1].x * this->m_scale.x, points[1].y * this->m_scale.y, 1);
			tmp.color.dcolor = this->m_PackCol;
			tmp.st = Vec2(0, 0);
			line1.push_back(tmp);

			tmp.xyz = Vec3(points[2].x * this->m_scale.x, points[2].y * this->m_scale.y, 1);
			line1.push_back(tmp);

			//push back orbiting comet vectors
			tmp.xyz = Vec3(points[3].x * this->m_scale.x, points[3].y * this->m_scale.y, 1);
			line2.push_back(tmp);

			tmp.xyz = Vec3(points[4].x * this->m_scale.x, points[4].y * this->m_scale.y, 1);
			line2.push_back(tmp);
		}

		//gets rid of jitter at the end point.
		{
			Vec2 * points = Util::BCurvePerpPoint(tEnd, p1, p2, p3, distances, 4);

			SVF_P3F_C4B_T2F tmp;

			//push back center comet vectors
			tmp.xyz = Vec3(points[1].x * this->m_scale.x, points[1].y * this->m_scale.y, 1);
			tmp.color.dcolor = this->m_PackCol;
			tmp.st = Vec2(0, 0);
			line1.push_back(tmp);

			tmp.xyz = Vec3(points[2].x * this->m_scale.x, points[2].y * this->m_scale.y, 1);
			line1.push_back(tmp);

			//push back orbiting comet vectors
			tmp.xyz = Vec3(points[3].x * this->m_scale.x, points[3].y * this->m_scale.y, 1);
			line2.push_back(tmp);

			tmp.xyz = Vec3(points[4].x * this->m_scale.x, points[4].y * this->m_scale.y, 1);
			line2.push_back(tmp);
		}

		//fade the tail ends.
		ColorF fade = this->m_Color;
		fade.a = 0.0;
		uint32 fCol = fade.pack_argb8888();
		line1[0].color.dcolor = fCol;
		line1[1].color.dcolor = fCol;
		line2[0].color.dcolor = fCol;
		line2[1].color.dcolor = fCol;

		if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line1[0], line1.size()); //draw center comet
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line2[0], line2.size()); //draw orbiting comet
		//this->line[0].clear(); //clear the vectors for the next run.
		//this->line[1].clear(); //clear the vectors for the next run.
		if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();
	}

	void CometTail::DrawRainbow(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
		DrawRainbow(p1, p2, p3, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	}

	void CometTail::DrawRainbow(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
		if (tStart >= tEnd) return;
		//http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

		std::vector<SVF_P3F_C4B_T2F> line;
		float distances[] = { RCOMET_SIZE*CometTail::m_rsize, -(RCOMET_SIZE*CometTail::m_rsize) };

		float hStart = tStart * 360; //starting rainbow color
		while (hStart >= 360) hStart -= 360; //make sure we dont go over 360 degrees! (doesnt matter but it's good to be cautious.)

		Util::HSV hsv;
		hsv.h = hStart; //starting rainbow color

		for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
			//calculate the rainbow!
			hsv.h += pointsSpecified ? RCOMET_SHIFT : RCOMET_SHIFT * 2;
			if (hsv.h >= 360) hsv.h = hsv.h - 360; //make sure we dont go over 360 degrees!
			uint32 col = Util::HSVToRGB(hsv).pack_argb8888(); //convert from hsv to rgb

			//get the points based on distances.
			Vec2 * points = Util::BCurvePerpPoint(i, p1, p2, p3, distances, 2);

			//push back the vectors.
			SVF_P3F_C4B_T2F tmp;
			tmp.xyz = Vec3(points[1].x * CometTail::m_rscale.x, points[1].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);

			tmp.xyz = Vec3(points[2].x * CometTail::m_rscale.x, points[2].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);
		}

		//gets rid of the jitter at the end.
		{
			//calculate the rainbow!
			hsv.h += pointsSpecified ? RCOMET_SHIFT : RCOMET_SHIFT * 2;
			if (hsv.h >= 360) hsv.h = hsv.h - 360; //make sure we dont go over 360 degrees!
			uint32 col = Util::HSVToRGB(hsv).pack_argb8888(); //convert from hsv to rgb

			//get the points based on distances.
			Vec2 * points = Util::BCurvePerpPoint(tEnd, p1, p2, p3, distances, 2);

			//push back the vectors.
			SVF_P3F_C4B_T2F tmp;
			tmp.xyz = Vec3(points[1].x * CometTail::m_rscale.x, points[1].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);

			tmp.xyz = Vec3(points[2].x * CometTail::m_rscale.x, points[2].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);
		}

		//fade the tail end
		hsv.h = hStart;
		uint32 fCol = Util::HSVToRGB(hsv, 0.0f).pack_argb8888();
		line[0].color.dcolor = fCol;
		line[1].color.dcolor = fCol;

		if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line[0], line.size()); //draw the rainbow comet tail
		if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();
	}


	void CometTail::Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
		if (tStart >= tEnd) return;
		this->Draw(p1, p2, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	}

	void CometTail::Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
		if (tStart >= tEnd) return;

		float distances[] = { COMET_SIZE*this->m_size, -(COMET_SIZE*this->m_size), 0, 0 };

		std::vector<SVF_P3F_C4B_T2F> line1, line2; //individualized or shared vector space?

		float iS = tStart * 360; //starting orbit for orbit comet

		for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
			//calculate orbiting comet distances
			iS += pointsSpecified ? COMET_ROT : COMET_ROT * 2;
			if (iS >= 360) iS -= 360;
			distances[2] = (COMET_SEP * sin(DEG2RAD(iS))) + COMET_SIZE;
			distances[3] = (COMET_SEP * sin(DEG2RAD(iS))) - COMET_SIZE;

			//get the points based on distances
			//Vec2 * points = Util::BCurvePerpPoint(i, p1, p2, p3, distances, 4);
			Vec2 * points = Util::PerpPointDist(p1, p2, i, distances, 4);

			SVF_P3F_C4B_T2F tmp;

			//push back center comet vectors
			tmp.xyz = Vec3(points[0].x * this->m_scale.x, points[0].y * this->m_scale.y, 1);
			tmp.color.dcolor = this->m_PackCol;
			tmp.st = Vec2(0, 0);
			line1.push_back(tmp);

			tmp.xyz = Vec3(points[1].x * this->m_scale.x, points[1].y * this->m_scale.y, 1);
			line1.push_back(tmp);

			//push back orbiting comet vectors
			tmp.xyz = Vec3(points[2].x * this->m_scale.x, points[2].y * this->m_scale.y, 1);
			line2.push_back(tmp);

			tmp.xyz = Vec3(points[3].x * this->m_scale.x, points[3].y * this->m_scale.y, 1);
			line2.push_back(tmp);
		}

		//gets rid of jitter at the end point.
		{
			//Vec2 * points = Util::BCurvePerpPoint(tEnd, p1, p2, p3, distances, 4);
			Vec2 * points = Util::PerpPointDist(p1, p2, tEnd, distances, 4);

			SVF_P3F_C4B_T2F tmp;

			//push back center comet vectors
			tmp.xyz = Vec3(points[0].x * this->m_scale.x, points[0].y * this->m_scale.y, 1);
			tmp.color.dcolor = this->m_PackCol;
			tmp.st = Vec2(0, 0);
			line1.push_back(tmp);

			tmp.xyz = Vec3(points[1].x * this->m_scale.x, points[1].y * this->m_scale.y, 1);
			line1.push_back(tmp);

			//push back orbiting comet vectors
			tmp.xyz = Vec3(points[2].x * this->m_scale.x, points[2].y * this->m_scale.y, 1);
			line2.push_back(tmp);

			tmp.xyz = Vec3(points[3].x * this->m_scale.x, points[3].y * this->m_scale.y, 1);
			line2.push_back(tmp);
		}

		//fade the tail ends.
		ColorF fade = this->m_Color;
		fade.a = 0.0;
		uint32 fCol = fade.pack_argb8888();
		line1[0].color.dcolor = fCol;
		line1[1].color.dcolor = fCol;
		line2[0].color.dcolor = fCol;
		line2[1].color.dcolor = fCol;

		if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line1[0], line1.size()); //draw center comet
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line2[0], line2.size()); //draw orbiting comet
		//this->line[0].clear(); //clear the vectors for the next run.
		//this->line[1].clear(); //clear the vectors for the next run.
		if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();
	}

	void CometTail::DrawRainbow(Vec2 p1, Vec2 p2, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
		DrawRainbow(p1, p2, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	}

	void CometTail::DrawRainbow(Vec2 p1, Vec2 p2, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
		if (tStart >= tEnd) return;
		//http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

		std::vector<SVF_P3F_C4B_T2F> line;
		float distances[] = { RCOMET_SIZE*CometTail::m_rsize, -(RCOMET_SIZE*CometTail::m_rsize) };

		float hStart = tStart * 360; //starting rainbow color
		while (hStart >= 360) hStart -= 360; //make sure we dont go over 360 degrees! (doesnt matter but it's good to be cautious.)

		Util::HSV hsv;
		hsv.h = hStart; //starting rainbow color

		for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
			//calculate the rainbow!
			hsv.h += pointsSpecified ? RCOMET_SHIFT : RCOMET_SHIFT * 2;
			if (hsv.h >= 360) hsv.h = hsv.h - 360; //make sure we dont go over 360 degrees!
			uint32 col = Util::HSVToRGB(hsv).pack_argb8888(); //convert from hsv to rgb

			//get the points based on distances.
			//Vec2 * points = Util::BCurvePerpPoint(i, p1, p2, p3, distances, 2);
			Vec2 * points = Util::PerpPointDist(p1, p2, i, distances, 2);

			//push back the vectors.
			SVF_P3F_C4B_T2F tmp;
			tmp.xyz = Vec3(points[0].x * CometTail::m_rscale.x, points[0].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);

			tmp.xyz = Vec3(points[1].x * CometTail::m_rscale.x, points[1].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);
		}

		//gets rid of the jitter at the end.
		{
			//calculate the rainbow!
			hsv.h += pointsSpecified ? RCOMET_SHIFT : RCOMET_SHIFT * 2;
			if (hsv.h >= 360) hsv.h = hsv.h - 360; //make sure we dont go over 360 degrees!
			uint32 col = Util::HSVToRGB(hsv).pack_argb8888(); //convert from hsv to rgb

			//get the points based on distances.
			//Vec2 * points = Util::BCurvePerpPoint(tEnd, p1, p2, p3, distances, 2);
			Vec2 * points = Util::PerpPointDist(p1, p2, tEnd, distances, 2);

			//push back the vectors.
			SVF_P3F_C4B_T2F tmp;
			tmp.xyz = Vec3(points[0].x * CometTail::m_rscale.x, points[0].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);

			tmp.xyz = Vec3(points[1].x * CometTail::m_rscale.x, points[1].y * CometTail::m_rscale.y, 1);
			tmp.color.dcolor = col;
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);
		}

		//fade the tail end
		hsv.h = hStart;
		uint32 fCol = Util::HSVToRGB(hsv, 0.0f).pack_argb8888();
		line[0].color.dcolor = fCol;
		line[1].color.dcolor = fCol;

		if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line[0], line.size()); //draw the rainbow comet tail
		if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();
	}
}