
#pragma once

#include "IGame.h"
#include "IGameObject.h"
#include "ILevelSystem.h"
//------------------------------------------------------
//#include "LyShine\ILyShine.h"

//#include "Core/CryMovie/Node/Note/PrototypeNoteNode.h"
#include "Core/Graphics/Resources/ResourceCollection.h"
#include "Core\Input\InputSystem.h"
#include "Core\Note\DivaAnimationNode.h"

#include "Core\Judge\OpenDivaJudge.h"

//#include "Sound\Port Audio System\Source\AudioSources.h"
//#include "Sound\Port Audio System\PortAudioSystem.h"
#include <AlternativeAudio/AlternativeAudioBus.h>
#include <PortAudio\PortAudioBus.h>

#include "Bus/DivaEventsBus.h"

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
	private: //Music stuff
			void musicInit();
			void musicShutdown();
			void musicUpdate();
	private: //CONSOLE COMMANDS
		void setupCommands();
	private: //CONSOLE COMMANDS
		static void LoadSongRedirect(IConsoleCmdArgs* pCmdArgs);
		void LoadSong(IConsoleCmdArgs* pCmdArgs);
		const char * songName;
	private: //Setup Functions
		void SetupDatabase();
	public: //Misc Functions
			//AZ::EntityId GetHudID() { return this->m_HudCanvasID; }
			//IDivaJudge * GetJudge(); //TODO!!!  //may stick with the ebus stuff...
	private: //Misc Variables
		CInputSystem * iSys;
		//CUIInputSystem * uiSys;
		//CMovieSystem * audioMovieSys;
		float audioMovieSysTime;
		AZ::EntityId m_HudCanvasID;
		//CCryAction* pCryAction;
	private: //TESTING FUNCTIONS
		void constructTesting();
		void destroyTesting();
		void setupTesting();
		void setupFlowgraph(); //can only work within CompleteInit function.
		void renderTesting();
		void loadSequences();
		void unloadSequences();
		void musicStartStop();
		void setupLua();
		void unloadLua();
		//void testLyShine();

		void initLyShine();
		void destroyLyShine();
	private: //TESTING VARIABLES
		ColorF textColor;

		IMovieSystem * iMovieSys;
		IAnimSequence * testSeq;
		IDivaJudge * testJudge;
		DivaAnimationNode * testDivaAnimationNode;

		//int renderTarget;

		//IEntity *testEnt;

		ITexture * testTex;
		IFFont * testFont;
		STextDrawContext testFontDrawContext;

		//IDraw2d::TextOptions textOps;

		//const char * unicodeChar;
		AZStd::string * unicodeStr;

		/*DivaNoteSingleNode * testSingleNode;
		DivaNoteSingleNode * testSingleNode2;*/
		//DivaNoteHoldNode * testHoldNode;
		//NoteResource *m_pNoteResource;
		//TailResource *m_pTailResource;
		//EffectResource * m_pEffectResource;
		//RatingResource * m_pRatingResource;
		ResourceCollection * m_pRC;

		IRenderer * iRenderer;
		IDraw2d * iDraw2d;

		//PortAudioSystem *paSystem;
		/*AudioSource_Libsnd*/AlternativeAudio::IAudioSource *testAudioFile, *testAudioFile2;
		long long testAudioFileID, testAudioFileID2;
		AlternativeAudio::AudioSourceTime m_prevTime;
		double m_songDelta;
		AlternativeAudio::AADSPEffect * masterVolumeDSP;

		IFlowGraphPtr testGraph;

		/*AZ::EntityId canvasEntityId;
		UiCanvasInterface* canvas;
		LYSwing::LYPanel * panel;
		LYSwing::LYLabel * label, *label2;*/

		AZ::Entity * ent;
    };

    SC_API extern OpenDivaGame* g_Game;
} // namespace OpenDiva