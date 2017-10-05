
#pragma once

#include "IGame.h"
#include "IGameObject.h"
#include "ILevelSystem.h"
//------------------------------------------------------
//#include "LyShine\ILyShine.h"

#include "Core/Graphics/Resources/ResourceCollection.h"
#include "Core\Input\InputSystem.h"
#include "Core\Note\DivaAnimationNode.h"

#include "Core\Judge\OpenDivaJudge.h"

#include <AlternativeAudio/AlternativeAudioBus.h>

#include "Bus/DivaEventsBus.h"
#include "Bus/DivaHudBus.h"

//#include "Files/LyricsFile.h"

#include <AzCore/Jobs/JobFunction.h>

#include "Graphics/Intro/OpenDivaIntro.h"

//#include "CryAction.h"

//Lyshine Stuff
//#include "Ui/LYSwing.h"
//------------------------------------------------------

/*!
 * #TODO
 * These defines should be replaced with appropriate name for your game project.
 */
#define GAME_NAME           "Open Diva"
#define GAME_LONGNAME       "Open Source Hatsune Miku Project Diva"

struct IGameFramework;

namespace OpenDiva
{
    class IGameInterface;
    class GameRules;
    class Actor;

    /*!
     * Platform types that the game can run on.
     */
    enum Platform
    {
        ePlatform_Unknown,
        ePlatform_PC,
        ePlatform_Xbox,
        ePlatform_PS4,
        ePlatform_Android,
        ePlatform_iOS,
        ePlatform_Count
    };

    /*!
     * Platform names.
     */
    static char const* s_PlatformNames[ePlatform_Count] =
    {
        "Unknown",
        "PC",
        "Xbox",
        "PS4",
        "Android",
        "iOS"
    };

    /*!
     * Initializes, runs, and handles a game's simulation.
     */
    class OpenDivaGame
        : public IGame
        , public ISystemEventListener
        , public IGameFrameworkListener
        , public ILevelSystemListener
    {
    public:
        OpenDivaGame();
        virtual ~OpenDivaGame();

        /*!
         * /return a pointer to the game's IGameFramework instance
         */
        IGameFramework* GetGameFramework() { return m_gameFramework; }

        //////////////////////////////////////////////////////////////////////////
        //! IGame
        bool Init(IGameFramework* gameFramework) override;
        bool CompleteInit() override;
        void Shutdown() override;
        int Update(bool hasFocus, unsigned int updateFlags) override;
        void PlayerIdSet(EntityId playerId) override;
        IGameFramework* GetIGameFramework() override { return m_gameFramework; }
        const char* GetLongName() override { return GAME_LONGNAME; }
        const char* GetName() override { return GAME_NAME; }
        EntityId GetClientActorId() const override { return m_clientEntityId; }
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        //! IGameFrameworkListener
        void OnActionEvent(const SActionEvent& event) override;
		void OnPostUpdate(float fDeltaTime) override; //! Called after Render, before PostUpdate.
		void OnPreRender() override;  //! Called after Update, but before Render.
        //////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//! ISystemEventListener
		void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);
		//////////////////////////////////////////////////////////////////////////
    protected:
        //////////////////////////////////////////////////////////////////////////
        //! IGame
        void LoadActionMaps(const char* fileName) override;
        //////////////////////////////////////////////////////////////////////////

        void ReleaseActionMaps();
        void SetGameRules(GameRules* rules) { m_gameRules = rules; }

        /*!
         * Reads a profile xml node and initializes ActionMapManager for the current platform.
         * /param[in] rootNode a refernece to profile xml node
         * /return returns true if ActionMapManger was succesfully initialized, false if failed
         */
        bool ReadProfile(const XmlNodeRef& rootNode);

        /*!
         * Reads a profile xml node and adds an input device mapping to ActionMapManager
         * /param[in] platformNode a refernece to profile xml node
         * /param[in] platformId current platform
         * /return returns true if mapping device was added, false if failed
         */
        bool ReadProfilePlatform(const XmlNodeRef& platformsNode, Platform platformId);

        Platform GetPlatform() const;
    protected:
        /*!
         * Platform information as defined in defaultProfile.xml.
         */
        struct PlatformInfo
        {
            Platform    m_platformId;
            BYTE        m_devices;

            PlatformInfo(Platform platformId = ePlatform_Unknown)
                : m_platformId(platformId)
                , m_devices(eAID_KeyboardMouse | eAID_XboxPad | eAID_PS4Pad) { }
        };

    protected:
        EntityId                    m_clientEntityId;
        GameRules*                  m_gameRules;
        IGameFramework*             m_gameFramework;
        IActionMap*                 m_defaultActionMap;
        PlatformInfo                m_platformInfo;
	private:
		enum EDivaState {
			eDS_EngineInit,
			eDS_Intro,
			eDS_MainMenu,
			eDS_Loading,
			eDS_SongSetup,
			eDS_Song,
			eDS_Grading,
			eDS_Editor
		};

		EDivaState m_DivaState;
		AZStd::mutex m_introMutex;
		IIntro *m_intro;
	private: //Music stuff
		void musicInit();
		void musicShutdown();
	public:
		static void changeResolution(int w, int h, bool f);
	//private: //CONSOLE COMMANDS
	//	void setupCommands();
	private:
		struct CanvasEnt{
			AZ::Entity * m_ent;
			AZ::EntityId m_canvasId;
			bool m_active;
			bool m_visible;
			bool m_loaded;
		};
	public:
		void LoadSong(AZStd::string uuid, AZStd::string luuid, bool demo);
		void PlaySong();
	private:
		void LoadMainMenu();
		void DisplayMainMenu();
		void HideMainMenu();
		void DestroyMainMenu();
		CanvasEnt m_menuCanvas;
	private:
		CanvasEnt m_songSetupCanvas, m_songGradeCanvas;
	private:
		static void RefreshDatabase(IConsoleCmdArgs* args);
	private: //Misc Variables
		//CCryAction* pCryAction;
		CInputSystem * iSys;
		IRenderer * iRenderer;
		IDraw2d * iDraw2d;
		IMovieSystem * iMovieSys;
		AlternativeAudio::AADSPEffect * m_masterVolumeDSP;
		ResourceCollection * m_pRC;
	private: //TESTING FUNCTIONS
		void constructTesting();
		void destroyTesting();
		void setupTesting();
		void setupFlowgraph(); //can only work within CompleteInit function.
		void renderTesting();
		void loadSequences();
		void unloadSequences();
		void setupLua();
		void unloadLua();
	private: //TESTING VARIABLES
		//ColorF textColor;
		//IAnimSequence * testSeq;
		//IDivaJudge * testJudge;
		//DivaAnimationNode * testDivaAnimationNode;

		//int renderTarget;

		//IEntity *testEnt;

		//ITexture * testTex;
		//IFFont * testFont;
		//STextDrawContext testFontDrawContext;

		//IDraw2d::TextOptions textOps;

		//const char * unicodeChar;
		//AZStd::string * unicodeStr;

		/*DivaNoteSingleNode * testSingleNode;
		DivaNoteSingleNode * testSingleNode2;*/
		//DivaNoteHoldNode * testHoldNode;
		//NoteResource *m_pNoteResource;
		//TailResource *m_pTailResource;
		//EffectResource * m_pEffectResource;
		//RatingResource * m_pRatingResource;

		//PortAudioSystem *paSystem;
		/*AudioSource_Libsnd*///AlternativeAudio::IAudioSource *testAudioFile, *testAudioFile2;
		//long long testAudioFileID, testAudioFileID2;
		//AlternativeAudio::AudioSourceTime m_prevTime;
		//double m_songDelta;

		//IFlowGraphPtr testGraph;

		/*AZ::EntityId canvasEntityId;
		UiCanvasInterface* canvas;
		LYSwing::LYPanel * panel;
		LYSwing::LYLabel * label, *label2;*/

		AZ::Entity * ent;
    };

    SC_API extern OpenDivaGame* g_Game;
} // namespace OpenDiva