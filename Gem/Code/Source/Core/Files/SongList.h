#ifndef _H_SONGLIST
#define _H_SONGLIST

#pragma once
namespace OpenDiva {
	class SongList : public AZ::Job {
	public:
		SongList(std::string path = "@songs@") : m_path(path), AZ::Job(true, nullptr) {}
	protected:
		void Process();
	private:
		std::string m_path;

	private:
		class SongGroup : public AZ::Job {
		public:
			SongGroup(std::string group, std::string path, std::string crc) : m_group(group), m_path(path), m_crc(crc), AZ::Job(true, nullptr) {}
		protected:
			void Process();
		private:
			std::string m_group;
			std::string m_path;
			std::string m_crc;

		private:
			class SongEntry : public AZ::Job {
			public:
				SongEntry(std::string grouphash, std::string songname, std::string path, std::string crc) : m_groupHash(grouphash), m_songName(songname), m_path(path), m_crc(crc), AZ::Job(true, nullptr) {}
			protected:
				void Process();
			private:
				std::string m_groupHash;
				std::string m_songName;
				std::string m_path;
				std::string m_crc;

			private:
				class SongNoteMap : public AZ::Job {
				public:
					SongNoteMap(std::string songhash, std::string notemapname, std::string path, std::string crc) : m_songHash(songhash), m_notemapName(notemapname), m_path(path), m_crc(crc), AZ::Job(true, nullptr) {}
				protected:
					void Process();
				private:
					std::string m_songHash;
					std::string m_notemapName;
					std::string m_path;
					std::string m_crc;
				};
			};
		};
	};

	/*
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

	class SongGroup {
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
	*/
}

#endif