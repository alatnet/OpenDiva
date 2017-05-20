#ifndef _H_SONGLIST
#define _H_SONGLIST

#pragma once

namespace LYGame {
	class SongEntry {
	public:
		SongEntry(string name, string path);
		~SongEntry();
	public:
		string getName() { return this->m_name; }
		string getPath() { return this->m_path; }
	private:
		std::vector<std::pair<string, NoteFile::NoteFileInfo>> m_notemaps;
		//std::vector<std::pair<string, TranslationFileInfo>> m_translations;
		string m_name;
		string m_path;
		SongFileInfo m_info;
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	public:
		void Log();
	public:
		AZ::Job * getJob() { return new SongEntryJob(this); }
	private:
		class SongEntryJob : public AZ::Job {
		public:
			SongEntryJob(SongEntry * entry);
		protected:
			void Process();
		private:
			SongEntry * m_pEntry;
		};
	};

	class SongGroup{
	public:
		SongGroup(string name, string path);
		~SongGroup();
	public:
		string getName() { return this->m_name; }
		string getPath() { return this->m_path; }
	private:
		std::vector<SongEntry *> m_entries;
		string m_name;
		string m_path;
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	public:
		void Log();
	public:
		AZ::Job* getJob() { return new SongGroupJob(this); }
	private:
		class SongGroupJob : public AZ::Job {
		public:
			SongGroupJob(SongGroup * group);
		protected:
			void Process();
		private:
			SongGroup * m_pGroup;
		};
	};

	class SongList {
	public:
		SongList(string path);
		~SongList();
	private:
		std::vector<SongGroup *> m_groups;
		string m_path;
	public:
		void GetMemoryUsage(ICrySizer* pSizer) const;
	public:
		void Log();
	private:
		class SongListJob : public AZ::Job {
		public:
			SongListJob(SongList * songlist);
		protected:
			void Process();
		private:
			SongList * m_pSonglist;
		};
	};
}

#endif