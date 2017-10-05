#ifndef _H_SONGINFO
#define _H_SONGINFO

#pragma once

namespace OpenDiva {

	class SongInfo {
	public:
		struct Global {
			AZ::Crc32 hash;
			bool hasArt;
			AZStd::string bg;
			float length;
			AZStd::pair<unsigned int, unsigned int> bpm;
			AZStd::string lib, vocal, melody, song;
		};

		struct Lang {
			AZ::Crc32 hash;
			AZStd::string lang, name, desc;
			AZStd::vector<AZStd::pair<AZStd::string, AZStd::string>> authors;
		};
	public:
		static Global GetGlobal(AZStd::string path);
		static Lang GetLang(AZStd::string path);
	};

	class GroupInfo {
	public:
		struct Lang {
			AZ::Crc32 hash;
			AZStd::string lang, name, desc;
		};
	public:
		static Lang GetLang(AZStd::string path);
	};
}

#endif