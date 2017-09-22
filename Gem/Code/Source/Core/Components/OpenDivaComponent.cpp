#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include "OpenDivaComponent.h"
#include "../Database/DatabaseManager.h"

#include "../OpenDivaGame.h"

namespace OpenDiva {
	void OpenDivaComponent::Reflect(AZ::ReflectContext* context) {
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

			behaviorContext->Class<OpenDivaComponent>("OpenDiva")
				->Method("LoadSong", &OpenDivaComponent::LoadSong, nullptr, "")
				->Method("PlaySong", &OpenDivaComponent::PlaySong, nullptr, "");
		}
	}

	void OpenDivaComponent::LoadSong(AZStd::string uuid, AZStd::string luuid, bool demo){
		g_Game->LoadSong(uuid,luuid,demo);
	}

	void OpenDivaComponent::PlaySong() {
		g_Game->PlaySong();
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

namespace AZ {
	AZ_TYPE_INFO_SPECIALIZE(OpenDiva::EHitScore, "{981D3CED-AF90-4076-92AB-7F1E4AB965A0}");
	AZ_TYPE_INFO_SPECIALIZE(OpenDiva::EDivaJudgeCompletion, "{BDE35F35-31F2-4370-959B-CF84872AD1A4}");
}