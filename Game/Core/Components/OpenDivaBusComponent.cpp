#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include "OpenDivaBusComponent.h"

namespace LYGame {
	void OpenDivaBusComponent::Reflect(AZ::ReflectContext* context) {
		/*AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
		if (serializeContext) {
			// we must include any fields we want to expose to the editor or lua in the serialize context
			serializeContext->Class<TestComponent>()
				->Version(1)
				->Field("Example property", &TestComponent::m_someProperty);


			// expose this component and a single variable to the editor 
			AZ::EditContext* editContext = serializeContext->GetEditContext();
			if (editContext) {
				editContext->Class<TestComponent>("TestComponent", "Example Component")
					->ClassElement(AZ_CRC("EditorData"), "")
					->Attribute("Category", "Open Diva")
					->Attribute("AppearsInAddComponentMenu", false)
					->DataElement(0, &TestComponent::m_someProperty, "SomeProperty", "Example property");
			}
		}*/

		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			//behaviorContext->Class<TestComponent>("TestComponent");
			//Enums
			behaviorContext->Class<OpenDivaBusComponent>("OpenDivaEnum")
				->Enum<EHitScore::eHS_Worst>("eHS_Worst")
				->Enum<EHitScore::eHS_Sad>("eHS_Sad")
				->Enum<EHitScore::eHS_Safe>("eHS_Safe")
				->Enum<EHitScore::eHS_Fine>("eHS_Fine")
				->Enum<EHitScore::eHS_Cool>("eHS_Cool")

				->Enum<EDivaJudgeCompletion::eDJC_Cheap>("eDJC_Cheap")
				->Enum<EDivaJudgeCompletion::eDJC_Standard>("eDJC_Standard")
				->Enum<EDivaJudgeCompletion::eDJC_Great>("eDJC_Great")
				->Enum<EDivaJudgeCompletion::eDJC_Excellent>("eDJC_Excellent")
				->Enum<EDivaJudgeCompletion::eDJC_Perfect>("eDJC_Perfect")

				/*->Enum<EActionActivationMode::eAAM_OnPress>("eAAM_OnPress")
				->Enum<EActionActivationMode::eAAM_OnRelease>("eAAM_OnRelease")
				->Enum<EActionActivationMode::eAAM_OnHold>("eAAM_OnHold")*/;

			//EActionActivationMode
			//eAAM_OnPress;
			//eAAM_OnRelease
			//eAAM_OnHold

			//OpenDivaHitScoreEventHandler
			/*behaviorContext->EBus<OpenDivaBus::OpenDivaHitScoreBus>("OpenDivaHitScoreBus")
				->Handler<OpenDivaHitScoreEventHandler>();*/

			//OpenDiva.Bus.Song
			/*behaviorContext->EBus<OpenDivaBus::OpenDivaSongBus>("OpenDivaSongBus")
				->Handler<OpenDivaSongHandler>();*/
			//->Event("OnStart", &OpenDivaBus::OpenDivaSongBus::Events::OnStart) //exposes ebus function to be able to call it
			//->Event("OnEnd", &OpenDivaBus::OpenDivaSongBus::Events::OnEnd); //exposes ebus function to be able to call it

			//OpenDiva.Bus.Completion
			/*behaviorContext->EBus<OpenDivaBus::OpenDivaCompletionBus>("OpenDivaCompletionBus")
				->Handler<OpenDivaCompletionHandler>();*/

			//OpenDiva.Bus.Score
			/*behaviorContext->EBus<OpenDivaBus::OpenDivaScoreBus>("OpenDivaScoreBus")
				->Handler<OpenDivaScoreHandler>();*/

			//OpenDiva.Bus.UiInput
			/*behaviorContext->EBus<OpenDivaBus::OpenDivaUiInputBus>("OpenDivaUiInputBus")
				->Handler<OpenDivaUiInputHandler>();*/

			//OpenDiva.Bus.ZoneEvents
			/*behaviorContext->EBus<OpenDivaBus::OpenDivaZoneEventsBus>("OpenDivaZoneEventsBus")
				->Handler<OpenDivaZoneEventsHandler>();*/
		}
	}
}