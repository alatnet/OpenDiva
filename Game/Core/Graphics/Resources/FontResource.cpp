#include <StdAfx.h>
#include "FontResource.h"

namespace LYGame {
	/*
	<fontresource>
		<font path="Fonts/PositecBold.xml" size="24" />
		<text>
			<cool>COOL</cool>
			<fine>FINE</fine>
			<safe>SAFE</safe>
			<sad>SAD</sad>
			<worst>BAD</worst>
    
			<misscool>COOL?</misscool>
			<missfine>FINE?</missfine>
			<misssafe>SAFE?</misssafe>
			<misssad>SAD?</misssad>

			<rushcross>RUSH</rushcross>
			<rushsquare>RUSH</rushsquare>
			<rushtriangle>RUSH</rushtriangle>
			<rushsquare>RUSH</rushsquare>
		</text>
	</fontresource>
	*/

	FontResource::FontResource(const char * folder) {
		CryLog("[FontResource] Loading Font Resource- Path: %s", folder);

		//default settings
		//m_textPointSize = 36.0f;
		for (int i = 0; i < NUMFONTEFFECTS(14); i++) m_textPointSize[i] = 36.0f;
		m_ImgScale = m_PosScale = Vec2(1.0f,1.0f);

		const char * fontPath = "Fonts/PositecBold.xml";

		for (int i = 0, x = 0; i < eHS_None + 4; i++) {
			x = i;
			if (x > eHS_None - 1) x = eHS_None - 1;
			this->m_ratingText[i] = sRatingEffectNames[x];
		}

		for (int i = 0; i < 4; i++) this->m_rushText[i] = "RUSH";

		//rating font configuration
		{
			//find the configuration
			string ratingConfig = folder;
			ratingConfig.append("/");
			ratingConfig.append(sFontConfigFileName);
			XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(ratingConfig);

			if (config) {
				CryLog("[FontResource] Found config.");

				//get the font child
				XmlNodeRef font = config->findChild("font");

				//get the font path
				fontPath = font->getAttr("path");
				//font->getAttr("size", m_textPointSize);

				//CryLog("[FontResource] Font: %s - Size: %f", fontPath, m_textPointSize);

				//get the text child
				XmlNodeRef text = config->findChild("text");

				//get text sizes
				for (int i = 0; i < NUMFONTEFFECTS(14); i++) text->findChild(sRatingEffectNames[i])->getAttr("size", this->m_textPointSize[i]);

				//get text for ratings
				for (int i = 0; i < eHS_None + 4; i++) {
					this->m_ratingText[i] = text->findChild(sRatingEffectNames[i])->getContent();
					CryLog("[FontResource] Rating Text: %s = %s", sRatingEffectNames[i], this->m_ratingText[i]);
				}

				//get rush text
				for (int i = 0; i < 4; i++) {
					this->m_rushText[i] = text->findChild(sRatingEffectNames[RUSHFONT(i)])->getContent();
					CryLog("[FontResource] Rush Text: %s = %s", sRatingEffectNames[RUSHFONT(i)], this->m_rushText[i]);
				}

				//get hold text
				for (int i = 0; i < 4; i++) {
					this->m_holdText[i] = text->findChild(sRatingEffectNames[HOLDFONT(i)])->getContent();
					CryLog("[FontResource] Hold Text: %s = %s", sRatingEffectNames[HOLDFONT(i)], this->m_holdText[i]);
				}
			}
		}

		//load font
		this->m_pFont = gEnv->pSystem->GetICryFont()->NewFont("DivaFontResource");
		this->m_pFont->Load(fontPath);
		this->m_pFont->AddRef();

		//list font effects
		for (int i = 0; i < this->m_pFont->GetNumEffects(); i++) CryLog("(FontResource) [Font Effect] %i - %s", i, this->m_pFont->GetEffectName(i));

		//setup font context
		for (int i = 0; i < NUMFONTEFFECTS(14) ; i++) {
			this->m_pFontDrawCtx[i].SetFlags(eDrawText_Center | eDrawText_CenterV | eDrawText_2D | eDrawText_FixedSize);
			this->m_pFontDrawCtx[i].SetSizeIn800x600(false);
			this->m_pFontDrawCtx[i].SetSize(vector2f(m_textPointSize[i], m_textPointSize[i]));
			this->m_pFontDrawCtx[i].SetProportional(true);
			this->m_pFontDrawCtx[i].SetEffect(this->m_pFont->GetEffectId(sRatingEffectNames[i]));
		}

		//setup text and sizes
		for (int i = 0; i < eHS_None + 4; i++) {
			this->m_ratingTextSizes[i] = this->m_pFont->GetTextSize(this->m_ratingText[i], false, this->m_pFontDrawCtx[i]);
			CryLog("(FontResource) [Text Sizes] %s - %f, %f", this->m_ratingText[i], this->m_ratingTextSizes[i].x, this->m_ratingTextSizes[i].y);
		}

		//setup rush text and sizes
		for (int i = 0; i < 4; i++) {
			this->m_rushTextSize[i] = this->m_pFont->GetTextSize(this->m_rushText[i], false, this->m_pFontDrawCtx[RUSHFONT(i)]);
			CryLog("(FontResource) [Rush Text Sizes] (%i - %i) %s - %f, %f", i, RUSHFONT(i), this->m_rushText[i], this->m_rushTextSize[i].x, this->m_rushTextSize[i].y);
		}

		//setup hold text and sizes
		for (int i = 0; i < 4; i++) {
			this->m_holdTextSize[i] = this->m_pFont->GetTextSize(this->m_holdText[i], false, this->m_pFontDrawCtx[HOLDFONT(i)]);
			CryLog("(FontResource) [Hold Text Sizes] (%i - %i) %s - %f, %f", i, HOLDFONT(i), this->m_holdText[i], this->m_holdTextSize[i].x, this->m_holdTextSize[i].y);
		}

		CryLog("[FontResource] Finished Loading Font Resource");
	}

	FontResource::~FontResource() {
		this->m_pFont->Release();
	}

	void FontResource::setImgScale(Vec2 scale) {
		for (int i = 0; i < NUMFONTEFFECTS(14); i++) this->m_pFontDrawCtx[i].SetSize(vector2f(m_textPointSize[i]*scale.x, m_textPointSize[i]*scale.y));
		for (int i = 0; i < eHS_None + 4; i++) this->m_ratingTextSizes[i] = this->m_pFont->GetTextSize(this->m_ratingText[i], false, this->m_pFontDrawCtx[i]);
		for (int i = 0; i < 4; i++) this->m_rushTextSize[i] = this->m_pFont->GetTextSize(this->m_rushText[i], false, this->m_pFontDrawCtx[RUSHFONT(i)]);
	}

	void FontResource::setScale(Vec2 scale) {
		this->setImgScale(scale);
		this->setPosScale(scale);
	}

	Vec2 FontResource::getSize(int type) {
		if (type < 0 || type > eHS_None + 4) return Vec2();

		return this->m_ratingTextSizes[type];
	}

	Vec2 FontResource::getMultiSize(){
		return this->m_pFont->GetTextSize("1", false, this->m_pFontDrawCtx[MULTIFONT]);
	}

	Vec2 FontResource::getRushSize(int type) {
		if (type < 0 || type >= 4) return Vec2();
		return this->m_rushTextSize[type - RUSHFONT(0)];
	}

	Vec2 FontResource::getHoldSize(int type) {
		if (type < 0 || type >= 4) return Vec2();
		return this->m_rushTextSize[type - HOLDFONT(0)];
	}

	void FontResource::DrawRating(int type, Vec2 pos, bool drawCombo, unsigned int combo) {
		if (type > eHS_None + 4) return;
		int i = type;
		if (i > eHS_None + 3) i = eHS_None + 3;
		this->m_pFont->DrawString(
			(pos.x*this->m_PosScale.x) - (this->m_ratingTextSizes[type].x / 2),
			(pos.y*this->m_PosScale.y)-(this->m_ratingTextSizes[type].y/2),
			this->m_ratingText[i],
			false,
			this->m_pFontDrawCtx[type]
		);

		if (drawCombo) {
			string comboStr = " ";
			comboStr += std::to_string(combo).c_str();

			this->m_pFont->DrawString(
				((pos.x*this->m_PosScale.x) - (this->m_ratingTextSizes[type].x / 2)) + (this->m_ratingTextSizes[type].x),
				(pos.y*this->m_PosScale.y) - (this->m_ratingTextSizes[type].y / 2),
				comboStr,
				false,
				this->m_pFontDrawCtx[COMBOFONT]
			);
		}

		#ifdef FONTRESOURCE_DEBUG
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}

	void FontResource::DrawMult(Vec2 pos, unsigned int mult) {
		string multString = "+";
		multString.append(std::to_string(mult).c_str());
		Vec2 stringSize = this->m_pFont->GetTextSize(multString, false, this->m_pFontDrawCtx[MULTIFONT]);

		this->m_pFont->DrawString(
			(pos.x*this->m_PosScale.x) - (stringSize.x / 2),
			(pos.y*this->m_PosScale.y) - (stringSize.y / 2),
			multString,
			false,
			this->m_pFontDrawCtx[MULTIFONT]
		);

		#ifdef FONTRESOURCE_DEBUG
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}

	void FontResource::DrawRush(ENoteType type, Vec2 pos) {
		if (type >= 4) return;
		this->m_pFont->DrawString(
			(pos.x*this->m_PosScale.x) - (this->m_rushTextSize[type].x / 2),
			(pos.y*this->m_PosScale.y) - (this->m_rushTextSize[type].y / 2),
			this->m_rushText[type],
			false,
			this->m_pFontDrawCtx[RUSHFONT(type)]
			);

		#ifdef FONTRESOURCE_DEBUG
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}

	void FontResource::DrawHold(ENoteType type, Vec2 pos) {
		if (type >= 4) return;
		this->m_pFont->DrawString(
			(pos.x*this->m_PosScale.x) - (this->m_holdTextSize[type].x / 2),
			(pos.y*this->m_PosScale.y) - (this->m_holdTextSize[type].y / 2),
			this->m_holdText[type],
			false,
			this->m_pFontDrawCtx[HOLDFONT(type)]
			);

		#ifdef FONTRESOURCE_DEBUG
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}
}