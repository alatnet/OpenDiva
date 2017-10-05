#ifndef _H_OPENDIVA_COMPONENT
#define _H_OPENDIVA_COMPONENT

#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "../Bus/DivaHudBus.h"
#include "../Bus/DivaEventsBus.h"

namespace OpenDiva {
	class OpenDivaComponent : public AZ::Component {
	public:
		~OpenDivaComponent() override = default;
		AZ_TYPE_INFO(OpenDivaComponent, "{1D0C75AA-964E-4026-AEBC-6C905830BDA5}");

		static void Reflect(AZ::ReflectContext* reflection);
	protected:
		static void LoadSong(AZStd::string uuid, AZStd::string luuid, bool demo);
		static void PlaySong();
		static void SetResolution(int width, int height, bool fullscreen);
	};
}

#endif