#ifndef _H_SONGINFO
#define _H_SONGINFO

#pragma once

namespace LYGame {
	struct SongFileInfo {
		string name, nameR, nameE; //song name

		//author information
		struct Author {
			string name, nameR, nameE;

			Author() :
				name(""),
				nameR(""),
				nameE("")
			{}
		};

		std::vector<std::pair<string, Author>> authors;

		string desc; //description of song
		string albumArtPath; //album art path

		std::pair<unsigned int, unsigned int> bpm;

		bool valid; //if the info is valid

		SongFileInfo() :
			name(""),
			nameR(""),
			nameE(""),
			desc(""),
			valid(false)
		{}

		void GetMemoryUsage(ICrySizer* pSizer) const {
			pSizer->AddObject(this, sizeof(*this));
		}
	};

	class SongInfo {
	public:
		SongInfo(string path);
		~SongInfo();
	private:
		SongFileInfo m_info;
		std::pair<string, string> m_music;
		string m_stage;
		string m_dance;
		std::vector<std::pair<int, string>> m_models;
	public:
		static SongFileInfo GetInfo(const char * filename);
	public:
		static SongFileInfo GetInfo(XmlNodeRef node);
	};
}

#endif