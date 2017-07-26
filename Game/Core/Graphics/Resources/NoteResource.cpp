#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "NoteResource.h"

namespace LYGame {
	///////////////////////////////////////////////////////////
	//Note Resources
	///////////////////////////////////////////////////////////
	IRenderer * NoteResource::iRenderer = NULL;

	NoteResource::NoteResource(const char * folder) {
		CryLog("[NoteResource] Loading Note Resources - Path: %s", folder);

		if (NoteResource::iRenderer == NULL) NoteResource::iRenderer = gEnv->pSystem->GetIRenderer();

		//target sizes for note images.
		NoteImageSizes sizes;
		NoteImageSizes swipeSizes;
		NoteImageSizes bigStarSizes;
		m_swipeExSize = AZ::Vector2( 32, 32 );
		sizes.note = AZ::Vector2( 50, 50 );
		sizes.noteE = AZ::Vector2( 50, 50 );
		bigStarSizes.note = AZ::Vector2( 50, 50 );
		bigStarSizes.noteE = AZ::Vector2( 50, 50 );
		swipeSizes.note = AZ::Vector2( 44, 44 );
		swipeSizes.noteE = AZ::Vector2( 44, 44 );
		m_swipeTickOffset = AZ::Vector2( 0, 0 );
		m_swipeTickSize = AZ::Vector2( 24,24 );
		sizes.timeArrow = swipeSizes.timeArrow = AZ::Vector2( 90, 90 );

		{
			AZStd::string noteConfig = folder;
			noteConfig.append("/");
			noteConfig.append(sNoteConfigFileName);
			XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(noteConfig.c_str());

			if (config) {
				XmlNodeRef note = config->findChild("note");
				XmlNodeRef bigStar = config->findChild("bigstar");
				XmlNodeRef swipe = config->findChild("swipe");
				XmlNodeRef timeArrow = config->findChild("timeArrow");

				Vec2 sizes1, sizes2, sizes3, sizes4, sizes5, sizes6, sizes7;

				note->getAttr("size", sizes1);
				sizes.noteE = sizes.note = AZ::Vector2(sizes1.x,sizes1.y);

				bigStar->getAttr("size", sizes2);
				bigStarSizes.noteE = bigStarSizes.note = AZ::Vector2(sizes2.x, sizes2.y);

				swipe->getAttr("size", sizes3);
				swipeSizes.noteE = swipeSizes.note = AZ::Vector2(sizes3.x, sizes3.y);

				swipe->getAttr("exSize", sizes4);
				m_swipeExSize = AZ::Vector2(sizes4.x, sizes4.y);
				swipe->getAttr("tickOffset", sizes5);
				m_swipeTickOffset = AZ::Vector2(sizes5.x, sizes5.y);
				swipe->getAttr("tickSize", sizes6);
				m_swipeTickSize = AZ::Vector2(sizes6.x, sizes6.y);

				timeArrow->getAttr("size", sizes7);
				swipeSizes.timeArrow = sizes.timeArrow = AZ::Vector2(sizes7.x, sizes7.y);
			}
		}

		//load timeArrow
		{
			AZStd::string timeArrowFilename = folder;
			timeArrowFilename.append("/");
			timeArrowFilename.append(sTimeArrowFileName);
			this->m_pTimeArrowTex = NoteResource::iRenderer->EF_LoadTexture(timeArrowFilename.c_str(), FT_DONT_STREAM); //load the image
			assert(m_pTimeArrowTex != NULL); //check if the file was loaded
		}

		//load note images
		//this->m_pNoteImages = new NoteImage*[eNT_Count];

		//temp storage for ending note image for hold note;
		ITexture * noteEH[4];

		#pragma omp parallel for
		for (int i = 0; i < eNT_Count; i++) {
			//note image itself
			AZStd::string noteFilename = folder;
			noteFilename.append("/");
			noteFilename.append(sNoteFileNames[(i*2)]);
			//ending note image
			AZStd::string noteEFilename = folder;
			noteEFilename.append("/");
			noteEFilename.append(sNoteFileNames[(i * 2) + 1]);

			//load the images
			ITexture * note = NoteResource::iRenderer->EF_LoadTexture(noteFilename.c_str(), FT_DONT_STREAM);
			ITexture * noteE = NoteResource::iRenderer->EF_LoadTexture(noteEFilename.c_str(), FT_DONT_STREAM);

			if (i < 4) noteEH[i] = noteE;

			//check to see if files are loaded
			assert(note != NULL);
			assert(noteE != NULL);

			//create the note image
			if (i == eNT_SwipeL || i == eNT_SwipeR) this->m_pNoteImages[i] = new NoteImage(note, noteE, m_pTimeArrowTex, swipeSizes); //set swipe sizes
			else if (i == eNT_BigStar) this->m_pNoteImages[i] = new NoteImage(note, noteE, m_pTimeArrowTex, bigStarSizes); //set big star size
			else this->m_pNoteImages[i] = new NoteImage(note, noteE, m_pTimeArrowTex, sizes); //set other note sizes
		}

		//hold note images
		//this->m_pHoldNoteImages = new NoteImage*[4];
		#pragma omp parallel for
		for (int i = 0; i < 4; i++) {
			//note image itself
			AZStd::string noteFilename = folder;
			noteFilename.append("/");
			noteFilename.append(sHoldNoteFileNames[i]);

			ITexture * note = NoteResource::iRenderer->EF_LoadTexture(noteFilename.c_str(), FT_DONT_STREAM);

			assert(note != NULL); //check if the file was loaded

			this->m_pHoldNoteImages[i] = new NoteImage(note, noteEH[i], m_pTimeArrowTex, sizes);
			this->m_pHoldNoteImages[i]->setIsHold();
		}

		//swipe tick images
		#pragma omp parallel for
		for (int i = 0; i < eSEXNT_Count; i++) {
			//swipe tick image
			AZStd::string noteFilename = folder;
			noteFilename.append("/");
			noteFilename.append(sSwipeEXFileNames[i]);

			//load the images
			m_pSwipeTickImages[i] = NoteResource::iRenderer->EF_LoadTexture(noteFilename.c_str(), FT_DONT_STREAM);

			assert(m_pSwipeTickImages[i] != NULL); //check if the file was loaded
		}
		CryLog("[NoteResource] Finished Loading Note Resources.");
	}

	NoteResource::~NoteResource() {
		for (int i = 0; i < eNT_Count; i++) delete this->m_pNoteImages[i];
		//delete[] this->m_pNoteImages;
		for (int i = 0; i < 4; i++) delete this->m_pHoldNoteImages[i];
		//delete[] this->m_pHoldNoteImages;
		for (int i = 0; i < 2; i++) this->m_pSwipeTickImages[i]->Release();

		this->m_pTimeArrowTex->Release();
	}


	void NoteResource::setImgScale(AZ::Vector2 scale) {
		for (int i = 0; i < eNT_Count; i++) this->m_pNoteImages[i]->setImgScale(scale);
		for (int i = 0; i < 4; i++) this->m_pHoldNoteImages[i]->setImgScale(scale);
	}
	void NoteResource::setPosScale(AZ::Vector2 scale) {
		for (int i = 0; i < eNT_Count; i++) this->m_pNoteImages[i]->setPosScale(scale);
		for (int i = 0; i < 4; i++) this->m_pHoldNoteImages[i]->setPosScale(scale);
	}
	void NoteResource::setScale(AZ::Vector2 scale) {
		this->setImgScale(scale);
		this->setPosScale(scale);
	}
}