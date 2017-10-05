#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include <SQLite\SQLiteBus.h>
#include <AzCore\std\parallel\mutex.h>

#include "StylesList.h"

#include <ctime>

namespace OpenDiva {
	AZStd::mutex StylesList::m_mutex;

	inline AZStd::string boolToStr(bool b) {
		return (b) ? "1" : "0";
	}

	void StylesList::Refresh() {
		time_t now = time(0);
		AZ::Crc32 crchash = AZ::Crc32(ctime(&now));
		AZStd::string crchash_str = Util::Crc32ToString(crchash);

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		sysDb->Exec("BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

		gEnv->pFileIO->FindFiles("@styles@", "*.*",
			[&](const char* filePath) -> bool {
				if (gEnv->pFileIO->IsDirectory(filePath)) { //if it is a directory
					//process directory
					AZStd::string xmlFile = filePath;
					xmlFile += "/styleinfo.xml";
					if (gEnv->pFileIO->Exists(xmlFile.c_str())) {
						XmlNodeRef xmldata = gEnv->pSystem->LoadXmlFromFile(xmlFile.c_str());

						if (xmldata){
							AZStd::string folder = PathUtil::GetFile(filePath);
							AZStd::string name = xmldata->findChild("name")->getContent();
							AZStd::string desc = xmldata->findChild("desc")->getContent();
							int ver = 1;
							xmldata->getAttr("version", ver);

							AZ::Crc32 stylecrc = AZ::Crc32(folder.c_str());
							stylecrc.Add(OPENDIVA_SALT);

							AZStd::string effectsDir, fontsDir, notesDir, tailsDir, hudFile, menuFile, gradeFile, setupFile, loadFile;
							effectsDir = fontsDir = notesDir = tailsDir = hudFile = menuFile = gradeFile = setupFile = loadFile = filePath;

							effectsDir += "/Effects/effectConfig.xml";
							fontsDir += "/Fonts/fontConfig.xml";
							notesDir += "/Note/noteConfig.xml";
							tailsDir += "/Tails/tailConfig.xml";
							hudFile += "/Canvas/hud.uicanvas";
							menuFile += "/Canvas/menu.uicanvas";
							gradeFile += "/Canvas/grade.uicanvas";
							setupFile += "/Canvas/setup.uicanvas";
							loadFile += "/Canvas/load.uicanvas";

							bool hasEffects = gEnv->pFileIO->Exists(effectsDir.c_str());
							bool hasFonts = gEnv->pFileIO->Exists(fontsDir.c_str());
							bool hasNotes = gEnv->pFileIO->Exists(notesDir.c_str());
							bool hasTails = gEnv->pFileIO->Exists(tailsDir.c_str());
							bool hasHud = gEnv->pFileIO->Exists(hudFile.c_str());
							bool hasMenu = gEnv->pFileIO->Exists(menuFile.c_str());
							bool hasGrade = gEnv->pFileIO->Exists(gradeFile.c_str());
							bool hasSetup = gEnv->pFileIO->Exists(setupFile.c_str());
							bool hasLoad = gEnv->pFileIO->Exists(loadFile.c_str());

							AZStd::string style_sql_str =
								"INSERT OR REPLACE INTO Styles ("
									"stuuid,"
									"name,"
									"dirname,"
									"description,"
									"version,"
									"hasEffects,"
									"hasFonts,"
									"hasNotes,"
									"hasTails,"
									"hasHud,"
									"hasMenu,"
									"hasGrade,"
									"hasSetup,"
									"hasLoad,"
									"dbcrc"
								") VALUES ("
									"\"" + Util::Crc32ToString(stylecrc) + "\","
									"\"" + name + "\","
									"\"" + folder + "\","
									"\"" + desc + "\","
									"\"" + AZStd::to_string(ver) + "\","
									"\"" + boolToStr(hasEffects)  + "\","
									"\"" + boolToStr(hasFonts) + "\","
									"\"" + boolToStr(hasNotes) + "\","
									"\"" + boolToStr(hasTails) + "\","
									"\"" + boolToStr(hasHud) + "\","
									"\"" + boolToStr(hasMenu) + "\","
									"\"" + boolToStr(hasGrade) + "\","
									"\"" + boolToStr(hasSetup) + "\","
									"\"" + boolToStr(hasLoad) + "\","
									"\"" + crchash_str + "\""
								");";

							sysDb->Exec(style_sql_str.c_str(), nullptr, nullptr, nullptr);
						}
					}
				}
				return true; // continue iterating
			}
		);

		sysDb->Exec("END TRANSACTION", nullptr, nullptr, nullptr);

		//database cleanup
		AZStd::string cleanup = "DELETE FROM Styles WHERE dbcrc != \"" + crchash_str + "\";";

		//sysDb->Exec("BEGIN TRANSACTION", nullptr, nullptr, nullptr);
		sysDb->Exec(cleanup.c_str(), nullptr, nullptr, nullptr);
		//sysDb->Exec("END TRANSACTION", nullptr, nullptr, nullptr);
	}
}