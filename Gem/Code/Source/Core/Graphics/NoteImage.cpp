#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "NoteImage.h"

namespace OpenDiva {
	///////////////////////////////////////////////////////////
	//Note Image
	///////////////////////////////////////////////////////////
	IDraw2d *NoteImage::iDraw2d = NULL;

	NoteImage::NoteImage(ITexture *note, ITexture *noteE, ITexture *timeArrow, NoteImageSizes sizes) : m_pNote(note), m_pNoteE(noteE), m_pTimeArrow(timeArrow), m_sizes(sizes){
		if (NoteImage::iDraw2d == NULL) NoteImage::iDraw2d = Draw2dHelper::GetDraw2d();
		this->m_ImgScale = AZ::Vector2(1.0f, 1.0f);
		this->m_PosScale = AZ::Vector2(1.0f, 1.0f);
		this->m_holdImg = false;
	}

	NoteImage::~NoteImage() {
		this->m_pNote->Release();
		if (!this->m_holdImg) {
			this->m_pNoteE->Release();
		}
	}

	void NoteImage::DrawNote(AZ::Vector2 pos) {
		//pos.x = pos.x * m_PosScale.x;
		//pos.y = pos.y * m_PosScale.y;
		pos = pos * this->m_PosScale;

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pNote->GetTextureID(),
			//AZ::Vector2(pos.x, pos.y),
			pos,
			//AZ::Vector2(this->m_sizes.note.x*m_ImgScale.x, this->m_sizes.note.y*m_ImgScale.y),
			this->m_sizes.note*m_ImgScale,
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center
			);
	}

	void NoteImage::DrawNoteE(AZ::Vector2 pos, float timeArrowRot, float scale) {
		if (scale <= 0.0f) return;
		//pos.x = pos.x * m_PosScale.x;
		//pos.y = pos.y * m_PosScale.y;
		pos = pos * m_PosScale;

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pNoteE->GetTextureID(),
			//AZ::Vector2(pos.x, pos.y),
			//AZ::Vector2(this->m_sizes.noteE.x*this->m_ImgScale.x*scale, this->m_sizes.noteE.y*this->m_ImgScale.y*scale),
			pos,
			this->m_sizes.noteE*this->m_ImgScale*scale,
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center
			);

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pTimeArrow->GetTextureID(),
			//AZ::Vector2(pos.x, pos.y),
			//AZ::Vector2(this->m_sizes.timeArrow.x*this->m_ImgScale.x*scale, this->m_sizes.timeArrow.y*this->m_ImgScale.y*scale),
			pos,
			this->m_sizes.timeArrow*this->m_ImgScale*scale,
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

	void NoteImage::DrawNoteE(AZ::Vector2 pos, float timeArrowRot, float scale, float rot, float opacity) {
		if (scale <= 0.0f) return;
		//pos.x = pos.x * m_PosScale.x;
		//pos.y = pos.y * m_PosScale.y;
		pos = pos * this->m_PosScale;

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pNoteE->GetTextureID(),
			//AZ::Vector2(pos.x, pos.y),
			//AZ::Vector2(this->m_sizes.noteE.x*this->m_ImgScale.x*scale, this->m_sizes.noteE.y*this->m_ImgScale.y*scale),
			pos,
			this->m_sizes.noteE*this->m_ImgScale*scale,
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
			opacity,
			rot
			);

		NoteImage::iDraw2d->DrawImageAligned(
			this->m_pTimeArrow->GetTextureID(),
			//AZ::Vector2(pos.x, pos.y),
			//AZ::Vector2(this->m_sizes.timeArrow.x*this->m_ImgScale.x*scale, this->m_sizes.timeArrow.y*this->m_ImgScale.y*scale),
			pos,
			this->m_sizes.timeArrow*this->m_ImgScale*scale,
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center,
			opacity,
			timeArrowRot+rot
			);
	}
}