#ifndef _H_TRANSLATIONFILE
#define _H_TRANSLATIONFILE

#pragma once

namespace OpenDiva {
	class LyricsFile {
	public:
		struct Info {
			AZ::Crc32 hash;
			AZStd::string lang;
			AZStd::string author;
			AZStd::string desc;
			int version;
			bool valid;

			Info() :
				lang("en"),
				author(""),
				desc(""),
				version(0),
				valid(false) {
			}
		};
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
		LyricsFile(const char * file);
		~LyricsFile();
	public:
		static Info GetInfo(const char * filename);
		Info GetInfo() { return this->m_info; }
		unsigned long long GetNumLines() { return this->m_lyrics.size(); }
		unsigned long long GetNumSubtexts() { return this->m_subtext.size(); }
		LineEntry* GetLine(unsigned long long index) { return this->m_lyrics[index]; }
		SubtextEntry* GetSubtext(unsigned long long index) { return this->m_subtext[index]; }
		IFFont * GetFont(AZStd::string font) { return this->m_fonts[font]; }
		AZ::Color GetColor(AZStd::string color) { return this->m_colors[color]; }
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	private:
		Info m_info;
	private:
		AZStd::unordered_map<AZStd::string, IFFont *> m_fonts;
		AZStd::unordered_map<AZStd::string, AZ::Color> m_colors;
		AZStd::vector<LineEntry*> m_lyrics;
		AZStd::vector<SubtextEntry*> m_subtext;
	private:
		static bool lyricsSort(LineEntry *a, LineEntry *b);
		static bool subtextSort(SubtextEntry *a, SubtextEntry *b);
		static Info GetInfo(XmlNodeRef xmlNode);
	};
}

#endif