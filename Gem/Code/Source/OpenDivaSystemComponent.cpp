
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "OpenDivaSystemComponent.h"
#include <Core/EditorGame.h>
#include "System/GameStartup.h"

#include "Core\Components\Components.h"

namespace OpenDiva
{
    void OpenDivaSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<OpenDivaSystemComponent, AZ::Component>()
                ->Version(0)
                ->SerializerForEmptyClass();

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<OpenDivaSystemComponent>("OpenDiva", "Open Diva System Component.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        // ->Attribute(AZ::Edit::Attributes::Category, "") Set a category
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System", 0xc94d118b))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }

		OpenDivaComponent::Reflect(context);
		OpenDivaBusComponent::Reflect(context);
    }

    void OpenDivaSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("EditorGameService", 0x8bb59498));
    }

    void OpenDivaSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("EditorGameService", 0x8bb59498));
    }

    void OpenDivaSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("CryLegacyService", 0xdfa3b326));
		required.push_back(AZ_CRC("SQLiteLYService", 0x9aac87df));
		required.push_back(AZ_CRC("AlternativeAudioService", 0x2eb4e627));
		required.push_back(AZ_CRC("AlternativeAudio_LibsndfileService", 0xf6713d77));
		required.push_back(AZ_CRC("PortAudioService", 0x3553a550));
    }

    void OpenDivaSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
		dependent.push_back(AZ_CRC("SQLiteLYService", 0x9aac87df));
		dependent.push_back(AZ_CRC("AlternativeAudioService", 0x2eb4e627));
		dependent.push_back(AZ_CRC("AlternativeAudio_LibsndfileService", 0xf6713d77));
		dependent.push_back(AZ_CRC("PortAudioService", 0x3553a550));
    }

    IGameStartup* OpenDivaSystemComponent::CreateGameStartup()
    {
        static char buffer[sizeof(GameStartup)];
        return static_cast<IGameStartup*>(new(buffer)GameStartup());
    }

    IEditorGame* OpenDivaSystemComponent::CreateEditorGame()
    {
        return new EditorGame();
    }

    /*const char* OpenDivaSystemComponent::GetGameName() const
    {
        return "OpenDiva";
    }*/

    void OpenDivaSystemComponent::Init()
    {
    }

    void OpenDivaSystemComponent::Activate()
    {
        OpenDivaRequestBus::Handler::BusConnect();
        EditorGameRequestBus::Handler::BusConnect();
    }

    void OpenDivaSystemComponent::Deactivate()
    {
        EditorGameRequestBus::Handler::BusConnect();
        OpenDivaRequestBus::Handler::BusDisconnect();
    }
}

#ifdef AZ_MONOLITHIC_BUILD
extern "C"
{
    IGameStartup* CreateGameStartup()
    {
        IGameStartup* pGameStartup = nullptr;
        EditorGameRequestBus::BroadcastResult(pGameStartup, &EditorGameRequestBus::Events::CreateGameStartup);
        return pGameStartup;
    }
}
#endif
