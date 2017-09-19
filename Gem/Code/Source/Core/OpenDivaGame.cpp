
#include "StdAfx.h"
#include <OpenDivaCommon.h>
//#include "Game/Actor.h"
#include "OpenDivaGame.h"
#include "IGameFramework.h"
#include "IGameRulesSystem.h"
//#include "OpenDivaNewGameRules.h"
#include "IPlatformOS.h"
#include <functional>

//#include <LyShine/UiComponentTypes.h>
#include <LyShine/Bus/World/UiCanvasRefBus.h>
#include <AzFramework\Script\ScriptComponent.h>
//#include <AzCore\Asset\AssetDatabaseBus.h>
#include <AzCore\Asset\AssetManagerBus.h>

#include "Components/Components.h"

//#include <SQLite/SQLiteBus.h>

#include <AlternativeAudio\DSP\VolumeDSPBus.h>

#include "Database\DatabaseManager.h"
#include "Files\SongList.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/*
#define REGISTER_FACTORY(host, name, impl, isAI) \
    (host)->RegisterFactory((name), (impl*)0, (isAI), (impl*)0)
*/

namespace OpenDiva
{
    OpenDivaGame* g_Game = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    OpenDivaGame::OpenDivaGame()
        : m_clientEntityId(INVALID_ENTITYID)
        , m_gameRules(nullptr)
        , m_gameFramework(nullptr)
        , m_defaultActionMap(nullptr)
        , m_platformInfo()
    {
        g_Game = this;
        GetISystem()->SetIGame(this);

		this->constructTesting();

		this->masterVolumeDSP = nullptr;

		this->currLyric = "";

		DivaHudLyricsBus::Handler::BusConnect();
    }

    OpenDivaGame::~OpenDivaGame()
    {
		DivaHudLyricsBus::Handler::BusDisconnect();
        m_gameFramework->EndGameContext(false);

        // Remove self as listener.
        m_gameFramework->UnregisterListener(this);
        m_gameFramework->GetILevelSystem()->RemoveListener(this);
        gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

        g_Game = nullptr;
        GetISystem()->SetIGame(nullptr);

        ReleaseActionMaps();

		Util::ClearCache("Fonts");
    }

    bool OpenDivaGame::Init(IGameFramework* framework)
    {
        m_gameFramework = framework;

        // Register the actor class so actors can spawn.
        // #TODO If you create a new actor, make sure to register a factory here.
        //REGISTER_FACTORY(m_gameFramework, "Actor", Actor, false);

        // Listen to system events, so we know when levels load/unload, etc.
        gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);
        m_gameFramework->GetILevelSystem()->AddListener(this);

        // Listen for game framework events (level loaded/unloaded, etc.).
        m_gameFramework->RegisterListener(this, "Game", FRAMEWORKLISTENERPRIORITY_GAME);

        // Load actions maps.
        LoadActionMaps("config/input/actionmaps.xml");

		//set aliases
		AZStd::string songAlias = gEnv->pFileIO->GetAlias("@assets@") + AZStd::string(PathUtil::ToNativePath(FolderStruct::Paths::sSongPath.c_str()));
		AZStd::string styleAlias = gEnv->pFileIO->GetAlias("@assets@") + AZStd::string(PathUtil::ToNativePath(FolderStruct::Paths::sStylesPath.c_str()));
		AZStd::string hudAlias = gEnv->pFileIO->GetAlias("@assets@") + AZStd::string(PathUtil::ToNativePath(FolderStruct::Paths::sHudPath.c_str()));
		gEnv->pFileIO->SetAlias("@songs@", songAlias.c_str());
		gEnv->pFileIO->SetAlias("@styles@", styleAlias.c_str());
		gEnv->pFileIO->SetAlias("@hud@", hudAlias.c_str());
        // Register game rules wrapper.
        //REGISTER_FACTORY(framework, "OpenDivaNewGameRules", OpenDivaNewGameRules, false);
        //IGameRulesSystem* pGameRulesSystem = g_Game->GetIGameFramework()->GetIGameRulesSystem();
        //pGameRulesSystem->RegisterGameRules("DummyRules", "OpenDivaNewGameRules");

        GetISystem()->GetPlatformOS()->UserDoSignIn(0);

		//create new input system
		this->iSys = new CInputSystem();
		//this->uiSys = new CUIInputSystem();

		//this->inputBusListener = new InputEBusListener();
		//this->uiSys->AddListener(this->inputBusListener);

		//this->dsfgEventListener = new DSFGEventListener();

		//setup console commands
		this->setupCommands();

		//initialize music system
		this->musicInit();

		//setup testing
		this->setupTesting();

		this->initLyShine();

		DatabaseManager::Init();

		//this->pCryAction = static_cast<CCryAction*>(gEnv->pGame->GetIGameFramework());

		//auto channelid = AZ::InputEventNotificationId(Input::ChannelId("default"), Input::ProcessedEventName("Forward"));

		return true;
    }

    bool OpenDivaGame::CompleteInit()
    {
		this->setupFlowgraph();
		this->setupLua();

		OpenDivaComponentFactory::getFactory().RegisterComponents();

		//------------------------------
		//Entity Component Stuff
		//------------------------------
		/*CryLog("Activating Test Ent.");
		this->ent->Activate();*/

		/*const AZ::Data::AssetType& assetType = azrtti_typeid<AZ::Data::AssetData>();
		AZ::Data::AssetId assetId;
		AZStd::string str = gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/Config/Input/diva.inputbindings");
		EBUS_EVENT_RESULT(assetId, AZ::Data::AssetCatalogRequestBus, GetAssetIdByPath, str.c_str(), assetType, true);
		AZ::Data::Asset<AZ::Data::AssetData> asset(assetId, assetType);
		if (assetId.IsValid()) {
		EBUS_EVENT_ID(this->ent, AZ::Data::AssetBus, OnAssetReady, asset);
		}*/

		/*CryLog("Setting Canvas.");
		AZStd::string canvasPath = gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/Test/text test.uicanvas");
		CryLog("Canvas Path: %s", canvasPath.c_str());

		UiCanvasAssetRefBus::Event(this->ent->GetId(), &UiCanvasAssetRefInterface::SetCanvasPathname, canvasPath);
		UiCanvasAssetRefBus::Event(this->ent->GetId(), &UiCanvasAssetRefInterface::SetIsAutoLoad, true);

		AZStd::string ret;
		UiCanvasAssetRefBus::EventResult(ret, this->ent->GetId(), &UiCanvasAssetRefInterface::GetCanvasPathname);
		CryLog("Canvas Asset Path: %s", ret.c_str());*/

		/*string dbPath = gEnv->pFileIO->GetAlias("@cache@");
		dbPath += "/testdb.db";
		SQLite::SQLiteRequestBus::Event(this->ent->GetId(), &SQLite::SQLiteRequestBus::Events::Open, dbPath.c_str());*/

		/*char * errmsg = 0;
		SQLite::SQLiteRequestBus::Event(
		this->ent->GetId(),
		&SQLite::SQLiteRequestBus::Events::Exec,
		"CREATE TABLE Persons ( \
		PersonID int, \
		LastName varchar(255), \
		FirstName varchar(255), \
		Address varchar(255), \
		City varchar(255) \
		);",
		[](void * notUsed, int argc, char** argv, char ** azColName) -> int {
		int i;
		for (i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		printf("\n");
		return 0;
		},
		nullptr,
		&errmsg
		);
		CryLog("SQLite Err Msg: %s", errmsg);*/

		//set input configuration
		/*AZ::Data::AssetId inputAssetId;
		AZStd::string inputStr = gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/Test/test.inputbindings");
		const AZ::Data::AssetType& inputAssetType = azrtti_typeid<Input::InputEventBindingsAsset>();
		AZ::Data::AssetCatalogRequestBus::BroadcastResult(inputAssetId, &AZ::Data::AssetCatalogRequests::GetAssetIdByPath, inputStr.c_str(), inputAssetType, true);

		if (inputAssetId.IsValid()) {
		AZ::Data::Asset<Input::InputEventBindingsAsset> inputAsset(inputAssetId, inputAssetType);
		//AZ::Data::AssetBus::Broadcast(&AZ::Data::AssetBus::Events::OnAssetReady, inputAsset);
		AZ::Data::AssetBus::Event(this->ent->GetId(), &AZ::Data::AssetBus::Events::OnAssetReady, inputAsset);
		}*/

		//CCryAction *pCryAction = CCryAction::GetCryAction();
		//CCryAction* pCryAction = static_cast<CCryAction*>(gEnv->pGame->GetIGameFramework());

		//setup audio devices
		auto lib = AZ_CRC("PortAudio");
		long long defaultDeviceId = -1;
		AlternativeAudio::OAudioDevice* defaultDevice;

		EBUS_EVENT_RESULT( //get default device id
			defaultDeviceId,
			AlternativeAudio::AlternativeAudioDeviceBus,
			GetDefaultPlaybackDevice,
			lib
		);
		EBUS_EVENT_RESULT( //create new device with default device id
			defaultDevice,
			AlternativeAudio::AlternativeAudioDeviceBus,
			NewDevice,
			lib,
			defaultDeviceId,
			44100.0, //samplerate
			AlternativeAudio::AudioFrame::Type::eT_af2, //audio format
			nullptr //userdata
		);
		EBUS_EVENT( //set the master device for alternative audio
			AlternativeAudio::AlternativeAudioDeviceBus,
			SetMasterDevice,
			defaultDevice
		);

		SongList::Refresh();

        return true;
    }

    int OpenDivaGame::Update(bool hasFocus, unsigned int updateFlags)
    {
        const float frameTime = gEnv->pTimer->GetFrameTime();

        const bool continueRunning = m_gameFramework->PreUpdate(true, updateFlags);
        m_gameFramework->PostUpdate(true, updateFlags);

        return static_cast<int>(continueRunning);
    }

    void OpenDivaGame::PlayerIdSet(EntityId playerId)
    {
        m_clientEntityId = playerId;
    }

    void OpenDivaGame::Shutdown()
    {
		this->destroyLyShine();

		//destroy testing
		this->destroyTesting();

		//shutdown music
		this->musicShutdown();

		//shutdown input system
		delete this->iSys;
		//delete this->uiSys;
		//delete this->inputBusListener;
		//delete this->dsfgEventListener;

		//delete the moviesystem that uses audio timing
		//this->audioMovieSys->RemoveAllSequences();
		//this->audioMovieSys->Release();

		gEnv->pFileIO->ClearAlias("@songs@");
		gEnv->pFileIO->ClearAlias("@styles@");
		gEnv->pFileIO->ClearAlias("@hud@");

		EBUS_EVENT( //set the master device for alternative audio
			AlternativeAudio::AlternativeAudioDeviceBus,
			SetMasterDevice,
			nullptr
		);

		this->masterVolumeDSP->Release();

        this->~OpenDivaGame();
    }

	void OpenDivaGame::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) {
		switch (event) {
		case ESYSTEM_EVENT_GAME_PAUSED:
			break;
		case ESYSTEM_EVENT_GAME_RESUMED:
			break;
		case ESYSTEM_EVENT_LEVEL_LOAD_START_LOADINGSCREEN:
			break;
		case ESYSTEM_EVENT_LEVEL_LOAD_LOADINGSCREEN_ACTIVE:
			break;
		case ESYSTEM_EVENT_LEVEL_LOAD_START:
			break;
		case ESYSTEM_EVENT_LEVEL_LOAD_END:
			break;
		case ESYSTEM_EVENT_LEVEL_GAMEPLAY_START:
			break;
			//case ESYSTEM_EVENT_FLOW_SYSTEM_REGISTER_EXTERNAL_NODES:
			//RegisterExternalFlowNodes();
			//break;
		case ESYSTEM_EVENT_EDITOR_GAME_MODE_CHANGED: //wparam: 0/1 - exit/enter
													 //gEnv->IsEditor();
			if (wparam == 0) { //exit
							   //this->audioMovieSys->StopSequence(this->testDivaSeq->GetSequence());
							   //gEnv->pMovieSystem->StopSequence(this->testDivaSeq->GetSequence());
							   //this->testDivaSeq->Reset(true);
				gEnv->pMovieSystem->StopSequence(this->testSeq);
				this->testSeq->Reset(true);
			} else { //enter
			}
			break;
		case ESYSTEM_EVENT_CHANGE_FOCUS: //wparam: 0/1 - not focused/focused
			if (wparam == 0) { //not focused
			} else { //focused
			}
			break;
		case ESYSTEM_EVENT_RESIZE: //resize resources.
								   //wparam=width, lparam=height
								   //AZ::Vector2 scale = AZ::Vector2((float)wparam / 1280.0f, (float)lparam / 720.0f);
								   //this->m_pRC->p_FontResource->setScale(scale);
			break;
		}
	}

    void OpenDivaGame::LoadActionMaps(const char* fileName)
    {
        if (g_Game->GetIGameFramework()->IsGameStarted())
        {
            CryLogAlways("[Profile] Can't change configuration while game is running (yet)");
            return;
        }

        XmlNodeRef rootNode = m_gameFramework->GetISystem()->LoadXmlFromFile(fileName);
        if (rootNode && ReadProfile(rootNode))
        {
            IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager();
            actionMapManager->SetLoadFromXMLPath(fileName);
            m_defaultActionMap = actionMapManager->GetActionMap("default");
        }
        else
        {
            CryLogAlways("[Profile] Warning: Could not open configuration file");
        }
    }

    void OpenDivaGame::ReleaseActionMaps()
    {
        if (m_defaultActionMap && m_gameFramework)
        {
            IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager();
            actionMapManager->RemoveActionMap(m_defaultActionMap->GetName());
            m_defaultActionMap = nullptr;
        }
    }

    bool OpenDivaGame::ReadProfile(const XmlNodeRef& rootNode)
    {
        bool successful = false;

        if (IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager())
        {
            actionMapManager->Clear();

            // Load platform information in.
            XmlNodeRef platforms = rootNode->findChild("platforms");
            if (!platforms || !ReadProfilePlatform(platforms, GetPlatform()))
            {
                CryLogAlways("[Profile] Warning: No platform information specified!");
            }

            successful = actionMapManager->LoadFromXML(rootNode);
        }

        return successful;
    }

    bool OpenDivaGame::ReadProfilePlatform(const XmlNodeRef& platformsNode, OpenDiva::Platform platformId)
    {
        bool successful = false;

        if (platformsNode && (platformId > ePlatform_Unknown) && (platformId < ePlatform_Count))
        {
            if (XmlNodeRef platform = platformsNode->findChild(s_PlatformNames[platformId]))
            {
                // Extract which Devices we want.
                if (!strcmp(platform->getAttr("keyboard"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_KeyboardMouse;
                }

                if (!strcmp(platform->getAttr("xboxpad"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_XboxPad;
                }

                if (!strcmp(platform->getAttr("ps4pad"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_PS4Pad;
                }

                if (!strcmp(platform->getAttr("androidkey"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_AndroidKey;
                }

                // Map the Devices we want.
                IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager();

                if (m_platformInfo.m_devices & eAID_KeyboardMouse)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_KeyboardMouse, "keyboard");
                }

                if (m_platformInfo.m_devices & eAID_XboxPad)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_XboxPad, "xboxpad");
                }

                if (m_platformInfo.m_devices & eAID_PS4Pad)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_PS4Pad, "ps4pad");
                }

                if (m_platformInfo.m_devices & eAID_AndroidKey)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_AndroidKey, "androidkey");
                }

                successful = true;
            }
            else
            {
                GameWarning("OpenDivaGame::ReadProfilePlatform: Failed to find platform, action mappings loading will fail");
            }
        }

        return successful;
    }

    OpenDiva::Platform OpenDivaGame::GetPlatform() const
    {
        OpenDiva::Platform platform = ePlatform_Unknown;

    #if defined(ANDROID)
        platform = ePlatform_Android;
    #elif defined(IOS)
        platform = ePlatform_iOS;
    #elif defined(WIN32) || defined(WIN64) || defined(DURANGO) || defined(APPLE) || defined(LINUX)
        platform = ePlatform_PC;
    #endif

        return platform;
    }

    void OpenDivaGame::OnActionEvent(const SActionEvent& event)
    {
        switch (event.m_event)
        {
		case eAE_loadLevel:
			//loading screen
			break;
		case eAE_inGame:
			//end loading screen.
			//bring up prep menu.
			break;
        case eAE_unloadLevel:
            /*!
             * #TODO
             * Add clean up code here.
             */
            break;
        }
    }

	//! Called after Render, before PostUpdate.
	void OpenDivaGame::OnPostUpdate(float fDeltaTime) {
		//this->audioMovieSys->Render();

		this->musicUpdate();

		this->renderTesting();

		/*SAnimContext ec;
		ec.dt = fDeltaTime;
		this->testDivaSeq->Animate(ec);*/
	}

	//! Called after Update, but before Render.
	void OpenDivaGame::OnPreRender() {
		//if (!this->iMovieSys->IsPlaying(this->testSeq)) this->iMovieSys->PlaySequence(this->testSeq, NULL, false, false, 0, 10);
		if (!gEnv->pMovieSystem->IsPlaying(this->testSeq)) {
			this->testSeq->Reset(true);
			/*this->testSingleNode->Reset();
			this->testSingleNode2->Reset();*/

			//this->testHoldNode->Reset();

			/*this->testDivaSeq->pushbackEffect({ 100,50 }, eEL_Cool);
			this->testDivaSeq->pushbackEffect({ 150,50 }, eEL_Fine);
			this->testDivaSeq->pushbackEffect({ 200,50 }, eEL_Safe);
			this->testDivaSeq->pushbackEffect({ 250,50 }, eEL_Sad);*/
			gEnv->pMovieSystem->PlaySequence(this->testSeq, NULL, false, false, 0, 10);
		}
	}

	void OpenDivaGame::LoadSong(IConsoleCmdArgs* pCmdArgs) { //first argument is always the command name
		/*if (pCmdArgs->GetArgCount() == 2) {
			this->songName = pCmdArgs->GetArg(1);

			//XmlNodeRef xmlFile = gEnv->pSystem->LoadXmlFromFile("");

			//gEnv->p3DEngine->UnloadLevel();
			//gEnv->p3DEngine->LoadLevel("foldername", "missionname"); //folder name is where level is. mission name is environment info.

			//LOOK IN CryAction/LevelSystem.cpp
			//Function LoadLevelInternal!

			//load song
			//load map
		} else {
			CryLog("Error, correct syntax is: g_loadSong songname");
		}*/

		/*if (!this->testSong.isPlaying()) this->testSong.play();
		else this->testSong.stop();*/

		//if (pCmdArgs->GetArgCount() == 1) this->musicStartStop();
		//else if (pCmdArgs->GetArgCount() == 2) this->musicStream->SetVol(0.5f);
		//else if (pCmdArgs->GetArgCount() == 3) this->musicStream->SetVol(1.0f);

		if (pCmdArgs->GetArgCount() == 1) {
			if (this->testAudioFileID == -1) {
				//this->testAudioFileID = this->paSystem->PlaySource(this->testAudioFile, eAS_Music);
				if (this->testAudioFile != nullptr) EBUS_EVENT_RESULT(this->testAudioFileID, AlternativeAudio::AlternativeAudioDeviceBus, PlaySource, this->testAudioFile);
			} else {
				if (this->testAudioFile != nullptr) {
					EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, StopSource, this->testAudioFileID);
					this->testAudioFileID = -1;
					if (this->testAudioFileID2 != -1 && this->testAudioFile2 != nullptr) {
						EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, StopSource, this->testAudioFileID2);
						this->testAudioFileID2 = -1;
					}
				}
			}
		} else if (pCmdArgs->GetArgCount() == 2) {
			/*if (this->testAudioFileID2 == -1) this->testAudioFileID2 = this->paSystem->PlaySource(this->testAudioFile2, eAS_SFX);
			while (this->testAudioFile2->HasError()) CryLog("testAudioFile2 Error: %s", this->testAudioFile2->GetError().str);*/

			if (this->testAudioFileID2 == -1 && this->testAudioFile2 != nullptr)
				EBUS_EVENT_RESULT(this->testAudioFileID2, AlternativeAudio::AlternativeAudioDeviceBus, PlaySource, this->testAudioFile2);
		} else if (pCmdArgs->GetArgCount() == 3) {
			/*if (this->testAudioFileID2 != -1) {
			this->paSystem->StopSource(this->testAudioFileID2);
			this->testAudioFileID2 = -1;
			}*/

			if (this->testAudioFileID2 != -1 && this->testAudioFile2 != nullptr) {
				EBUS_EVENT(AlternativeAudio::AlternativeAudioDeviceBus, StopSource, this->testAudioFileID2);
				this->testAudioFileID2 = -1;
			}
		} else if (pCmdArgs->GetArgCount() == 4 && this->masterVolumeDSP) {
			EBUS_EVENT_ID(
				this->masterVolumeDSP,
				AlternativeAudio::DSP::VolumeDSPBus,
				SetVol,
				0.5f
			);
		} else if (pCmdArgs->GetArgCount() == 5 && this->masterVolumeDSP) {
			EBUS_EVENT_ID(
				this->masterVolumeDSP,
				AlternativeAudio::DSP::VolumeDSPBus,
				SetVol,
				1.0f
			);
		}

		/* else if (pCmdArgs->GetArgCount() == 4) {
			if (this->testAudioFileID != -1) {
				//stop
				this->paSystem->StopSource(this->testAudioFileID);
				this->testAudioFileID = -1;
			} else {
				//play
				this->testAudioFileID = this->paSystem->PlaySource(this->testAudioFile, eAS_Music);
			}

			if (this->testAudioFileID2 != -1) {
				//stop
				this->paSystem->StopSource(this->testAudioFileID2);
				this->testAudioFileID = -1;
			} else {
				//play
				this->testAudioFileID2 = this->paSystem->PlaySource(this->testAudioFile2, eAS_SFX);
			}
		}*/
	}
	void OpenDivaGame::LoadSongRedirect(IConsoleCmdArgs* pCmdArgs) {
		g_Game->LoadSong(pCmdArgs);
	}

	void OpenDivaGame::musicInit() {
		/*int err = Pa_Initialize();

		if (err != paNoError) {
			string errString("PA Error: \n");
			errString += Pa_GetErrorText(err);
			CryLog("[OpenDiva] Music Init Error: %s",errString);
		}

		CryLog("[OpenDiva] Port Audio Version: %s", Pa_GetVersionText());
		CryLog("[OpenDiva] libsndfile Version: %s", sf_version_string());
		CryLog("[OpenDiva] libsamplerate Version: %s", src_get_version());*/
	}

	void OpenDivaGame::musicShutdown() {
		/*int err = Pa_Terminate();

		if (err != paNoError) {
			string errString("PA Error: \n");
			errString += Pa_GetErrorText(err);
			CryLog(errString);
		}*/
	}

	void OpenDivaGame::musicUpdate() {
	}

	void OpenDivaGame::setupCommands() {
		IConsole* pConsole = gEnv->pSystem->GetIConsole();
		pConsole->AddCommand("loadSong", OpenDivaGame::LoadSongRedirect);
	}

	/*IDivaJudge * OpenDivaGame::GetJudge() {
		return nullptr;
	}*/

	//TESTING FUNCTIONS
	void OpenDivaGame::constructTesting() {
		//this->textColor = ColorF(0.95, 0.63, 0.02, 1.0);
		this->textColor = ColorF(1.0, 1.0, 1.0, 1.0);

		this->testSeq = NULL;
		this->testDivaAnimationNode = NULL;
		//this->testButtonNode = NULL;
		//this->testButtonNode2 = NULL;
		/*this->testSingleNode = NULL;
		this->testSingleNode2 = NULL;*/

		//this->testHoldNode = NULL;

		//OpenDivaGame::musicCBActive = true;
	}

	void OpenDivaGame::destroyTesting() {
		//this->testSeq->RemoveNode(this->testNode);
		//this->iMovieSys->RemoveSequence(this->testSeq);

		//this->iRenderer->DestroyRenderTarget(this->renderTarget);

		//if (this->testButtonNode != NULL) delete this->testButtonNode;

		/*this->ent->Deactivate();
		delete this->ent;*/

		this->testFont->Release();
		//delete this->testFont;
		//this->iRenderer->DeleteFont(this->testFont);

		this->unloadSequences();

		/*delete this->m_pRC->p_NoteResource;
		delete this->m_pRC->p_TailResource;
		delete this->m_pRC->p_EffectResource;
		delete this->m_pRC->p_RatingResource;
		delete this->m_pRC->p_RatingResourceFont;*/
		delete this->m_pRC;
		delete this->unicodeStr;
		delete this->testJudge;
		//delete this->testDivaSeq;

		//this->testGraph->SetActive(false);
		//this->testGraph->Clear(); //reuse this graph?
		//this->testGraph->Release();

		//delete this->paSystem;
		if (this->testAudioFile != nullptr) delete this->testAudioFile;
		if (this->testAudioFile2 != nullptr) delete this->testAudioFile2;

		delete this->lyrics;
	}

	void OpenDivaGame::setupTesting() {
		/*SEntitySpawnParams params;
		params.sName = "TestEntity";
		params.sLayerName = "TestLayer";
		params.nFlags = ENTITY_FLAG_NO_SAVE | ENTITY_FLAG_SPAWNED;
		params.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
		params.vScale = Vec3(1, 1, 1);
		params.vPosition = Vec3(0, 0, 0);

		testEnt = gEnv->pEntitySystem->SpawnEntity(params, true);*/

		/*WIN_HWND hwnd = framework->GetISystem()->GetHWND();
		framework->GetISystem()->GetViewCamera().GetViewPort();*/

		this->iRenderer = gEnv->pSystem->GetIRenderer();
		this->iDraw2d = Draw2dHelper::GetDraw2d();
		//this->textOps = this->iDraw2d->GetDefaultTextOptions();

		//this->renderTarget = this->iRenderer->CreateRenderTarget(800, 600, ETEX_Format::eTF_R32G32B32A32F);
		//this->iRenderer->SetRenderTarget(this->renderTarget);

		//IImageFile *img = this->iRenderer->EF_LoadImage("fileName", 0);
		//ITexture *tex = this->iRenderer->EF_LoadTexture("texName", FT_DONT_STREAM);

		//this->testTex = this->iRenderer->EF_LoadTexture("Images\\Note\\circle.dds", FT_DONT_STREAM);

		//this->m_pNoteResource->setImgScale(Vec2(0.75f, 0.75f));
		//this->m_pNoteResource->setPosScale(Vec2(1.5f, 1.5f));

		//char buff[MAX_PATH + 1];
		/*std::string path(getcwd(buff, MAX_PATH + 1));
		path += "/OpenDiva/Resources/Styles/PPDXXX/";*/

		AZStd::string assetsPath(gEnv->pFileIO->GetAlias("@assets@"));
		AZStd::string path(assetsPath);

		//string path(getcwd(buff, MAX_PATH + 1));
		path += AZStd::string(FolderStruct::Paths::sStylesPath.c_str()) + "/PPDXXX/";

		AZStd::string noteR(path + AZStd::string(FolderStruct::Folders::sNoteFolder.c_str()));
		AZStd::string effectR(path + AZStd::string(FolderStruct::Folders::sEffectsFolder.c_str()));
		AZStd::string tailsR(path + AZStd::string(FolderStruct::Folders::sTailFolder.c_str()));
		AZStd::string fontsR(path + AZStd::string(FolderStruct::Folders::sFontsFolder.c_str()));
		//std::string ratingR(path + "Ratings");

		/*this->m_pNoteResource = new NoteResource(noteR.c_str());
		this->m_pTailResource = new TailResource(noteR.c_str());
		this->m_pEffectResource = new EffectResource(effectR.c_str());
		this->m_pRatingResource = new RatingResource(ratingR.c_str());*/

		this->m_pRC = new ResourceCollection();

		this->m_pRC->p_NoteResource = new NoteResource(noteR.c_str());
		this->m_pRC->p_TailResource = new TailResource(tailsR.c_str());
		//this->m_pRC->p_EffectResource = new EffectResource(effectR.c_str());
		this->m_pRC->p_EffectResource = new EffectResource(effectR.c_str());
		//this->m_pRC->p_RatingResource = new RatingResource(ratingR.c_str());
		this->m_pRC->p_FontResource = new FontResource(fontsR.c_str());

		AZ::Vector2 scale = AZ::Vector2(gEnv->pRenderer->GetWidth() / 1280.0f, gEnv->pRenderer->GetHeight() / 720.0f);
		this->m_pRC->p_FontResource->setScale(scale);

		//float avgscale = ((gEnv->pRenderer->GetWidth() / 1280.0f) + (gEnv->pRenderer->GetHeight() / 720.0f)) / 2;

		this->testFont = gEnv->pSystem->GetICryFont()->NewFont("NotoSansCJKjp-Regular");
		this->testFont->Load("Fonts/NotoSansCJKjp/NotoSansCJKjp-Regular.xml");
		this->testFont->AddRef();

		this->lyrics = new LyricsFile("@songs@/Test Group/Test Song/Lyrics/default.xml");
		LyricsFile::Info info = this->lyrics->GetInfo();
		if (info.valid) {
			CryLog("Lyrics File is valid. %i", this->lyrics->GetNumLines());
			for (int i = 0; i < this->lyrics->GetNumLines(); i++) {
				CLOG("-Lyric: %s", this->lyrics->GetLine(i)->text);
			}

			for (int i = 0; i < this->lyrics->GetNumSubtexts(); i++) {
				CLOG("-Subtext: %s", this->lyrics->GetSubtext(i)->text);
			}
		};

		/*this->textOps.font = this->testFont;
		this->textOps.color = this->textColor.toVec3();
		this->textOps.effectIndex = 0;*/

		testFontDrawContext.SetSizeIn800x600(false);
		testFontDrawContext.SetSize(vector2f(24, 24));
		testFontDrawContext.SetColor(this->textColor);
		//testFontDrawContext.SetEffect(testFont->GetEffectId("cool"));

		//for (int i = 0; i < testFont->GetNumEffects(); i++) CryLog("[Font Effect] %i - %s", i, testFont->GetEffectName(i));

		//judges
		std::vector<std::string> judgeNames = DivaJudgeFactory::getFactory().getJudges();
		if (judgeNames.size() > 0) {
			CryLog("Judges:");
			for (std::string name : judgeNames) CryLog("-%s", name.c_str());
		}

		//this->testJudge = new OpenDivaJudge();
		this->testJudge = DivaJudgeFactory::getFactory().newJudge(judgeNames[0]);

		//string notefilepath(getcwd(buff, MAX_PATH + 1));
		//notefilepath += "/" + OpenDiva::Folders::sOpenDivaFolder + "/Test/testNoteFile.xml";
		//NoteFile noteFile(notefilepath);

		this->loadSequences();

		XmlNodeRef unicodeTest = m_gameFramework->GetISystem()->LoadXmlFromFile("Resources/test/unicode.xml");

		//assert(unicodeTest != 0);
		//XmlNodeRef content = unicodeTest->findChild("content");
		//this->unicodeChar = unicodeTest->getContent();

		this->unicodeStr = new AZStd::string(unicodeTest->getContent());

		//music testing

		AZStd::string songpath(assetsPath);
		songpath += AZStd::string(FolderStruct::Paths::sSongPath.c_str()) + "/Test Group/Test Song/testSong.ogg";

		AZStd::string songpath2(assetsPath);
		songpath2 += AZStd::string(FolderStruct::Paths::sSongPath.c_str()) + "/Test Group/Test Song/testSong4.ogg";
		//CryLog("SongPath: %s", songpath);
		//CryLog("SongPath c_str: %s", songpath.c_str());
		/*songpath += "/OpenDiva/Songs/Test Group/Test Song/";
		songpath += "testSong3.flac";*/

		//path.append("\\OpenDiva");
		//path.append("Songs\\Test Group\\Test Song\\testSong.ogg");
		//CryLog("Path:");
		//CryLog(path.c_str());

		//this->paSystem = new PortAudioSystem();
		//while (this->paSystem->HasError()) CryLog("PortAudioSystem Error: %s", this->paSystem->GetError().str);

		//this->testAudioFile = AudioSourceFactory::getFactory().newAudioSource("libsndfile-memory", songpath.c_str());
		//while (this->testAudioFile->HasError()) CryLog("TestAudioFile Error: %s", this->testAudioFile->GetError().str);

		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> libs;
		EBUS_EVENT_RESULT(
			libs,
			AlternativeAudio::AlternativeAudioSourceBus,
			GetAudioLibraryNames
		);

		CryLog("Audio Libraries:");
		for (AZStd::pair<AZStd::string, AZ::Crc32> lib : libs)
			CryLog("- %s, %i", lib.first.c_str(), lib.second);

		EBUS_EVENT_RESULT(
			this->testAudioFile,
			AlternativeAudio::AlternativeAudioSourceBus,
			NewAudioSource,
			AZ_CRC("libsndfile"),
			songpath.c_str(),
			nullptr
		);
		if (this->testAudioFile != nullptr) while (this->testAudioFile->HasError()) CryLog("TestAudioFile Error: %s", this->testAudioFile->GetError().str);

		//this->testAudioFile2 = AudioSourceFactory::getFactory().newAudioSource("libsndfile", songpath2.c_str());
		//while (this->testAudioFile2->HasError()) CryLog("TestAudioFile Error: %s", this->testAudioFile2->GetError().str);

		EBUS_EVENT_RESULT(
			this->testAudioFile2,
			AlternativeAudio::AlternativeAudioSourceBus,
			NewAudioSource,
			AZ_CRC("libsndfile_memory"),
			songpath2.c_str(),
			nullptr
		);
		if (this->testAudioFile2 != nullptr) while (this->testAudioFile2->HasError()) CryLog("TestAudioFile2 Error: %s", this->testAudioFile2->GetError().str);

		testAudioFileID = -1;
		testAudioFileID2 = -1;

		EBUS_EVENT_RESULT(
			this->masterVolumeDSP,
			AlternativeAudio::AlternativeAudioDSPBus,
			NewDSPEffect,
			AZ_CRC("AAVolumeControl"),
			nullptr
		);

		this->masterVolumeDSP->AddRef();
		//this->testAudioFile->AddEffect(this->masterVolumeDSP, 0);
		this->testAudioFile->AddEffectFreeSlot(this->masterVolumeDSP);

		/*EBUS_EVENT(
			AlternativeAudio::AlternativeAudioRequestBus,
			AddDSPEffect,
			AlternativeAudio::DSPSection::eDS_Output,
			AZ_CRC("AAVolumeControl"),
			nullptr,
			0
		);*/

		/*EBUS_EVENT_RESULT(
			this->masterVolumeDSP,
			AlternativeAudio::AlternativeAudioRequestBus,
			GetDSPEffect,
			AlternativeAudio::DSPSection::eDS_Output,
			0
		);*/

		/*IFlowGraphPtr graph = gEnv->pFlowSystem->CreateFlowGraph(); //create a graph
		graph->SerializeXML(XmlNodeRef(), true); //reading from xml
		graph->SetActive(true);

		graph->SetActive(false);
		graph->UnregisterFromFlowSystem();
		graph->Release();*/

		/*
		ILyShine *ls = gEnv->pLyShine;
		AZ::Entity ent = ls->LoadCanvas("");

		ent.Activate();

		ent.Deactivate();
		*/

		//-------------------------
		//Entity Component stuff
		//-------------------------
		//AZ::EntityId id = gEnv->pLyShine->LoadCanvas("");
		//AZ::Entity * entC = new AZ::Entity(id);

		//this->ent = new AZ::Entity("Test Entity");

		//CryLog("Creating Test Ent.");
		////AZ::EntityId id = gEnv->pLyShine->LoadCanvas(canvasPath.c_str());
		////this->ent = new AZ::Entity("LyShine Test Entity");
		////AZ::Component * lyshineComponent = ent->CreateComponent(LyShine::lyShineSystemComponentUuid);
		//AzFramework::ScriptComponent * scriptComponent = this->ent->CreateComponent<AzFramework::ScriptComponent>();
		////AZ::Component * canvasAssetComponent = this->ent->CreateComponent("{05BED4D7-E331-4020-9C17-BD3F4CE4DE85}");

		////AZ::Component * inputComponent = this->ent->CreateComponent("{3106EE2A-4816-433E-B855-D17A6484D5EC}");

		//AZ::Component * sqlLiteComponent = this->ent->CreateComponent(SQLite::SQLiteSystemComponentUUID);
		//
		///*AZ::Component * inputComponent = this->ent->CreateComponent("{3106EE2A-4816-433E-B855-D17A6484D5EC}");
		//Input::InputConfigurationComponent * inputComponent = this->ent->CreateComponent<Input::InputConfigurationComponent>();*/

		//if (this->ent->GetId().IsValid()) CryLog("Entity is Valid.");
		//if (scriptComponent != nullptr) CryLog("Script Component is Valid.");
		////if (canvasAssetComponent != nullptr) CryLog("Canvas Asset Component is Valid.");
		////if (inputComponent != nullptr) CryLog("Input Configuration Component is Valid.");

		//CryLog("Initializing Test Ent.");
		//this->ent->Init();

		//CryLog("Loading Script.");
		//const AZ::Data::AssetType& scriptAssetType = azrtti_typeid<AZ::ScriptAsset>();

		//AZ::Data::AssetId assetId;
		//AZStd::string str = gEnv->pFileIO->GetAlias("@assets@") + AZStd::string("/Test/test.lua");
		//CryLog("Script Path: %s", str.c_str());
		//EBUS_EVENT_RESULT(assetId, AZ::Data::AssetCatalogRequestBus, GetAssetIdByPath, str.c_str(), scriptAssetType, true);
		//AZ::Data::AssetCatalogRequestBus::BroadcastResult(assetId, &AZ::Data::AssetCatalogRequests::GetAssetIdByPath, str.c_str(), scriptAssetType, true);

		//AZStd::string retstr;
		////EBUS_EVENT_RESULT(retstr, AZ::Data::AssetCatalogRequestBus, GetAssetPathById, assetId);
		/*AZ::Data::AssetCatalogRequestBus::BroadcastResult(retstr, &AZ::Data::AssetCatalogRequests::GetAssetPathById, assetId);
		CryLog("Asset Script Path: %s", retstr.c_str());

		if (assetId.IsValid()) {
			CryLog("Setting Script.");
			AZ::Data::Asset<AZ::ScriptAsset> scriptAsset(assetId, scriptAssetType);
			scriptComponent->SetScript(scriptAsset);
		}*/

		/*ent->Deactivate();
		delete ent;*/

		/*testRatingResourceFont = new RatingResourceFont("");

		Vec2 scale = Vec2(gEnv->pRenderer->GetWidth()/1280.0f, gEnv->pRenderer->GetHeight()/720.0f);

		testRatingResourceFont->setImgScale(scale);
		testRatingResourceFont->setPosScale(scale);

		CryLog("Renderer Size: %i x %i", gEnv->pRenderer->GetWidth(), gEnv->pRenderer->GetHeight());
		CryLog("Overlay Renderer Size: %i x %i", gEnv->pRenderer->GetOverlayWidth(), gEnv->pRenderer->GetOverlayHeight());

		int x, y, w, h;

		gEnv->pRenderer->GetViewport(&x,&y,&w,&h);

		CryLog("Renderer Viewport: (%i,%i) - %i x %i", x,y,w,h);*/

		//gEnv->pCryPak->OpenPacks("/Paks/Styles/*.pak");
		//gEnv->pCryPak->OpenPack("/Paks/Styles/[name].pak");

		//this->testLyShine();

		CryLog("Alias Paths:");
		CryLog("-Root: %s", gEnv->pFileIO->GetAlias("@root@"));
		CryLog("-User: %s", gEnv->pFileIO->GetAlias("@user@"));
		CryLog("-Log: %s", gEnv->pFileIO->GetAlias("@log@"));
		CryLog("-Cache: %s", gEnv->pFileIO->GetAlias("@cache@"));
		CryLog("-Assets: %s", gEnv->pFileIO->GetAlias("@assets@"));
		CryLog("-Dev Root: %s", gEnv->pFileIO->GetAlias("@devroot@"));
		CryLog("-Dev Assets: %s", gEnv->pFileIO->GetAlias("@devassets@"));
		CryLog("Custom Alias Paths:");
		CryLog("-Songs: %s", gEnv->pFileIO->GetAlias("@songs@"));
		CryLog("-Styles: %s", gEnv->pFileIO->GetAlias("@styles@"));
		CryLog("-Hud: %s", gEnv->pFileIO->GetAlias("@hud@"));

		/*gEnv->pFileIO->FindFiles("@songs@", "*.*",
			[&](const char* filePath) -> bool {
				CryLog("File Found: %s", filePath);
				gEnv->pFileIO->FindFiles(filePath, "*.*",
					[&](const char* filePath) -> bool {
						CryLog("-File Found: %s", filePath);
							gEnv->pFileIO->FindFiles(filePath, "*.xml",
							[&](const char* filePath) -> bool {
								CryLog("--File Found: %s", filePath);
								return true;
							}
						);
						return true;
					}
				);
				return true; // continue iterating
			}
		);*/

		//int ret;
		//SQLite3::SQLiteDB * sysDb;
		////SQLite::SQLiteRequestBus::EventResult(sysDb, AZ::EntityId(0), &SQLite::SQLiteRequestBus::Events::GetConnection);
		//SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection);
		//AZ_Assert(sysDb, "sysDb is null.");

		////int ret;
		////SQLite3::SQLiteDBBus::EventResult(ret, sysDb, &SQLite3::SQLiteDBBus::Events::Exec, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
		//SQLITEDB_BUS(ret, sysDb, Exec, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
		//AZ_Assert(ret == SQLITE_OK, "setting foreing keys failed.");

		////create a table
		////SQLite3::SQLiteDBBus::EventResult(ret, sysDb, &SQLite3::SQLiteDBBus::Events::Exec, "CREATE TABLE System (Col1 int, Col2 varchar(255));", nullptr, nullptr, nullptr);
		//SQLITEDB_BUS(ret, sysDb, Exec, "CREATE TABLE IF NOT EXISTS System (Col1 int, Col2 varchar(255));", nullptr, nullptr, nullptr);
		//AZ_Assert(ret == SQLITE_OK, "Create Table Failed.");

		////insert a row
		////SQLite3::SQLiteDBBus::EventResult(ret, sysDb, &SQLite3::SQLiteDBBus::Events::Exec, "INSERT INTO System (Col1, Col2) VALUES(0,'FIRST');", nullptr, nullptr, nullptr);
		//SQLITEDB_BUS(ret, sysDb, Exec, "INSERT INTO System (Col1, Col2) VALUES(0,'FIRST');", nullptr, nullptr, nullptr);
		//AZ_Assert(ret == SQLITE_OK, "Insert 1 Failed.");

		////insert a row
		////SQLite3::SQLiteDBBus::EventResult(ret, sysDb, &SQLite3::SQLiteDBBus::Events::Exec, "INSERT INTO System (Col1, Col2) VALUES(1,'SECOND');", nullptr, nullptr, nullptr);
		//SQLITEDB_BUS(ret, sysDb, Exec, "INSERT INTO System (Col1, Col2) VALUES(1,'SECOND');", nullptr, nullptr, nullptr);
		//AZ_Assert(ret == SQLITE_OK, "Insert 2 Failed.");

		////insert a row
		////SQLite3::SQLiteDBBus::EventResult(ret, sysDb, &SQLite3::SQLiteDBBus::Events::Exec, "INSERT INTO System (Col1, Col2) VALUES(2,'THIRD');", nullptr, nullptr, nullptr);
		//SQLITEDB_BUS(ret, sysDb, Exec, "INSERT INTO System (Col1, Col2) VALUES(2,'THIRD');", nullptr, nullptr, nullptr);
		//AZ_Assert(ret == SQLITE_OK, "Insert 3 Failed.");

		//CryLog("Testing DB Select...");
		////select all from the table
		////SQLite3::SQLiteDBBus::EventResult(
		////	ret,
		////	sysDb,
		////	&SQLite3::SQLiteDBBus::Events::Exec,
		////	"SELECT * FROM System;",
		////	[](void* cbarg, int argc, char **argv, char **azColName) -> int {
		////		for (int i = 0; i < argc; i++) CryLog("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
		////		return 0;
		////	},
		////	nullptr,
		////	nullptr
		////);
		//SQLITEDB_BUS(
		//	ret,
		//	sysDb,
		//	Exec,
		//	"SELECT * FROM System;",
		//	[](void* cbarg, int argc, char **argv, char **azColName) -> int {
		//		for (int i = 0; i < argc; i++) CryLog("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
		//		return 0;
		//	},
		//	nullptr,
		//	nullptr
		//);
		//AZ_Assert(ret == SQLITE_OK, "Select Failed");
	}

	void OpenDivaGame::setupFlowgraph() {
		/*CryLog("loading test graph xml.");
		char buff[MAX_PATH + 1];
		string graphpath(getcwd(buff, MAX_PATH + 1));
		graphpath += "/" + OpenDiva::Folders::sOpenDivaFolder + "/Test/default.xml";
		XmlNodeRef graphXML = gEnv->pSystem->LoadXmlFromFile(graphpath);

		testGraph = gEnv->pSystem->GetIFlowSystem()->CreateFlowGraph();
		//testGraph = gEnv->pFlowSystem->CreateFlowGraph();
		testGraph->SerializeXML(graphXML, true);
		testGraph->SetActive(true);
		CryLog("done loading test graph xml.");*/
	}

	void OpenDivaGame::renderTesting() {
		//PASLength currTime = this->musicStream->GetCurrTimeFormated();
		//PASLength timeLen = this->musicStream->GetLength();
		//char * musicStr = "";

		//sprintf(musicStr, "Music Time: %02d : %02d : %06.3f / %02d: %02d : %06.3f", currTime.hrs, currTime.min, currTime.sec, timeLen.hrs, timeLen.min, timeLen.sec);

		/*this->iRenderer->Draw2dLabel(
		10,
		10,
		1,
		this->textColor,
		false,
		"Music Time: %02d : %02d : %06.3f / %02d: %02d : %06.3f",
		currTime.hrs, currTime.min, currTime.sec,
		timeLen.hrs, timeLen.min, timeLen.sec
		);*/

		//draw commands here

		//const char * str = Unicode::Convert<const char*, wchar_t*>(L"\u3042");
		//this->iRenderer->Draw2dLabel(10, 10, 12, this->textColor, false, this->unicodeChar);
		//this->iRenderer->Draw2dLabelWithFlags(10, 30, 2, this->textColor, eDrawText_2D | eDrawText_FixedSize, this->unicodeChar);

		//this->iRenderer->Draw2dLabel(10, 10, 2, this->textColor, false, "TEST \u1E2A");
		//this->iRenderer->Draw2dLabelWithFlags(10, 30, 2, this->textColor, eDrawText_2D | eDrawText_FixedSize, "TEST2 U+3042");

		//this->testFont->DrawString(0, 0, "TEST UNICODE \u0444", true, testFontDrawContext);

		//std::shared_ptr<OD_Draw2d> OD_Draw2dPtr = OD_Draw2d::GetPtr();

		//OD_Draw2dPtr->BeginDraw2d(1920, 1080);
		OD_Draw2d::getDraw2D().BeginDraw2d(1280, 720);
		//OD_Draw2dPtr->BeginDraw2d();
		//this->iDraw2d->DrawText(musicStr, { 10, 10 }, 12);
		//this->iDraw2d->DrawText(this->unicodeChar, { 30, 30 }, 24);
		//this->iDraw2d->DrawText("Testing Unicode", { 50, 40 }, 24, 1.0f, &this->textOps);
		//this->iDraw2d->DrawText(this->unicodeStr->c_str(), { 50, 60 }, 24, 1.0f, &this->textOps);
		//this->testFont->DrawString(50, 40, "Testing Unicode", false, testFontDrawContext);
		//this->testFont->DrawString(10, 10, this->unicodeStr->c_str(), true, testFontDrawContext);

		/*string hud = "Score: ";
		hud.append(std::to_string(this->testJudge->getScore()).c_str());
		hud.append("   HP: ");
		hud.append(std::to_string(this->testJudge->getHealth()).c_str());

		this->iDraw2d->DrawText(hud, { 10, 10 }, 32);*/

		this->iDraw2d->DrawText(std::to_string(gEnv->pTimer->GetFrameTime()).c_str(), AZ::Vector2(10, 40), 16);
		//this->iDraw2d->DrawText(std::to_string(this->paSystem->GetDeltaTime()).c_str(), AZ::Vector2(10, 60), 16);

		if (this->testAudioFileID != -1 && this->testAudioFile != nullptr) {
			AlternativeAudio::AudioSourceTime currTime;// = this->paSystem->GetTime(this->testAudioFileID);
			EBUS_EVENT_RESULT(currTime, AlternativeAudio::AlternativeAudioDeviceBus, GetTime, this->testAudioFileID);
			if (this->m_prevTime.totalSec != currTime.totalSec) {
				this->iDraw2d->DrawText(std::to_string(currTime.totalSec - this->m_prevTime.totalSec).c_str(), AZ::Vector2(10, 80), 16);
				this->m_prevTime = currTime;
			} else {
				this->iDraw2d->DrawText(std::to_string(0).c_str(), AZ::Vector2(10, 80), 16);
			}
		} else {
			this->iDraw2d->DrawText("waiting...", AZ::Vector2(10, 80), 16);
		}

		AZStd::string lyric = "CurrLyric: " + this->currLyric;
		this->iDraw2d->DrawText(lyric.c_str(), AZ::Vector2(10, 100), 16);

		OD_Draw2d::getDraw2D().EndDraw2d();

		//this->iDraw2d->BeginDraw2d(true);
		/*if (!this->testSeq->IsPaused()) {
		Vec3 seqPos = this->testSeq->GetNode(0)->GetPos();
		cPos.set(seqPos.x, seqPos.y);
		}*/
		//this->iDraw2d->DrawImage(this->testTex->GetTextureID(), cPos, cSize);
		//this->iDraw2d->DrawImageAligned(this->testTex->GetTextureID(), cPos, cSize, IDraw2d::HAlign::Center, IDraw2d::VAlign::Center);
		//this->iDraw2d->DrawText("test3 U+1E2A", { 10, 100 }, 12);

		//this->testFont->DrawString(150, 150, "TEST4 \u3042", true, this->testFontDrawContext);
		//this->iDraw2d->EndDraw2d();

		//this->testFont->DrawString(40, 40, "TEST3 \u0444", false, this->testFontDrawContext);

		//if (this->testSingleNode2 != NULL) this->testSingleNode2->Render();
		//if (this->testSingleNode != NULL) this->testSingleNode->Render();
		//if (this->testHoldNode != NULL) this->testHoldNode->Render();

		//this->testDivaSeq->Render();
	}

	void OpenDivaGame::loadSequences() {
		this->iMovieSys = gEnv->pSystem->GetIMovieSystem();

		CryLog("Loading Sequences.");
		if (this->testSeq == NULL) {
			CryLog("Creating Test Sequence.");
			this->testSeq = this->iMovieSys->CreateSequence("DivaSequence", false, 0U, eSequenceType_SequenceComponent);
			this->testSeq->SetTimeRange(Range(0.0f, 15.0f + BIEZER_TACKON));
			this->testSeq->SetFlags(IAnimSequence::eSeqFlags_NoAbort | IAnimSequence::eSeqFlags_CutScene | IAnimSequence::eSeqFlags_OutOfRangeConstant | IAnimSequence::eSeqFlags_NoMPSyncingNeeded | IAnimSequence::eSeqFlags_NoSpeed);
			this->testSeq->AddRef();

			this->testDivaAnimationNode = new DivaAnimationNode(this->m_pRC);
			this->testDivaAnimationNode->AddRef();
			this->testSeq->AddNode(this->testDivaAnimationNode);

			this->iSys->AddListener(this->testDivaAnimationNode);

			//this->testSeq->AddTrackEventListener(this->dsfgEventListener);

			NoteFile noteFile("@songs@/Test Group/Test Song/NoteMaps/test.xml");

			SongInfo::Global g = SongInfo::GetGlobal("@songs@/Test Group/Test Song/SongInfo/global.xml");
			CLOG("Global song info:");
			CLOG("-Lib: %s", g.lib.c_str());
			CLOG("-vocal: %s", g.vocal.c_str());
			CLOG("-melody: %s", g.melody.c_str());
			CLOG("-song: %s", g.song.c_str());

			if (this->testDivaAnimationNode->InitNotes(&noteFile)) CLOG("Notes Initialized.");
			if (this->testDivaAnimationNode->InitLyrics(this->lyrics)) CLOG("Lyrics Initialized.");
			if (this->testDivaAnimationNode->InitAudio(g)) CLOG("Audio Initialized.");
		}

		/*if (this->testButtonNode2 == NULL) {
			CryLog("Creating Button Node 2.");
			this->testButtonNode2 = new PrototypeNoteNode(0, ENoteType::eNT_Square, this->m_pRC);
			this->testSeq->AddNode(this->testButtonNode2);
			this->testButtonNode2->CreateDefaultTracks();
			this->testButtonNode2->AddRef();

			this->testButtonNode2->SetBCurve(Vec2(100, 115), Vec2(500, 215), Vec2(700, 515), 1.5f, 5.5f);
		}

		if (this->testButtonNode == NULL) {
			CryLog("Creating Button Node.");
			this->testButtonNode = new PrototypeNoteNode(1, ENoteType::eNT_Square, this->m_pRC);
			this->testSeq->AddNode(this->testButtonNode);
			this->testButtonNode->CreateDefaultTracks();
			this->testButtonNode->AddRef();

			this->testButtonNode->SetBCurve(Vec2(100, 100), Vec2(500, 200), Vec2(700, 500), 1.0f, 5.0f);
		}*/

		/*this->testButtonNode->SetPos(0, Vec3(0, 0, 0));
		this->testButtonNode->SetPos(1, Vec3(50, 100, 0));
		this->testButtonNode->SetPos(2, Vec3(200, 100, 0));*/


		/*this->testButtonNode->SetScale(1, 1.0f);
		this->testButtonNode->SetScale(1.5, 1.25f);
		this->testButtonNode->SetScale(2, 1.0f);*/

		/*this->testButtonNode->SetRot(2, 0);
		this->testButtonNode->SetRot(6, 360);*/

		/*this->testButtonNode->SetOpacity(0, 1.0f);
		this->testButtonNode->SetOpacity(2, 0.0f);*/

		/*IAnimTrack * posTrack = this->testButtonNode->GetTrackByIndex(0);
		posTrack->SetValue(0, Vec3(0, 0, 0));
		posTrack->SetValue(1, Vec3(50, 100, 0));
		posTrack->SetValue(2, Vec3(200, 100, 0));

		IAnimTrack * scaleTrack = this->testButtonNode->GetTrackByIndex(1);
		scaleTrack->SetValue(0, Vec3(1.0f, 1.0f, 0));
		scaleTrack->SetValue(2, Vec3(0.5f, 0.5f, 0));*/

		/*IAnimTrack * opacityTrack = this->testButtonNode->GetTrackByIndex(3);
		opacityTrack->SetValue(0, 1.0f);
		opacityTrack->SetValue(2, 0.0f);*/
		//}


		/*if (this->testSeq != NULL) {
			XmlNodeRef testXML = gEnv->pSystem->CreateXmlNode("testSeq.xml");
			this->testSeq->Serialize(testXML,false);
			testXML->saveToFile("testSeq.xml");
		}*/

		//this->testDivaSeq = new DivaSequence(this->m_pRC);
		//this->testDivaSeq->AddRef();

		//this->testDivaSeq->GetSequence()->AddTrackEventListener(this->dsfgEventListener);

		//this->iMovieSys->AddSequence(this->testDivaSeq);

		//gEnv->pMovieSystem->AddSequence(this->testDivaSeq->GetSequence());

		//this->iSys->AddListener(this->testDivaSeq);

		/*if (this->testSingleNode == NULL) {
			CryLog("Creating testSingleNode.");

			NoteEntrySingle entrySingle;
			entrySingle.id = 0;
			entrySingle.angle = 0;
			entrySingle.pType = ePT_Line;
			entrySingle.sType = eST_Norm;
			entrySingle.time = 4;
			entrySingle.type = eNT_Circle;
			entrySingle.pos = Vec2(200, 100);
			entrySingle.ctrlDist1 = 0.5f;
			entrySingle.ctrlDist2 = -250.0f;

			NoteEntryBPM entryBPM;
			//entryBPM.beats = 1;
			entryBPM.bpm = 60;
			entryBPM.time = 0;
			entryBPM.id = 0;
			entryBPM.sType = eST_Norm;

			this->testSingleNode = new DivaNoteSingleNode(2, this->m_pRC, this->testJudge, entrySingle);

			this->testSeq->AddNode(this->testSingleNode);
			this->testSingleNode->Init(entryBPM);
			this->testSingleNode->AddRef();
			this->testSingleNode->setDivaSeqEff(this->testDivaSeq);

			this->iSys->AddListener(this->testSingleNode);
		}

		if (this->testSingleNode2 == NULL) {
			CryLog("Creating testSingleNode 2.");

			NoteEntrySingle entrySingle;
			entrySingle.id = 0;
			entrySingle.angle = 90;
			entrySingle.pType = ePT_Line;
			entrySingle.sType = eST_Norm;
			entrySingle.time = 5;
			entrySingle.type = eNT_Circle;
			entrySingle.pos = Vec2(200, 100);
			entrySingle.ctrlDist1 = 0.5f;
			entrySingle.ctrlDist2 = -250.0f;

			NoteEntryBPM entryBPM;
			//entryBPM.beats = 1;
			entryBPM.bpm = 60;
			entryBPM.time = 0;
			entryBPM.id = 0;
			entryBPM.sType = eST_Norm;

			this->testSingleNode2 = new DivaNoteSingleNode(3, this->m_pRC, this->testJudge, entrySingle);

			this->testSeq->AddNode(this->testSingleNode2);
			this->testSingleNode2->Init(entryBPM);
			this->testSingleNode2->AddRef();
			this->testSingleNode2->setDivaSeqEff(this->testDivaSeq);

			this->iSys->AddListener(this->testSingleNode2);
		}*/

		/*if (this->testHoldNode == NULL) {
			NoteEntryHold entryHold;
			entryHold.id = 0;
			entryHold.angle = 0;
			entryHold.pType = ePT_Line;
			entryHold.sType = eST_Norm;
			entryHold.hold1 = 5;
			entryHold.hold2 = 10;
			entryHold.type = eNT_Cross;
			entryHold.pos = Vec2(200, 200);
			entryHold.ctrlDist1 = 0.5f;
			entryHold.ctrlDist2 = -250.0f;

			NoteEntryBPM entryBPM;
			entryBPM.bpm = 60;
			entryBPM.time = 0;
			entryBPM.id = 0;
			entryBPM.sType = eST_Norm;

			this->testHoldNode = new DivaNoteHoldNode(4, this->m_pRC, this->testJudge, &entryHold);

			this->testSeq->AddNode(this->testHoldNode);
			this->testHoldNode->Init(entryBPM);
			this->testHoldNode->AddRef();
			this->testHoldNode->setDivaSeqEff(this->testDivaSeq);

			this->iSys->AddListener(this->testHoldNode);
		}*/

		/*DivaNode * dNode = new DivaNode(this->m_pRC);
		this->iMovieSys->AddSequence(dNode);
		dNode->Init(nullptr);
		dNode->AddRef();*/
	}

	void OpenDivaGame::unloadSequences() {
		CryLog("Unloading Sequences.");
		/*if (this->testButtonNode) {
			CryLog("Removing Button Node.");
			this->testSeq->RemoveNode(this->testButtonNode);
			this->testButtonNode->Release();
			this->testButtonNode = NULL;
		}

		if (this->testButtonNode2) {
			CryLog("Removing Button Node 2.");
			this->testSeq->RemoveNode(this->testButtonNode2);
			this->testButtonNode2->Release();
			this->testButtonNode2 = NULL;
		}*/

		/*if (this->testSingleNode) {
			CryLog("Removing testSingleNode.");
			this->testSeq->RemoveNode(this->testSingleNode);
			this->testSingleNode->Release();
			this->testSingleNode = NULL;
		}

		if (this->testSingleNode2) {
			CryLog("Removing testSingleNode 2.");
			this->testSeq->RemoveNode(this->testSingleNode2);
			this->testSingleNode2->Release();
			this->testSingleNode2 = NULL;
		}*/

		/*if (this->testHoldNode) {
			CryLog("Removing testHoldNode.");
			this->testSeq->RemoveNode(this->testHoldNode);
			this->testHoldNode->Release();
			this->testHoldNode = NULL;
		}*/

		if (this->testSeq) {
			CryLog("Removing Test Sequence.");
			this->iMovieSys->RemoveSequence(this->testSeq);
			this->testSeq->Release();
			this->testSeq = NULL;
			this->testDivaAnimationNode->Release();
			this->testDivaAnimationNode = NULL;
		}
	}

	void OpenDivaGame::musicStartStop() {
	}

	void OpenDivaGame::setupLua() {
		IScriptSystem* iScript = gEnv->pScriptSystem;

		/*CryLog("Testing Scripting:");
		string scriptpath(gEnv->pFileIO->GetAlias("@assets@"));
		scriptpath += "/Scripts/testScript.lua";

		CryLog("-Loading");
		iScript->ExecuteFile(scriptpath);
		CryLog("-Function");
		iScript->BeginCall("TEST", "Func");
		iScript->PushFuncParam(0);
		iScript->EndCall();
		CryLog("-Unloading");
		iScript->UnloadScript("testScript.lua");*/

		/*IScriptTable * table = iScript->CreateTable(true);
		table->BeginSetGetChain();
		table->SetValueChain("key1", 9);
		table->SetValueChain("key2", 1.0f);
		table->SetValueChain("key3", true);
		table->SetValueChain("key4", "string");
		table->EndSetGetChain();*/
	}

	void OpenDivaGame::unloadLua() {
	}

	//void OpenDivaGame::testLyShine() {
		/*CryLog("[testLyShine]");

		char buff[MAX_PATH + 1];
		string canvaspath(getcwd(buff, MAX_PATH + 1));
		canvaspath += OpenDiva::Paths::sResourcesPath + "/Hud/PPDXXX/Hud.uicanvas";

		AZ::EntityId id = gEnv->pLyShine->LoadCanvas(canvaspath);

		CryLog("[testLyShine] id: %s", id.ToString().c_str());

		//update flownode
		EBUS_EVENT(OpenDivaFlowgraphBus::OpenDivaCanvasIDBus, SetHudCanvasID, id);

		UiCanvasInterface* canvas = UiCanvasBus::FindFirstHandler(id);
		IUiAnimationSystem *animSys = canvas->GetAnimationSystem();

		unsigned int numSequences = animSys->GetNumSequences();
		CryLog("[testLyShine] Number of Sequences: %i", numSequences);

		if (numSequences > 0) for (int i = 0; i < numSequences; i++) CryLog("-%s", animSys->GetSequence(i)->GetName());

		gEnv->pLyShine->ReleaseCanvas(id);*/

		/*AZ::EntityId canvasEntityId = gEnv->pLyShine->CreateCanvas();
		UiCanvasInterface* canvas = UiCanvasBus::FindFirstHandler(canvasEntityId);
		canvas->SetEnabled(true);

		LYSwing::LYPanel * panel = new LYSwing::LYPanel("TEST PANEL", canvas);
		panel->Init();
		panel->Activate();

		LYSwing::LYLabel * label = new LYSwing::LYLabel("TEST LABEL", panel);
		label->Init();
		label->Activate();
		label->SetText("TEST TEXT");

		delete label;
		delete panel;

		gEnv->pLyShine->ReleaseCanvas(canvasEntityId);*/
	//}



	void OpenDivaGame::initLyShine() {
		/*this->canvasEntityId = gEnv->pLyShine->CreateCanvas();
		this->canvas = UiCanvasBus::FindFirstHandler(this->canvasEntityId);
		this->canvas->SetEnabled(true);

		this->label = new LYSwing::LYLabel("TEST LABEL", this->canvas);
		this->label->Init();
		this->label->Activate();
		this->label->SetText("TEST TEXT");
		this->label->SetColor(ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		this->label->SetTextAlignment(IDraw2d::HAlign::Left, IDraw2d::VAlign::Top);
		this->label->SetAnchors(UiTransform2dInterface::Anchors(0.0f, 0.0f, 0.0f, 0.0f), false, false);
		this->label->SetOffsets(UiTransform2dInterface::Offsets(0, 0, 100, 100));

		this->panel = new LYSwing::LYPanel("TEST PANEL", this->canvas);
		this->panel->Init();
		this->panel->Activate();

		this->label2 = new LYSwing::LYLabel("TEST LABEL 2", panel);
		this->label2->Init();
		this->label2->Activate();
		this->label2->SetText("TEST TEXT 2");
		//this->label2->SetFont("Fonts/NotoSansCJKjp/NotoSansCJKjp-Regular.xml");
		//this->label2->SetFontEffect(0);
		//this->label2->SetFontSize(32);
		this->label2->SetColor(ColorF(1.0f, 0.0f, 1.0f, 1.0f));
		this->label2->SetTextAlignment(IDraw2d::HAlign::Left, IDraw2d::VAlign::Top);
		this->label2->SetAnchors(UiTransform2dInterface::Anchors(0.0f, 0.0f, 0.0f, 0.0f),false,false);
		this->label2->SetOffsets(UiTransform2dInterface::Offsets(0, 0, 50, 50));*/
	}
	void OpenDivaGame::destroyLyShine() {
		//gEnv->pLyShine->ReleaseCanvas(this->canvasEntityId);
	}

	void OpenDivaGame::SetLyrics(AZStd::string lyrics) {
		currLyric = lyrics;
	}

} // namespace OpenDivaGame
