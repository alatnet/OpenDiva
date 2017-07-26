#ifndef _H_TRANSLATIONFILE
#define _H_TRANSLATIONFILE

#pragma once

namespace LYGame {
	struct LyricsFileInfo {
		AZStd::string locale;
		AZStd::string author;
		AZStd::string desc;
		int version;
		bool valid;

		LyricsFileInfo() :
			locale("us-en"),
			author(""),
			desc(""),
			version(0),
			valid(false)
		{}
	};

	class LyricsFile {
	public:
		struct LineEntry {
			AZStd::string text;
			AZStd::string romaji;
			float time;
			/*int effect;
			AZStd::string color;
			AZStd::string font;*/

			void GetMemoryUsage(ICrySizer* pSizer) const {
				pSizer->AddObject(this, sizeof(*this));
			}
		};
	public:
		LyricsFile(const char * file);
		~LyricsFile();
	public:
		static LyricsFileInfo GetInfo(const char * filename);
		LyricsFileInfo GetInfo() { return this->m_info; }
		unsigned int GetNumLines() { return this->m_lyrics.size(); }
		LineEntry GetLine(unsigned int index) { return this->m_lyrics[index]; }
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	private:
		LyricsFileInfo m_info;
	private:
		AZStd::unordered_map<AZStd::string, IFFont *> m_fonts;
		//AZStd::unordered_map<AZStd::string, ColorF> m_colors;
		AZStd::vector<LineEntry> m_lyrics;
	private:
		static bool lyricsSort(LineEntry a, LineEntry b);
		static LyricsFileInfo GetInfo(XmlNodeRef xmlNode);
	};

	/*
	class TranslationFile {
	public:
		struct LineEntry {
			string text;
			double time;
			int effect;
			string color;
			string font;
		};

		struct SubtitleEntry {
			string text;
			double start, end;
			Vec2 pos;
			double rot;
			string font;
			int effect;
			string color;
			double size;
		};
	public:
		TranslationFile();
		~TranslationFile();
	public:
		static LyricsFileInfo GetInfo(const char * filename);
		LyricsFileInfo GetInfo() { return this->m_info; }
		unsigned int GetNumLines() { return this->m_lyrics.size(); }
		LineEntry GetLine(unsigned int index) { return this->m_lyrics[index]; }
		string toString();
	private:
		LyricsFileInfo m_info;
	private:
		std::unordered_map<std::string, IFFont *> m_fonts;
		std::unordered_map<std::string, ColorF> m_colors;
		std::vector<LineEntry> m_lyrics;
		std::vector<SubtitleEntry> m_subtitles;
	private:
		static bool lyricsSort(LineEntry a, LineEntry b);
		static LyricsFileInfo GetInfo(XmlNodeRef xmlNode);
	};
	*/
}

#endif