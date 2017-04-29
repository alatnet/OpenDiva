#include <StdAfx.h>
#include "LyricsFile.h"

namespace LYGame {
	LyricsFile::LyricsFile(const char * folder) {
		string file(folder);
		file += "/lyrics.xml";

		string fontpath(folder);
		fontpath += "/Fonts/";

		XmlNodeRef lyrics = gEnv->pSystem->LoadXmlFromFile(file);

		this->m_info = LyricsFile::GetInfo(lyrics);

		if (this->m_info.valid) {
			//load fonts and setup colors
			XmlNodeRef setup = lyrics->findChild("setup");
			for (int i = 0; i < setup->getChildCount(); i++) {
				XmlNodeRef entry = setup->getChild(i);
				std::string tag = entry->getTag();

				if (entry->haveAttr("xml")) {
					string fontxml = entry->getAttr("xml");
					std::string fontname = "lf-";
					fontname += tag;

					//copy fonts to cache.
					Util::CopyToCache(fontpath + fontxml, "Fonts/Lyrics/"); //copy the xml file
					string fontXMLCache = Util::GetCachePath(fontpath + fontxml, "Fonts/Lyrics/");
					XmlNodeRef fontXMLFile = gEnv->pSystem->LoadXmlFromFile(fontXMLCache); //open the cache xml file version
					string fontFilePath = fontXMLFile->findChild("font")->getAttr("path"); //get the font path
					fontXMLFile->findChild("font")->setAttr("path", PathUtil::GetFile(fontFilePath)); //edit the font path to be just the font name itself
					fontXMLFile->saveToFile(fontXMLCache); //save the cache xml file version
					Util::CopyToCache(gEnv->pFileIO->GetAlias("@assets@") + string("/") + fontFilePath, "Fonts/Lyrics/"); //copy the font file itself to the cache

					IFFont * font = gEnv->pSystem->GetICryFont()->NewFont(fontname.c_str());
					font->Load(fontXMLCache);
					font->AddRef();

					this->m_fonts.insert({ tag, font });
				} else if (entry->haveAttr("rgba")) {
					Vec4 color;
					entry->getAttr("rgba", color);
					this->m_colors.insert({ tag, ColorF(color) });
				} else {
					CryLog("(Lyrics File) Unknown setup type: %s", tag);
				}
			}

			//get all lines for the lyrics
			#pragma omp parallel for
			for (int i = 0; i < lyrics->getChildCount(); i++) {
				XmlNodeRef entry = lyrics->getChild(i);
				string tag = entry->getTag();

				if (tag.compare("line") == 0) {
					XmlNodeRef text = entry->findChild("text");
					XmlNodeRef romaji = entry->findChild("romaji");

					LineEntry lEntry;

					if (text != nullptr) lEntry.text = text->getContent(); else lEntry.text = "";
					if (romaji != nullptr) lEntry.romaji = romaji->getContent(); else lEntry.romaji = "";

					entry->getAttr("time", lEntry.time);
					entry->getAttr("effect", lEntry.effect);
					string font = entry->getAttr("font");
					string color = entry->getAttr("color");
					lEntry.font = font;
					lEntry.color = color;

					#pragma omp critical
					{
						this->m_lyrics.push_back(lEntry);
					}
				}
			}

			//sort the lyrics based on time
			std::stable_sort(this->m_lyrics.begin(), this->m_lyrics.end(), LyricsFile::lyricsSort);
		}
	}

	LyricsFile::~LyricsFile() {
		for each (std::pair<std::string, IFFont *> entry in this->m_fonts) entry.second->Release();
		Util::ClearCache("Fonts/Lyrics/");
	}

	//sort lyrics by time
	bool LyricsFile::lyricsSort(LineEntry a, LineEntry b) {
		return a.time < b.time;
	}

	LyricsFileInfo LyricsFile::GetInfo(const char * filename){
		XmlNodeRef xmlNode = gEnv->pSystem->LoadXmlFromFile(filename);
		LyricsFileInfo info;

		if (xmlNode != 0) {
			info.author = xmlNode->getAttr("author");
			info.locale = xmlNode->getAttr("locale");
			xmlNode->getAttr("version", info.version);

			XmlNodeRef desc = xmlNode->findChild("desc");
			if (!desc) info.desc = desc->getContent(); else info.desc = "";
			info.valid = true;
		}

		return info;
	}

	LyricsFileInfo LyricsFile::GetInfo(XmlNodeRef xmlNode){
		LyricsFileInfo info;

		if (xmlNode != 0) {
			info.author = xmlNode->getAttr("author");
			info.locale = xmlNode->getAttr("locale");
			xmlNode->getAttr("version", info.version);

			XmlNodeRef desc = xmlNode->findChild("desc");
			if (!desc) info.desc = desc->getContent(); else info.desc = "";
			info.valid = true;
		}

		return info;
	}
	
	void LyricsFile::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		//pSizer->AddObject(this->m_colors);
		//pSizer->AddObject(this->m_fonts);
		pSizer->AddObject(this->m_lyrics);
	}
}