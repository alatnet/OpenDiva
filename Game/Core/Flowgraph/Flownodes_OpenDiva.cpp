#include <StdAfx.h>
#include <AzCore\Component\EntityId.h>
#include <LyShine/ILyShine.h>
#include <LyShine/Bus/UiCanvasBus.h>
#include <FlowSystem\Nodes\FlowBaseNode.h>

#include "Bus\Flownodes_OpenDiva_Bus.h"

#include "../OpenDivaGame.h"

namespace LYGame {
	//#define ActivateIfConnected(actInfo,port,value) if (IsOutputConnected(##actinfo##, ##port##)) ActivateOutput(##actinfo##,##port##,##value##)
	
	template <class T>
	ILINE void ActivateIfConnected(IFlowNode::SActivationInfo* pActInfo, int nPort, const T& value) {
		try { //try to activate the port
			if (pActInfo->pGraph != nullptr) {
				SFlowAddress addr(pActInfo->myID, nPort, true);
				if (pActInfo->pGraph->IsOutputConnected(addr)) pActInfo->pGraph->ActivatePort<T>(addr, value);
			}
		} catch (...) {
			//if we catch an exception, it's most likely pGraph's __vptr being null (to which we cant freaking check for...)
		}
	}

	#define BusConnectC() if(!this->m_busConnected) { BusConnect(); this->m_busConnected = true; }
	#define BusDisconnectC() if(this->m_busConnected) { BusDisconnect(); this->m_busConnected = false; }

	class CFlowNode_OpenDiva_Song : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaSongBus::Handler {
	public:
		CFlowNode_OpenDiva_Song(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			BusConnectC()
		};

		~CFlowNode_OpenDiva_Song() {
			BusDisconnectC()
		};

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_OpenDiva_Song(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig_Void("Start", _HELP("Activates when the song starts.")),
				OutputPortConfig_Void("End", _HELP("Activates when the song ends.")),
				{ 0 }
			};

			config.sDescription = _HELP("Song events.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
		}

		void OnStart() {
			ActivateIfConnected(&m_actInfo, OutputPorts::Start, 0);
		}

		void OnEnd() {
			ActivateIfConnected(&m_actInfo, OutputPorts::End, 0);
		}
	private:
		SActivationInfo m_actInfo;

		enum InputPorts {
		};

		enum OutputPorts {
			Start = 0,
			End
		};

		bool m_busConnected = false;
	};

	class CFlowNode_GetCanvasID : public CFlowBaseNode<eNCT_Singleton> {
	public:
		CFlowNode_GetCanvasID(SActivationInfo* pActInfo) {
		};

		~CFlowNode_GetCanvasID() {
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig_Void("Activate", _HELP("Trigger to retrieve the loaded hud canvas ID.")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<int>("ID", _HELP("The loaded Canvas ID.")),
				OutputPortConfig<FlowEntityId>("Entity", _HELP("The loaded Canvas Entity.")),
				{ 0 }
			};

			config.sDescription = _HELP("Gets the Canvas ID for the hud that has been loaded by OpenDiva.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			if (eFE_Activate == event && IsPortActive(pActInfo, InputPorts::Activate)) {
				AZ::EntityId hudID = g_Game->GetHudID();
				if (this->m_hudID != hudID) {
					this->m_hudID = hudID;
					EBUS_EVENT_ID_RESULT(this->m_canvasId, this->m_hudID, UiCanvasBus, GetCanvasId); // Until flowgraph supports ports for 64-bit EntityIds we need a separate UI canvas ID.
				}
				if (this->m_hudID.IsValid()) { //check to see if hud is loaded
					ActivateIfConnected(pActInfo, OutputPorts::CanvasID, this->m_canvasId);
					ActivateIfConnected(pActInfo, OutputPorts::CanvasEntity, FlowEntityId(this->m_hudID));
				}
			}
		}
	private:
		enum InputPorts {
			Activate = 0
		};

		enum OutputPorts {
			CanvasID = 0,
			CanvasEntity
		};

		AZ::EntityId m_hudID;
		LyShine::CanvasId m_canvasId;
	};

	class CFlowNode_Lyrics : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaLyricsBus::Handler {
	public:
		CFlowNode_Lyrics(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			this->m_lyrics = this->m_romaji = this->m_translation = "";
			BusConnectC();
		};

		~CFlowNode_Lyrics() {
			BusDisconnectC();
		};

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_Lyrics(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig_Void("Activate", _HELP("Trigger to retrieve current lyrics, romaji, and translations.")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<string>("Lyrics", _HELP("The lyrics of the song. (Expect this to be multilingual).")),
				OutputPortConfig<string>("Romaji", _HELP("The romanization of the lyrics. (Can be blank).")),
				OutputPortConfig<string>("Translation", _HELP("The translation of the lyrics (Expect this to be multilingual).")),
				{ 0 }
			};

			config.sDescription = _HELP("Gets the current lyrics, romaji, and translations that are to be displayed.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action

			if (eFE_Activate == event && IsPortActive(pActInfo, InputPorts::Activate)) {
				ActivateIfConnected(pActInfo, OutputPorts::Lyrics, this->m_lyrics);
				ActivateIfConnected(pActInfo, OutputPorts::Romaji, this->m_romaji);
				ActivateIfConnected(pActInfo, OutputPorts::Translation, this->m_translation);
			}
		}

		//ebus
		void SetLyrics(string lyrics, string romaji, string translation) {
			this->m_lyrics = lyrics;
			this->m_romaji = romaji;
			this->m_translation = translation;

			ActivateIfConnected(&m_actInfo, OutputPorts::Lyrics, lyrics);
			ActivateIfConnected(&m_actInfo, OutputPorts::Romaji, romaji);
			ActivateIfConnected(&m_actInfo, OutputPorts::Translation, translation);
		}

		void Reset() {
			this->m_lyrics = this->m_romaji = this->m_translation = "";
			ActivateIfConnected(&m_actInfo, OutputPorts::Lyrics, string(""));
			ActivateIfConnected(&m_actInfo, OutputPorts::Romaji, string(""));
			ActivateIfConnected(&m_actInfo, OutputPorts::Translation, string(""));
		}
	private:
		SActivationInfo m_actInfo;
		string m_lyrics, m_romaji, m_translation;

		enum InputPorts {
			Activate = 0
		};

		enum OutputPorts {
			Lyrics = 0,
			Romaji,
			Translation
		};

		bool m_busConnected = false;
	};

	class CFlowNode_HP : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaHPBus::Handler {
	public:
		CFlowNode_HP(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			this->m_hp = this->m_maxhp = 0;
			BusConnectC();
		};

		~CFlowNode_HP() {
			BusDisconnectC();
		};

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_HP(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig_Void("Activate", _HELP("Trigger to retreve the current HP and max HP.")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<string>("Text", _HELP("Current HP as a string.")),
				OutputPortConfig<int>("Value", _HELP("Current HP.")),
				OutputPortConfig<int>("Max", _HELP("The maximum the HP can be (Judge Specific).")),
				{ 0 }
			};

			config.sDescription = _HELP("Get's the player's current HP and Judge's Max HP.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action

			if (eFE_Activate == event && IsPortActive(pActInfo, InputPorts::Activate)) {
				ActivateIfConnected(pActInfo, OutputPorts::Value, this->m_hp);
				ActivateIfConnected(pActInfo, OutputPorts::Text, string(std::to_string(this->m_hp).c_str()));
				ActivateIfConnected(pActInfo, OutputPorts::Max, this->m_maxhp);
			}
		}


		void SetHP(int value) {
			this->m_hp = value;
			ActivateIfConnected(&m_actInfo, OutputPorts::Value, value);
			ActivateIfConnected(&m_actInfo, OutputPorts::Text, string(std::to_string(value).c_str()));
		}

		void SetMaxHP(int value) {
			this->m_maxhp = value;
			ActivateIfConnected(&m_actInfo, OutputPorts::Max, value);
		}

		void Reset() {
			this->m_hp = this->m_maxhp = 0;
			ActivateIfConnected(&m_actInfo, OutputPorts::Value, 0);
			ActivateIfConnected(&m_actInfo, OutputPorts::Text, string("0"));
			ActivateIfConnected(&m_actInfo, OutputPorts::Max, 0);
		}
	private:
		SActivationInfo m_actInfo;
		int m_hp, m_maxhp;

		enum InputPorts {
			Activate = 0
		};

		enum OutputPorts {
			Text = 0,
			Value,
			Max
		};

		bool m_busConnected = false;
	};

	class CFlowNode_Score : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaScoreBus::Handler {
	public:
		CFlowNode_Score(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			//this->m_score = 0;
			BusConnectC();
		};

		~CFlowNode_Score() {
			BusDisconnectC();
		}

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_Score(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig_Void("Activate", _HELP("Trigger to retreve the current Score.")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<string>("Text", _HELP("Current Score as a string.")),
				OutputPortConfig<int>("Value", _HELP("Current Score.")),
				{ 0 }
			};

			config.sDescription = _HELP("Get's the player's current score.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action

			if (eFE_Activate == event && IsPortActive(pActInfo, InputPorts::Activate)) {
				//EBUS_EVENT_RESULT(this->m_score, DivaJudgeBus, getScore);

				unsigned int score;
				EBUS_EVENT_RESULT(score, DivaJudgeBus, getScore);
				
				ActivateIfConnected(pActInfo, OutputPorts::Value, score);
				ActivateIfConnected(pActInfo, OutputPorts::Text, string(std::to_string(score).c_str()));
			}
		}

		void SetScore(int value) {
			//this->m_score = value;
			ActivateIfConnected(&m_actInfo, OutputPorts::Value, value);
			ActivateIfConnected(&m_actInfo, OutputPorts::Text, string(std::to_string(value).c_str()));
		}

		void Reset() {
			//this->m_score = 0;
			ActivateIfConnected(&m_actInfo, OutputPorts::Value, 0);
			ActivateIfConnected(&m_actInfo, OutputPorts::Text, string("0"));
		}
	private:
		SActivationInfo m_actInfo;
		//int m_score;

		enum InputPorts {
			Activate = 0
		};

		enum OutputPorts {
			Text = 0,
			Value
		};

		bool m_busConnected = false;
	};

	class CFlowNode_Completion : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaCompletionBus::Handler {
	public:
		CFlowNode_Completion(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			m_currNotes = m_totalNotes = 0;
			m_rating = eDJC_Cheap;
			BusConnectC();
		};

		~CFlowNode_Completion() {
			BusDisconnectC();
		}

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_Completion(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig_Void("Activate", _HELP("Trigger to retrieve the completion rating.")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<int>("NumNotes", _HELP("Number of notes contributing to the completion rating.")),
				OutputPortConfig<int>("Total", _HELP("Total number of notes.")),
				OutputPortConfig<float>("Percentage", _HELP("Percentage of completion.")),
				OutputPortConfig<int>("Rating", _HELP("Completion rating (Judge Specific):\n0 - Perfect\n1 - Excellent\n2 - Great\n3 - Standard\n4 - Cheap")),
				{ 0 }
			};

			config.sDescription = _HELP("Get's the song completion rating.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action

			if (eFE_Activate == event && IsPortActive(pActInfo, InputPorts::Activate)) {
				ActivateIfConnected(pActInfo, OutputPorts::NumNotes, this->m_currNotes);
				ActivateIfConnected(pActInfo, OutputPorts::TotalNotes, this->m_totalNotes);
				ActivateIfConnected(pActInfo, OutputPorts::Percentage, (this->m_currNotes / this->m_totalNotes) * 100);
				int irating = this->m_rating;
				ActivateIfConnected(pActInfo, OutputPorts::Rating, irating);
			}
		}

		void SetNotes(int value) {
			this->m_currNotes = value;
			ActivateIfConnected(&m_actInfo, OutputPorts::NumNotes, value);
			if (this->m_currNotes != 0 && this->m_totalNotes != 0) ActivateIfConnected(&m_actInfo, OutputPorts::Percentage, (this->m_currNotes / this->m_totalNotes) * 100);
		}

		void SetTotalNotes(int value) {
			this->m_totalNotes = value;
			ActivateIfConnected(&m_actInfo, OutputPorts::TotalNotes, value);
			if (this->m_currNotes != 0 && this->m_totalNotes != 0) ActivateIfConnected(&m_actInfo, OutputPorts::Percentage, (this->m_currNotes / this->m_totalNotes) * 100);
		}

		void SetRating(EDivaJudgeCompletion rating) {
			this->m_rating = rating;
			int irating = rating;
			ActivateIfConnected(&m_actInfo, OutputPorts::Rating, irating);
		}

		void Reset() {
			m_currNotes = m_totalNotes = 0;
			m_rating = eDJC_Cheap;
			ActivateIfConnected(&m_actInfo, OutputPorts::NumNotes, 0);
			ActivateIfConnected(&m_actInfo, OutputPorts::TotalNotes, 0);
			ActivateIfConnected(&m_actInfo, OutputPorts::Percentage, 0);
			ActivateIfConnected(&m_actInfo, OutputPorts::Rating, 0);
		}
	private:
		SActivationInfo m_actInfo;
		int m_currNotes, m_totalNotes;
		EDivaJudgeCompletion m_rating;

		enum InputPorts {
			Activate = 0
		};

		enum OutputPorts {
			NumNotes,
			TotalNotes,
			Percentage,
			Rating
		};

		bool m_busConnected = false;
	};

	class CFlowNode_ZoneEvents : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaZoneEventsBus::Handler {
	public:
		CFlowNode_ZoneEvents(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			BusConnectC();
		};

		~CFlowNode_ZoneEvents() {
			BusDisconnectC();
		}

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_ZoneEvents(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig_Void("Chance Time Enter", _HELP("Triggers when entering chance time.")),
				OutputPortConfig_Void("Chance Time Exit", _HELP("Triggers when exiting chance time.")),
				OutputPortConfig_Void("Technical Enter", _HELP("Triggers when entering technical.")),
				OutputPortConfig_Void("Technical Exit", _HELP("Triggers when exiting technical.")),
				{ 0 }
			};

			config.sDescription = _HELP("Zone Events.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
		}
		
		void OnChanceEnter() {
			ActivateIfConnected(&m_actInfo, OutputPorts::CTEnter, 0);
			//CryLog("CT Flow Enter Triggered");
		}

		void OnChanceExit() {
			ActivateIfConnected(&m_actInfo, OutputPorts::CTExit, 0);
			//CryLog("CT Flow Exit Triggered");
		}

		void OnTechEnter() {
			ActivateIfConnected(&m_actInfo, OutputPorts::TEnter, 0);
			//CryLog("Tech Flow Enter Triggered");
		}

		void OnTechExit() {
			ActivateIfConnected(&m_actInfo, OutputPorts::TExit, 0);
			//CryLog("Tech Flow Exit Triggered");
		}
	private:
		SActivationInfo m_actInfo;

		enum InputPorts {
		};

		enum OutputPorts {
			CTEnter = 0,
			CTExit,
			TEnter,
			TExit
		};

		bool m_busConnected = false;
	};

	class CFlowNode_TechnicalZoneInfo : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaTechnicalZoneInfoBus::Handler {
	public:
		CFlowNode_TechnicalZoneInfo(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			BusConnectC();
			this->m_notesLeft = 0;
			this->m_active = false;
		};

		~CFlowNode_TechnicalZoneInfo() {
			BusDisconnectC();
		}

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_TechnicalZoneInfo(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig_Void("Activate", _HELP("Trigger to retrieve Technical Zone information.")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<int>("NotesLeft",_HELP("How many notes are left to hit.")),
				OutputPortConfig<bool>("Active",_HELP("Is the zone still active (i.e. did not miss a note or get a bad rating).")),
				{ 0 }
			};

			config.sDescription = _HELP("Technical Zone Information.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action

			if (eFE_Activate == event && IsPortActive(pActInfo, InputPorts::Activate)) {
				//EBUS_EVENT_RESULT(this->m_notesLeft, DivaJudgeBus, numTechZoneNotes);
				//EBUS_EVENT_RESULT(this->m_active, DivaJudgeBus, isTechZoneActive);
				ActivateIfConnected(pActInfo, OutputPorts::NotesLeft, this->m_notesLeft);
				ActivateIfConnected(pActInfo, OutputPorts::Active, this->m_active);
			}
		}

		void SetNumNotes(int value) {
			this->m_notesLeft = value;
			ActivateIfConnected(&m_actInfo, OutputPorts::NotesLeft, value);
		}

		void SetActive(bool value) {
			this->m_active = value;
			ActivateIfConnected(&m_actInfo, OutputPorts::Active, value);
		}

		void Reset() {
			this->m_notesLeft = 0;
			this->m_active = false;
			ActivateIfConnected(&m_actInfo, OutputPorts::NotesLeft, 0);
			ActivateIfConnected(&m_actInfo, OutputPorts::Active, false);
		}
	private:
		SActivationInfo m_actInfo;
		int m_notesLeft;
		bool m_active;

		enum InputPorts {
			Activate = 0
		};

		enum OutputPorts {
			NotesLeft = 0,
			Active
		};

		bool m_busConnected = false;
	};

	class CFlowNode_Hitscore : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaHitScoreBus::Handler {
	public:
		CFlowNode_Hitscore(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
			BusConnectC();
		};

		~CFlowNode_Hitscore() {
			BusDisconnectC();
		}

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_Hitscore(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<int>("Hitscore",_HELP("What the hit score of the note is.")),
				OutputPortConfig<bool>("Wrong",_HELP("If the note that was hit was the wrong button input.")),
				{ 0 }
			};

			config.sDescription = _HELP("Events for when the player hits a note.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
		}

		void SetHitScore(EHitScore hitScore, bool wrong) {
			int ihitScore = hitScore;
			ActivateIfConnected(&m_actInfo, OutputPorts::HitScore, ihitScore);
			ActivateIfConnected(&m_actInfo, OutputPorts::Wrong, wrong);
		}
	private:
		SActivationInfo m_actInfo;

		enum InputPorts {
		};

		enum OutputPorts {
			HitScore = 0,
			Wrong
		};

		bool m_busConnected = false;
	};

	class CFlowNode_UiInput : public CFlowBaseNode<eNCT_Instanced>, public OpenDivaFlowgraphBus::OpenDivaUiInputBus::Handler {
	public:
		CFlowNode_UiInput(SActivationInfo* pActInfo){
			this->m_actInfo = *pActInfo; //used for async action
			BusConnectC();
		};

		~CFlowNode_UiInput() {
			BusDisconnectC();
		}

		virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
		{
			return new CFlowNode_UiInput(pActInfo);
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig<int>("Input", _HELP("Which input to read."),NULL,_UICONFIG("enum_int:Up=0,Down=1,Left=2,Right=3,Select=4,Cancel=5")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				OutputPortConfig<int>("Mode",_HELP("The mode of the input when triggered.")),
				OutputPortConfig<float>("Value",_HELP("The value of the input when triggered.")),
				OutputPortConfig_Void("Pressed",_HELP("If the input was pressed")),
				OutputPortConfig_Void("Hold",_HELP("If the input is held.")),
				{ 0 }
			};

			config.sDescription = _HELP("Reads UI Input events.");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
		}

		void OnUi_Up(int mode, float value) {
			if (GetPortInt(&m_actInfo, InputPorts::Input) == UI_Input::Up) Activate(mode, value);
		}

		void OnUi_Down(int mode, float value) {
			if (GetPortInt(&m_actInfo, InputPorts::Input) == UI_Input::Down) Activate(mode, value);
		}

		void OnUi_Left(int mode, float value) {
			if (GetPortInt(&m_actInfo, InputPorts::Input) == UI_Input::Left) Activate(mode, value);
		}

		void OnUi_Right(int mode, float value) {
			if (GetPortInt(&m_actInfo, InputPorts::Input) == UI_Input::Right) Activate(mode, value);
		}

		void OnUi_Select(int mode, float value) {
			if (GetPortInt(&m_actInfo, InputPorts::Input) == UI_Input::Select) Activate(mode, value);
		}

		void OnUi_Cancel(int mode, float value) {
			if (GetPortInt(&m_actInfo, InputPorts::Input) == UI_Input::Cancel) Activate(mode, value);
		}
	private:
		void Activate(int mode, float value) {
			ActivateIfConnected(&m_actInfo, OutputPorts::Mode, mode);
			ActivateIfConnected(&m_actInfo, OutputPorts::Value, value);
			switch (mode) {
			case eAAM_OnPress:
				ActivateIfConnected(&m_actInfo, OutputPorts::Pressed, 0);
				break;
				break;
			case eAAM_OnHold:
				ActivateIfConnected(&m_actInfo, OutputPorts::Hold, 0);
				break;
			}
		}

		SActivationInfo m_actInfo;

		enum UI_Input {
			Up = 0,
			Down,
			Left,
			Right,
			Select,
			Cancel
		};

		enum InputPorts {
			Input = 0
		};

		enum OutputPorts {
			Mode = 0,
			Value,
			Pressed,
			Hold
		};

		bool m_busConnected = false;
	};

	#undef BusConnectC
	#undef BusDisconnectC

	//register the flow nodes
	REGISTER_FLOW_NODE("OpenDiva:HUD:Song", CFlowNode_OpenDiva_Song);
	REGISTER_FLOW_NODE("OpenDiva:HUD:Get Canvas ID", CFlowNode_GetCanvasID);
	REGISTER_FLOW_NODE("OpenDiva:HUD:Lyrics", CFlowNode_Lyrics);
	REGISTER_FLOW_NODE("OpenDiva:HUD:HP", CFlowNode_HP);
	REGISTER_FLOW_NODE("OpenDiva:HUD:Score", CFlowNode_Score);
	REGISTER_FLOW_NODE("OpenDiva:HUD:Completion", CFlowNode_Completion);
	REGISTER_FLOW_NODE("OpenDiva:HUD:Zone Events", CFlowNode_ZoneEvents);
	REGISTER_FLOW_NODE("OpenDiva:HUD:Technical Zone Info", CFlowNode_TechnicalZoneInfo);
	REGISTER_FLOW_NODE("OpenDiva:HUD:Hitscore", CFlowNode_Hitscore);
	REGISTER_FLOW_NODE("OpenDiva:Menu:UI Input", CFlowNode_UiInput);

	//prototype
	/*class CFlowNode_OpenDiva_ : public CFlowBaseNode<eNCT_Instanced> {
	public:
		CFlowNode_OpenDiva_(SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action
										 //BusConnect();
		};

		virtual void GetMemoryUsage(ICrySizer* s) const {
			s->Add(*this);
		}

		virtual void GetConfiguration(SFlowNodeConfig& config) {
			static const SInputPortConfig in_config[] = {
				InputPortConfig_Void("Activate", _HELP("")),
				{ 0 }
			};

			static const SOutputPortConfig out_config[] = {
				{ 0 }
			};

			config.sDescription = _HELP("");
			config.pInputPorts = in_config;
			config.pOutputPorts = out_config;
			config.SetCategory(EFLN_APPROVED);
		}

		virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) {
			this->m_actInfo = *pActInfo; //used for async action

			if (eFE_Activate == event && IsPortActive(pActInfo, InputPorts::Activate)) {
			}
		}
	private:
		SActivationInfo m_actInfo;

		enum InputPorts {
			Activate = 0
		};

		enum OutputPorts {
		};

		bool m_busConnected = false;
	};*/
}