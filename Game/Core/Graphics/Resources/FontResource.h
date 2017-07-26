#ifndef _H_FONTRESOURCE_
#define _H_FONTRESOURCE_
#pragma once

//Positec seems to be the font type.

namespace LYGame {
	#define MISSRATING(x) eHS_None + x
	#define COMBOFONT MISSRATING(4)
	#define MULTIFONT MISSRATING(5)
	#define RUSHFONT(x) MISSRATING(6) + x
	#define HOLDFONT(x) MISSRATING(10) + x
	#define NUMFONTEFFECTS(x) eHS_None + x

	//#define FONTRESOURCE_DEBUG 1

	static const char * sRatingEffectNames[NUMFONTEFFECTS(14)]{
		"cool",
		"fine",
		"safe",
		"sad",
		"worst", //eHS_None (5)

		"misscool",
		"missfine",
		"misssafe",
		"misssad", //eHS_None + 4

		"combo", //eHS_None + 5
		"mult", //eHS_None + 6

		"rushcross",
		"rushcircle",
		"rushsquare",
		"rushtriangle", //eHS_None + 10

		"holdcross",
		"holdcircle",
		"holdsquare",
		"holdtriangle" //eHS_None + 14
	};

	static const char * sFontConfigFileName = "fontConfig.xml";

	class FontResource {
	public:
		FontResource(const char * folder);
		~FontResource();
	public:
		void DrawRating(int type, AZ::Vector2 pos, bool drawCombo = false, unsigned int combo = 0);
		void DrawMult(AZ::Vector2 pos, unsigned int mult);
		void DrawRush(ENoteType type, AZ::Vector2 pos);
		void DrawHold(ENoteType type, AZ::Vector2 pos);
	public:
		void setImgScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));
		void setPosScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f)) { this->m_PosScale = scale; }
		void setScale(AZ::Vector2 scale = AZ::Vector2(1.0f, 1.0f));
	public:
		AZ::Vector2 getSize(int type);
		AZ::Vector2 getMultiSize();
		AZ::Vector2 getRushSize(int type);
		AZ::Vector2 getHoldSize(int type);
	private:
		IFFont * m_pFont; //the all mighty font
		STextDrawContext m_pFontDrawCtx[NUMFONTEFFECTS(14)]; //font contexts for all text
		float m_textPointSize[NUMFONTEFFECTS(14)]; //point sizes for all text
		AZ::Vector2 m_ImgScale, m_PosScale; //scales

		//rating
		AZStd::string m_ratingText[eHS_None + 4];
		AZ::Vector2 m_ratingTextSizes[eHS_None + 4];

		//rush
		AZStd::string m_rushText[4];
		AZ::Vector2 m_rushTextSize[4];

		//hold
		AZStd::string m_holdText[4];
		AZ::Vector2 m_holdTextSize[4];
	};
}

#endif //_H_RATINGRESOURCE_