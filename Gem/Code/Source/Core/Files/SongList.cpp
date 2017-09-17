#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include <SQLite\SQLiteBus.h>

#include "NoteFile.h"
#include "LyricsFile.h"
#include "SongInfo.h"
#include "SongList.h"

#include <ctime>

namespace OpenDiva {
	void SongList::Refresh() {
		time_t now = time(0);
		AZ::Crc32 crchash = AZ::Crc32(ctime(&now));
		AZStd::string crchash_str = Crc32ToString(crchash);

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");


		sysDb->Exec("BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

		gEnv->pFileIO->FindFiles("@songs@", "*.*",
			[&](const char* filePath) -> bool {
				if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
					ProcessGroup(filePath, crchash_str, sysDb);
				}
				return true; // continue iterating
			}
		);

		sysDb->Exec("END TRANSACTION", nullptr, nullptr, nullptr);

		//database cleanup
		AZStd::string groupclean = "DELETE FROM Groups WHERE dbcrc != \"" + crchash_str + "\";";
		AZStd::string groupnamesclean = "DELETE FROM GroupNames WHERE dbcrc != \"" + crchash_str + "\";";
		AZStd::string songclean = "DELETE FROM Songs WHERE dbcrc != \"" + crchash_str + "\";";
		AZStd::string songnamesclean = "DELETE FROM SongNames WHERE dbcrc != \"" + crchash_str + "\";";
		AZStd::string notemapclean = "DELETE FROM Notemaps WHERE dbcrc != \"" + crchash_str + "\";";
		AZStd::string lyricsclean = "DELETE FROM Lyrics WHERE dbcrc != \"" + crchash_str + "\";";

		sysDb->Exec("BEGIN TRANSACTION", nullptr, nullptr, nullptr);
		sysDb->Exec(groupclean.c_str(), nullptr, nullptr, nullptr);
		sysDb->Exec(groupnamesclean.c_str(), nullptr, nullptr, nullptr);
		sysDb->Exec(songclean.c_str(), nullptr, nullptr, nullptr);
		sysDb->Exec(songnamesclean.c_str(), nullptr, nullptr, nullptr);
		sysDb->Exec(notemapclean.c_str(), nullptr, nullptr, nullptr);
		sysDb->Exec(lyricsclean.c_str(), nullptr, nullptr, nullptr);
		sysDb->Exec("END TRANSACTION", nullptr, nullptr, nullptr);
	}

	void SongList::ProcessGroup(AZStd::string path, AZStd::string dbcrc, SQLite3::SQLiteDB * sysDb) {
		AZStd::string folder = PathUtil::GetFile(path.c_str());

		AZStd::string artPath = path + "/art.ddc";
		artPath = PathUtil::ToNativePath(artPath.c_str()).c_str();

		if (!gEnv->pFileIO->Exists(artPath.c_str())) artPath = "";

		AZ::Crc32 guuid = AZ::Crc32(folder.c_str());
		guuid.Add(OPENDIVA_SALT);
		AZStd::string guuid_str = Crc32ToString(guuid);

		AZStd::string group_sql_str =
			"INSERT OR REPLACE INTO Groups ("
				"guuid,"
				"name,"
				"artpath,"
				"dbcrc"
			") VALUES ("
				"\"" + guuid_str + "\","
				"\"" + folder + "\","
				"\"" + artPath + "\","
				"\"" + dbcrc + "\""
			");";

		//execute group sql string
		sysDb->Exec(group_sql_str.c_str(), nullptr, nullptr, nullptr);

		/*
		INSERT OR REPLACE INTO Groups
		(
			guuid,
			name,
			artpath,
			dbcrc
		)
		VALUES
		(
			"",
			"",
			"",
			""
		);
		*/

		AZStd::string groupInfoPath = path + "/GroupInfo";

		//if we have a group info directory
		if (gEnv->pFileIO->Exists(groupInfoPath.c_str())) {
			gEnv->pFileIO->FindFiles(groupInfoPath.c_str(), "*.xml", //look only for xml files
				[&](const char* filePath) -> bool {
					AZStd::string file = PathUtil::GetFile(filePath);

					GroupInfo::Lang info = GroupInfo::GetLang(filePath);

					AZ::Crc32 gnuuid = AZ::Crc32(file.c_str());
					gnuuid.Add(guuid_str.c_str());
					gnuuid.Add(OPENDIVA_SALT);

					//if the info is valid
					if (info.hash != AZ_CRC("NotValid", 0x072bdcd9)){
						AZStd::string groupname_sql_str =
							"INSERT OR REPLACE INTO GroupNames ("
								"gnuuid,"
								"name,"
								"description,"
								"lang,"
								//"mcrc,"
								"dbcrc,"
								"guuid"
							") VALUES ("
								"\"" + Crc32ToString(gnuuid) + "\","
								"\"" + info.name +"\","
								"\"" + info.desc + "\","
								"\"" + info.lang + "\","
								//"\"" +  + "\","
								"\"" + dbcrc +"\","
								"\"" + guuid_str +"\""
							");";

						//execute group name sql string
						sysDb->Exec(groupname_sql_str.c_str(), nullptr, nullptr, nullptr);
					}

					return true; // continue iterating
				}
			);
		}

		/*
		INSERT OR REPLACE INTO GroupNames
		(
			gnuuid,
			name,
			description,
			lang,
			//mcrc,
			dbcrc,
			guuid
		)
		VALUES
		(
			"",
			"",
			"",
			"",
			"",
			"",
			""
		);
		*/

		//process songs
		gEnv->pFileIO->FindFiles(path.c_str(), "*.*",
			[&](const char* filePath) -> bool {
				if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
					ProcessSong(filePath, guuid_str, dbcrc, sysDb);
				}
				return true;
			}
		);
	}

	void SongList::ProcessSong(AZStd::string path, AZStd::string guuid, AZStd::string dbcrc, SQLite3::SQLiteDB * sysDb) {
		AZStd::string folder = PathUtil::GetFile(path.c_str());
		if (folder.compare("GroupInfo") == 0) return; //dont process group info.

		AZ::IO::FileIOBase* fileIO = gEnv->pFileIO;

		//folder structure
		AZStd::string songInfoFolder = path + "/SongInfo";
		AZStd::string globalInfo = songInfoFolder + "/global.xml";

		AZStd::string notemapFolder = path + "/NoteMaps";

		AZStd::string lyricsFolder = path + "/Lyrics";
		AZStd::string defaultLyrics = lyricsFolder + "/default.xml";

		//error checking
		if (!fileIO->Exists(globalInfo.c_str())) {
			//error out
			return;
		}
		if (!fileIO->Exists(defaultLyrics.c_str())) {
			//error out
			return;
		}

		//read global.xml
		SongInfo::Global ginfo = SongInfo::GetGlobal(globalInfo);

		if (ginfo.hash == AZ_CRC("NotValid", 0x072bdcd9)) return; //error out

		//create song uuid
		AZ::Crc32 suuid_hash = AZ::Crc32(folder.c_str());
		suuid_hash.Add(guuid.c_str());
		suuid_hash.Add(OPENDIVA_SALT);
		AZStd::string suuid = Crc32ToString(suuid_hash);

		AZStd::string bpm = "";

		if (ginfo.bpm.first == ginfo.bpm.second) bpm = std::to_string(ginfo.bpm.first).c_str();
		else {
			bpm = std::to_string(ginfo.bpm.first).c_str();
			bpm += "-";
			bpm += std::to_string(ginfo.bpm.second).c_str();
		}

		AZStd::string artPath = path + "/art.ddc";
		if (!ginfo.hasArt) artPath = "";
		artPath = PathUtil::ToNativePath(artPath.c_str());

		AZStd::string globalSongInfo_sql_str =
		"INSERT OR REPLACE INTO Songs ("
			"suuid," //dirname + guuid + salt
			"name,"
			"bpm,"
			"length,"
			"artpath,"
			//"models"
			"dbcrc,"
			"guuid"
		") VALUES ("
			"\"" + suuid + "\","
			"\"" + folder + "\","
			"\"" + bpm + "\","
			"\"" + std::to_string(ginfo.length).c_str() + "\","
			"\"" + artPath + "\","
			//"\"" + +"\","
			"\"" + dbcrc + "\","
			"\"" + guuid + "\""
		");";

		//execute global song info sql string
		sysDb->Exec(globalSongInfo_sql_str.c_str(), nullptr, nullptr, nullptr);

		/*
		INSERT OR REPLACE INTO Songs
		(
			suuid, //dirname + guuid + salt
			name,
			bpm,
			length,
			artpath,
			//models
			dbcrc,
			guuid
		)
		VALUES
		(
			"",
			"",
			"",
			"",
			"",
			//"",
			"",
			""
		);
		*/

		//read song name translations xml's
		gEnv->pFileIO->FindFiles(songInfoFolder.c_str(), "*.xml",
			[&](const char* filePath) -> bool {
				AZStd::string fileStr = PathUtil::GetFileName(filePath);

				if (fileStr.compare("global") == 0) return true; //skip global xml.
				SongInfo::Lang info = SongInfo::GetLang(filePath);

				//create song namd uuid
				AZ::Crc32 snuuid_hash = AZ::Crc32(fileStr.c_str());
				snuuid_hash.Add(suuid.c_str());
				snuuid_hash.Add(OPENDIVA_SALT);

				if (info.hash != AZ_CRC("NotValid", 0x072bdcd9)) {
					AZStd::string authors = "";

					for (AZStd::pair<AZStd::string, AZStd::string> author : info.authors) authors += author.first + "-" + author.second + ";";

					AZStd::string songname_sql_str =
						"INSERT OR REPLACE INTO SongNames ("
							"snuuid,"
							"name,"
							"description,"
							"authors,"
							"lang,"
							//"mcrc"
							"dbcrc,"
							"suuid"
						") VALUES ("
							"\"" + Crc32ToString(snuuid_hash) + "\","
							"\"" + info.name + "\","
							"\"" + info.desc + "\","
							"\"" + authors + "\","
							"\"" + info.lang + "\","
							//","
							"\"" + dbcrc + "\","
							"\"" + suuid + "\""
						")";

					//execute song name translation sql string
					sysDb->Exec(songname_sql_str.c_str(), nullptr, nullptr, nullptr);
				}

				return true;
			}
		);
		/*
		INSERT OR REPLACE INTO SongNames
		(
			snuuid, //filename + suuid + salt
			name,
			description,
			authors,
			lang,
			//mcrc
			dbcrc,
			suuid
		)
		VALUES
		(
			"",
			"",
			"",
			"",
			"",
			//"",
			"",
			""
		);
		*/

		//process notemaps
		gEnv->pFileIO->FindFiles(notemapFolder.c_str(), "*.xml",
			[&](const char* filePath) -> bool {
				CLOG("[SongList] Processing Notemap: %s", filePath);
				AZStd::string fileStr = PathUtil::GetFile(filePath); //*.xml
				AZStd::string fileNameStr = PathUtil::GetFileName(filePath);

				NoteFile::Info info = NoteFile::GetInfo(filePath);

				//create notemap uuid
				AZ::Crc32 nmuuid_hash = AZ::Crc32(fileNameStr.c_str());
				nmuuid_hash.Add(suuid.c_str());
				nmuuid_hash.Add(OPENDIVA_SALT);


				if (info.hash != AZ_CRC("NotValid", 0x072bdcd9)) {
					AZStd::string notemap_sql_str =
						"INSERT OR REPLACE INTO Notemaps ("
							"nmuuid," //filename + suuid + salt
							"filename,"
							"name,"
							"author,"
							"description,"
							"difficulty,"
							"version,"
							//"mcrc"
							"dbcrc,"
							"suuid"
						") VALUES ("
							"\"" + Crc32ToString(nmuuid_hash) + "\","
							"\"" + fileStr + "\","
							"\"" + fileNameStr + "\","
							"\"" + info.author + "\","
							"\"" + info.desc + "\","
							"\"" + std::to_string(info.difficulty).c_str() + "\","
							"\"" + std::to_string(info.version).c_str() + "\","
							//"\"" + + "\","
							"\"" + dbcrc + "\","
							"\"" + suuid + "\""
						");";

					//execute notemap sql string
					sysDb->Exec(notemap_sql_str.c_str(), nullptr, nullptr, nullptr);
				}
				
				return true;
			}
		);
		/*
		INSERT OR REPLACE INTO Notemaps
		(
			nmuuid, //filename + suuid + salt
			filename,
			name,
			author,
			description,
			difficulty,
			version,
			//mcrc
			dbcrc,
			suuid
		)
		VALUES
		(
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			//"",
			"",
			""
		);
		*/

		//process lyrics
		gEnv->pFileIO->FindFiles(lyricsFolder.c_str(), "*.xml",
			[&](const char* filePath) -> bool {
				AZStd::string fileStr = PathUtil::GetFile(filePath); //*.xml
				AZStd::string fileNameStr = PathUtil::GetFileName(filePath);

				LyricsFile::Info info = LyricsFile::GetInfo(filePath);

				//create lyrics uuid
				AZ::Crc32 luuid_hash = AZ::Crc32(fileNameStr.c_str());
				luuid_hash.Add(suuid.c_str());
				luuid_hash.Add(OPENDIVA_SALT);

				if (info.hash != AZ_CRC("NotValid", 0x072bdcd9)){
					AZStd::string lyrics_sql_str =
						"INSERT OR REPLACE INTO Lyrics ("
							"luuid," //filename + suuid + salt
							"filename,"
							"name,"
							"author,"
							"description,"
							"version,"
							"lang,"
							//"mcrc,"
							"dbcrc,"
							"suuid"
						") VALUES ("
							"\"" + Crc32ToString(luuid_hash) + "\","
							"\"" + fileStr + "\","
							"\"" + fileNameStr + "\","
							"\"" + info.author + "\","
							"\"" + info.desc + "\","
							"\"" + std::to_string(info.version).c_str() + "\","
							"\"" + info.lang + "\","
							//"\"" + + "\","
							"\"" + dbcrc + "\","
							"\"" + suuid + "\""
							""
						");";

					//execute lyrics sql string
					sysDb->Exec(lyrics_sql_str.c_str(), nullptr, nullptr, nullptr);
				}

				return true;
			}
		);
		/*
		INSERT OR REPLACE INTO Lyrics
		(
			luuid, //filename + suuid + salt
			filename,
			name,
			author,
			description,
			lang,
			//mcrc
			dbcrc,
			suuid
		)
		VALUES
		(
			"",
			"",
			"",
			"",
			"",
			"",
			//"",
			"",
			""
		);
		*/
	}

	AZStd::string SongList::Crc32ToString(AZ::Crc32 crc) {
		//char str[11]; /* 11 bytes: 10 for the digits, 1 for the null character */
		//uint32_t n = 12345;
		//snprintf(str, sizeof str, "%lu", (unsigned long)n); /* Method 1 */
		//snprintf(str, sizeof str, "%" PRIu32, n); /* Method 2 */

		char str[11];
		snprintf(str, sizeof str, "%" PRIu32, crc);
		return AZStd::string(str);
	}
}