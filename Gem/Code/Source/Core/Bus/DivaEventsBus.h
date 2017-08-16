#ifndef _H_DIVAEVENTS_BUS
#define _H_DIVAEVENTS_BUS

#pragma once

#include <OpenDivaCommon.h>

#include <AzCore/EBus/EBus.h>

namespace OpenDiva {
	class DivaEventsGroup : public AZ::EBusTraits {
	public:
		virtual void OnSongStart() = 0;
		virtual void OnSongPause() = 0;
		virtual void OnSongResume() = 0;
		virtual void OnSongEnd() = 0;
		virtual void OnSongReset() = 0;
	};

	using DivaEventsBus = AZ::EBus<DivaEventsGroup>;

	//Lua Handlers
	class DivaEventsHandler : public DivaEventsBus::Handler, public AZ::BehaviorEBusHandler {
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			DivaEventsHandler,
			"{DEEB5314-98A7-45FF-8297-7F30A08E68E4}",
			AZ::SystemAllocator,
			OnSongStart,
			OnSongPause,
			OnSongResume,
			OnSongEnd,
			OnSongReset
		);

	public:
		void OnSongStart() { Call(FN_OnSongStart); }
		void OnSongPause() { Call(FN_OnSongPause); }
		void OnSongResume() { Call(FN_OnSongResume); }
		void OnSongEnd() { Call(FN_OnSongEnd); }
		void OnSongReset() { Call(FN_OnSongReset); }
	};
}

#endif