#include <StdAfx.h>

#include <AzCore\Jobs\Job.h>
#include <AzCore\Jobs\JobManager.h>

#include "NoteFile.h"
#include "LyricsFile.h"
#include "SongInfo.h"
#include "SongList.h"

namespace LYGame {
}

namespace LYGame {
	//----------------------------------------------
	//Song Entry
	//----------------------------------------------
	SongEntry::SongEntry(string name, string path) {
		this->m_name = name;
		this->m_path = path;
	}

	SongEntry::~SongEntry() {
	}

	void SongEntry::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(this->m_notemaps);
		//pSizer->AddObject(this->m_translations);
	}

	void SongEntry::Log() {
		CryLog("---Name: %s", this->m_info.name);
		CryLog("---Authors:");
		for each (std::pair<string, SongFileInfo::Author> auth in this->m_info.authors) {
			CryLog("---- %s, %s, %s", auth.second.name, auth.second.nameE, auth.second.nameR);
		}
		CryLog("---Number of notemaps: %i", this->m_notemaps.size());
		//CryLog("---Number of translations: %i", this->m_translations.size());
	}

	//----------------------------------------------
	SongEntry::SongEntryJob::SongEntryJob(SongEntry * entry) : AZ::Job(true, nullptr) {
		this->m_pEntry = entry;
	}

	void SongEntry::SongEntryJob::Process(){
		/*this->m_notemaps.clear(); //clear the list of notemaps.
		this->m_translations.clear(); //clear the list of translations.

		//load song info file.
		this->m_info = SongInfo::GetInfo(this->m_path + "/SongInfo.xml"); //get the song info.

		if (!this->m_info.valid) return; //if the song file info is not valid, do not process.

		//notemaps
		//open up the note map diretory.
		string nmDirStr = this->m_path + "/NoteMaps";
		DIR * nmDir = opendir(nmDirStr.c_str());

		dirent *nmEntry = readdir(nmDir); //read the first entry.

		//while we have entries.
		while (nmEntry != NULL) {
			string entryName(nmEntry->d_name); //get the entry name,
			if (
				entryName.compare(".") != 0 && //if we are not the "." directory/file
				entryName.compare("..") != 0 && //if we are not the ".." directory/file
				nmEntry->d_type == DT_REG //and if we are a file.
			) {
				string noteFilePath = nmDirStr + "/" + entryName; //get the path of the file.
				NoteFileInfo info = NoteFile::GetInfo(noteFilePath.c_str()); //get the note map info.
				if (info.valid) //if the info is valid.
					this->m_notemaps.push_back(std::make_pair(entryName, info)); //add it to the list of notemaps.
			}
			nmEntry = readdir(nmDir);
		}

		closedir(nmDir);

		//translations
		string tDirStr = this->m_path + "/Translations";
		DIR * tDir = opendir(tDirStr.c_str());

		dirent *tEntry = readdir(tDir);
		while (tEntry != NULL) {
			string entryName(tEntry->d_name);
			if (
				entryName.compare(".") != 0 &&
				entryName.compare("..") != 0 &&
				tEntry->d_type == DT_REG
			) {
				string translationFilePath = tDirStr + "/" + entryName;
				TranslationFileInfo info = TranslationFile::GetInfo(translationFilePath.c_str());
				if (info.valid)
					this->m_translations.push_back(std::make_pair(entryName, info));
			}
			tEntry = readdir(tDir);
		}

		closedir(tDir);*/

		//load songinfo.xml
		//list note files
		//list translations
	}
	//----------------------------------------------


	//----------------------------------------------
	//Song Group
	//----------------------------------------------
	SongGroup::SongGroup(string name, string path){
		this->m_name = name;
		this->m_path = path;
	}

	SongGroup::~SongGroup() {
		this->m_entries.clear();
	}

	void SongGroup::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(this->m_entries);
	}

	void SongGroup::Log() {
		for (SongEntry *entry : this->m_entries) {
			CryLog("--%s",entry->getName());
			entry->Log();
		}
	}

	//----------------------------------------------
	SongGroup::SongGroupJob::SongGroupJob(SongGroup * group) : Job(true, nullptr) {
		this->m_pGroup = group;
	}

	void SongGroup::SongGroupJob::Process() {
		this->m_pGroup->m_entries.clear();
		//load group info

		//list folders in group.
		gEnv->pFileIO->FindFiles(this->m_pGroup->m_path, "*.*",
			[&](const char* filePath) -> bool {
				if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
					if (gEnv->pFileIO->Exists(string(filePath) + "/SongInfo.xml")) { //if the songinfo.xml exists in that directory
						string spath = filePath;
						size_t slash = spath.find_last_of("/\\");
						//string path = spath.substr(0, slash);
						string folder = spath.substr(slash + 1);

						SongEntry * newEntry = new SongEntry(folder, filePath); //create a new entry.
						this->m_pGroup->m_entries.push_back(newEntry); //push it to the list of entries.
						this->StartAsChild(newEntry->getJob()); //start the processing of the entry.
					}
				}
				return true; // continue iterating
			}
		);

		this->WaitForChildren(); //wait until all entries have finished.
	}
	//----------------------------------------------

	//----------------------------------------------
	//Song List
	//----------------------------------------------
	SongList::SongList(string path = "@songs@"){
		this->m_path = path;
		SongListJob(this);
	}

	SongList::~SongList() {
		this->m_groups.clear();
	}

	void SongList::GetMemoryUsage(ICrySizer* pSizer) const {
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(this->m_groups);
	}

	void SongList::Log() {
		CryLog("Song List:");
		for (SongGroup *group : this->m_groups) {
			CryLog("-%s", group->getName());
			group->Log();
		}
	}

	//----------------------------------------------
	SongList::SongListJob::SongListJob(SongList * songlist) : Job(true, nullptr) {
		this->m_pSonglist = songlist;
		this->StartAndWaitForCompletion();
	}

	void SongList::SongListJob::Process() {
		this->m_pSonglist->m_groups.clear(); //clear the groups.
		
		//list folders
		gEnv->pFileIO->FindFiles(this->m_pSonglist->m_path, "*.*",
			[&](const char* filePath) -> bool {
				if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
					if (gEnv->pFileIO->Exists(string(filePath) + "/GroupInfo.xml")) { //if the group info xml file exists in that directory
						string spath = filePath;
						size_t slash = spath.find_last_of("/\\");
						//string path = spath.substr(0, slash);
						string folder = spath.substr(slash + 1);

						SongGroup * newGroup = new SongGroup(folder, filePath);
						this->m_pSonglist->m_groups.push_back(newGroup); //push it to the list of groups.
						this->StartAsChild(newGroup->getJob()); //start the processing of the group.
					}
				}
				return true; // continue iterating
			}
		);

		this->WaitForChildren(); //wait until all the groups have finished.
	}
	//----------------------------------------------
}