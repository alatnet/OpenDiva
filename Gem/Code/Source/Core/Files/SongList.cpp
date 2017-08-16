#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include <AzCore\Jobs\Job.h>
#include <AzCore\Jobs\JobManager.h>

#include "../Utils/hashlib2plus/include/hashlibpp.h"

#include <SQLite\SQLiteBus.h>

#include "NoteFile.h"
#include "LyricsFile.h"
#include "SongInfo.h"
#include "SongList.h"

#include <ctime>

namespace OpenDiva {
	void SongList::Process() {
		//verification check to be able to trim the database.
		wrapperfactory wraper;
		hashwrapper* hashw = wraper.create(HL_Wrappertype::HL_MD5);
		time_t now = time(0);
		std::string crchash = hashw->getHashFromString(ctime(&now));
		delete hashw;


		gEnv->pFileIO->FindFiles(this->m_path.c_str(), "*.*",
			[&](const char* filePath) -> bool {
				if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
					if (gEnv->pFileIO->Exists(string(filePath) + "/GroupInfo.xml")) { //if the group info xml file exists in that directory
						std::string spath = filePath;
						size_t slash = spath.find_last_of("/\\");
						std::string folder = spath.substr(slash + 1);
						//PathUtil::GetPath("");

						this->StartAsChild(new SongGroup(folder, filePath, crchash));
					}
				}
				return true; // continue iterating
			}
		);

		this->WaitForChildren();

		//clean up the tables
		std::string groupclean = "DELETE FROM Groups WHERE crc != " + crchash + ";";
		std::string songclean = "DELETE FROM Songs WHERE crc != " + crchash + ";";
		std::string notemapclean = "DELETE FROM Notemaps WHERE crc != " + crchash + ";";

		SQLite3::SQLiteDB * db;
		SQLITE_BUS(db, AZ::EntityId(0), GetConnection);

		if (db != nullptr) {
			int ret;
			SQLITEDB_BUS(ret, db, Exec, groupclean.c_str(), nullptr, nullptr, nullptr);
			AZ_Assert(ret == SQLITE_OK, "Group table cleanup failed.");

			SQLITEDB_BUS(ret, db, Exec, songclean.c_str(), nullptr, nullptr, nullptr);
			AZ_Assert(ret == SQLITE_OK, "Song table cleanup failed.");

			SQLITEDB_BUS(ret, db, Exec, notemapclean.c_str(), nullptr, nullptr, nullptr);
			AZ_Assert(ret == SQLITE_OK, "Notemap table cleanup failed.");
		}
	}

	void SongList::SongGroup::Process() {
		SQLite3::SQLiteDB * db;
		SQLITE_BUS(db, AZ::EntityId(0), GetConnection);

		if (db != nullptr) {
			wrapperfactory wraper;
			hashwrapper* hashw = wraper.create(HL_Wrappertype::HL_MD5);
			std::string grouphash = hashw->getHashFromString(this->m_group + this->m_path);
			delete hashw;

			//get group info
			//update or create database entry

			/*
			"CREATE TABLE IF NOT EXISTS Groups ( \
				uuid TEXT PRIMARY KEY UNIQUE NOT NULL, \
				crc INTEGER NOT NULL, \
				name TEXT, \
				name_en TEXT, \
				name_rj TEXT, \
				desc TEXT, \
				desc_en TEXT \
			);"

			INSERT OR REPLACE INTO Groups(uuid, crc, name, name_en, name_rj, desc, desc_en) VALUES (grouphash, m_crc, "", "", "", "", "");
			*/

			gEnv->pFileIO->FindFiles(this->m_path.c_str(), "*.*",
				[&](const char* filePath) -> bool {
					if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
						if (gEnv->pFileIO->Exists(string(filePath) + "/SongInfo.xml")) { //if the songinfo.xml exists in that directory
							std::string spath = filePath;
							size_t slash = spath.find_last_of("/\\");
							std::string folder = spath.substr(slash + 1);

							this->StartAsChild(new SongEntry(grouphash, folder, filePath, m_crc));
						}
					}
					return true; // continue iterating
				}
			);
		}

		this->WaitForChildren();
	}

	void SongList::SongGroup::SongEntry::Process() {
		SQLite3::SQLiteDB * db;
		SQLITE_BUS(db, AZ::EntityId(0), GetConnection);

		if (db != nullptr) {
			wrapperfactory wraper;
			hashwrapper* hashw = wraper.create(HL_Wrappertype::HL_MD5);
			std::string songhash = hashw->getHashFromString(this->m_groupHash + this->m_songName + this->m_path);
			delete hashw;

			//get song info
			//update or create database entry

			/*
			"CREATE TABLE IF NOT EXISTS Songs ( \
				uuid TEXT PRIMARY KEY UNIQUE NOT NULL, \
				groupUUID TEXT NOT NULL, \
				crc INTEGER NOT NULL, \
				name TEXT, \
				name_en TEXT, \
				name_rj TEXT, \
				desc TEXT, \
				desc_en TEXT, \
				albumArt TEXT, \
				bpm TEXT, \
				models TEXT, \
				genre TEXT, \
				authors TEXT, \
				path TEXT NOT NULL, \
				FOREIGN KEY(groupUUID) REFERENCES Groups(uuid) \
			);"

			INSERT OR REPLACE INTO Songs(uuid, groupUUID, crc, name, name_en, name_rj, desc, desc_en, albumArt, bpm, models, genre, authors, path) VALUES (songhash, m_groupHash, m_crc, "", "", "", "", "", "", "", "", "", "", "");
			*/

			std::string notemapPath = this->m_path + "/NoteMaps";
			gEnv->pFileIO->FindFiles(notemapPath.c_str(), "*.xml", //look for xml files
				[&](const char* filePath) -> bool {
					std::string spath = filePath;
					size_t slash = spath.find_last_of("/\\");
					std::string notemapFileName = spath.substr(slash + 1);
					std::string notemapName = notemapFileName.substr(0, notemapFileName.length() - 4);

					this->StartAsChild(new SongNoteMap(songhash, notemapName, filePath, m_crc));
					return true; // continue iterating
				}
			);

			this->WaitForChildren();
		}
	}

	void SongList::SongGroup::SongEntry::SongNoteMap::Process() {
		SQLite3::SQLiteDB * db;
		SQLITE_BUS(db, AZ::EntityId(0), GetConnection);

		if (db != nullptr) {
			wrapperfactory wraper;
			hashwrapper* hashw = wraper.create(HL_Wrappertype::HL_MD5);
			std::string notemapHash = hashw->getHashFromString(this->m_songHash + this->m_notemapName + this->m_path);
			delete hashw;

			//get notemap info
			//update or create database entry

			/*
			"CREATE TABLE IF NOT EXISTS Notemaps ( \
				uuid TEXT PRIMARY KEY UNIQUE NOT NULL, \
				crc INTEGER NOT NULL, \
				songUUID TEXT NOT NULL, \
				author TEXT, \
				author_en TEXT, \
				author_rj TEXT, \
				desc TEXT, \
				desc_en TEXT, \
				difficulty INTEGER NOT NULL, \
				version INTEGER NOT NULL, \
				path TEXT NOT NULL, \
				FOREIGN KEY(songUUID) REFERENCES Songs(uuid) \
			);"

			INSERT OR REPLACE INTO Notemaps(uuid, songUUID, crc, author, author_en, author_rj, desc, desc_en, dificulty, version, path) VALUES (notemapHash, m_songHash, m_crc, "", "", "", "", "", 0, 0, "");
			*/
		}
	}
}

//namespace LYGame {
//	//----------------------------------------------
//	//Song Entry
//	//----------------------------------------------
//	SongEntry::SongEntry(string name, string path) {
//		this->m_name = name;
//		this->m_path = path;
//	}
//
//	SongEntry::~SongEntry() {
//	}
//
//	void SongEntry::GetMemoryUsage(ICrySizer* pSizer) const {
//		pSizer->AddObject(this, sizeof(*this));
//		pSizer->AddObject(this->m_notemaps);
//		//pSizer->AddObject(this->m_translations);
//	}
//
//	void SongEntry::Log() {
//		CryLog("---Name: %s", this->m_info.name);
//		CryLog("---Authors:");
//		for each (std::pair<string, SongFileInfo::Author> auth in this->m_info.authors) {
//			CryLog("---- %s, %s, %s", auth.second.name, auth.second.nameE, auth.second.nameR);
//		}
//		CryLog("---Number of notemaps: %i", this->m_notemaps.size());
//		//CryLog("---Number of translations: %i", this->m_translations.size());
//	}
//
//	//----------------------------------------------
//	SongEntry::SongEntryJob::SongEntryJob(SongEntry * entry) : AZ::Job(true, nullptr) {
//		this->m_pEntry = entry;
//	}
//
//	void SongEntry::SongEntryJob::Process() {
//		/*this->m_notemaps.clear(); //clear the list of notemaps.
//		this->m_translations.clear(); //clear the list of translations.
//
//		//load song info file.
//		this->m_info = SongInfo::GetInfo(this->m_path + "/SongInfo.xml"); //get the song info.
//
//		if (!this->m_info.valid) return; //if the song file info is not valid, do not process.
//
//		//notemaps
//		//open up the note map diretory.
//		string nmDirStr = this->m_path + "/NoteMaps";
//		DIR * nmDir = opendir(nmDirStr.c_str());
//
//		dirent *nmEntry = readdir(nmDir); //read the first entry.
//
//		//while we have entries.
//		while (nmEntry != NULL) {
//		string entryName(nmEntry->d_name); //get the entry name,
//		if (
//		entryName.compare(".") != 0 && //if we are not the "." directory/file
//		entryName.compare("..") != 0 && //if we are not the ".." directory/file
//		nmEntry->d_type == DT_REG //and if we are a file.
//		) {
//		string noteFilePath = nmDirStr + "/" + entryName; //get the path of the file.
//		NoteFileInfo info = NoteFile::GetInfo(noteFilePath.c_str()); //get the note map info.
//		if (info.valid) //if the info is valid.
//		this->m_notemaps.push_back(std::make_pair(entryName, info)); //add it to the list of notemaps.
//		}
//		nmEntry = readdir(nmDir);
//		}
//
//		closedir(nmDir);
//
//		//translations
//		string tDirStr = this->m_path + "/Translations";
//		DIR * tDir = opendir(tDirStr.c_str());
//
//		dirent *tEntry = readdir(tDir);
//		while (tEntry != NULL) {
//		string entryName(tEntry->d_name);
//		if (
//		entryName.compare(".") != 0 &&
//		entryName.compare("..") != 0 &&
//		tEntry->d_type == DT_REG
//		) {
//		string translationFilePath = tDirStr + "/" + entryName;
//		TranslationFileInfo info = TranslationFile::GetInfo(translationFilePath.c_str());
//		if (info.valid)
//		this->m_translations.push_back(std::make_pair(entryName, info));
//		}
//		tEntry = readdir(tDir);
//		}
//
//		closedir(tDir);*/
//
//		//load songinfo.xml
//		//list note files
//		//list translations
//	}
//	//----------------------------------------------
//
//
//	//----------------------------------------------
//	//Song Group
//	//----------------------------------------------
//	SongGroup::SongGroup(string name, string path) {
//		this->m_name = name;
//		this->m_path = path;
//	}
//
//	SongGroup::~SongGroup() {
//		this->m_entries.clear();
//	}
//
//	void SongGroup::GetMemoryUsage(ICrySizer* pSizer) const {
//		pSizer->AddObject(this, sizeof(*this));
//		pSizer->AddObject(this->m_entries);
//	}
//
//	void SongGroup::Log() {
//		for (SongEntry *entry : this->m_entries) {
//			CryLog("--%s", entry->getName());
//			entry->Log();
//		}
//	}
//
//	//----------------------------------------------
//	SongGroup::SongGroupJob::SongGroupJob(SongGroup * group) : Job(true, nullptr) {
//		this->m_pGroup = group;
//	}
//
//	void SongGroup::SongGroupJob::Process() {
//		this->m_pGroup->m_entries.clear();
//		//load group info
//
//		//list folders in group.
//		gEnv->pFileIO->FindFiles(this->m_pGroup->m_path, "*.*",
//			[&](const char* filePath) -> bool {
//			if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
//				if (gEnv->pFileIO->Exists(string(filePath) + "/SongInfo.xml")) { //if the songinfo.xml exists in that directory
//					string spath = filePath;
//					size_t slash = spath.find_last_of("/\\");
//					//string path = spath.substr(0, slash);
//					string folder = spath.substr(slash + 1);
//
//					SongEntry * newEntry = new SongEntry(folder, filePath); //create a new entry.
//					this->m_pGroup->m_entries.push_back(newEntry); //push it to the list of entries.
//					this->StartAsChild(newEntry->getJob()); //start the processing of the entry.
//				}
//			}
//			return true; // continue iterating
//		}
//		);
//
//		this->WaitForChildren(); //wait until all entries have finished.
//	}
//	//----------------------------------------------
//
//	//----------------------------------------------
//	//Song List
//	//----------------------------------------------
//	SongList::SongList(string path = "@songs@") {
//		this->m_path = path;
//		SongListJob(this);
//	}
//
//	SongList::~SongList() {
//		this->m_groups.clear();
//	}
//
//	void SongList::GetMemoryUsage(ICrySizer* pSizer) const {
//		pSizer->AddObject(this, sizeof(*this));
//		pSizer->AddObject(this->m_groups);
//	}
//
//	void SongList::Log() {
//		CryLog("Song List:");
//		for (SongGroup *group : this->m_groups) {
//			CryLog("-%s", group->getName());
//			group->Log();
//		}
//	}
//
//	//----------------------------------------------
//	SongList::SongListJob::SongListJob(SongList * songlist) : Job(true, nullptr) {
//		this->m_pSonglist = songlist;
//		this->StartAndWaitForCompletion();
//	}
//
//	void SongList::SongListJob::Process() {
//		this->m_pSonglist->m_groups.clear(); //clear the groups.
//
//											 //list folders
//		gEnv->pFileIO->FindFiles(this->m_pSonglist->m_path, "*.*",
//			[&](const char* filePath) -> bool {
//			if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
//				if (gEnv->pFileIO->Exists(string(filePath) + "/GroupInfo.xml")) { //if the group info xml file exists in that directory
//					string spath = filePath;
//					size_t slash = spath.find_last_of("/\\");
//					//string path = spath.substr(0, slash);
//					string folder = spath.substr(slash + 1);
//
//					SongGroup * newGroup = new SongGroup(folder, filePath);
//					this->m_pSonglist->m_groups.push_back(newGroup); //push it to the list of groups.
//					this->StartAsChild(newGroup->getJob()); //start the processing of the group.
//				}
//			}
//			return true; // continue iterating
//		}
//		);
//
//		this->WaitForChildren(); //wait until all the groups have finished.
//	}
//	//----------------------------------------------
//}