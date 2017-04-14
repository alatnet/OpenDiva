#ifndef _H_TRANSLATIONFILE
#define _H_TRANSLATIONFILE

#pragma once

namespace LYGame {
	struct TranslationFileInfo {
		string author;
		unsigned int version;
		string desc;
		bool valid;
		TranslationFileInfo() :
			author(""),
			version(1),
			desc(""),
			valid(false)
		{}

		void GetMemoryUsage(ICrySizer* pSizer) const {
			pSizer->AddObject(this, sizeof(*this));
		}
	};

	class TranslationFileFont {
	public:
		TranslationFileFont(string name, string fontPath);
		~TranslationFileFont();
	public:
		void DrawText(string text, Vec2 pos, Vec2 size, ColorF color, unsigned int effect, float rotation);
	private:
		IFFont * m_pFont;
		STextDrawContext m_context;
	};

	struct TranslationFileSetup {
		std::unordered_map<std::string, TranslationFileFont *> fonts; //fonts[font name] = font
		std::unordered_map<std::string, ColorF> colors; //colors[color name] = ColorF
		std::unordered_map<std::string, Vec2> textSizes; //textSize[size name] = Vec2
	};
	//setup.fonts[font name]->DrawText(text, pos, textSizes[size name], colors[color name], effect, rotation);

	struct TranslationFileLine {
		float time;
		std::string font;
		unsigned int fontEffect;
		std::string color;
		string data;
	};

	struct TranslationFileSubtitle {
		float sTime, eTime;
		Vec2 pos;
		float rotation;
		std::string font;
		unsigned int fontEffect;
		std::string color;
		string data;
	};

	class TranslationFile {
	public:
		TranslationFile(string path);
		~TranslationFile();
	public:
		static TranslationFileInfo GetInfo(const char * filename);
	private:
		std::vector<TranslationFileLine> lines; //sort by time
		std::vector<TranslationFileSubtitle> subtitles; //sort by sTime
		TranslationFileSetup setup;
	};
}

#endif