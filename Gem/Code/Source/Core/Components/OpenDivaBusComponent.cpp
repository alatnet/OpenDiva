#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include "OpenDivaBusComponent.h"

namespace OpenDiva {
	void OpenDivaBusComponent::Reflect(AZ::ReflectContext* context) {
		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			//buses
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

			#undef LuaEBus
		}
	}
}