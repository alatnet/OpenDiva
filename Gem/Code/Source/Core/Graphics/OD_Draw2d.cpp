#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "OD_Draw2d.h"

namespace OpenDiva {
	IDraw2d* OD_Draw2d::idraw2d = NULL;
	IRenderer* OD_Draw2d::renderer = NULL;
	const int OD_Draw2d::g_defaultBlendState = GS_BLSRC_SRCALPHA | GS_BLDST_ONEMINUSSRCALPHA;

	OD_Draw2d::OD_Draw2d() {
		if (OD_Draw2d::idraw2d == NULL) OD_Draw2d::idraw2d = Draw2dHelper::GetDraw2d();
		if (OD_Draw2d::renderer == NULL) OD_Draw2d::renderer = gEnv->pSystem->GetIRenderer();
		m_deferDraw = false;
	}

	void OD_Draw2d::BeginDraw2d(int width, int height, float znear, float zfar) {
		OD_Draw2d::renderer->SetCullMode(R_CULL_DISABLE);
		OD_Draw2d::renderer->Set2DMode(width, height, prevMat, znear, zfar);

		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);
	}

	void OD_Draw2d::DrawLine(AZ::Vector2 start, AZ::Vector2 end, ColorF color, int texID) {
		/*SVF_P3F_C4B_T2F verts[2];
		uint32 col = color.pack_abgr8888();

		verts[0].xyz = Vec3(start.x, start.y, 1);
		verts[0].color.dcolor = col;
		verts[0].st = Vec2(0, 0);

		verts[1].xyz = Vec3(start.x, start.y, 1);
		verts[1].color.dcolor = col;
		verts[1].st = Vec2(1, 1);

		OD_Draw2d::renderer->SetTexture(texID);
		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);
		OD_Draw2d::renderer->DrawDynVB(verts, nullptr, 2, 0, prtLineList);*/

		DeferredLinePrim prim;
		prim.start = start;
		prim.end = end;
		prim.color = color;
		prim.texID = texID;

		this->DrawOrDefer(&prim);
	}

	void OD_Draw2d::DrawLineStrip(SVF_P3F_C4B_T2F * verts, int numVerts, uint16 * inds, int numInds, int texID) {
		/*OD_Draw2d::renderer->SetTexture(texID);
		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);
		OD_Draw2d::renderer->DrawDynVB(verts, inds, numVerts, numInds, prtLineStrip);*/

		DeferredLineStripPrim prim;
		prim.verts = verts;
		prim.numVerts = numVerts;
		prim.inds = inds;
		prim.numInds = numInds;
		prim.texID = texID;

		this->DrawOrDefer(&prim);
	}

	void OD_Draw2d::DrawTriangleStrip(SVF_P3F_C4B_T2F * verts, int numVerts, uint16 * inds, int numInds, int texID) {
		/*OD_Draw2d::renderer->SetTexture(texID);
		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);
		OD_Draw2d::renderer->DrawDynVB(verts, inds, numVerts, numInds, prtTriangleStrip);*/

		DeferredTriangleStripPrim prim;
		prim.verts = verts;
		prim.numVerts = numVerts;
		prim.inds = inds;
		prim.numInds = numInds;
		prim.texID = texID;

		this->DrawOrDefer(&prim);
	}

	AZ::Vector2 OD_Draw2d::Align(AZ::Vector2 position, AZ::Vector2 size, HAlign horizontalAlignment, VAlign verticalAlignment) {
		AZ::Vector2 result;
		switch (horizontalAlignment) {
		case HAlign::Left:
			result.SetX(position.GetX());
			break;
		case HAlign::Center:
			result.SetX(position.GetX() - size.GetX() * 0.5f);
			break;
		case HAlign::Right:
			result.SetX(position.GetX() - size.GetX());
			break;
		}

		switch (verticalAlignment) {
		case VAlign::Top:
			result.SetY(position.GetY());
			break;
		case VAlign::Center:
			result.SetY(position.GetY() - size.GetY() * 0.5f);
			break;
		case VAlign::Bottom:
			result.SetY(position.GetY() - size.GetY());
			break;
		}

		return result;
	}

	void OD_Draw2d::DeferDraw(int width, int height, float znear, float zfar) {
		// Set up the 2D drawing state
		OD_Draw2d::renderer->SetCullMode(R_CULL_DISABLE);
		OD_Draw2d::renderer->Set2DMode(width, height, this->prevMat, znear, zfar);

		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);

		// Draw the deferred prims
		for (auto prim : this->m_dPrims) {
			prim->Draw();
			delete prim;
		}

		this->m_dPrims.clear();

		// End the 2D drawing state
		OD_Draw2d::renderer->Unset2DMode(prevMat);

	}

	OD_Draw2d& OD_Draw2d::getDraw2D() {
		static OD_Draw2d theInstance;
		return theInstance;
	}

	void OD_Draw2d::DeferredLinePrim::Draw() const {
		SVF_P3F_C4B_T2F verts[2];
		uint32 col = this->color.pack_abgr8888();

		verts[0].xyz = Vec3(this->start.GetX(), this->start.GetY(), 1);
		verts[0].color.dcolor = col;
		verts[0].st = Vec2(0, 0);

		verts[1].xyz = Vec3(this->start.GetX(), this->start.GetY(), 1);
		verts[1].color.dcolor = col;
		verts[1].st = Vec2(1, 1);

		OD_Draw2d::renderer->SetTexture(this->texID);
		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);
		OD_Draw2d::renderer->DrawDynVB(verts, nullptr, 2, 0, prtLineList);
	}

	void OD_Draw2d::DeferredLineStripPrim::Draw() const {
		OD_Draw2d::renderer->SetTexture(this->texID);
		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);
		OD_Draw2d::renderer->DrawDynVB(this->verts, this->inds, this->numVerts, this->numInds, prtLineStrip);
	}

	void OD_Draw2d::DeferredTriangleStripPrim::Draw() const {
		OD_Draw2d::renderer->SetTexture(this->texID);
		OD_Draw2d::renderer->SetColorOp(eCO_MODULATE, eCO_MODULATE, DEF_TEXARG0, DEF_TEXARG0);
		OD_Draw2d::renderer->SetState(OD_Draw2d::g_defaultBlendState | GS_NODEPTHTEST);
		OD_Draw2d::renderer->DrawDynVB(this->verts, this->inds, this->numVerts, this->numInds, prtTriangleStrip);
	}
}