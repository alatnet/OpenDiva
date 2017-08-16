#ifndef _H_SONGINFO
#define _H_SONGINFO

#pragma once

namespace OpenDiva {
	struct SongFileInfo {
		AZStd::string name, nameR, nameE; //song name

		//author information
		struct Author {
			AZStd::string name, nameR, nameE;

			Author() :
				name(""),
				nameR(""),
				nameE("")
			{}
		};

		AZStd::vector<AZStd::pair<AZStd::string, Author>> authors;

		string desc; //description of song
		string albumArtPath; //album art path

		AZStd::pair<unsigned int, unsigned int> bpm;

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
		SongInfo(AZStd::string path);
		~SongInfo();
	private:
		SongFileInfo m_info;

		AZStd::pair<AZStd::string, AZStd::string> m_music;
		AZStd::string m_stage;
		AZStd::string m_dance;
		AZStd::vector<AZStd::pair<int, AZStd::string>> m_models;
	public:
		static SongFileInfo GetInfo(const char * filename);
	public:
		static SongFileInfo GetInfo(XmlNodeRef node);
	};
}

#endif