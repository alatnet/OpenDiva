#include <StdAFX.h>
#include <../CryCommon/IMovieSystem.h>
#include "../../Flowgraph/Bus/Flownodes_OpenDiva_Bus.h"
#include "DSFGEventListener.h"

namespace LYGame {
	void DSFGEventListener::OnTrackEvent(IAnimSequence* pSequence, int reason, const char* event, void* pUserData) {
		if (reason == eTrackEventReason_Triggered) {
			if (string(event).compare("ZoneEvent") == 0) {
				string sparam((const char *)pUserData);

				if (sparam.compare("ChanceEnter") == 0) EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaZoneEventsBus, OnChanceEnter);
				else if (sparam.compare("ChanceExit") == 0) EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaZoneEventsBus, OnChanceExit);
				else if (sparam.compare("TechEnter") == 0) EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaZoneEventsBus, OnTechEnter);
				else if (sparam.compare("TechExit") == 0) EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaZoneEventsBus, OnTechExit);
			}
		}
	}
}