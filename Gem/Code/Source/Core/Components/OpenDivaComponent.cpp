#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include "OpenDivaComponent.h"
#include "../Database/DatabaseManager.h"
#include "../Listing/SongList.h"

#include "../Files/NoteFileEntries.h"

#include "../OpenDivaGame.h"

namespace OpenDiva {
	void OpenDivaComponent::Reflect(AZ::ReflectContext* context) {
		if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context)) {
			//class serialize reflection
			IDivaJudgeParams::Reflect(serialize);
		}

		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			//Enums
			behaviorContext->Class<EHitScore>("DivaHitScore")
				->Enum<EHitScore::eHS_Worst>("Worst")
				->Enum<EHitScore::eHS_Sad>("Sad")
				->Enum<EHitScore::eHS_Safe>("Safe")
				->Enum<EHitScore::eHS_Fine>("Fine")
				->Enum<EHitScore::eHS_Cool>("Cool");

			behaviorContext->Class<EDivaJudgeCompletion>("DivaJudgeCompletion")
				->Enum<EDivaJudgeCompletion::eDJC_Cheap>("Cheap")
				->Enum<EDivaJudgeCompletion::eDJC_Standard>("Standard")
				->Enum<EDivaJudgeCompletion::eDJC_Great>("Great")
				->Enum<EDivaJudgeCompletion::eDJC_Excellent>("Excellent")
				->Enum<EDivaJudgeCompletion::eDJC_Perfect>("Perfect");

			behaviorContext->Class<ENoteHitType>("DivaNoteHitType")
				->Enum<ENoteHitType::eNHT_Single>("Single")
				->Enum<ENoteHitType::eNHT_Hold>("Hold")
				->Enum<ENoteHitType::eNHT_Arrow>("Arrow")
				->Enum<ENoteHitType::eNHT_Star>("Star")
				->Enum<ENoteHitType::eNHT_BigStar>("BigStar")
				->Enum<ENoteHitType::eNHT_Swipe>("Swipe")
				->Enum<ENoteHitType::eNHT_SwipeHold>("SwipeHold")
				->Enum<ENoteHitType::eNHT_Rush>("Rush")
				->Enum<ENoteHitType::eNHT_BPM>("BPM")
				->Enum<ENoteHitType::eNHT_INVALID>("INVALID");

			behaviorContext->Class<ENoteType>("DivaNoteType")
				->Enum<ENoteType::eNT_Cross>("Cross")
				->Enum<ENoteType::eNT_Circle>("Circle")
				->Enum<ENoteType::eNT_Square>("Square")
				->Enum<ENoteType::eNT_Triangle>("Triangle")
				->Enum<ENoteType::eNT_Left>("Left")
				->Enum<ENoteType::eNT_Right>("Right")
				->Enum<ENoteType::eNT_Up>("Up")
				->Enum<ENoteType::eNT_Down>("Down")
				->Enum<ENoteType::eNT_Star>("Star")
				->Enum<ENoteType::eNT_BigStar>("BigStar")
				->Enum<ENoteType::eNT_SwipeL>("SwipeL")
				->Enum<ENoteType::eNT_SwipeR>("SwipeR")
				->Enum<ENoteType::eNT_Count>("Count");

			behaviorContext->Class<SectionType>("DivaSectionType")
				->Enum<SectionType::eST_Norm>("Norm")
				->Enum<SectionType::eST_Tech>("Tech")
				->Enum<SectionType::eST_Chance>("Chance");


			behaviorContext->Class<OpenDivaComponent>("OpenDiva")
				->Method("LoadSong", &OpenDivaComponent::LoadSong)
				->Method("PlaySong", &OpenDivaComponent::PlaySong)
				->Method("SetResolution", &OpenDivaComponent::SetResolution)
				->Method("GroupCount", &SongList::GroupCount)
				->Method("SongCount", &SongList::SongCount)
				->Method("GetGlobalSetting", &DatabaseManager::GetGlobalSetting)
				->Method("SetGlobalSetting", &DatabaseManager::SetGlobalSetting)
				->Method("GetSetting", &DatabaseManager::GetSetting)
				->Method("SetSetting", &DatabaseManager::SetSetting)
				//->Method("GetMenuSetting", &DatabaseManager::GetMenuSetting)
				//->Method("SetMenuSetting", &DatabaseManager::SetMenuSetting)
				;

			//class behavior reflection
			IDivaJudgeParams::Behavior(behaviorContext);
		}
	}

	void OpenDivaComponent::LoadSong(AZStd::string uuid, AZStd::string luuid, bool demo){
		g_Game->LoadSong(uuid,luuid,demo);
	}

	void OpenDivaComponent::PlaySong() {
		g_Game->PlaySong();
	}

	void OpenDivaComponent::SetResolution(int width, int height, bool fullscreen) {
		/*AZStd::string fullscreen_str = "false";
		if (fullscreen) fullscreen_str = "true";

		AZStd::string display_sql_str =
			"INSERT OR REPLACE INTO Global ("
				"setting,"
				"value"
			") VALUES ("
				"resW,"
				"\"" + AZStd::to_string(width) + "\""
			"),"
			"("
				"resH,"
				"\"" + AZStd::to_string(height) + "\""
			"),"
			"("
				"fullscreen,"
				"\"" + fullscreen_str + "\""
			");";

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		sysDb->Exec(display_sql_str.c_str(), nullptr, nullptr, nullptr);*/

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		sysDb->Exec("BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
		DatabaseManager::SetGlobalSetting("resW", AZStd::to_string(width));
		DatabaseManager::SetGlobalSetting("resH", AZStd::to_string(height));
		DatabaseManager::SetGlobalSetting("fullscreen", fullscreen ? "true" : "false");
		sysDb->Exec("END TRANSACTION", nullptr, nullptr, nullptr);

		OpenDivaGame::changeResolution(width, height, fullscreen);
	}
}

//state flow
/*
-main menu
-before song
-song
-after song (grading)
-main menu
*/