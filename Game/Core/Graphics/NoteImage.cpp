#include <StdAfx.h>
#include "NoteImage.h"

namespace LYGame {
	///////////////////////////////////////////////////////////
	//Note Image
	///////////////////////////////////////////////////////////
	IDraw2d *NoteImage::iDraw2d = NULL;

	NoteImage::NoteImage(ITexture *note, ITexture *noteE, ITexture *timeArrow, NoteImageSizes sizes) : m_pNote(note), m_pNoteE(noteE), m_pTimeArrow(timeArrow), m_sizes(sizes){
		if (NoteImage::iDraw2d == NULL) NoteImage::iDraw2d = Draw2dHelper::GetDraw2d();
		this->m_ImgScale = Vec2(1.0f, 1.0f);
		this->m_PosScale = Vec2(1.0f, 1.0f);
		this->m_holdImg = false;
	}

	NoteImage::~NoteImage() {
		this->m_pNote->Release();
		if (!this->m_holdImg) {
			this->m_pNoteE->Release();
		}
	}

	void NoteImage::DrawNote(Vec2 pos) {
		pos.x = pos.x * m_PosScale.x;
		pos.y = pos.y * m_PosScale.y;

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pNote->GetTextureID(),
			pos,
			Vec2(this->m_sizes.note.x*m_ImgScale.x, this->m_sizes.note.y*m_ImgScale.y),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center
			);
	}

	void NoteImage::DrawNoteE(Vec2 pos, float timeArrowRot, float scale) {
		if (scale <= 0.0f) return;
		pos.x = pos.x * m_PosScale.x;
		pos.y = pos.y * m_PosScale.y;

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pNoteE->GetTextureID(),
			pos,
			Vec2(this->m_sizes.noteE.x*this->m_ImgScale.x*scale, this->m_sizes.noteE.y*this->m_ImgScale.y*scale),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center
			);

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pTimeArrow->GetTextureID(),
			pos,
			Vec2(this->m_sizes.timeArrow.x*this->m_ImgScale.x*scale, this->m_sizes.timeArrow.y*this->m_ImgScale.y*scale),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
			1.0f,
			timeArrowRot
			);

		/*EffectImage::iDraw2d->DrawImageAligned(
			this->m_pImg->GetTextureID(),
			pos,
			Vec2(this->m_size.x*m_ImgScale.x*scale, this->m_size.y*m_ImgScale.y*scale),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
			opacity,
			rot
		);*/
	}

	void NoteImage::DrawNoteE(Vec2 pos, float timeArrowRot, float scale, float rot, float opacity) {
		if (scale <= 0.0f) return;
		pos.x = pos.x * m_PosScale.x;
		pos.y = pos.y * m_PosScale.y;

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pNoteE->GetTextureID(),
			pos,
			Vec2(this->m_sizes.noteE.x*this->m_ImgScale.x*scale, this->m_sizes.noteE.y*this->m_ImgScale.y*scale),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
			opacity,
			rot
			);

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pTimeArrow->GetTextureID(),
			pos,
			Vec2(this->m_sizes.timeArrow.x*this->m_ImgScale.x*scale, this->m_sizes.timeArrow.y*this->m_ImgScale.y*scale),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
			opacity,
			timeArrowRot+rot
			);
	}
}