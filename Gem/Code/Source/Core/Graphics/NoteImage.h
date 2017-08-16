#ifndef _H_NOTEIMAGE_
#define _H_NOTEIMAGE_
#pragma once

namespace OpenDiva {
	//handles drawing note images to screen and storing note textures.

	struct NoteImageSizes {
		AZ::Vector2 note, noteE, timeArrow;
	};

	class NoteImage {
	public:
		NoteImage(ITexture *note, ITexture *noteE, ITexture *timeArrow, NoteImageSizes sizes);
		~NoteImage();
	public:
		void DrawNote(AZ::Vector2 pos);
		void DrawNoteE(AZ::Vector2 pos, float timeArrowRot, float scale = 1.0); //timeArrorRot is in degrees (0-360)!
		void DrawNoteE(AZ::Vector2 pos, float timeArrowRot, float scale = 1.0, float rot = 0.0f, float opacity = 1.0f); //timeArrorRot is in degrees (0-360)!
		void setImgScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { this->m_ImgScale = scale; }
		void setPosScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { this->m_PosScale = scale; }
		void setScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { this->setImgScale(scale); this->setPosScale(scale); }

		void setIsHold() { this->m_holdImg = true; }

		NoteImageSizes getSizes() { return this->m_sizes; }
	private:
		ITexture *m_pNote, *m_pNoteE, *m_pTimeArrow;
		AZ::Vector2 m_PosScale, m_ImgScale;
		NoteImageSizes m_sizes;
		bool m_holdImg;
	private:
		static IDraw2d * iDraw2d;
	};
}

#endif //_H_NOTEIMAGE_