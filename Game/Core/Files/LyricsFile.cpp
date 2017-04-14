#include <StdAfx.h>
#include "LyricsFile.h"

namespace LYGame {
	TranslationFileFont::TranslationFileFont(string name, string fontPath) {
		this->m_pFont = gEnv->pSystem->GetICryFont()->NewFont("Translation Font: " + name);
		this->m_pFont->Load(fontPath);
		this->m_pFont->AddRef();

		this->m_context.SetFlags(eDrawText_Center | eDrawText_CenterV | eDrawText_2D | eDrawText_FixedSize);
		this->m_context.SetSizeIn800x600(false);
		this->m_context.SetProportional(true);
	}

	TranslationFileFont::~TranslationFileFont() {
		this->m_pFont->Release();
	}


	void TranslationFileFont::DrawText(string text, Vec2 pos, Vec2 size, ColorF color, unsigned int effect, float rotation) {
		STextDrawContext context = this->m_context;
		context.SetSize(size);
		context.SetEffect(effect);
		context.SetColor(color);
		//TODO: deal with rotation.
		//context.SetTransform(nullptr);

		this->m_pFont->DrawString(pos.x, pos.y, text, false, context);
	}

	TranslationFile::TranslationFile(string path) {
		XmlNodeRef file = gEnv->pSystem->LoadXmlFromFile(path);
		//read file
	}

	TranslationFile::~TranslationFile() {
	}

	TranslationFileInfo TranslationFile::GetInfo(const char * filename) {
		TranslationFileInfo ret;

		XmlNodeRef file = gEnv->pSystem->LoadXmlFromFile(filename);

		if (file != 0) {
			ret.author = file->getAttr("author");
			file->getAttr("version", ret.version);

			XmlNodeRef desc = file->findChild("desc");
			if (desc != 0) ret.desc = desc->getContent();

			ret.valid = true;
		}

		return ret;
	}
}