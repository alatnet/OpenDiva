#ifndef _H_NOTEIMAGE_
#define _H_NOTEIMAGE_
#pragma once

namespace LYGame {
	//handles drawing note images to screen and storing note textures.

	struct NoteImageSizes {
		Vec2 note, noteE, timeArrow;
	};

	class NoteImage {
	public:
		NoteImage(ITexture *note, ITexture *noteE, ITexture *timeArrow, NoteImageSizes sizes);
		~NoteImage();
	public:
		void DrawNote(Vec2 pos);
		void DrawNoteE(Vec2 pos, float timeArrowRot, float scale = 1.0); //timeArrorRot is in degrees (0-360)!
		void DrawNoteE(Vec2 pos, float timeArrowRot, float scale = 1.0, float rot = 0.0f, float opacity = 1.0f); //timeArrorRot is in degrees (0-360)!
		void setImgScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_ImgScale = scale; }
		void setPosScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_PosScale = scale; }
		void setScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->setImgScale(scale); this->setPosScale(scale); }

		void setIsHold() { this->m_holdImg = true; }

		NoteImageSizes getSizes() { return this->m_sizes; }
	private:
		ITexture *m_pNote, *m_pNoteE, *m_pTimeArrow;
		Vec2 m_PosScale, m_ImgScale;
		NoteImageSizes m_sizes;
		bool m_holdImg;
	private:
		static IDraw2d * iDraw2d;
	};
}

#endif //_H_NOTEIMAGE_