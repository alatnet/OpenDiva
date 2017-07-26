#ifndef _H_DIVAEVENTS_BUS
#define _H_DIVAEVENTS_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>

namespace LYGame {
	class DivaEventsGroup : public AZ::EBusTraits {
	public:
		virtual void OnSongStart() = 0;
		virtual void OnSongEnd() = 0;
	};

	using DivaEventsBus = AZ::EBus<DivaEventsGroup>;
}

#endif