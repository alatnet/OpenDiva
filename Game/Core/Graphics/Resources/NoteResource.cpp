#include <StdAfx.h>
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
		m_swipeExSize = { 32, 32 };
		sizes.note = { 50, 50 };
		sizes.noteE = { 50, 50 };
		bigStarSizes.note = { 50, 50 };
		bigStarSizes.noteE = { 50, 50 };
		swipeSizes.note = { 44, 44 };
		swipeSizes.noteE = { 44, 44 };
		m_swipeTickOffset = { 0, 0 };
		m_swipeTickSize = { 24,24 };
		sizes.timeArrow = swipeSizes.timeArrow = { 90, 90 };

		{
			string noteConfig = folder;
			noteConfig.append("/");
			noteConfig.append(sNoteConfigFileName);
			XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(noteConfig);

			if (config) {
				XmlNodeRef note = config->findChild("note");
				XmlNodeRef bigStar = config->findChild("bigstar");
				XmlNodeRef swipe = config->findChild("swipe");
				XmlNodeRef timeArrow = config->findChild("timeArrow");

				note->getAttr("size", sizes.note);
				sizes.noteE = sizes.note;

				bigStar->getAttr("size", bigStarSizes.note);
				bigStarSizes.noteE = bigStarSizes.note;

				swipe->getAttr("size", swipeSizes.note);
				swipeSizes.noteE = swipeSizes.note;
				swipe->getAttr("exSize", m_swipeExSize);

				swipe->getAttr("tickOffset", m_swipeTickOffset);
				swipe->getAttr("tickSize", m_swipeTickSize);

				timeArrow->getAttr("size", sizes.timeArrow);
				swipeSizes.timeArrow = sizes.timeArrow;
			}
		}

		//load timeArrow
		{
			string timeArrowFilename = folder;
			timeArrowFilename.append("/");
			timeArrowFilename.append(sTimeArrowFileName);
			this->m_pTimeArrowTex = NoteResource::iRenderer->EF_LoadTexture(timeArrowFilename, FT_DONT_STREAM); //load the image
			assert(m_pTimeArrowTex != NULL); //check if the file was loaded
		}

		//load note images
		//this->m_pNoteImages = new NoteImage*[eNT_Count];

		//temp storage for ending note image for hold note;
		ITexture * noteEH[4];

		#pragma omp parallel for
		for (int i = 0; i < eNT_Count; i++) {
			//note image itself
			string noteFilename = folder;
			noteFilename.append("/");
			noteFilename.append(sNoteFileNames[(i*2)]);
			//ending note image
			string noteEFilename = folder;
			noteEFilename.append("/");
			noteEFilename.append(sNoteFileNames[(i * 2) + 1]);

			//load the images
			ITexture * note = NoteResource::iRenderer->EF_LoadTexture(noteFilename, FT_DONT_STREAM);
			ITexture * noteE = NoteResource::iRenderer->EF_LoadTexture(noteEFilename, FT_DONT_STREAM);

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
			string noteFilename = folder;
			noteFilename.append("/");
			noteFilename.append(sHoldNoteFileNames[i]);

			ITexture * note = NoteResource::iRenderer->EF_LoadTexture(noteFilename, FT_DONT_STREAM);

			assert(note != NULL); //check if the file was loaded

			this->m_pHoldNoteImages[i] = new NoteImage(note, noteEH[i], m_pTimeArrowTex, sizes);
			this->m_pHoldNoteImages[i]->setIsHold();
		}

		//swipe tick images
		#pragma omp parallel for
		for (int i = 0; i < eSEXNT_Count; i++) {
			//swipe tick image
			string noteFilename = folder;
			noteFilename.append("/");
			noteFilename.append(sSwipeEXFileNames[i]);

			//load the images
			m_pSwipeTickImages[i] = NoteResource::iRenderer->EF_LoadTexture(noteFilename, FT_DONT_STREAM);

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


	void NoteResource::setImgScale(Vec2 scale) {
		for (int i = 0; i < eNT_Count; i++) this->m_pNoteImages[i]->setImgScale(scale);
		for (int i = 0; i < 4; i++) this->m_pHoldNoteImages[i]->setImgScale(scale);
	}
	void NoteResource::setPosScale(Vec2 scale) {
		for (int i = 0; i < eNT_Count; i++) this->m_pNoteImages[i]->setPosScale(scale);
		for (int i = 0; i < 4; i++) this->m_pHoldNoteImages[i]->setPosScale(scale);
	}
	void NoteResource::setScale(Vec2 scale) {
		this->setImgScale(scale);
		this->setPosScale(scale);
	}
}