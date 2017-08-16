
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "OpenDivaSystemComponent.h"
#include <Core/EditorGame.h>
#include "System/GameStartup.h"

namespace OpenDiva
{
    void OpenDivaSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<OpenDivaNewSystemComponent, AZ::Component>()
                ->Version(0)
                ->SerializerForEmptyClass();

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<OpenDivaNewSystemComponent>("OpenDiva", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        // ->Attribute(AZ::Edit::Attributes::Category, "") Set a category
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void OpenDivaSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("EditorGameService"));
    }

    void OpenDivaSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("EditorGameService"));
    }

    void OpenDivaSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        //required.push_back(AZ_CRC("CryLegacyService"));
    }

    void OpenDivaSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
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

    const char* OpenDivaSystemComponent::GetGameName() const
    {
        return "OpenDiva";
    }

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
