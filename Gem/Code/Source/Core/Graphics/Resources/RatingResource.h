#ifndef _H_RATINGRESOURCE_
#define _H_RATINGRESOURCE_
#pragma once

//switch to using font file.
//Positec seems to be the font type.

namespace OpenDiva {
	#define MISSRATING(x) eHS_None + x
	static char const* sRatingFileNames[eHS_None + 4] {
		"cool.dds",
		"fine.dds",
		"safe.dds",
		"sad.dds",
		"worst.dds",
		"misscool.dds",
		"missfine.dds",
		"misssafe.dds",
		"misssad.dds"
	};

	static const char * sRatingConfigFileName = "ratingConfig.xml";

	//retire this class.
	class RatingResource {
	public:
		RatingResource(const char * folder);
		~RatingResource();
	public:
		void Draw(int type, Vec2 pos);
	public:
		int getWidth(int type) { return this->m_pRatingTextures[type]->GetWidth(); }
		int getHeight(int type) { return this->m_pRatingTextures[type]->GetHeight(); }
	public:
		void setImgScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_ImgScale = scale; }
		void setPosScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_PosScale = scale; }
		void setScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->setImgScale(scale); this->setPosScale(scale); }
	private:
		ITexture * m_pRatingTextures[eHS_None + 4];
		Vec2 m_ImgScale, m_PosScale;
		Vec2 m_RatingSizes[eHS_None + 4];
	private:
		static IRenderer * iRenderer;
		static IDraw2d * iDraw2d;
	};

	/*
	static const char * sRatingEffectNames[eHS_None + 6]{
		"cool",
		"fine",
		"safe",
		"sad",
		"worst",
		"misscool",
		"missfine",
		"misssafe",
		"misssad",
		"combo",
		"hold"
	};

	static const char * sRatingFontConfigFileName = "ratingFontConfig.xml";

	//add in rush text
	//rush text are individualised colors
	class RatingResourceFont {
	public:
		RatingResourceFont(const char * folder);
		~RatingResourceFont();
	public:
		void Draw(int type, Vec2 pos, unsigned int combo = 0);
		void DrawMult(Vec2 pos, unsigned int holdMult);
	public:
		void setImgScale(Vec2 scale = Vec2(1.0f, 1.0f));
		void setPosScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_PosScale = scale; }
		void setScale(Vec2 scale = Vec2(1.0f, 1.0f));
	public:
		Vec2 getSize(int type);
	private:
		IFFont * m_pFont;
		STextDrawContext m_pFontDrawCtx[eHS_None + 6];
		Vec2 m_stringSizes[eHS_None + 4];
		float m_textPointSize;
		Vec2 m_ImgScale, m_PosScale;

		string m_ratingText[eHS_None + 4];
	};*/
}

#endif //_H_RATINGRESOURCE_