#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "LyricsFile.h"
#include <AzCore\std\sort.h>

namespace OpenDiva {
	//------------------------------------------
	// LyricsFile
	//------------------------------------------
	LyricsFile::LyricsFile(const char * folder) {
		AZStd::string file(folder);
		file += "/lyrics.xml";

		AZStd::string fontpath(folder);
		fontpath += "/Fonts/";

		XmlNodeRef lyrics = gEnv->pSystem->LoadXmlFromFile(file.c_str());

		this->m_info = LyricsFile::GetInfo(lyrics);

		if (this->m_info.valid) {
			//load fonts and setup colors
			XmlNodeRef setup = lyrics->findChild("setup");
			for (int i = 0; i < setup->getChildCount(); i++) {
				XmlNodeRef entry = setup->getChild(i);
				AZStd::string tag = entry->getTag();

				if (entry->haveAttr("xml")) {
					AZStd::string fontxml = entry->getAttr("xml");
					AZStd::string fontname = "lf-";
					fontname += tag;

					//copy fonts to cache.
					Util::CopyToCache(fontpath + fontxml, "Fonts/Lyrics/"); //copy the xml file
					AZStd::string fontXMLCache = Util::GetCachePath(fontpath + fontxml, "Fonts/Lyrics/");
					XmlNodeRef fontXMLFile = gEnv->pSystem->LoadXmlFromFile(fontXMLCache.c_str()); //open the cache xml file version
					AZStd::string fontFilePath = fontXMLFile->findChild("font")->getAttr("path"); //get the font path
					fontXMLFile->findChild("font")->setAttr("path", PathUtil::GetFile(fontFilePath.c_str())); //edit the font path to be just the font name itself
					fontXMLFile->saveToFile(fontXMLCache.c_str()); //save the cache xml file version
					Util::CopyToCache(AZStd::string(gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/") + fontFilePath), "Fonts/Lyrics/"); //copy the font file itself to the cache

					IFFont * font = gEnv->pSystem->GetICryFont()->NewFont(fontname.c_str());
					font->Load(fontXMLCache.c_str());
					font->AddRef();

					this->m_fonts.insert({ tag, font });
				/*} else if (entry->haveAttr("rgba")) {
					Vec4 color;
					entry->getAttr("rgba", color);
					this->m_colors.insert({ tag, ColorF(color) });*/
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
					//entry->getAttr("effect", lEntry.effect);
					//string font = entry->getAttr("font");
					//string color = entry->getAttr("color");
					//lEntry.font = font;
					//lEntry.color = color;

					#pragma omp critical
					{
						this->m_lyrics.push_back(lEntry);
					}
				}
			}

			//sort the lyrics based on time
			//std::stable_sort(this->m_lyrics.begin(), this->m_lyrics.end(), LyricsFile::lyricsSort);
			//AZStd::stable_sort(this->m_lyrics.begin(), this->m_lyrics.end(), LyricsFile::lyricsSort, this->m_lyrics.get_allocator());
		}
	}

	LyricsFile::~LyricsFile() {
		for (AZStd::pair<AZStd::string, IFFont *> entry : this->m_fonts) entry.second->Release();
		this->m_fonts.clear();
		this->m_lyrics.clear();
		Util::ClearCache("Fonts/Lyrics/");
	}

	//sort lyrics by time
	bool LyricsFile::lyricsSort(LineEntry *a, LineEntry *b) {
		return a->time < b->time;
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
		//pSizer->AddObject(this->m_lyrics);
	}

	//------------------------------------------
	// TranslationFile
	//------------------------------------------
	TranslationFile::TranslationFile(const char * filename) {
		AZStd::string fontpath(
			PathUtil::GetParentDirectory(
				PathUtil::GetPath(filename)
			)
		);
		fontpath += "/Fonts/";

		XmlNodeRef translations = gEnv->pSystem->LoadXmlFromFile(filename);

		this->m_info = TranslationFile::GetInfo(translations);

		if (this->m_info.valid) {
			//load fonts and setup colors
			XmlNodeRef setup = translations->findChild("setup");

			for (int i = 0; i < setup->getChildCount(); i++) {
				XmlNodeRef entry = setup->getChild(i);
				AZStd::string tag = entry->getTag();

				if (entry->haveAttr("xml")) {
					AZStd::string fontxml = entry->getAttr("xml");
					AZStd::string fontname = "tf-";
					fontname += tag;

					//copy fonts to cache.
					Util::CopyToCache(fontpath + fontxml, "Fonts/Translations/"); //copy the xml file
					AZStd::string fontXMLCache = Util::GetCachePath(fontpath + fontxml, "Fonts/Translations/");
					XmlNodeRef fontXMLFile = gEnv->pSystem->LoadXmlFromFile(fontXMLCache.c_str()); //open the cache xml file version
					AZStd::string fontFilePath = fontXMLFile->findChild("font")->getAttr("path"); //get the font path
					fontXMLFile->findChild("font")->setAttr("path", PathUtil::GetFile(fontFilePath.c_str())); //edit the font path to be just the font name itself
					fontXMLFile->saveToFile(fontXMLCache.c_str()); //save the cache xml file version
					Util::CopyToCache(AZStd::string(gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/") + fontFilePath), "Fonts/Lyrics/"); //copy the font file itself to the cache

					IFFont * font = gEnv->pSystem->GetICryFont()->NewFont(fontname.c_str());
					font->Load(fontXMLCache.c_str());
					font->AddRef();

					this->m_fonts.insert({ tag, font });
				} else if (entry->haveAttr("rgba")) {
					Vec4 color;
					entry->getAttr("rgba", color);
					this->m_colors.insert({ tag, AZ::Color(color.x, color.y, color.z, color.w) });
				} else {
					CryLog("(Lyrics File) Unknown setup type: %s", tag);
				}
			}
			
			//get translations and subtexts
			#pragma omp parallel for
			for (int i = 0; i < translations->getChildCount(); i++) {
				XmlNodeRef entry = translations->getChild(i);
				string tag = entry->getTag();

				if (tag.compare("line") == 0) {
					LineEntry lEntry;

					lEntry.text = entry->getContent();

					entry->getAttr("time", lEntry.time);
					#pragma omp critical
					{
						this->m_lyrics.push_back(lEntry);
					}
				} else if (tag.compare("subtext") == 0) {
					SubtextEntry sEntry;

					sEntry.text = entry->getContent();

					entry->getAttr("start",sEntry.start);
					entry->getAttr("end", sEntry.end);
					Vec2 pos;
					entry->getAttr("pos", pos);
					sEntry.pos.Set(pos.x, pos.y);
					entry->getAttr("rot", sEntry.rot);
					sEntry.font = entry->getAttr("font");
					entry->getAttr("effect", sEntry.effect);
					sEntry.color = entry->getAttr("color");
					entry->getAttr("size", sEntry.size);

					#pragma omp critical
					{
						this->m_subtext.push_back(sEntry);
					}
				}
			}

			//sort the data
			//AZStd::stable_sort(this->m_lyrics.begin(), this->m_lyrics.end(), TranslationFile::lyricsSort, this->m_lyrics.get_allocator());
			//AZStd::stable_sort(this->m_subtext.begin(), this->m_subtext.end(), TranslationFile::subtextSort, this->m_subtext.get_allocator());
		}
	}

	TranslationFile::~TranslationFile() {
		for (AZStd::pair<AZStd::string, IFFont *> entry : this->m_fonts) entry.second->Release();
		this->m_fonts.clear();
		this->m_colors.clear();
		this->m_lyrics.clear();
		this->m_subtext.clear();
		Util::ClearCache("Fonts/Translations/");
	}

	LyricsFileInfo TranslationFile::GetInfo(const char * filename) {
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

	LyricsFileInfo TranslationFile::GetInfo(XmlNodeRef xmlNode) {
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

	//sort lyrics by time
	bool TranslationFile::lyricsSort(LineEntry *a, LineEntry *b) {
		return a->time < b->time;
	}

	bool TranslationFile::subtextSort(SubtextEntry *a, SubtextEntry *b) {
		return a->start < b->start;
	}

	void TranslationFile::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		//pSizer->AddObject(this->m_colors);
		//pSizer->AddObject(this->m_fonts);
		//pSizer->AddObject(this->m_lyrics);
	}
}