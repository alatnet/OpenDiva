#ifndef _H_OPENDIVABUS_COMPONENT
#define _H_OPENDIVABUS_COMPONENT

#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace OpenDiva {
	class OpenDivaBusComponent {
	public:
		AZ_TYPE_INFO(OpenDivaBusComponent, "{418E3DC9-9AEF-47F4-987B-08C6ADDE495C}");

		static void Reflect(AZ::ReflectContext* reflection);
	};
}

#endif