#ifndef _H_SONGLIST
#define _H_SONGLIST

#pragma once

namespace LYGame {
	class SongEntry : public AZ::Job {
	public:
		SongEntry(string name, string path);
		~SongEntry();
	public:
		string getName() { return this->m_name; }
		string getPath() { return this->m_path; }
	protected:
		void Process();
	private:
		std::vector<std::pair<string, NoteFileInfo>> m_notemaps;
		std::vector<std::pair<string, TranslationFileInfo>> m_translations;
		string m_name;
		string m_path;
		SongFileInfo m_info;
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	public:
		void Log();
	};

	class SongGroup : public AZ::Job {
	public:
		SongGroup(string name, string path);
		~SongGroup();
	public:
		string getName() { return this->m_name; }
		string getPath() { return this->m_path; }
	protected:
		void Process();
	private:
		std::vector<SongEntry *> m_entries;
		string m_name;
		string m_path;
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	public:
		void Log();
	};

	class SongList : public AZ::Job {
	public:
		SongList(string path);
		~SongList();
	protected:
		void Process();
	private:
		std::vector<SongGroup *> m_groups;
		string m_path;
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	public:
		void Log();
	};
}

#endif