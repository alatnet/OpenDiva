#include "StdAfx.h"
#include "DatabaseManager.h"

WX_DISABLE_(4390)

namespace OpenDiva {
	void DatabaseManager::Init() {
		SQLite3::SQLiteDB * sysDb = nullptr;
		//SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		EBUS_EVENT_ID_RESULT(sysDb, AZ::EntityId(0), SQLite::SQLiteRequestBus, GetConnection);
		AZ_Assert(sysDb, "sysDb is null.");

		AZStd::string root = gEnv->pFileIO->GetAlias("@cache@");
		root += "/";

		AZStd::string dbPath = root + "database.db";

		//open db
		int ret = sysDb->Open(dbPath.c_str());
		AZ_Error("DatabaseManager", "Opening DB Failed. - %s", sysDb->ErrMsg());

		//enable foreign key support
		if (ret == SQLITE_OK) {
			ret = sysDb->Exec("PRAGMA foreign_keys = \"1\";", nullptr, nullptr, nullptr);
			AZ_Error("DatabaseManager", "setting foreing keys failed. - %s", sysDb->ErrMsg());
		}

		if (ret == SQLITE_OK) {
			InitVersions(sysDb);
			InitGroups(sysDb);
			InitSongs(sysDb);
			InitNotemaps(sysDb);
			InitLyrics(sysDb);
			InitPlayers(sysDb);
			InitScores(sysDb);
			InitJudges(sysDb);
			InitStyles(sysDb);
			InitGlobal(sysDb);
			InitSettings(sysDb);
			//InitMenuSettings(sysDb);
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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Versions Table Failed. - %s", sysDb->ErrMsg());

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
				"),"
				"("
					"10,"
					"'Judges',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"11,"
					"'Huds',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"12,"
					"'Styles',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"13,"
					"'MainMenus',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"14,"
					"'Global',"
					"1,"
					"0,"
					"0"
				"),"
				"("
					"15,"
					"'Settings',"
					"1,"
					"0,"
					"0"
				");",
				nullptr, nullptr, nullptr
			);
			if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Versions Data Failed. - %s", sysDb->ErrMsg());
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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Groups Table Failed. - %s", sysDb->ErrMsg());

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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating GroupNames Table Failed. - %s", sysDb->ErrMsg());

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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Songs Table Failed. - %s", sysDb->ErrMsg());

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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating SongNames Table Failed. - %s", sysDb->ErrMsg());

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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Notemaps Table Failed. - %s", sysDb->ErrMsg());

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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Lyrics Table Failed. - %s", sysDb->ErrMsg());

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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Players Table Failed. - %s", sysDb->ErrMsg());

		//setup default player
		if (ret == SQLITE_OK) {
			ret = sysDb->Exec(
				"INSERT OR IGNORE INTO Players ("
					"puuid,"
					"username"
				") VALUES ("
					"'0',"
					"'default'"
				");",
				nullptr, nullptr, nullptr
			);
			if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Default Player Failed. - %s", sysDb->ErrMsg());
		}

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
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Scores Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}

	bool DatabaseManager::InitJudges(SQLite3::SQLiteDB * sysDb) {
		//create judges table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Judges ("
				"juuid TEXT NOT NULL PRIMARY KEY UNIQUE,"
				"name TEXT NOT NULL,"
				"type TEXT NOT NULL,"
				"dbcrc TEXT NOT NULL" //database modify crc
			");",
			nullptr, nullptr, nullptr
		);
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Judges Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitStyles(SQLite3::SQLiteDB * sysDb) {
		//create style table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Styles ("
				"stuuid TEXT NOT NULL PRIMARY KEY UNIQUE,"
				"name TEXT NOT NULL,"
				"dirname TEXT NOT NULL,"
				"description TEXT,"
				"version INTEGER NOT NULL,"
				"hasEffects INTEGER NOT NULL,"
				"hasFonts INTEGER NOT NULL,"
				"hasNotes INTEGER NOT NULL,"
				"hasTails INTEGER NOT NULL,"
				"hasHud INTEGER NOT NULL,"
				"hasMenu INTEGER NOT NULL,"
				"hasGrade INTEGER NOT NULL,"
				"hasSetup INTEGER NOT NULL,"
				"hasLoad INTEGER NOT NULL,"
				"dbcrc TEXT NOT NULL" //database modify crc
			");",
			nullptr, nullptr, nullptr
		);
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Styles Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}

	bool DatabaseManager::InitGlobal(SQLite3::SQLiteDB * sysDb) {
		//create main menus table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Global ("
				"id INTEGER NOT NULL PRIMARY KEY UNIQUE,"
				"setting TEXT NOT NULL UNIQUE,"
				"value TEXT NOT NULL UNIQUE"
			");",
			nullptr, nullptr, nullptr
		);
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Global Settings Table Failed. - %s", sysDb->ErrMsg());

		//setup settings data
		if (ret == SQLITE_OK) {
			ret = sysDb->Exec(
				"INSERT OR IGNORE INTO Global ("
					"id,"
					"setting,"
					"value"
				") VALUES ("
					"0,"
					"'player',"
					"'0'"
				"),"
				"("
					"1,"
					"'fullscreen',"
					"'false'"
				"),"
				"("
					"2,"
					"'resW',"
					"'1280'"
				"),"
				"("
					"3,"
					"'resH',"
					"'720'"
				");",
				nullptr, nullptr, nullptr
			);
			if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Global Settings Data Failed. - %s", sysDb->ErrMsg());
		}

		return ret == SQLITE_OK;
	}
	bool DatabaseManager::InitSettings(SQLite3::SQLiteDB * sysDb) {
		//create main menus table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS Settings ("
				"id INTEGER NOT NULL PRIMARY KEY UNIQUE,"
				"puuid TEXT NOT NULL,"
				"setting TEXT NOT NULL UNIQUE,"
				"value TEXT NOT NULL UNIQUE,"
				"FOREIGN KEY(puuid) REFERENCES Players(puuid)"
			");",
			nullptr, nullptr, nullptr
		);
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Player Settings Table Failed. - %s", sysDb->ErrMsg());

		//setup settings data
		if (ret == SQLITE_OK) {
			ret = sysDb->Exec(
				"INSERT OR IGNORE INTO Settings ("
					"id,"
					"puuid,"
					"setting,"
					"value"
				") VALUES ("
					"0,"
					"'0',"
					"'songOffset',"
					"'0'"
				");",
				nullptr, nullptr, nullptr
			);
			if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Default Player Settings Data Failed. - %s", sysDb->ErrMsg());
		}

		return ret == SQLITE_OK;
	}
	/*bool DatabaseManager::InitMenuSettings(SQLite3::SQLiteDB * sysDb) {
		//create main menus table
		int ret = sysDb->Exec(
			"CREATE TABLE IF NOT EXISTS MenuSettings ("
				"id INTEGER NOT NULL PRIMARY KEY UNIQUE,"
				"puuid TEXT NOT NULL,"
				"mmuuid TEXT NOT NULL,"
				"setting TEXT NOT NULL UNIQUE,"
				"value TEXT NOT NULL UNIQUE,"
				"FOREIGN KEY(puuid) REFERENCES Players(puuid)"
				"FOREIGN KEY(mmuuid) REFERENCES MainMenus(mmuuid)"
			");",
			nullptr, nullptr, nullptr
		);
		if (ret != SQLITE_OK) AZ_Error("DatabaseManager", "Creating Menu Settings Table Failed. - %s", sysDb->ErrMsg());

		return ret == SQLITE_OK;
	}*/

	//paths
	//[0] - song path - @songs@/<groupdir>/<songdir>/
	//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
	//[2] - notemap - @songs@/<groupdir>/<songdir>/Notemaps/<notemap filename>.xml
	//[3] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml
	AZStd::array<AZStd::string, 4> DatabaseManager::BuildSongPaths(AZStd::string nmuuid, AZStd::string luuid) {
		AZStd::string suuid = "", guuid = "";
		AZStd::string notemapfn = "";
		AZStd::string lyricsfn = "";
		AZStd::string songdir = "";
		AZStd::string groupdir = "";
		AZStd::array<AZStd::string, 4> ret;
		ret[0] = ret[1] = ret[2] = ret[3] = "";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return ret;

		//get suuid
		//get guuid
		//get song dir name
		//get group dir name
		//get notemap filename
		//get lyrics filename
		//build paths
		
		//having no lyrics uuid doesnt nessesarily mean that we cant find the other data.
		if (!luuid.empty()) {
			AZStd::string lyrics_stmt_str = "SELECT filename FROM Lyrics WHERE luuid=\'" + luuid + "\';"; //get lyrics filename - <Lyrics filename>

			SQLite3::SQLiteStmt * lyrics_stmt = sysDb->Prepare_v2(lyrics_stmt_str.c_str(), -1, nullptr);

			lyrics_stmt->Step();

			if (lyrics_stmt->Column_Count() != 1) {
				CLOG("Lyrics Column Count: %i", lyrics_stmt->Column_Count());
				//return ret; //error out
			}

			lyricsfn = lyrics_stmt->Column_Text(0); //filename
			delete lyrics_stmt;

			if (lyricsfn.empty()) {
				CLOG("lyricsfn empty.");
				//return ret; //error out
			}
		}

		AZStd::string notemap_stmt_str = "SELECT suuid, filename FROM Notemaps WHERE nmuuid=\'" + nmuuid + "\';"; //get suuid and notemap filename - <notemap filename>

		//stage 1 statements
		SQLite3::SQLiteStmt * notemap_stmt = sysDb->Prepare_v2(notemap_stmt_str.c_str(), -1, nullptr);

		notemap_stmt->Step();

		if (notemap_stmt->Column_Count() != 2) {
			CLOG("Notemap Column Count: %i", notemap_stmt->Column_Count());
			return ret; //error out
		}

		suuid = notemap_stmt->Column_Text(0); //suuid
		notemapfn = notemap_stmt->Column_Text(1); //filename
		delete notemap_stmt;

		if (suuid.empty() || notemapfn.empty()) {
			CLOG("suuid and/or notemapfn empty.");
			return ret; //error out
		}

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

		if (guuid.empty() || songdir.empty()) {
			CLOG("guuid and/or songdir empty.");
			return ret; //error out
		}

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

		if (groupdir.empty()) {
			CLOG("groupdir empty.");
			return ret; //error out
		}

		//paths
		//[0] - song path - @songs@/<groupdir>/<songdir>/
		//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
		//[2] - notemap - @songs@/<groupdir>/<songdir>/Notemaps/<notemap filename>.xml
		//[3] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml

		//build paths
		ret[0] = "@songs@/" + groupdir + "/" + songdir + "/";
		ret[1] = "@songs@/" + groupdir + "/" + songdir + "/SongInfo/global.xml";
		ret[2] = "@songs@/" + groupdir + "/" + songdir + "/Notemaps/" + notemapfn;
		if (!luuid.empty()) ret[3] = "@songs@/" + groupdir + "/" + songdir + "/Lyrics/" + lyricsfn;

		//convert slashes to native slash
		ret[0] = PathUtil::ToNativePath(ret[0].c_str()).c_str();
		ret[1] = PathUtil::ToNativePath(ret[1].c_str()).c_str();
		ret[2] = PathUtil::ToNativePath(ret[2].c_str()).c_str();
		if (!luuid.empty()) ret[3] = PathUtil::ToNativePath(ret[3].c_str()).c_str();

		return ret;
	}

	//paths
	//[0] - song path - @songs@/<groupdir>/<songdir>/
	//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
	//[2] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml
	AZStd::array<AZStd::string, 3> DatabaseManager::BuildSongPathsWatch(AZStd::string suuid, AZStd::string luuid) {
		AZStd::string guuid = "";
		AZStd::string lyricsfn = "";
		AZStd::string songdir = "";
		AZStd::string groupdir = "";
		AZStd::array<AZStd::string, 3> ret;
		ret[0] = ret[1] = ret[2] = "";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return ret;

		//having no lyrics uuid doesnt nessesarily mean that we cant find the other data.
		if (!luuid.empty()) {
			AZStd::string lyrics_stmt_str = "SELECT filename FROM Lyrics WHERE luuid=\'" + luuid + "\';"; //get lyrics filename - <Lyrics filename>

			//stage 1 statements
			SQLite3::SQLiteStmt * lyrics_stmt = sysDb->Prepare_v2(lyrics_stmt_str.c_str(), -1, nullptr);

			lyrics_stmt->Step();
			lyricsfn = lyrics_stmt->Column_Text(0); //filename
			delete lyrics_stmt;

			if (lyricsfn.empty()) {
				CLOG("lyricsfn empty.");
				//return ret; //error out
			}
		}

		AZStd::string song_stmt_str = "SELECT guuid, name FROM Songs WHERE suuid=\'" + suuid + "\';"; //get guuid and song dir - <songdir>

		//stage 2 statement
		SQLite3::SQLiteStmt * song_stmt = sysDb->Prepare_v2(song_stmt_str.c_str(), -1, nullptr);

		song_stmt->Step();
		guuid = song_stmt->Column_Text(0); //guuid
		songdir = song_stmt->Column_Text(1); //songdir
		delete song_stmt;

		if (guuid.empty() || songdir.empty()) {
			CLOG("guuid and/or songdir empty.");
			return ret; //error out
		}

		AZStd::string group_stmt_str = "SELECT name FROM Groups WHERE guuid=\'" + guuid + "\';"; //get group dir - <groupdir>

																								 //stage 3 statement
		SQLite3::SQLiteStmt * group_stmt = sysDb->Prepare_v2(group_stmt_str.c_str(), -1, nullptr);

		group_stmt->Step();
		groupdir = group_stmt->Column_Text(0); //groupdir
		delete group_stmt;

		if (groupdir.empty()) {
			CLOG("groupdir empty.");
			return ret; //error out
		}

		//paths
		//[0] - song path - @songs@/<groupdir>/<songdir>/
		//[1] - songinfo - @songs@/<groupdir>/<songdir>/SongInfo/global.xml
		//[2] - lyrics - @songs@/<groupdir>/<songdir>/Lyrics/<Lyrics filename>.xml

		//build paths
		ret[0] = "@songs@/" + groupdir + "/" + songdir + "/";
		ret[1] = "@songs@/" + groupdir + "/" + songdir + "/SongInfo/global.xml";
		if (!luuid.empty()) ret[2] = "@songs@/" + groupdir + "/" + songdir + "/Lyrics/" + lyricsfn;

		//convert slashes to native slash
		ret[0] = PathUtil::ToNativePath(ret[0].c_str()).c_str();
		ret[1] = PathUtil::ToNativePath(ret[1].c_str()).c_str();
		if (!luuid.empty()) ret[2] = PathUtil::ToNativePath(ret[2].c_str()).c_str();

		return ret;
	}

	AZStd::string DatabaseManager::GetSetting(AZStd::string setting) {
		AZStd::string sql = 
			"SELECT value FROM Settings WHERE Settings.puuid IN"
				"("
					"SELECT value FROM Global WHERE setting = 'player'"
				")"
			"AND Settings.setting = '" + setting + "' LIMIT 1;";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return "";

		SQLite3::SQLiteStmt * stmt = sysDb->Prepare_v2(sql.c_str(), -1, nullptr);

		AZStd::string ret;
		if (stmt->Step() == SQLITE_ROW) {
			ret = stmt->Column_Text(0);
		} else {
			ret = "";
			AZ_Error("DatabaseManager", "Error getting setting %s: %s", setting.c_str(), sysDb->ErrMsg());
		}

		delete stmt;
		return ret;
	}
	bool DatabaseManager::SetSetting(AZStd::string setting, AZStd::string value) {
		AZStd::string sql =
			"INSERT OR REPLACE INTO Settings"
			"("
				"puuid,"
				"setting,"
				"value"
			") VALUES ("
				"("
					"SELECT value FROM Global WHERE setting = 'player'"
				"),"
				"'" + setting + "',"
				"'" + value + "'"
			");";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return false;

		int ret = sysDb->Exec(sql.c_str(), nullptr, nullptr, nullptr);
		if (ret != SQLITE_OK)
			AZ_Error("DatabaseManager", "Error setting %s to %s: %s", setting.c_str(), value.c_str(), sysDb->ErrMsg());
		return ret == SQLITE_OK;
	}

	AZStd::string DatabaseManager::GetGlobalSetting(AZStd::string setting) {
		AZStd::string sql = "SELECT value FROM Global WHERE setting = '" + setting + "' LIMIT 1;";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return "";

		SQLite3::SQLiteStmt * stmt = sysDb->Prepare_v2(sql.c_str(), -1, nullptr);

		AZStd::string ret;

		if (stmt->Step() == SQLITE_ROW) {
			ret = stmt->Column_Text(0);
		} else {
			ret = "";
			AZ_Error("DatabaseManager", "Error getting global setting %s: %s", setting.c_str(), sysDb->ErrMsg());
		}

		delete stmt;
		return ret;
	}
	bool DatabaseManager::SetGlobalSetting(AZStd::string setting, AZStd::string value) {
		AZStd::string sql =
			"INSERT OR REPLACE INTO Global ("
				"setting,"
				"value"
			") VALUES ("
				"'" + setting + "',"
				"'" + value + "'"
			");";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return false;

		int ret = sysDb->Exec(sql.c_str(), nullptr, nullptr, nullptr);
		if (ret != SQLITE_OK)
			AZ_Error("DatabaseManager", "Error setting global %s to %s: %s", setting.c_str(), value.c_str(), sysDb->ErrMsg());
		return ret == SQLITE_OK;
	}

	/*AZStd::string DatabaseManager::GetMenuSetting(AZStd::string setting) {
		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return "";

		AZStd::string sql = 
			"SELECT value FROM MenuSettings WHERE"
				"MenuSettings.puuid IN ("
					"SELECT value FROM Global WHERE setting = 'player' LIMIT 1" //get player uuid
				")"
				"AND MenuSettings.mmuuid IN ("
					"SELECT value FROM Settings WHERE Settings.puuid IN (" //get menu uuid
						"SELECT value FROM Global WHERE setting = 'player' LIMIT 1" //get player uuid
					") AND setting = 'mainmenu' LIMIT 1"
				")"
				"AND MenuSettings.setting = '" + setting + "' LIMIT 1";

		SQLite3::SQLiteStmt * stmt = sysDb->Prepare_v2(sql.c_str(), -1, nullptr);

		AZStd::string ret;

		if (stmt->Step() == SQLITE_ROW) {
			ret = stmt->Column_Text(0);
		} else ret = "";

		delete stmt;
		return ret;
	}
	bool DatabaseManager::SetMenuSetting(AZStd::string setting, AZStd::string value) {
		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		if (!sysDb) return false;

		AZStd::string sql =
			"INSERT OR REPLACE INTO MenuSettings"
			"("
				"puuid,"
				"mmuuid,"
				"setting,"
				"value"
			") VALUES ("
				"("
					"SELECT value FROM Global WHERE setting = 'player' LIMIT 1" //get player uuid
				"),"
				"("
					"SELECT value FROM Settings WHERE Settings.puuid IN (" //get menu uuid
						"SELECT value FROM Global WHERE setting = 'player' LIMIT 1;" //get player uuid
					") AND setting = 'mainmenu'"
				"),"
				"'" + setting + "',"
				"'" + value + "'"
			");";

		return sysDb->Exec(sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
	}*/
}

WX_ENABLE_(4390)