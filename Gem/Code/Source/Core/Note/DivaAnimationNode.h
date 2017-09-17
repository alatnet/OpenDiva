#ifndef _H_DIVASEQUENCE
#define _H_DIVASEQUENCE

#pragma once

#include <IMovieSystem.h>

#include "../Input/InputSystem.h"
#include "Nodes/DivaNoteNodes.h"
#include "../Files/NoteFile.h"
#include "../Graphics/Resources/ResourceCollection.h"
#include "../Graphics/Particles.h"
#include <../CryMovie/AnimSequence.h>
#include "../Bus/DivaSequenceBus.h"
#include "../Bus/DivaJudgeBus.h"
#include "../Bus/DivaHudBus.h"
#include "../Files/LyricsFile.h"

#include <AlternativeAudio\AlternativeAudioBus.h>

//how fast the fade in/out of the song should be
#define DIVAFADETIME 3.0f

namespace OpenDiva {
	class DivaAnimationNode :
		public IAnimNode,
		public ITrackEventListener,
		public IInputSystemListener,
		public DivaSequenceJudgeBus::Handler,
		public DivaSequenceEffectsBus::Handler,
		public DivaHudHitScoreBus::Handler {
	public:
		DivaAnimationNode(ResourceCollection * rc);
		~DivaAnimationNode();
	public:
		bool Init(NoteFile *noteFile);
		bool InitLyrics(LyricsFile * lyrics);
		bool InitAudio();
	public: //IInputSystemListener
		// only the current note needs to listen for input.
		void OnCross(LyInputEventType mode, float value);
		void OnCircle(LyInputEventType mode, float value);
		void OnSquare(LyInputEventType mode, float value);
		void OnTriangle(LyInputEventType mode, float value);

		void OnLeft(LyInputEventType mode, float value);
		void OnRight(LyInputEventType mode, float value);
		void OnUp(LyInputEventType mode, float value);
		void OnDown(LyInputEventType mode, float value);

		void OnStar(LyInputEventType mode, float value);

		void OnSwipeL(LyInputEventType mode, float value);
		void OnSwipeR(LyInputEventType mode, float value);

	protected: //buses
		void PushbackRating(AZ::Vector2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong);
		void PushbackEffect(AZ::Vector2 pos, EEffectList eff);
		void PushbackHoldMulti(AZ::Vector2 pos, unsigned int score, EHitScore hitscore, bool wrong);
		void UpdateNoteHit();

	public:
		void SetHitScore(EHitScore hs, bool wrong); //using wrong to lower vocal volume

	public: // CAnimNode has these.
		virtual void OnStart();
		virtual void OnReset();
		virtual void OnResetHard();
		virtual void OnPause();
		virtual void OnResume();
		virtual void OnStop();
		virtual void OnLoop();

	public:
		virtual void Animate(SAnimContext & ec) override;
		virtual void Render() override;
		virtual bool NeedToRender() const override { return true; }

	public: // Inherited via ITrackEventListener
		//void TriggerTrackEvent(const char* event, const char* param) {}
		void OnTrackEvent(IAnimSequence* pSequence, int reason, const char* event, void* pUserData);

	public:
		// Inherited via IAnimNode
		virtual void SetName(const char * name) override {}
		virtual const char * GetName() override { return "DivaAnimationNode"; }
		virtual EAnimNodeType GetType() const override { return EAnimNodeType(); }
		virtual IAnimSequence * GetSequence() override;
		virtual void SetSequence(IAnimSequence * sequence) override;
		virtual void Activate(bool bActivate) override {}
		virtual void SetFlags(int flags) override {}
		virtual int GetFlags() const override { return 0; }
		virtual bool AreFlagsSetOnNodeOrAnyParent(EAnimNodeFlags flagsToCheck) const override { return false; }
		virtual void SetEntityGuid(const EntityGUID & guid) override {}
		virtual EntityGUID * GetEntityGuid() override { return nullptr; }
		virtual IEntity * GetEntity() override { return nullptr; }
		virtual void SetAzEntityId(const AZ::EntityId & id) override {}
		virtual AZ::EntityId GetAzEntityId() override { return AZ::EntityId(); }
		virtual void SetEntityGuidTarget(const EntityGUID & guid) override {}
		virtual void SetEntityGuidSource(const EntityGUID & guid) override {}
		virtual IMovieSystem * GetMovieSystem() override { return gEnv->pMovieSystem; }
		virtual void SetPos(float time, const Vec3 & pos) override {}
		virtual void SetRotate(float time, const Quat & quat) override {}
		virtual void SetScale(float time, const Vec3 & scale) override {}
		virtual Vec3 GetPos() override { return Vec3(0, 0, 0); }
		virtual Quat GetRotate() override { return Quat(0, 0, 0, 0); }
		virtual Vec3 GetScale() override { return Vec3(0, 0, 0); }
		virtual bool SetParamValue(float time, CAnimParamType param, float value) override { return false; }
		virtual bool SetParamValue(float time, CAnimParamType param, const Vec3 & value) override { return false; }
		virtual bool SetParamValue(float time, CAnimParamType param, const Vec4 & value) override { return false; }
		virtual bool GetParamValue(float time, CAnimParamType param, float & value) override { return false; }
		virtual bool GetParamValue(float time, CAnimParamType param, Vec3 & value) override { return false; }
		virtual bool GetParamValue(float time, CAnimParamType param, Vec4 & value) override { return false; }
		virtual void StillUpdate() override {}
		virtual unsigned int GetParamCount() const override { return 0; }
		virtual CAnimParamType GetParamType(unsigned int nIndex) const override { return CAnimParamType(); }
		virtual bool IsParamValid(const CAnimParamType & paramType) const override { return false; }
		virtual const char * GetParamName(const CAnimParamType & paramType) const override { return nullptr; }
		virtual EAnimValue GetParamValueType(const CAnimParamType & paramType) const override { return EAnimValue(); }
		virtual ESupportedParamFlags GetParamFlags(const CAnimParamType & paramType) const override { return ESupportedParamFlags(); }
		virtual int GetTrackCount() const override { return 0; }
		virtual IAnimTrack * GetTrackByIndex(int nIndex) const override { return nullptr; }
		virtual IAnimTrack * GetTrackForParameter(const CAnimParamType & paramType) const override { return nullptr; }
		virtual IAnimTrack * GetTrackForParameter(const CAnimParamType & paramType, uint32 index) const override { return nullptr; }
		virtual uint32 GetTrackParamIndex(const IAnimTrack * pTrack) const override { return 0; }
		virtual IAnimTrack * CreateTrack(const CAnimParamType & paramType) override { return nullptr; }
		virtual void InitializeTrackDefaultValue(IAnimTrack * pTrack, const CAnimParamType & paramType) override {}
		virtual void SetTrack(const CAnimParamType & paramType, IAnimTrack * track) override {}
		virtual void SetTimeRange(Range timeRange) override {}
		virtual void AddTrack(IAnimTrack * pTrack) override {}
		virtual bool RemoveTrack(IAnimTrack * pTrack) override { return false; }
		virtual void CreateDefaultTracks() override {}
		virtual void SetNodeOwner(IAnimNodeOwner * pOwner) override;
		virtual IAnimNodeOwner * GetNodeOwner() override;
		virtual void Serialize(XmlNodeRef & xmlNode, bool bLoading, bool bLoadEmptyTracks) override {}
		virtual void SerializeAnims(XmlNodeRef & xmlNode, bool bLoading, bool bLoadEmptyTracks) override {}
		virtual void SetParent(IAnimNode * pParent) override;
		virtual IAnimNode * GetParent() const override;
		virtual IAnimNode * HasDirectorAsParent() const override;
		virtual void GetMemoryUsage(ICrySizer * pSizer) const override;
		virtual void UpdateDynamicParams() override {}

	private:
		IAnimSequence* m_pSequence;
		IAnimNodeOwner* m_pOwner;
		IAnimNode* m_pParentNode;
	private:
		unsigned int m_renderRangeStart, m_renderRangeEnd; //the render range we need to render.
		AZStd::vector<DivaNoteBaseNode*> m_NoteNodes;
		unsigned int m_hitNote;
	private:
		ResourceCollection * m_pRC;
		AZStd::vector<ParticleBase*> m_Particles;
	private:
		LyricsFile *m_lyrics;
	private:
		//PortAudioSystem *m_pPASystem;
		struct {
			long long m_idVocal, m_idMelody, m_idSong;
			bool m_wrongVol;
			bool m_type; //true - vocal/melody, false - song
			AlternativeAudio::IAudioSource *m_audioVocal, *m_audioMelody, *m_audioSong;
		} m_Music;
	private:
		IAnimNode * m_pEvents, * m_pFader;
	};
}

#endif