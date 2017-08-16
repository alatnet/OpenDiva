#ifndef _H_NOTERESOURCE_
#define _H_NOTERESOURCE_
#pragma once

#include "../NoteImage.h"

namespace LYGame {
	#define HOLDIMG(x) x+eNT_Count*2

	/*struct NoteAnim {
		float time;
		float scale;
	};*/

	static const char * sNoteFileNames[eNT_Count * 2] {
		"cross.dds", //note image itself
		"crossE.dds", //ending note image
		"circle.dds",
		"circleE.dds",
		"square.dds",
		"squareE.dds",
		"triangle.dds",
		"triangleE.dds",

		"left.dds",
		"leftE.dds",
		"right.dds",
		"rightE.dds",
		"up.dds",
		"upE.dds",
		"down.dds",
		"downE.dds",

		"star.dds",
		"starE.dds",
		"bigStar.dds",
		"bigStarE.dds",

		"swipeL.dds",
		"swipeLE.dds",
		"swipeR.dds",
		"swipeRE.dds"
	};

	//for hold note specific images
	static const char * sHoldNoteFileNames[4] {
		"crossH.dds",
		"circleH.dds",
		"squareH.dds",
		"triangleH.dds"
	};

	//for swipe tick specific images
	//create a specific class for swipe images?
	static const char * sSwipeEXFileNames[eSEXNT_Count]{
		"swipeLEX.dds",
		"swipeLEXTick.dds",
		"swipeREX.dds",
		"swipeREXTick.dds"
	};

	static const char * sTimeArrowFileName = "timeArrow.dds";
	static const char * sNoteConfigFileName = "noteConfig.xml";

	//loads note images.
	class NoteResource {
	public:
		NoteResource(const char *folder);
		~NoteResource();
	public:
		NoteImage * getNote(ENoteType type) { return this->m_pNoteImages[type]; }
		NoteImage * getHoldNote(ENoteType type) { return this->m_pHoldNoteImages[type]; }
		NoteImageSizes getNoteSizes(ENoteType type) { return this->m_pNoteImages[type]->getSizes(); }
	public:
		ITexture * getSwipeTick(ESwipeEXNoteType dir) { return this->m_pSwipeTickImages[dir]; }
		AZ::Vector2 getSwipeTickOffset() { return this->m_swipeTickOffset; }
		AZ::Vector2 getSwipeTickSize() { return this->m_swipeTickSize; }
		AZ::Vector2 getSwipeExSize() { return this->m_swipeExSize; }
	public:
		void setImgScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));
		void setPosScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));
		void setScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));
	private:
		NoteImage * m_pNoteImages[eNT_Count];
		NoteImage * m_pHoldNoteImages[4];
		ITexture * m_pTimeArrowTex;
	private:
		AZ::Vector2 m_swipeExSize, m_swipeTickOffset, m_swipeTickSize;
		ITexture * m_pSwipeTickImages[eSEXNT_Count];
	private:
		static IRenderer * iRenderer;
	};
}

/*
Random dribbles:
Draw Order for Note Image: DrawNoteE, DrawTrail, DrawNote
*/
#endif //_H_NOTERESOURCE_