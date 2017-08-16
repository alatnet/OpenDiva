#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "FontResource.h"

#include <AzCore\std\string\regex.h>

namespace OpenDiva {
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
		m_ImgScale = m_PosScale = AZ::Vector2(1.0f,1.0f);

		AZStd::string fontXML = "Fonts/PositecBold.xml";

		for (int i = 0, x = 0; i < eHS_None + 4; i++) {
			x = i;
			if (x > eHS_None - 1) x = eHS_None - 1;
			this->m_ratingText[i] = sRatingEffectNames[x];
		}

		for (int i = 0; i < 4; i++) this->m_rushText[i] = "RUSH";

		//rating font configuration
		{
			//find the configuration
			AZStd::string ratingConfig = folder;
			ratingConfig.append("/");
			ratingConfig.append(sFontConfigFileName);
			XmlNodeRef config = gEnv->pSystem->LoadXmlFromFile(ratingConfig.c_str());

			if (config) {
				CryLog("[FontResource] Found config.");

				//get the font child
				XmlNodeRef font = config->findChild("font");

				//get the font xml path
				fontXML = font->getAttr("path");
				
				fontXML = AZStd::regex_replace(fontXML, AZStd::regex("\\$\\(Fonts\\)"), folder);

				//fontXML.replace("$(Fonts)", folder);
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

		//copy fonts to cache.
		Util::CopyToCache(fontXML, "Fonts/Style/"); //copy the xml file
		AZStd::string fontXMLCache = Util::GetCachePath(fontXML, "Fonts/Style/");
		XmlNodeRef fontXMLFile = gEnv->pSystem->LoadXmlFromFile(fontXMLCache.c_str()); //open the cache xml file version
		AZStd::string fontFilePath = fontXMLFile->findChild("font")->getAttr("path"); //get the font path
		fontXMLFile->findChild("font")->setAttr("path", PathUtil::GetFile(fontFilePath.c_str())); //edit the font path to be just the font name itself
		fontXMLFile->saveToFile(fontXMLCache.c_str()); //save the cache xml file version
		Util::CopyToCache(gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/") + fontFilePath,"Fonts/Style/"); //copy the font file itself to the cache

		CryLog("[FontResource] Font XML Cache Path: %s", fontXMLCache.c_str());
		CryLog("[FontResource] Font File Cache Path: %s", Util::GetCachePath(fontFilePath, "Fonts/Style/").c_str());
		CryLog("[FontResource] Font File Path: %s", AZStd::string(gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/") + fontFilePath).c_str());

		//load font
		this->m_pFont = gEnv->pSystem->GetICryFont()->NewFont("DivaFontResource");
		this->m_pFont->Load(fontXMLCache.c_str());
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
			Vec2 size = this->m_pFont->GetTextSize(this->m_ratingText[i].c_str(), false, this->m_pFontDrawCtx[i]);
			this->m_ratingTextSizes[i] = AZ::Vector2(size.x,size.y);
			CryLog("(FontResource) [Text Sizes] %s - %f, %f", this->m_ratingText[i], this->m_ratingTextSizes[i].GetX(), this->m_ratingTextSizes[i].GetY());
		}

		//setup rush text and sizes
		for (int i = 0; i < 4; i++) {
			Vec2 size = this->m_pFont->GetTextSize(this->m_rushText[i].c_str(), false, this->m_pFontDrawCtx[RUSHFONT(i)]);
			this->m_rushTextSize[i] = AZ::Vector2(size.x,size.y);
			CryLog("(FontResource) [Rush Text Sizes] (%i - %i) %s - %f, %f", i, RUSHFONT(i), this->m_rushText[i], this->m_rushTextSize[i].GetX(), this->m_rushTextSize[i].GetY());
		}

		//setup hold text and sizes
		for (int i = 0; i < 4; i++) {
			Vec2 size = this->m_pFont->GetTextSize(this->m_holdText[i].c_str(), false, this->m_pFontDrawCtx[HOLDFONT(i)]);
			this->m_holdTextSize[i] = AZ::Vector2(size.x,size.y);
			CryLog("(FontResource) [Hold Text Sizes] (%i - %i) %s - %f, %f", i, HOLDFONT(i), this->m_holdText[i], this->m_holdTextSize[i].GetX(), this->m_holdTextSize[i].GetY());
		}

		CryLog("[FontResource] Finished Loading Font Resource");
	}

	FontResource::~FontResource() {
		this->m_pFont->Release();
		Util::ClearCache("Fonts/Style");
	}

	void FontResource::setImgScale(AZ::Vector2 scale) {
		for (int i = 0; i < NUMFONTEFFECTS(14); i++) this->m_pFontDrawCtx[i].SetSize(vector2f(m_textPointSize[i]*scale.GetX(), m_textPointSize[i]*scale.GetY()));
		for (int i = 0; i < eHS_None + 4; i++) {
			Vec2 size = this->m_pFont->GetTextSize(this->m_ratingText[i].c_str(), false, this->m_pFontDrawCtx[i]);
			this->m_ratingTextSizes[i] = AZ::Vector2(size.x,size.y);
		}
		for (int i = 0; i < 4; i++) {
			Vec2 size = this->m_pFont->GetTextSize(this->m_rushText[i].c_str(), false, this->m_pFontDrawCtx[RUSHFONT(i)]);
			this->m_rushTextSize[i] = AZ::Vector2(size.x,size.y);
		}
	}

	void FontResource::setScale(AZ::Vector2 scale) {
		this->setImgScale(scale);
		this->setPosScale(scale);
	}

	AZ::Vector2 FontResource::getSize(int type) {
		if (type < 0 || type > eHS_None + 4) return AZ::Vector2();

		return this->m_ratingTextSizes[type];
	}

	AZ::Vector2 FontResource::getMultiSize(){
		Vec2 size = this->m_pFont->GetTextSize("1", false, this->m_pFontDrawCtx[MULTIFONT]);
		return AZ::Vector2(size.x, size.y);
	}

	AZ::Vector2 FontResource::getRushSize(int type) {
		if (type < 0 || type >= 4) return AZ::Vector2();
		return this->m_rushTextSize[type - RUSHFONT(0)];
	}

	AZ::Vector2 FontResource::getHoldSize(int type) {
		if (type < 0 || type >= 4) return AZ::Vector2();
		return this->m_rushTextSize[type - HOLDFONT(0)];
	}

	void FontResource::DrawRating(int type, AZ::Vector2 pos, bool drawCombo, unsigned int combo) {
		if (type > eHS_None + 4) return;
		int i = type;
		if (i > eHS_None + 3) i = eHS_None + 3;

		AZ::Vector2 position = (pos*this->m_PosScale) - (this->m_ratingTextSizes[type] / 2);

		this->m_pFont->DrawString(
			//(pos.x*this->m_PosScale.x) - (this->m_ratingTextSizes[type].x / 2),
			//(pos.y*this->m_PosScale.y)-(this->m_ratingTextSizes[type].y/2),
			position.GetX(),
			position.GetY(),
			this->m_ratingText[i].c_str(),
			false,
			this->m_pFontDrawCtx[type]
		);

		if (drawCombo) {
			AZStd::string comboStr = " ";
			comboStr += std::to_string(combo).c_str();

			this->m_pFont->DrawString(
				((pos.GetX()*this->m_PosScale.GetX()) - (this->m_ratingTextSizes[type].GetX() / 2)) + (this->m_ratingTextSizes[type].GetX()),
				(pos.GetY()*this->m_PosScale.GetY()) - (this->m_ratingTextSizes[type].GetY() / 2),
				comboStr.c_str(),
				false,
				this->m_pFontDrawCtx[COMBOFONT]
			);
		}

		#ifdef FONTRESOURCE_DEBUG
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}

	void FontResource::DrawMult(AZ::Vector2 pos, unsigned int mult) {
		AZStd::string multString = "+";
		multString.append(std::to_string(mult).c_str());
		Vec2 sSize = this->m_pFont->GetTextSize(multString.c_str(), false, this->m_pFontDrawCtx[MULTIFONT]);
		AZ::Vector2 stringSize = AZ::Vector2(sSize.x, sSize.y);

		AZ::Vector2 stringPos = (pos * this->m_PosScale) - (stringSize/2);

		this->m_pFont->DrawString(
			//(pos.x*this->m_PosScale.x) - (stringSize.x / 2),
			//(pos.y*this->m_PosScale.y) - (stringSize.y / 2),
			stringPos.GetX(),
			stringPos.GetY(),
			multString.c_str(),
			false,
			this->m_pFontDrawCtx[MULTIFONT]
		);

		#ifdef FONTRESOURCE_DEBUG
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
			Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}

	void FontResource::DrawRush(ENoteType type, AZ::Vector2 pos) {
		if (type >= 4) return;

		AZ::Vector2 position = (pos * this->m_PosScale) - (this->m_rushTextSize[type] / 2);

		this->m_pFont->DrawString(
			//(pos.x*this->m_PosScale.x) - (this->m_rushTextSize[type].x / 2),
			//(pos.y*this->m_PosScale.y) - (this->m_rushTextSize[type].y / 2),
			position.GetX(),
			position.GetY(),
			this->m_rushText[type].c_str(),
			false,
			this->m_pFontDrawCtx[RUSHFONT(type)]
			);

		#ifdef FONTRESOURCE_DEBUG
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}

	void FontResource::DrawHold(ENoteType type, AZ::Vector2 pos) {
		if (type >= 4) return;

		AZ::Vector2 position = (pos * this->m_PosScale) - (this->m_holdTextSize[type] / 2);

		this->m_pFont->DrawString(
			//(pos.x*this->m_PosScale.x) - (this->m_holdTextSize[type].x / 2),
			//(pos.y*this->m_PosScale.y) - (this->m_holdTextSize[type].y / 2),
			position.GetX(),
			position.GetY(),
			this->m_holdText[type].c_str(),
			false,
			this->m_pFontDrawCtx[HOLDFONT(type)]
			);

		#ifdef FONTRESOURCE_DEBUG
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x - 5, pos.y), Vec2(pos.x + 5, pos.y), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
				Draw2dHelper::GetDraw2d()->DrawLine(Vec2(pos.x, pos.y - 5), Vec2(pos.x, pos.y + 5), ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		#endif
	}
}