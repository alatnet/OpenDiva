
#pragma once

#include <IGameFramework.h>
#include <IWindowMessageHandler.h>
#include <IPlatformOS.h>

namespace OpenDivaNew
{
    /*!
     * Handles the initialization, running, and shutting down of the game.
     */
    class GameStartup
        : public IGameStartup
        , public ISystemEventListener
        , public IWindowMessageHandler
    {
    public:
        // IGameStartup
        IGameRef Init(SSystemInitParams& startupParams) override;
        void Shutdown() override;
        int Update(bool hasFocus, unsigned int updateFlags) override;
        int Run(const char* autoStartLevelName) override;
        // ~IGameStartup

        // ISystemEventListener
        virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
        // ~ISystemEventListener

        /*!
         * Re-initializes the Game
         * /return a new instance of OpenDivaNew::SteveGameGame() or nullptr if failed to initialize.
         */
        IGameRef Reset();

        // Only allow this component class to create instances of GameStartup
        friend class OpenDivaNewSystemComponent;

    protected:
        GameStartup();
        virtual ~GameStartup();

    private:
#if defined(WIN32)
        bool HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
#endif

        IGameFramework*         m_Framework;
        IGame*                  m_Game;
    };
} // namespace OpenDivaNew
