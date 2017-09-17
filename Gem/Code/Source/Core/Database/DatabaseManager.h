#pragma once

#include <SQLite\SQLiteBus.h>

namespace OpenDiva {
	class DatabaseManager {
	public:
		static void Init();
	private:
		static bool InitVersions(SQLite3::SQLiteDB * sysDb);
		static bool InitGroups(SQLite3::SQLiteDB * sysDb);
		static bool InitSongs(SQLite3::SQLiteDB * sysDb);
		static bool InitNotemaps(SQLite3::SQLiteDB * sysDb);
		static bool InitLyrics(SQLite3::SQLiteDB * sysDb);
		static bool InitPlayers(SQLite3::SQLiteDB * sysDb);
		static bool InitScores(SQLite3::SQLiteDB * sysDb);
	public:

		//for playing a song
		//paths
		//[0] - song path - @songs@/<groupdir>/<songdir>/
		//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
		//[2] - notemap - @songs@/<groupdir>/<songdir>/Notemaps/<notemap filename>.xml
		//[3] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml
		static AZStd::array<AZStd::string, 4> BuildSongPaths(AZStd::string nmuuid, AZStd::string luuid);

		//for watching a song instead of playing one. (i.e. demo mode)
		//paths
		//[0] - song path - @songs@/<groupdir>/<songdir>/
		//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
		//[3] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml
		static AZStd::array<AZStd::string,3> BuildSongPathsWatch(AZStd::string suuid, AZStd::string luuid);
	};
}