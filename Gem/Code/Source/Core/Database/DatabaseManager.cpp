#include "StdAfx.h"
#include "DatabaseManager.h"

namespace OpenDiva {
	void DatabaseManager::Init() {
		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		AZStd::string dbPath = "@cache@/database.db";

		//open db
		int ret = sysDb->Open(dbPath.c_str());
		AZ_Assert(ret == SQLITE_OK, "Opening DB Failed. - %s", sysDb->ErrMsg());

		//enable foreign key support
		if (ret == SQLITE_OK) {
			ret = sysDb->Exec("PRAGMA foreign_keys = \"1\";", nullptr, nullptr, nullptr);
			AZ_Assert(ret == SQLITE_OK, "setting foreing keys failed. - %s", sysDb->ErrMsg());
		}

		if (ret == SQLITE_OK) {
			InitVersions(sysDb);
			InitGroups(sysDb);
			InitSongs(sysDb);
			InitNotemaps(sysDb);
			InitLyrics(sysDb);
			InitPlayers(sysDb);
			InitScores(sysDb);
		}
	}

	bool DatabaseManager::InitVersions(SQLite3::SQLiteDB * sysDb) {
		//create Versions table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Versions ("
				"id INTEGER UNIQUE NOT NULL,"
				"Database TEXT UNIQUE NOT NULL,"
				"High INTEGER NOT NULL,"
				"Low INTEGER NOT NULL,"
				"Rev INTEGER NOT NULL,"
				"PRIMARY KEY (id, Database)"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating Versions Table Failed. - %s", sysDb->ErrMsg());

		//setup versions data
		if (ret == SQLITE_OK) {
			ret = sysDb->Exec(
				"INSERT OR IGNORE INTO Versions("
					"id,"
					"Database,"
					"High,"
					"Low,"
					"Rev"
				") VALUES ("
					"0,"
					"'Versions',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"1,"
					"'Database',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"2,"
					"'Groups',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"3,"
					"'GroupNames',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"4,"
					"'Songs',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"5,"
					"'SongNames',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"6,"
					"'Notemaps',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"7,"
					"'Lyrics',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"8,"
					"'Players',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"9,"
					"'Scores',"
					"1,"
					"0,"
					"0"
				");",
				nullptr, nullptr, nullptr
			);
			AZ_Assert(ret == SQLITE_OK, "Creating Versions Data Failed. - %s", sysDb->ErrMsg());
		}

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitGroups(SQLite3::SQLiteDB * sysDb) {
		//create song Groups table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Groups("
				"guuid TEXT NOT NULL PRIMARY KEY UNIQUE," //dirname + salt
				"name TEXT NOT NULL," //dirname
				"artpath TEXT," //full path to the artwork
				"dbcrc TEXT NOT NULL" //database modify crc
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating Groups Table Failed. - %s", sysDb->ErrMsg());

		//create table to hold translations of group names
		ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS GroupNames ("
				"gnuuid TEXT NOT NULL PRIMARY KEY UNIQUE," //filename + guuid + salt
				"name TEXT,"
				"description TEXT,"
				"lang TEXT NOT NULL," //doubles as filenam
				//"mcrc TEXT NOT NULL," //file modify crc
				"dbcrc TEXT NOT NULL," //database modify crc
				"guuid TEXT NOT NULL,"
				"FOREIGN KEY (guuid) REFERENCES Groups (guuid)"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating GroupNames Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitSongs(SQLite3::SQLiteDB * sysDb) {
		//create Songs table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Songs (" //global song data
				"suuid TEXT NOT NULL PRIMARY KEY UNIQUE," //dirname + guuid + salt
				"name TEXT NOT NULL," //dirname
				"bpm TEXT NOT NULL,"
				"length INTEGER NOT NULL,"
				"artpath TEXT," //full path to the artwork
				//"models TEXT,"
				"dbcrc TEXT NOT NULL," //database modify crc
				"guuid TEXT NOT NULL,"
				"FOREIGN KEY (guuid) REFERENCES Groups (guuid)"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating Songs Table Failed. - %s", sysDb->ErrMsg());

		//create table to hold translations of song names
		ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS SongNames ("
				"snuuid TEXT NOT NULL PRIMARY KEY UNIQUE," //filename + suuid + salt
				"name TEXT,"
				"description TEXT,"
				"authors TEXT,"
				"lang TEXT NOT NULL," //doubles as filename
				//"mcrc TEXT NOT NULL," //file modify crc
				"dbcrc TEXT NOT NULL," //database modify crc
				"suuid TEXT NOT NULL,"
				"FOREIGN KEY (suuid) REFERENCES Songs (suuid)"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating SongNames Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitNotemaps(SQLite3::SQLiteDB * sysDb) {
		//create song Notemaps table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Notemaps ("
				"nmuuid TEXT NOT NULL PRIMARY KEY UNIQUE," //filename + suuid + salt
				"filename TEXT NOT NULL,"
				"name TEXT NOT NULL,"
				"author TEXT,"
				"description TEXT,"
				"difficulty INT NOT NULL,"
				"version INT NOT NULL,"
				//"mcrc TEXT NOT NULL," //file modify crc
				"dbcrc TEXT NOT NULL," //database modify crc
				"suuid TEXT NOT NULL,"
				"FOREIGN KEY (suuid) REFERENCES Songs (suuid)"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating Notemaps Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitLyrics(SQLite3::SQLiteDB * sysDb) { //change to lyrics and merge translation subsystem?
		//create lyrics table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Lyrics("
				"luuid TEXT NOT NULL PRIMARY KEY UNIQUE," //filename + suuid + salt
				"filename TEXT NOT NULL,"
				"name TEXT,"
				"author TEXT,"
				"description TEXT,"
				"version INTEGER,"
				"lang TEXT,"
				//"mcrc TEXT NOT NULL," //file modify crc
				"dbcrc TEXT NOT NULL," //database modify crc
				"suuid TEXT NOT NULL,"
				"FOREIGN KEY(suuid) REFERENCES Songs(suuid)"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating Lyrics Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitPlayers(SQLite3::SQLiteDB * sysDb) {
		//create Players table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Players ("
				"puuid TEXT PRIMARY KEY UNIQUE NOT NULL,"
				"username TEXT UNIQUE NOT NULL"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating Players Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitScores(SQLite3::SQLiteDB * sysDb) {
		//create Scores table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Scores ("
				"id INT NOT NULL PRIMARY KEY UNIQUE,"
				"score INT NOT NULL,"
				"nmuuid TEXT NOT NULL,"
				"suuid TEXT NOT NULL,"
				"puuid TEXT NOT NULL,"
				"FOREIGN KEY(puuid) REFERENCES Players(puuid),"
				"FOREIGN KEY(suuid) REFERENCES Songs(suuid),"
				"FOREIGN KEY(nmuuid) REFERENCES Notemaps(nmuuid)"
			");",
			nullptr, nullptr, nullptr
		);
		AZ_Assert(ret == SQLITE_OK, "Creating Scores Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}

	//paths
	//[0] - song path - @songs@/<groupdir>/<songdir>/
	//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
	//[2] - notemap - @songs@/<groupdir>/<songdir>/Notemaps/<notemap filename>.xml
	//[3] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml
	AZStd::array<AZStd::string, 4> DatabaseManager::BuildSongPaths(AZStd::string nmuuid, AZStd::string luuid) {
		AZStd::string suuid, guuid;
		AZStd::string notemapfn;
		AZStd::string lyricsfn;
		AZStd::string songdir;
		AZStd::string groupdir;
		AZStd::array<AZStd::string, 4> ret;
		ret[0] = ret[1] = ret[2] = ret[3] = "";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		//get suuid
		//get guuid
		//get song dir name
		//get group dir name
		//get notemap filename
		//get lyrics filename
		//build paths

		AZStd::string notemap_stmt_str = "SELECT suuid, filename FROM Notemaps WHERE nmuuid=\'" + nmuuid + "\';"; //get suuid and notemap filename - <notemap filename>
		AZStd::string lyrics_stmt_str = "SELECT filename FROM Lyrics WHERE luuid=\'" + luuid + "\'"; //get lyrics filename - <Lyrics filename>

		//stage 1 statements
		SQLite3::SQLiteStmt * notemap_stmt = sysDb->Prepare_v2(notemap_stmt_str.c_str(), -1, nullptr);
		SQLite3::SQLiteStmt * lyrics_stmt = sysDb->Prepare_v2(lyrics_stmt_str.c_str(), -1, nullptr);

		notemap_stmt->Step();

		if (notemap_stmt->Column_Count() != 2) {
			CLOG("Notemap Column Count: %i", notemap_stmt->Column_Count());
			return ret; //error out
		}

		suuid = notemap_stmt->Column_Text(0); //suuid
		notemapfn = notemap_stmt->Column_Text(1); //filename
		delete notemap_stmt;

		if (suuid.empty() || notemapfn.empty()) return ret; //error out

		lyrics_stmt->Step();

		if (lyrics_stmt->Column_Count() != 1) {
			CLOG("Lyrics Column Count: %i", lyrics_stmt->Column_Count());
			return ret; //error out
		}

		lyricsfn = lyrics_stmt->Column_Text(0); //filename
		delete lyrics_stmt;

		if (lyricsfn.empty()) return ret; //error out

		AZStd::string song_stmt_str = "SELECT guuid, name FROM Songs WHERE suuid=\'" + suuid + "\';"; //get guuid and song dir - <songdir>

		//stage 2 statement
		SQLite3::SQLiteStmt * song_stmt = sysDb->Prepare_v2(song_stmt_str.c_str(), -1, nullptr);

		song_stmt->Step();

		if (song_stmt->Column_Count() != 2) {
			CLOG("Song Column Count: %i", song_stmt->Column_Count());
			return ret; //error out
		}

		guuid = song_stmt->Column_Text(0); //guuid
		songdir = song_stmt->Column_Text(1); //songdir
		delete song_stmt;

		if (guuid.empty() || songdir.empty()) return ret; //error out

		AZStd::string group_stmt_str = "SELECT name FROM Groups WHERE guuid=\'" + guuid + "\';"; //get group dir - <groupdir>

		//stage 3 statement
		SQLite3::SQLiteStmt * group_stmt = sysDb->Prepare_v2(group_stmt_str.c_str(), -1, nullptr);

		group_stmt->Step();

		if (group_stmt->Column_Count() != 1) {
			CLOG("Group Column Count: %i", group_stmt->Column_Count());
			return ret; //error out
		}

		groupdir = group_stmt->Column_Text(0); //groupdir
		delete group_stmt;

		if (groupdir.empty()) return ret; //error out

		//paths
		//[0] - song path - @songs@/<groupdir>/<songdir>/
		//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
		//[2] - notemap - @songs@/<groupdir>/<songdir>/Notemaps/<notemap filename>.xml
		//[3] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml

		//build paths
		ret[0] = "@songs@/" + groupdir + "/" + songdir + "/";
		ret[1] = "@songs@/" + groupdir + "/" + songdir + "/SongInfo/global.xml";
		ret[2] = "@songs@/" + groupdir + "/" + songdir + "/Notemaps/" + notemapfn;
		ret[3] = "@songs@/" + groupdir + "/" + songdir + "/Lyrics/" + lyricsfn;

		//convert slashes to native slash
		ret[0] = PathUtil::ToNativePath(ret[0].c_str()).c_str();
		ret[1] = PathUtil::ToNativePath(ret[1].c_str()).c_str();
		ret[2] = PathUtil::ToNativePath(ret[2].c_str()).c_str();
		ret[3] = PathUtil::ToNativePath(ret[3].c_str()).c_str();

		return ret;
	}

	//paths
	//[0] - song path - @songs@/<groupdir>/<songdir>/
	//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
	//[2] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml
	AZStd::array<AZStd::string, 3> DatabaseManager::BuildSongPathsWatch(AZStd::string suuid, AZStd::string luuid) {
		AZStd::string guuid;
		AZStd::string lyricsfn;
		AZStd::string songdir;
		AZStd::string groupdir;
		AZStd::array<AZStd::string, 3> ret;
		ret[0] = ret[1] = ret[2] = "";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		AZStd::string lyrics_stmt_str = "SELECT filename FROM Lyrics WHERE luuid=\'" + luuid + "\'"; //get lyrics filename - <Lyrics filename>

		//stage 1 statements
		SQLite3::SQLiteStmt * lyrics_stmt = sysDb->Prepare_v2(lyrics_stmt_str.c_str(), -1, nullptr);

		lyrics_stmt->Step();
		lyricsfn = lyrics_stmt->Column_Text(0); //filename
		delete lyrics_stmt;

		if (lyricsfn.empty()) return ret; //error out

		AZStd::string song_stmt_str = "SELECT guuid, name FROM Songs WHERE suuid=\'" + suuid + "\';"; //get guuid and song dir - <songdir>

		//stage 2 statement
		SQLite3::SQLiteStmt * song_stmt = sysDb->Prepare_v2(song_stmt_str.c_str(), -1, nullptr);

		song_stmt->Step();
		guuid = song_stmt->Column_Text(0); //guuid
		songdir = song_stmt->Column_Text(1); //songdir
		delete song_stmt;

		if (guuid.empty() || songdir.empty()) return ret; //error out

		AZStd::string group_stmt_str = "SELECT name FROM Groups WHERE guuid=\'" + guuid + "\';"; //get group dir - <groupdir>

																								 //stage 3 statement
		SQLite3::SQLiteStmt * group_stmt = sysDb->Prepare_v2(group_stmt_str.c_str(), -1, nullptr);

		group_stmt->Step();
		groupdir = group_stmt->Column_Text(0); //groupdir
		delete group_stmt;

		if (groupdir.empty()) return ret; //error out

		//paths
		//[0] - song path - @songs@/<groupdir>/<songdir>/
		//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
		//[2] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml

		//build paths
		ret[0] = "@songs@/" + groupdir + "/" + songdir + "/";
		ret[1] = "@songs@/" + groupdir + "/" + songdir + "/SongInfo/global.xml";
		ret[2] = "@songs@/" + groupdir + "/" + songdir + "/Lyrics/" + lyricsfn;

		//convert slashes to native slash
		ret[0] = PathUtil::ToNativePath(ret[0].c_str()).c_str();
		ret[1] = PathUtil::ToNativePath(ret[1].c_str()).c_str();
		ret[2] = PathUtil::ToNativePath(ret[2].c_str()).c_str();

		return ret;
	}
}