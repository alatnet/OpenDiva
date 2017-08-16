#include <StdAfx.h>
#include "RatingResource.h"

namespace LYGame {
	IDraw2d *RatingResource::iDraw2d = NULL;
	IRenderer *RatingResource::iRenderer = NULL;

	RatingResource::RatingResource(const char * folder) : m_ImgScale({ 1.0f, 1.0f }), m_PosScale({ 1.0f, 1.0f }) {
		CryLog("[RatingResource] Loading Rating Resources - Path: %s", folder);

		if (RatingResource::iRenderer == NULL) RatingResource::iRenderer = gEnv->pSystem->GetIRenderer();
		if (RatingResource::iDraw2d == NULL) RatingResource::iDraw2d = Draw2dHelper::GetDraw2d();

		this->m_ImgScale = Vec2(1.0f, 1.0f);
		this->m_PosScale = Vec2(1.0f, 1.0f);

		this->m_RatingSizes[eHS_Cool] = { 46.6875, 15.1875 };
		this->m_RatingSizes[eHS_Fine] = { 49.5, 15.1875 };
		this->m_RatingSizes[eHS_Safe] = { 46.6875, 15.1875 };
		this->m_RatingSizes[eHS_Sad] = { 37.6875, 15.1875 };
		this->m_RatingSizes[eHS_Worst] = { 65.8125, 15.1875 };
		this->m_RatingSizes[MISSRATING(eHS_Cool)] = { 65.25, 15.1875 };
		this->m_RatingSizes[MISSRATING(eHS_Fine)] = { 65.25, 15.1875 };
		this->m_RatingSizes[MISSRATING(eHS_Safe)] = { 65.25, 15.1875 };
		this->m_RatingSizes[MISSRATING(eHS_Sad)] = { 65.25, 15.1875 };

		//target sizes for rating images.
		{
			string ratingConfig = folder;
			ratingConfig.append("/");
			ratingConfig.append(sRatingConfigFileName);
			XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(ratingConfig);

			if (config) {
				XmlNodeRef cool = config->findChild("cool");
				XmlNodeRef fine = config->findChild("fine");
				XmlNodeRef safe = config->findChild("safe");
				XmlNodeRef sad = config->findChild("sad");
				XmlNodeRef worst = config->findChild("worst");

				cool->getAttr("size", this->m_RatingSizes[eHS_Cool]);
				cool->getAttr("sizeMiss", this->m_RatingSizes[MISSRATING(eHS_Cool)]);
				fine->getAttr("size", this->m_RatingSizes[eHS_Fine]);
				fine->getAttr("sizeMiss", this->m_RatingSizes[MISSRATING(eHS_Fine)]);
				safe->getAttr("size", this->m_RatingSizes[eHS_Safe]);
				safe->getAttr("sizeMiss", this->m_RatingSizes[MISSRATING(eHS_Safe)]);
				sad->getAttr("size", this->m_RatingSizes[eHS_Sad]);
				sad->getAttr("sizeMiss", this->m_RatingSizes[MISSRATING(eHS_Sad)]);
				worst->getAttr("size", this->m_RatingSizes[eHS_Worst]);
			}
		}

		#pragma omp parallel for
		for (int i = 0; i < eHS_None + 4; i++) {
			//effect image itself
			string effectFilename = folder;
			effectFilename.append("/");
			effectFilename.append(sRatingFileNames[i]);

			this->m_pRatingTextures[i] = RatingResource::iRenderer->EF_LoadTexture(effectFilename, FT_DONT_STREAM);

			assert(m_pRatingTextures[i] != NULL);
		}

		CryLog("[RatingResource] Finished Loading Rating Resources.");
	}

	RatingResource::~RatingResource() {
		for (int i = 0; i < eHS_None + 4; i++)
			this->m_pRatingTextures[i]->Release();
	}

	void RatingResource::Draw(int type, Vec2 pos) {
		pos.x = pos.x * m_PosScale.x;
		pos.y = pos.y * m_PosScale.y;

		RatingResource::iDraw2d->DrawImageAligned(
			this->m_pRatingTextures[type]->GetTextureID(),
			pos,
			Vec2(this->m_RatingSizes[type].x*m_ImgScale.x, this->m_RatingSizes[type].y*m_ImgScale.y),
			IDraw2d::HAlign::Center, IDraw2d::VAlign::Center
		);
	}

	/*
	<ratingfont>
	  <font path="" size="36" />
	  <ratingText>
	    <cool>COOL</cool>
		<fine>FINE</fine>
		<safe>SAFE</safe>
		<sad>SAD</sad>
		<worst>WORST</worst>
	  </ratingText>
	</ratingfont>
	*/

	/*
	RatingResourceFont::RatingResourceFont(const char * folder) {
		CryLog("[RatingResourceFont] Loading Rating Font - Path: %s", folder);

		//default settings
		m_textPointSize = 36.0f;
		m_ImgScale = m_PosScale = Vec2(1.0f,1.0f);

		const char * fontPath = "Fonts/PositecBold.xml";

		for (int i = 0, x = 0; i < eHS_None + 4; i++) {
			x = i;
			if (x > eHS_None - 1) x = eHS_None - 1;
			this->m_ratingText[i] = sRatingEffectNames[x];
		}

		//rating font configuration
		{
			string ratingConfig = folder;
			ratingConfig.append("/");
			ratingConfig.append(sRatingFontConfigFileName);
			XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(ratingConfig);

			if (config) {
				CryLog("[RatingResourceFont] Fount config.");

				XmlNodeRef font = config->findChild("font");

				fontPath = font->getAttr("path");
				font->getAttr("size", m_textPointSize);

				CryLog("[RatingResourceFont] Font: %s - Size: %i", fontPath, m_textPointSize);

				XmlNodeRef text = config->findChild("ratingText");
				for (int i = 0; i < eHS_None + 4; i++) {
					this->m_ratingText[i] = text->findChild(sRatingEffectNames[i])->getContent();
					CryLog("[RatingResourceFont] Rating: %s = %s", sRatingEffectNames[i], this->m_ratingText[i]);
				}
			}
		}

		//load font
		this->m_pFont = gEnv->pSystem->GetICryFont()->NewFont("DivaRatingFont");
		this->m_pFont->Load(fontPath);
		this->m_pFont->AddRef();

		//list font effects
		for (int i = 0; i < this->m_pFont->GetNumEffects(); i++) CryLog("(RatingResourceFont) [Font Effect] %i - %s", i, this->m_pFont->GetEffectName(i));

		//setup font context
		for (int i = 0; i < eHS_None + 6; i++) {
			this->m_pFontDrawCtx[i].SetFlags(eDrawText_Center | eDrawText_CenterV | eDrawText_2D | eDrawText_FixedSize);
			this->m_pFontDrawCtx[i].SetSizeIn800x600(false);
			this->m_pFontDrawCtx[i].SetSize(vector2f(m_textPointSize, m_textPointSize));
			this->m_pFontDrawCtx[i].SetProportional(true);
			this->m_pFontDrawCtx[i].SetEffect(this->m_pFont->GetEffectId(sRatingEffectNames[i]));
		}

		//setup text and sizes
		for (int i = 0; i < eHS_None + 4; i++) {
			this->m_stringSizes[i] = this->m_pFont->GetTextSize(this->m_ratingText[i], false, this->m_pFontDrawCtx[i]);
			CryLog("(RatingResourceFont) [Text Sizes] %s - %f, %f", this->m_ratingText[i], this->m_stringSizes[i].x, this->m_stringSizes[i].y);
		}
	}

	RatingResourceFont::~RatingResourceFont() {
		this->m_pFont->Release();
	}

	void RatingResourceFont::setImgScale(Vec2 scale) {
		for (int i = 0; i < eHS_None + 6; i++) this->m_pFontDrawCtx[i].SetSize(vector2f(m_textPointSize*scale.x, m_textPointSize*scale.y));
		for (int i = 0; i < eHS_None + 4; i++) this->m_stringSizes[i] = this->m_pFont->GetTextSize(this->m_ratingText[i], false, this->m_pFontDrawCtx[i]);
	}

	void RatingResourceFont::setScale(Vec2 scale) {
		this->setImgScale(scale);
		this->setPosScale(scale);
	}


	Vec2 RatingResourceFont::getSize(int type) {
		if (type > eHS_None + 4) return Vec2();
		return this->m_stringSizes[type];
	}

	void RatingResourceFont::Draw(int type, Vec2 pos, unsigned int combo) {
		if (type > eHS_None + 4) return;
		int i = type;
		if (i > eHS_None + 3) i = eHS_None + 3;
		this->m_pFont->DrawString(
			(pos.x*this->m_PosScale.x) - (this->m_stringSizes[type].x / 2),
			(pos.y*this->m_PosScale.y)-(this->m_stringSizes[type].y/2),
			this->m_ratingText[i],
			false,
			this->m_pFontDrawCtx[type]
		);

		if (combo >= 5) {
			this->m_pFont->DrawString(
				((pos.x*this->m_PosScale.x) - (this->m_stringSizes[type].x / 2)) + (this->m_stringSizes[type].x),
				(pos.y*this->m_PosScale.y) - (this->m_stringSizes[type].y / 2),
				std::to_string(combo).c_str(),
				false,
				this->m_pFontDrawCtx[eHS_None + 4]
				);
		}

		Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
	}

	void RatingResourceFont::DrawMult(Vec2 pos, unsigned int holdMult) {
		string holdMultString = "+";
		holdMultString.append(std::to_string(holdMult).c_str());
		Vec2 stringSize = this->m_pFont->GetTextSize(holdMultString, false, this->m_pFontDrawCtx[eHS_None + 5]);

		this->m_pFont->DrawString(
			(pos.x*this->m_PosScale.x) - (stringSize.x / 2),
			(pos.y*this->m_PosScale.y) - (stringSize.y / 2),
			holdMultString,
			false,
			this->m_pFontDrawCtx[eHS_None + 5]
			);

		Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
	}*/
}