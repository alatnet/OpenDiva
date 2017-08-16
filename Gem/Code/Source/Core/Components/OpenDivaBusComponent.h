#ifndef _H_OPENDIVABUS_COMPONENT
#define _H_OPENDIVABUS_COMPONENT

#pragma once

#include "OpenDivaComponentFactory.h"

#include "../Bus/DivaHudBus.h"
#include "../Bus/DivaEventsBus.h"

namespace OpenDiva {
	class OpenDivaBusComponent : public AZ::Component {
	OPENDIVA_REGISTER_COMPONENT(OpenDivaBusComponent);
	public:
		~OpenDivaBusComponent() override = default;
		AZ_COMPONENT(OpenDivaBusComponent, "{418E3DC9-9AEF-47F4-987B-08C6ADDE495C}");

		static void Reflect(AZ::ReflectContext* reflection);
	public:
	protected:
		// AZComponent
		void Init() override {};
		void Activate() override {};
		void Deactivate() override {};
	};
}

#endif