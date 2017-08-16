#ifndef _H_TRANSLATIONFILE
#define _H_TRANSLATIONFILE

#pragma once

namespace OpenDiva {
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
		AZStd::vector<LineEntry> m_lyrics;
	private:
		static bool lyricsSort(LineEntry *a, LineEntry *b);
		static LyricsFileInfo GetInfo(XmlNodeRef xmlNode);
	};

	class TranslationFile {
	public:
		struct LineEntry {
			AZStd::string text;
			float time;
		};

		struct SubtextEntry {
			AZStd::string text;
			float start, end;
			AZ::Vector2 pos;
			float rot;
			AZStd::string font;
			int effect;
			AZStd::string color;
			float size;
		};
	public:
		TranslationFile(const char * filename);
		~TranslationFile();
	public:
		static LyricsFileInfo GetInfo(const char * filename);
		LyricsFileInfo GetInfo() { return this->m_info; }
		unsigned int GetNumLines() { return this->m_lyrics.size(); }
		unsigned int GetNumSubtexts() { return this->m_subtext.size(); }
		LineEntry GetLine(unsigned int index) { return this->m_lyrics[index]; }
		SubtextEntry GetSubtext(unsigned int index) { return this->m_subtext[index]; }
		IFFont * GetFont(AZStd::string font){ return this->m_fonts[font]; }
		AZ::Color GetColor(AZStd::string color){ return this->m_colors[color]; }
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	private:
		LyricsFileInfo m_info;
	private:
		AZStd::unordered_map<AZStd::string, IFFont *> m_fonts;
		AZStd::unordered_map<AZStd::string, AZ::Color> m_colors;
		AZStd::vector<LineEntry> m_lyrics;
		AZStd::vector<SubtextEntry> m_subtext;
	private:
		static bool lyricsSort(LineEntry *a, LineEntry *b);
		static bool subtextSort(SubtextEntry *a, SubtextEntry *b);
		static LyricsFileInfo GetInfo(XmlNodeRef xmlNode);
	};
}

#endif