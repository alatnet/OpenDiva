#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include "OpenDivaBusComponent.h"

namespace OpenDiva {
	void OpenDivaBusComponent::Reflect(AZ::ReflectContext* context) {
		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			//Enums
			behaviorContext->Class<OpenDivaBusComponent>("OpenDiva")
				->Enum<EHitScore::eHS_Worst>("eHS_Worst")
				->Enum<EHitScore::eHS_Sad>("eHS_Sad")
				->Enum<EHitScore::eHS_Safe>("eHS_Safe")
				->Enum<EHitScore::eHS_Fine>("eHS_Fine")
				->Enum<EHitScore::eHS_Cool>("eHS_Cool")

				->Enum<EDivaJudgeCompletion::eDJC_Cheap>("eDJC_Cheap")
				->Enum<EDivaJudgeCompletion::eDJC_Standard>("eDJC_Standard")
				->Enum<EDivaJudgeCompletion::eDJC_Great>("eDJC_Great")
				->Enum<EDivaJudgeCompletion::eDJC_Excellent>("eDJC_Excellent")
				->Enum<EDivaJudgeCompletion::eDJC_Perfect>("eDJC_Perfect");

			#define LuaEBus(name) \
				behaviorContext->EBus<##name##Bus>(#name"Bus") \
					->Handler<##name##Handler>()

			LuaEBus(DivaEvents);
			LuaEBus(DivaHudChanceEvents);
			LuaEBus(DivaHudTechEvents);
			LuaEBus(DivaHudHp);
			LuaEBus(DivaHudScore);
			LuaEBus(DivaHudCompletion);
			LuaEBus(DivaHudHitScore);
			LuaEBus(DivaHudLyrics);
			LuaEBus(DivaHudRomaji);
			LuaEBus(DivaHudTranslation);

			#undef LuaEBus
		}
	}
}