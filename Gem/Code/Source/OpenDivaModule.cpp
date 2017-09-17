
#include "StdAfx.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>

#include "OpenDivaSystemComponent.h"

#include <IGem.h>

namespace OpenDiva
{
    class OpenDivaModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(OpenDivaModule, "{B8D95A61-55CB-4B2D-ABD4-5C5CE7B13C7D}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(OpenDivaModule, AZ::SystemAllocator, 0);

        OpenDivaModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                OpenDivaSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<OpenDivaSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(OpenDivaNew_95e41eeb9802469b8f7dd874f19720d9, OpenDiva::OpenDivaModule)
