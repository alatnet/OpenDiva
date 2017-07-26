#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "HoldTail.h"

namespace LYGame {
	//HoldTail::HoldTail(HoldTailGeom * geoms, const unsigned int numGeoms) : m_NumGeoms(numGeoms), m_pDistances(nullptr), m_pAdjDists(nullptr), m_pCols(nullptr), m_size(1.0f)/*, line(nullptr)*/ {
	//	//this->line = new std::vector<SVF_P3F_C4B_T2F>[numGeoms];
	//	this->m_pDistances = new float[numGeoms * 2];
	//	this->m_pAdjDists = new float[numGeoms * 2];
	//	this->m_pCols = new uint32[numGeoms * 2];

	//	for (int i = 0; i < numGeoms; i++) { //for each geometry
	//		//convert the colors to uint32 for quicker assignment to vector data.
	//		m_pCols[(i * 2)] = geoms[i].color1.pack_argb8888();
	//		m_pCols[(i * 2) + 1] = geoms[i].color2.pack_argb8888();
	//		//convert the distances to an array that we can use.
	//		this->m_pDistances[i * 2] = geoms[i].d1;
	//		this->m_pDistances[(i * 2) + 1] = geoms[i].d2;
	//		this->m_pAdjDists[i * 2] = geoms[i].d1;
	//		this->m_pAdjDists[(i * 2) + 1] = geoms[i].d2;
	//	}
	//}

	//HoldTail::~HoldTail() {
	//	//for (int i = 0; i < this->m_NumGeoms; i++) this->line[i].clear(); //clear the data just to be sure.
	//	//delete[] this->line;
	//	delete[] this->m_pDistances;
	//	delete[] this->m_pAdjDists;
	//	delete[] this->m_pCols;
	//}

	//void HoldTail::Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
	//	if (tStart >= tEnd) return;
	//	this->Draw(p1, p2, p3, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	//}

	//void HoldTail::Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
	//	if (tStart >= tEnd) return;

	//	std::vector<SVF_P3F_C4B_T2F> * line = new std::vector<SVF_P3F_C4B_T2F>[m_NumGeoms];

	//	for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
	//		Vec2 * points = Util::BCurvePerpPoint(i, p1, p2, p3, this->m_pAdjDists, this->m_NumGeoms * 2); //calculate the distances at time i.

	//		for (int i2 = 0; i2 < this->m_NumGeoms; i2++) { //for each geometry
	//			SVF_P3F_C4B_T2F tmp;

	//			//push back the vectors
	//			tmp.xyz = Vec3(points[(i2 * 2) + 1].x, points[(i2 * 2) + 1].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2)];
	//			tmp.st = Vec2(1, 1);
	//			line[i2].push_back(tmp);

	//			tmp.xyz = Vec3(points[(i2 * 2) + 2].x, points[(i2 * 2) + 2].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2) + 1];
	//			line[i2].push_back(tmp);
	//		}
	//	}

	//	//gets rid of jitter at the end point.
	//	{
	//		Vec2 * points = Util::BCurvePerpPoint(tEnd, p1, p2, p3, this->m_pAdjDists, this->m_NumGeoms * 2);

	//		for (int i2 = 0; i2 < this->m_NumGeoms; i2++) { //for each geometry
	//			SVF_P3F_C4B_T2F tmp;

	//			//push back the vectors
	//			tmp.xyz = Vec3(points[(i2 * 2) + 1].x, points[(i2 * 2) + 1].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2)];
	//			tmp.st = Vec2(0, 0);
	//			line[i2].push_back(tmp);

	//			tmp.xyz = Vec3(points[(i2 * 2) + 2].x, points[(i2 * 2) + 2].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2) + 1];
	//			line[i2].push_back(tmp);
	//		}
	//	}

	//	if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
	//	for (int i = 0; i < this->m_NumGeoms; i++) { //for each geometry
	//		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line[i][0], line[i].size()); //draw the line
	//		//this->line[i].clear(); //done using the line, clear it for next pass.
	//	}
	//	if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();

	//	for (int i = 0; i < this->m_NumGeoms; i++) line[i].clear(); //clear the data just to be sure.
	//	delete[] line; //free up the data.
	//}

	//void HoldTail::Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
	//	if (tStart >= tEnd) return;
	//	this->Draw(p1, p2, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	//}

	//void HoldTail::Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
	//	if (tStart >= tEnd) return;

	//	std::vector<SVF_P3F_C4B_T2F> * line = new std::vector<SVF_P3F_C4B_T2F>[m_NumGeoms];

	//	for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
	//		//Vec2 * points = Util::BCurvePerpPoint(i, p1, p2, p3, this->m_pDistances, this->m_NumGeoms * 2); //calculate the distances at time i.
	//		Vec2 * points = Util::PerpPointDist(p1, p2, i, this->m_pAdjDists, this->m_NumGeoms * 2);

	//		for (int i2 = 0; i2 < this->m_NumGeoms; i2++) { //for each geometry
	//			SVF_P3F_C4B_T2F tmp;

	//			//push back the vectors
	//			tmp.xyz = Vec3(points[(i2 * 2)].x, points[(i2 * 2)].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2)];
	//			tmp.st = Vec2(1, 1);
	//			line[i2].push_back(tmp);

	//			tmp.xyz = Vec3(points[(i2 * 2) + 1].x, points[(i2 * 2) + 1].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2) + 1];
	//			line[i2].push_back(tmp);
	//		}
	//	}

	//	//gets rid of jitter at the end point.
	//	{
	//		//Vec2 * points = Util::BCurvePerpPoint(tEnd, p1, p2, p3, this->m_pDistances, this->m_NumGeoms * 2);
	//		Vec2 * points = Util::PerpPointDist(p1, p2, tEnd, this->m_pAdjDists, this->m_NumGeoms * 2);


	//		for (int i2 = 0; i2 < this->m_NumGeoms; i2++) { //for each geometry
	//			SVF_P3F_C4B_T2F tmp;

	//			//push back the vectors
	//			tmp.xyz = Vec3(points[(i2 * 2)].x, points[(i2 * 2)].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2)];
	//			tmp.st = Vec2(0, 0);
	//			line[i2].push_back(tmp);

	//			tmp.xyz = Vec3(points[(i2 * 2) + 1].x, points[(i2 * 2) + 1].y, 1);
	//			tmp.color.dcolor = this->m_pCols[(i2 * 2) + 1];
	//			line[i2].push_back(tmp);
	//		}
	//	}

	//	if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
	//	for (int i = 0; i < this->m_NumGeoms; i++) { //for each geometry
	//		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line[i][0], line[i].size()); //draw the line
	//		//this->line[i].clear(); //done using the line, clear it for next pass.
	//	}
	//	if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();

	//	for (int i = 0; i < this->m_NumGeoms; i++) line[i].clear(); //clear the data just to be sure.
	//	delete[] line; //free up the data.
	//}

	//void HoldTail::setSize(float size) {
	//	for (int i = 0; i < this->m_NumGeoms; i++) {
	//		this->m_pAdjDists[i * 2] = this->m_pDistances[i * 2] * size;
	//		this->m_pAdjDists[(i * 2) + 1] = this->m_pDistances[(i * 2) + 1] * size;
	//	}

	//	this->m_size = size;
	//}

	//--------------------------------------------------------------------------------

	HoldTail::~HoldTail() {
		if (this->m_pTex != nullptr) this->m_pTex->Release();
	}

	void HoldTail::Draw(AZ::Vector2 p1, AZ::Vector2 p2, AZ::Vector2 p3, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
		if (tStart >= tEnd) return;
		this->Draw(p1, p2, p3, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	}

	void HoldTail::Draw(AZ::Vector2 p1, AZ::Vector2 p2, AZ::Vector2 p3, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
		if (tStart >= tEnd) return;
		if (this->m_pTex == nullptr) return;

		//int numPoints = (int)(-(tStart - tEnd) / step);

		AZStd::vector<SVF_P3F_C4B_T2F> line;
		ColorF white(1.0f, 1.0f, 1.0f, 1.0f);
		float dists[2] = {
			(this->m_width*this->m_size) / 2,
			-(this->m_width*this->m_size / 2)
		};

		for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
			AZ::Vector2 * points = Util::BCurvePerpPoint(i, p1, p2, p3, dists, 2); //calculate the distances at time i.

			SVF_P3F_C4B_T2F tmp;
			tmp.xyz = Vec3(points[1].GetX() * this->m_scale.GetX(), points[1].GetY() * this->m_scale.GetY(), 0);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(1, 0);

			line.push_back(tmp);

			tmp.xyz = Vec3(points[2].GetX() * this->m_scale.GetX(), points[2].GetY() * this->m_scale.GetY(), 0);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(1, 1);
			line.push_back(tmp);
		}

		//gets rid of jitter at the end point.
		{
			AZ::Vector2 * points = Util::BCurvePerpPoint(tEnd, p1, p2, p3, dists, 2);

			SVF_P3F_C4B_T2F tmp;
			tmp.xyz = Vec3(points[1].GetX() * this->m_scale.GetX(), points[1].GetY() * this->m_scale.GetY(), 0);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);

			tmp.xyz = Vec3(points[2].GetX() * this->m_scale.GetX(), points[2].GetY() * this->m_scale.GetY(), 0);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(0, 1);
			line.push_back(tmp);
		}

		if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line[0], line.size(), nullptr, 0, this->m_pTex->GetTextureID()); //draw the line
		if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();
	}

	void HoldTail::Draw(AZ::Vector2 p1, AZ::Vector2 p2, float tStart, float tEnd, int numPoints, bool isolatedDraw) {
		if (tStart >= tEnd) return;
		this->Draw(p1, p2, tStart, tEnd, (float)((float)(tEnd - tStart) / (float)(numPoints)), isolatedDraw, true);
	}

	void HoldTail::Draw(AZ::Vector2 p1, AZ::Vector2 p2, float tStart, float tEnd, float step, bool isolatedDraw, bool pointsSpecified) {
		if (tStart >= tEnd) return;
		if (this->m_pTex == nullptr) return;

		//int numPoints = (int)(-(tStart - tEnd) / step);

		AZStd::vector<SVF_P3F_C4B_T2F> line;
		ColorF white(1.0f, 1.0f, 1.0f, 1.0f);
		float dists[2] = {
			this->m_width*this->m_size / 2,
			-(this->m_width*this->m_size / 2)
		};

		for (float i = tStart; i < tEnd; i += step) { //from start time to end time with a step between each
			AZ::Vector2 * points = Util::PerpPointDist(p1, p2, i, dists, 2); //calculate the distances at time i.

			SVF_P3F_C4B_T2F tmp;

			//push back the vectors
			tmp.xyz = Vec3(points[1].GetX() * this->m_scale.GetX(), points[1].GetY() * this->m_scale.GetY(), 0);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(1, 0);
			line.push_back(tmp);

			tmp.xyz = Vec3(points[2].GetX() * this->m_scale.GetX(), points[2].GetY() * this->m_scale.GetY(), 1);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(1, 1);
			line.push_back(tmp);
		}

		//gets rid of jitter at the end point.
		{
			AZ::Vector2 * points = Util::PerpPointDist(p1, p2, tEnd, dists, 2);

			SVF_P3F_C4B_T2F tmp;

			//push back the vectors
			tmp.xyz = Vec3(points[1].GetX() * this->m_scale.GetX(), points[1].GetY() * this->m_scale.GetY(), 1);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(0, 0);
			line.push_back(tmp);

			tmp.xyz = Vec3(points[2].GetX() * this->m_scale.GetX(), points[2].GetY() * this->m_scale.GetY(), 1);
			tmp.color.dcolor = white.pack_argb8888();
			tmp.st = Vec2(0, 1);
			line.push_back(tmp);
		}

		if (isolatedDraw) OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		OD_Draw2d::getDraw2D().DrawTriangleStrip(&line[0], line.size(), nullptr, 0, this->m_pTex->GetTextureID()); //draw the line
		if (isolatedDraw) OD_Draw2d::getDraw2D().EndDraw2d();
	}
}