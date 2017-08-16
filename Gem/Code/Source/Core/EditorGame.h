
#pragma once

#include <IGameRef.h>
#include <IEditorGame.h>

namespace OpenDiva
{
    /*!
     * Handles the game instance while using Sandbox.
     */
    class EditorGame
        : public IEditorGame
    {
    public:
        EditorGame();
        virtual ~EditorGame() {}

        //////////////////////////////////////////////////////////////////////////
        //! IEditorGame
        bool Init(ISystem* system, IGameToEditorInterface* gameToEditorInterface) override;
        int Update(bool hasFocus, unsigned int updateFlags) override;
        void Shutdown() override;
        bool SetGameMode(bool isInGame) override;
        IEntity* GetPlayer() override;
        void SetPlayerPosAng(Vec3 position, Vec3 viewDirection) override;
        void HidePlayer(bool hide) override;
        void OnBeforeLevelLoad() override;
        void OnAfterLevelLoad(const char* levelName, const char* levelFolder) override;
        IFlowSystem* GetIFlowSystem() override;
        IGameTokenSystem* GetIGameTokenSystem() override;
        //////////////////////////////////////////////////////////////////////////

    protected:
        enum NetContextConfiguration
        {
            eNetContextConfiguration_Enable,    //!< Enables the net context.
            eNetContextConfiguration_Disable,   //!< Disables the net context.
            eNetContextConfiguration_Reset      //!< Disables, then Enables the net context.
        };

        /*!
         * \param[in] configuration NetContextConfiguration enum value
         *
         * \return a bool indicating the method successfully configured
         */
        bool ConfigureNetContext(NetContextConfiguration configuration);

        IGameRef m_Game;                //!< Reference to the game instance.
        IGameStartup* m_GameStartup;    //!< A pointer to an IGameStartup used to initiailize a game instance.
        bool m_InGame;                  //!< True if in game, false if editing
        bool m_NetContextEnabled;       //!< True if net context is enabled currently, false if not
    };
} // namespace OpenDiva
