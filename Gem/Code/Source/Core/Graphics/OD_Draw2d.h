#ifndef _OD_DRAW2D_H_
#define _OD_DRAW2D_H_
#pragma once

//augmentation class for LyShine/Draw2D.
//add in useful 2D graphics stuff here that isnt provided by LyShine/Draw2D.

namespace OpenDiva {
	class OD_Draw2d {
	public:
		void BeginDraw2d(int width = OD_Draw2d::renderer->GetOverlayWidth(), int height = OD_Draw2d::renderer->GetOverlayHeight(), float znear = -1e10f, float zfar = 1e10f);
		void EndDraw2d() { OD_Draw2d::renderer->Unset2DMode(this->prevMat); }
		void SetDefer(bool defer) { this->m_deferDraw = defer; }
		void DeferDraw(int width = OD_Draw2d::renderer->GetOverlayWidth(), int height = OD_Draw2d::renderer->GetOverlayHeight(), float znear = -1e10f, float zfar = 1e10f);
	public:
		void DrawLine(AZ::Vector2 start, AZ::Vector2 end, ColorF color, int texID = OD_Draw2d::renderer->GetWhiteTextureId());
		void DrawLineStrip(SVF_P3F_C4B_T2F * verts, int numVerts, uint16 * inds = nullptr, int numInds = 0, int texID = OD_Draw2d::renderer->GetWhiteTextureId());
		void DrawTriangleStrip(SVF_P3F_C4B_T2F * verts, int numVerts, uint16 * inds = nullptr, int numInds = 0, int texID = OD_Draw2d::renderer->GetWhiteTextureId());
	public:
		//! Horizontal alignment can be used for both text and image drawing
		enum class HAlign {
			Left,
			Center,
			Right,
		};

		//! Vertical alignment can be used for both text and image drawing
		enum class VAlign {
			Top,
			Center,
			Bottom,
		};
		static AZ::Vector2 Align(AZ::Vector2 position, AZ::Vector2 size, HAlign horizontalAlignment, VAlign verticalAlignment);
	protected:
		static IDraw2d *idraw2d;
		static IRenderer* renderer;
		static const int g_defaultBlendState;
	private:
		class DeferredPrim
		{
		public:
			virtual ~DeferredPrim() {};
			virtual void Draw() const = 0;
		};

		class DeferredLinePrim : public DeferredPrim {
		public:
			void Draw() const;

			AZ::Vector2 start;
			AZ::Vector2 end;
			ColorF color;
			int texID;
		};

		class DeferredLineStripPrim : public DeferredPrim {
		public:
			void Draw() const;

			SVF_P3F_C4B_T2F * verts;
			int numVerts;
			uint16 * inds;
			int numInds;
			int texID;
		};

		class DeferredTriangleStripPrim : public DeferredPrim {
		public:
			void Draw() const;

			SVF_P3F_C4B_T2F * verts;
			int numVerts;
			uint16 * inds;
			int numInds;
			int texID;
		};

		template<class T>
		void DrawOrDefer(T * prim) {
			if (this->m_deferDraw) {
				T* newPrim = new T;
				*newPrim = *prim;
				this->m_dPrims.push_back(newPrim);
			}
			else {
				prim->Draw();
			}
		}

		bool m_deferDraw;
		AZStd::vector<DeferredPrim*> m_dPrims;
		TransformationMatrices prevMat;
	private:
		OD_Draw2d();
		OD_Draw2d(const OD_Draw2d&) = delete;
		OD_Draw2d& operator= (const OD_Draw2d&) = delete;
	public:
		static OD_Draw2d& getDraw2D();
	};
}

#endif //_OD_DRAW2D_H_