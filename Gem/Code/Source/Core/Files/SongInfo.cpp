#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "SongInfo.h"

namespace OpenDiva {
	SongInfo::Global SongInfo::GetGlobal(AZStd::string path) {
		AZ::IO::FileIOBase* fileIO = gEnv->pFileIO;

		SongInfo::Global ret;
		ret.hash = AZ_CRC("NotValid", 0x072bdcd9);
		ret.hasArt = false;
		ret.length = 0.0f;
		ret.bpm.first = 0;
		ret.bpm.second = 0;
		ret.lib = "libsndfile";
		ret.vocal = "";
		ret.melody = "";
		ret.song = "";

		XmlNodeRef file = gEnv->pSystem->LoadXmlFromFile(path.c_str());

		if (!file) return ret;

		AZStd::string rootPath = PathUtil::GetParentDirectory(PathUtil::GetPath(path.c_str()));
		AZStd::string artPath = rootPath + "/SongInfo/art.dds";
		ret.hasArt = fileIO->Exists(artPath.c_str());

		ret.bg = rootPath + "/bg.dds";

		ret.length = (float)atof(file->findChild("length")->getContent());
		
		XmlNodeRef bpm = file->findChild("bpm");
		if (bpm->getChildCount() > 0) {
			ret.bpm.first = atoi(bpm->findChild("start")->getContent());
			ret.bpm.second = atoi(bpm->findChild("end")->getContent());
		} else ret.bpm.first = ret.bpm.second = atoi(bpm->getContent());
		
		XmlNodeRef music = file->findChild("music");
		if (music) {
			if (music->haveAttr("lib")) ret.lib = music->getAttr("lib");
			if (music->findChild("vocal")) ret.vocal = rootPath + "/" + music->findChild("vocal")->getContent();
			if (music->findChild("melody")) ret.melody = rootPath + "/" + music->findChild("melody")->getContent();
			if (music->findChild("song")) ret.song = rootPath + "/" + music->findChild("song")->getContent();
		}

		//if there is no override for vocal, find the vocal file.
		if (ret.vocal.empty()){
			fileIO->FindFiles(
				rootPath.c_str(),
				"vocal.*",
				[&](const char* file) -> bool {
					ret.vocal = file;
					return false;
				}
			);
		}
		//if there is no override for melody, find the melody file.
		if (ret.melody.empty()) {
			fileIO->FindFiles(
				rootPath.c_str(),
				"melody.*",
				[&](const char* file) -> bool {
					ret.melody = file;
					return false;
				}
			);
		}
		//if there is no override for song, find the song file.
		if (ret.song.empty()) {
			fileIO->FindFiles(
				rootPath.c_str(),
				"song.*",
				[&](const char* file) -> bool {
					ret.song = file;
					return false;
				}
			);
		}

		ret.hash = AZ::Crc32(path.c_str());
		ret.hash.Add(OPENDIVA_SALT);

		//char str[11]; /* 11 bytes: 10 for the digits, 1 for the null character */
		//uint32_t n = 12345;
		//snprintf(str, sizeof str, "%lu", (unsigned long)n); /* Method 1 */
		//snprintf(str, sizeof str, "%" PRIu32, n); /* Method 2 */

		return ret;
	}

	SongInfo::Lang SongInfo::GetLang(AZStd::string path) {
		SongInfo::Lang ret;

		ret.hash = AZ_CRC("NotValid", 0x072bdcd9);
		ret.lang = "XX";
		ret.name = "";
		ret.desc = "";
		ret.authors.clear();

		XmlNodeRef file = gEnv->pSystem->LoadXmlFromFile(path.c_str());

		//compare if the language codes match
		AZStd::string langfile = PathUtil::GetFileName(path.c_str());
		ret.lang = file->getAttr("lang");

		if (langfile.compare(ret.lang) != 0) return ret; //language file name doesnt match internal language code.

		XmlNodeRef name = file->findChild("name");
		XmlNodeRef desc = file->findChild("desc");
		if (name) ret.name = name->getContent();
		if (desc) ret.desc = desc->getContent();

		XmlNodeRef authors = file->findChild("authors");
		for (unsigned int i = 0; i < authors->getChildCount(); i++) {
			XmlNodeRef author = authors->getChild(i);
			AZStd::pair<AZStd::string, AZStd::string> data;
			data.first = author->getTag();
			data.second = author->getContent();
			ret.authors.push_back(data);
		}

		ret.hash = AZ::Crc32(path.c_str());
		ret.hash.Add(ret.lang.c_str());
		ret.hash.Add(OPENDIVA_SALT);

		return ret;
	}

	GroupInfo::Lang GroupInfo::GetLang(AZStd::string path) {
		GroupInfo::Lang ret;

		ret.hash = AZ_CRC("NotValid", 0x072bdcd9);
		ret.lang = "XX";
		ret.name = "";
		ret.desc = "";

		XmlNodeRef file = gEnv->pSystem->LoadXmlFromFile(path.c_str());

		//compare if the language codes match
		AZStd::string langfile = PathUtil::GetFileName(path.c_str());
		ret.lang = file->getAttr("lang");

		if (langfile.compare(ret.lang) != 0) return ret; //language file name doesnt match internal language code.

		XmlNodeRef name = file->findChild("name");
		XmlNodeRef desc = file->findChild("desc");
		if (name) ret.name = name->getContent();
		if (desc) ret.desc = desc->getContent();

		ret.hash = AZ::Crc32(path.c_str());
		ret.hash.Add(ret.lang.c_str());
		ret.hash.Add(OPENDIVA_SALT);

		return ret;
	}
}