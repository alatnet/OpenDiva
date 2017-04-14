#include <StdAfx.h>
#include "TailResource.h"

namespace LYGame {

	TailResource::TailResource(const char * folder) : /*m_hTailStyle(0), m_numHTailStyles(0),*/ m_pCometTails(nullptr), m_pHoldTails(nullptr)/*, m_pHStyles(nullptr)*/ {
		CryLog("[TailResource] Loading Tail Resources - Path: %s", folder);

		//load the tails data
		string tailsXMLFilename = folder;
		tailsXMLFilename.append("/");
		tailsXMLFilename.append(sTailFileName);

		XmlNodeRef tailsXML = gEnv->pSystem->LoadXmlFromFile(tailsXMLFilename);

		if (tailsXML) {
			Vec4 col;

			XmlNodeRef comet = tailsXML->findChild("comet");
			if (comet != NULL) {
				int cChildCount = comet->getChildCount();
				if (cChildCount == 4 || cChildCount == 8 || cChildCount == 11) this->loadCTail(comet);
				else this->setCTailDefaults();
			} else {
				this->setCTailDefaults();
			}

			XmlNodeRef hold = tailsXML->findChild("hold");
			if (/*hold->getChildCount() != 0*/ hold != NULL) {
				this->m_pHoldTails = new HoldTail*[4];
				//this->loadHTail(hold);

				//NoteResource::iRenderer->EF_LoadTexture(timeArrowFilename, FT_DONT_STREAM);

				IRenderer* iRender = gEnv->pRenderer;
				string tailsHoldFolder = folder;
				ITexture *tex;
				float width;

				XmlNodeRef holdT = hold->findChild("triangle");
				tex = iRender->EF_LoadTexture(tailsHoldFolder + "/" + holdT->getAttr("img"), FT_DONT_STREAM);
				holdT->getAttr("width", width);
				this->m_pHoldTails[eNT_Triangle] = new HoldTail(tex, width);

				XmlNodeRef holdCr = hold->findChild("cross");
				tex = iRender->EF_LoadTexture(tailsHoldFolder + "/" + holdCr->getAttr("img"), FT_DONT_STREAM);
				holdCr->getAttr("width", width);
				this->m_pHoldTails[eNT_Cross] = new HoldTail(tex, width);

				XmlNodeRef holdS = hold->findChild("square");
				tex = iRender->EF_LoadTexture(tailsHoldFolder + "/" + holdS->getAttr("img"), FT_DONT_STREAM);
				holdS->getAttr("width", width);
				this->m_pHoldTails[eNT_Square] = new HoldTail(tex, width);

				XmlNodeRef holdC = hold->findChild("circle");
				tex = iRender->EF_LoadTexture(tailsHoldFolder + "/" + holdC->getAttr("img"), FT_DONT_STREAM);
				holdC->getAttr("width", width);
				this->m_pHoldTails[eNT_Circle] = new HoldTail(tex, width);
			} else {
				//this->setHTailDefaults();
				this->m_pHoldTails = new HoldTail*[4];
				for (int i = 0; i < 4; i++) this->m_pHoldTails[i] = new HoldTail(nullptr,0);
			}
		} else {
			this->setCTailDefaults();
			//this->setHTailDefaults();
			this->m_pHoldTails = new HoldTail*[4];
			for (int i = 0; i < 4; i++) this->m_pHoldTails[i] = new HoldTail(nullptr, 0);
		}

		CryLog("[TailResource] Finsihed Loading Tail Resources.");
	}

	TailResource::~TailResource() {
		/*for (int i = 0; i < this->m_numHTailStyles; i++) { //for each tail style
			for (int i2 = 0; i2 < 4; i2++) //for each hold tail
				delete this->m_pHStyles[i].m_pHoldTails[i2]; //delete the tail
		}

		delete[] this->m_pHStyles; //delete the hold tail style*/

		for (int i = 0; i < 4; i++) delete this->m_pHoldTails[i];
		delete[] this->m_pHoldTails;

		for (int i = 0; i < eNT_Count; i++) //for each comet tail
			delete this->m_pCometTails[i]; //delete the tail
		delete[] this->m_pCometTails; //delete the array
	}

	void TailResource::loadCTail(XmlNodeRef comet) {
		this->m_pCometTails = new CometTail*[eNT_Count];

		Vec4 col;

		XmlNodeRef cometCR = comet->findChild("cross");
		XmlNodeRef cometC = comet->findChild("circle");
		XmlNodeRef cometS = comet->findChild("square");
		XmlNodeRef cometT = comet->findChild("triangle");

		XmlNodeRef cometL = comet->findChild("left");
		XmlNodeRef cometR = comet->findChild("right");
		XmlNodeRef cometU = comet->findChild("up");
		XmlNodeRef cometD = comet->findChild("down");

		XmlNodeRef cometST = comet->findChild("star");
		//XmlNodeRef cometBS = comet->findChild("bigstar");
		XmlNodeRef cometSL = comet->findChild("swipeL");
		XmlNodeRef cometSR = comet->findChild("swipeR");

		cometCR->getAttr("color", col);
		m_pCometTails[eNT_Cross] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		cometC->getAttr("color", col);
		m_pCometTails[eNT_Circle] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		cometS->getAttr("color", col);
		m_pCometTails[eNT_Square] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		cometT->getAttr("color", col);
		m_pCometTails[eNT_Triangle] = new CometTail(ColorF(col.x, col.y, col.z, col.w));

		cometL->getAttr("color", col);
		m_pCometTails[eNT_Left] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		cometR->getAttr("color", col);
		m_pCometTails[eNT_Right] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		cometU->getAttr("color", col);
		m_pCometTails[eNT_Up] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		cometD->getAttr("color", col);
		m_pCometTails[eNT_Down] = new CometTail(ColorF(col.x, col.y, col.z, col.w));

		cometST->getAttr("color", col);
		m_pCometTails[eNT_Star] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		
		//big star doesnt need a tail as it only uses rainbow because of being in chance time
		m_pCometTails[eNT_BigStar] = new CometTail(ColorF(0, 0, 0, 0));

		cometSL->getAttr("color", col);
		m_pCometTails[eNT_SwipeL] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
		cometSR->getAttr("color", col);
		m_pCometTails[eNT_SwipeR] = new CometTail(ColorF(col.x, col.y, col.z, col.w));
	}

	/*void TailResource::loadHTail(XmlNodeRef hold) {
		this->m_numHTailStyles = hold->getChildCount();

		this->m_pHStyles = new HoldTailStyle[this->m_numHTailStyles];

		//#pragma omp parallel for //parallize the hell out of this!
		for (int i = 0; i < this->m_numHTailStyles; i++) { //for each style
			XmlNodeRef style = hold->getChild(i); //get the style

			this->m_pHStyles[i].name = style->getAttr("name"); //set the name

			XmlNodeRef hHNote;
			unsigned int numLinesHN;
			HoldTailGeom * geomsHN;

			//get triangle for that style
			hHNote = style->findChild("triangle");
			numLinesHN = hHNote->getChildCount(); //number of lines
			geomsHN = new HoldTailGeom[numLinesHN]; //create the geom list
			#pragma omp parallel for //parallize the hell out of this!
			for (int i2 = 0; i2 < numLinesHN; i2++) { //for each line
				XmlNodeRef line = hHNote->getChild(i2); //get the line
				Vec4 col;
				//get color1
				line->getAttr("color1", col);
				geomsHN[i2].color1 = ColorF(col.x, col.y, col.z, col.w);
				//get color2
				line->getAttr("color2", col);
				geomsHN[i2].color2 = ColorF(col.x, col.y, col.z, col.w);

				line->getAttr("d1", geomsHN[i2].d1); //get distance 1
				line->getAttr("d2", geomsHN[i2].d2); //get distance 2
			}
			this->m_pHStyles[i].m_pHoldTails[eNT_Triangle] = new HoldTail(geomsHN, numLinesHN); //create the hold tail
			delete[] geomsHN; //release the memory for the geom list

			//get circle for that style
			hHNote = style->findChild("circle");
			numLinesHN = hHNote->getChildCount(); //number of lines
			geomsHN = new HoldTailGeom[numLinesHN]; //create the geom list
			#pragma omp parallel for //parallize the hell out of this!
			for (int i2 = 0; i2 < numLinesHN; i2++) { //for each line
				XmlNodeRef line = hHNote->getChild(i2); //get the line
				Vec4 col;
				//get color1
				line->getAttr("color1", col);
				geomsHN[i2].color1 = ColorF(col.x, col.y, col.z, col.w);
				//get color2
				line->getAttr("color2", col);
				geomsHN[i2].color2 = ColorF(col.x, col.y, col.z, col.w);

				line->getAttr("d1", geomsHN[i2].d1); //get distance 1
				line->getAttr("d2", geomsHN[i2].d2); //get distance 2
			}
			this->m_pHStyles[i].m_pHoldTails[eNT_Circle] = new HoldTail(geomsHN, numLinesHN); //create the hold tail
			delete[] geomsHN; //release the memory for the geom list

			//get cross for that style
			hHNote = style->findChild("cross");
			numLinesHN = hHNote->getChildCount(); //number of lines
			geomsHN = new HoldTailGeom[numLinesHN]; //create the geom list
			#pragma omp parallel for //parallize the hell out of this!
			for (int i2 = 0; i2 < numLinesHN; i2++) { //for each line
				XmlNodeRef line = hHNote->getChild(i2); //get the line
				Vec4 col;
				//get color1
				line->getAttr("color1", col);
				geomsHN[i2].color1 = ColorF(col.x, col.y, col.z, col.w);
				//get color2
				line->getAttr("color2", col);
				geomsHN[i2].color2 = ColorF(col.x, col.y, col.z, col.w);

				line->getAttr("d1", geomsHN[i2].d1); //get distance 1
				line->getAttr("d2", geomsHN[i2].d2); //get distance 2
			}
			this->m_pHStyles[i].m_pHoldTails[eNT_Cross] = new HoldTail(geomsHN, numLinesHN); //create the hold tail
			delete[] geomsHN; //release the memory for the geom list

			//get square for that style
			hHNote = style->findChild("square");
			numLinesHN = hHNote->getChildCount(); //number of lines
			geomsHN = new HoldTailGeom[numLinesHN]; //create the geom list
			#pragma omp parallel for //parallize the hell out of this!
			for (int i2 = 0; i2 < numLinesHN; i2++) { //for each line
				XmlNodeRef line = hHNote->getChild(i2); //get the line
				Vec4 col;
				//get color1
				line->getAttr("color1", col);
				geomsHN[i2].color1 = ColorF(col.x, col.y, col.z, col.w);
				//get color2
				line->getAttr("color2", col);
				geomsHN[i2].color2 = ColorF(col.x, col.y, col.z, col.w);

				line->getAttr("d1", geomsHN[i2].d1); //get distance 1
				line->getAttr("d2", geomsHN[i2].d2); //get distance 2
			}
			this->m_pHStyles[i].m_pHoldTails[eNT_Square] = new HoldTail(geomsHN, numLinesHN); //create the hold tail
			delete[] geomsHN; //release the memory for the geom list
		}
	}*/

	/*const char * TailResource::getHTailStyleName(unsigned int index) {
		if (index < 0 && index >= this->m_numHTailStyles) return "";
		return this->m_pHStyles[index].name;
	}

	void TailResource::setHTailStyle(unsigned int index) {
		if (index < 0 && index >= this->m_numHTailStyles) return;
		this->m_hTailStyle = index;
	}*/

	void TailResource::setCTailDefaults() {
		//defaults
		this->m_pCometTails[eNT_Circle] = new CometTail(ColorF(1.0f, 0.0f, 0.0f, 0.75f));
		this->m_pCometTails[eNT_Cross] = new CometTail(ColorF(0.0f, 0.0f, 1.0f, 0.75f));
		this->m_pCometTails[eNT_Square] = new CometTail(ColorF(0.737255f, 0.560784f, 0.560784f, 0.75f));
		this->m_pCometTails[eNT_Triangle] = new CometTail(ColorF(0.0f, 1.0f, 0.0f, 0.75f));

		this->m_pCometTails[eNT_Left] = new CometTail(ColorF(0.737255f, 0.560784f, 0.560784f, 0.75f));
		this->m_pCometTails[eNT_Right] = new CometTail(ColorF(1.0f, 0.0f, 0.0f, 0.75f));
		this->m_pCometTails[eNT_Up] = new CometTail(ColorF(0.0f, 1.0f, 0.0f, 0.75f));
		this->m_pCometTails[eNT_Down] = new CometTail(ColorF(0.0f, 0.0f, 1.0f, 0.75f));

		this->m_pCometTails[eNT_Star] = new CometTail(ColorF(0.85f, 0.85f, 0.10f, 0.75f));
		this->m_pCometTails[eNT_BigStar] = new CometTail(ColorF(0, 0, 0, 0));
		this->m_pCometTails[eNT_SwipeL] = new CometTail(ColorF(0.85f, 0.85f, 0.10f, 0.75f));
		this->m_pCometTails[eNT_SwipeR] = new CometTail(ColorF(0.85f, 0.85f, 0.10f, 0.75f));
	}

	/*void TailResource::setHTailDefaults() {
		this->m_numHTailStyles = 1;
		this->m_pHStyles = new HoldTailStyle[1];

		this->m_pHStyles[0].name = "Default";

		HoldTailGeom geom[2];

		geom[0].color1 = ColorF(0.56f, 0.85f, 0.85f, 1.0f); //light color
		geom[0].color2 = ColorF(0.0f, 0.0f, 1.0f, 1.0f); //solid color
		geom[0].d1 = 0;
		geom[0].d2 = 3;
		geom[1].color1 = ColorF(0.56f, 0.85f, 0.85f, 1.0f); //light color
		geom[1].color2 = ColorF(0.0f, 0.0f, 1.0f, 1.0f); //solid color
		geom[1].d1 = 0;
		geom[1].d2 = -3;

		this->m_pHStyles[0].m_pHoldTails[eNT_Circle] = new HoldTail(geom, 2);

		geom[0].color1 = ColorF(1.0f, 0.41f, 0.71f, 1.0f); //light color
		geom[0].color2 = ColorF(1.0f, 0.0f, 0.0f, 1.0f); //solid color
		geom[0].d1 = 0;
		geom[0].d2 = 3;
		geom[1].color1 = ColorF(1.0f, 0.41f, 0.71f, 1.0f); //light color
		geom[1].color2 = ColorF(1.0f, 0.0f, 0.0f, 1.0f); //solid color
		geom[1].d1 = 0;
		geom[1].d2 = -3;

		this->m_pHStyles[0].m_pHoldTails[eNT_Cross] = new HoldTail(geom, 2);

		geom[0].color1 = ColorF(1.0f, 0.75f, 0.80f, 1.0f); //light color
		geom[0].color2 = ColorF(0.98f, 0.5f, 0.45f, 1.0f); //solid color
		geom[0].d1 = 0;
		geom[0].d2 = 3;
		geom[1].color1 = ColorF(1.0f, 0.75f, 0.80f, 1.0f); //light color
		geom[1].color2 = ColorF(0.98f, 0.5f, 0.45f, 1.0f); //solid color
		geom[1].d1 = 0;
		geom[1].d2 = -3;

		this->m_pHStyles[0].m_pHoldTails[eNT_Square] = new HoldTail(geom, 2);

		geom[0].color1 = ColorF(0.56f, 0.93f, 0.56f, 1.0f); //light color
		geom[0].color2 = ColorF(1.0f, 0.0f, 0.0f, 1.0f); //solid color
		geom[0].d1 = 0;
		geom[0].d2 = 3;
		geom[1].color1 = ColorF(0.56f, 0.93f, 0.56f, 1.0f); //light color
		geom[1].color2 = ColorF(1.0f, 0.0f, 0.0f, 1.0f); //solid color
		geom[1].d1 = 0;
		geom[1].d2 = -3;

		this->m_pHStyles[0].m_pHoldTails[eNT_Triangle] = new HoldTail(geom, 2);
	}*/
}