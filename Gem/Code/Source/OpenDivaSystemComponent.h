
#pragma once

#include <AzCore/Component/Component.h>

#include <OpenDiva/OpenDivaBus.h>
#include <IEditorGame.h>
#include "System/GameStartup.h"

namespace OpenDiva
{
    class OpenDivaSystemComponent
        : public AZ::Component
        , protected EditorGameRequestBus::Handler
        , protected OpenDivaRequestBus::Handler
    {
    public:
        AZ_COMPONENT(OpenDivaSystemComponent, "{FA842856-2A5D-403C-8095-70343E7DD269}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // OpenDivaRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // EditorGameRequestBus interface implementation
        IGameStartup* CreateGameStartup() override;
        IEditorGame* CreateEditorGame() override;
        const char* GetGameName() const override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
