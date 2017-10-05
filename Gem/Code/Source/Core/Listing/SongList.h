#ifndef _H_SONGLIST
#define _H_SONGLIST

#pragma once

#include <SQLite\SQLiteBus.h>

namespace OpenDiva {
	//REDESIGN! (well, maybe not...)
	/*
	1. build file list in memory
		-crc hash of directory names + xml names
		-crc hash of modify date (+ possible other stuff)
		-crc db entry hash (db entries that are not in the file system) [used to remove entries from db]
	2. get database list based on crc hash of dir name + xml name
	3. compare crc modify date hash with database list
		-if crc modify hash matches, ignore
		-if crc modify hash does not match, update
	4. remove entry in memory file list, go to 3 until end of database list.
	5. any extra in memory file list, add directly to database
	6. check if there are entries in db that do not match db entry hash and remove them.
	*/

	class SongList {
	public:
		static void Refresh();
	public:
		static long long GroupCount();
		static long long SongCount();
	private:
		static void ProcessGroup(AZStd::string path, AZStd::string dbcrc, SQLite3::SQLiteDB * sysDb);
		static void ProcessSong(AZStd::string path, AZStd::string guuid, AZStd::string dbcrc, SQLite3::SQLiteDB * sysDb);
	private:
		static long long m_groups, m_songs;
		static AZStd::mutex m_mutex;
	};
}

#endif