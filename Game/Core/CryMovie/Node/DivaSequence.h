#ifndef _H_DIVASEQUENCE
#define _H_DIVASEQUENCE

#pragma once

#include <IMovieSystem.h>

#include "../../Judge/IDivaJudge.h"
#include "../../../Input/InputSystem.h"
#include "Note/DivaNoteNodes.h"
#include "../../Files/NoteFile.h"
#include "../../../Graphics/Resources/ResourceCollection.h"
#include "../../../Graphics/Particles.h"
#include "../../CryEngine/CryMovie/AnimSequence.h"
#include "../../Bus/OpenDiva_Bus.h"
#include "IDivaSequence.h"
#include "../../Files/LyricsFile.h"


namespace LYGame {

	WX_DISABLE_(4264)

	class DivaSequenceRedirect;

	class DivaSequence : public CAnimSequence, public IInputSystemListener, public IDivaSequenceEffects, public IDivaSequenceJudge {
	public:
		DivaSequence(ResourceCollection * rc);
		~DivaSequence();
	public:
		bool Init(NoteFile *noteFile, IDivaJudge * judge);
		bool InitLyrics(LyricsFile * lyrics);
	public: //all the stuff we do need.
		void Animate(SAnimContext &ec);
		void GetMemoryUsage(ICrySizer *pSizer) const;
		void Render();
		bool NeedToRender() const { return true; }
		void Reset(bool bSeekToStart);
		void ResetHard();
		//potential use
		void TriggerTrackEvent(const char* event, const char* param = NULL);
	public:
		void pushbackRating(Vec2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong);
		void pushbackEffect(Vec2 pos, EEffectList eff);
		void pushbackHoldMulti(Vec2 pos, unsigned int score, EHitScore hitscore, bool wrong);
		void updateNoteHit();
	private:
		unsigned int m_renderRangeStart, m_renderRangeEnd; //the render range we need to render.
		std::vector<DivaNoteBaseNode*> m_NoteNodes;
		unsigned int m_hitNote;
	private:
		ResourceCollection * m_pRC;
		//std::vector<RatingParticle*> m_RatingParticles;
		//std::vector<EffectParticle*> m_EffectParticles;
		std::vector<ParticleBase*> m_Particles;
		DivaSequenceRedirect * m_DivaSeqRedirect;
	public: //IInputSystemListener
		// only the current note needs to listen for input.
		void OnCross(int mode, float value);
		void OnCircle(int mode, float value);
		void OnSquare(int mode, float value);
		void OnTriangle(int mode, float value);

		void OnLeft(int mode, float value);
		void OnRight(int mode, float value);
		void OnUp(int mode, float value);
		void OnDown(int mode, float value);

		void OnStar(int mode, float value);

		void OnSwipeL(int mode, float value);
		void OnSwipeR(int mode, float value);
	private:
		IAnimNode * m_pZoneEvents;
		IAnimNode * m_pLyricsEvents;
	public: //all the stuff we dont need.
		/*void SetName(const char *name) {}
		const char * GetName() { return "DivaSequence"; }
		EAnimNodeType GetType() { return eAnimNodeType_Group; }
		IAnimSequence* GetSequence() { return nullptr; }
		void Activate(bool bActivate) {}
		void SetFlags(int flags) {}
		int GetFlags() const { return 0; }
		void SetEntityGuid(const EntityGUID &guid) {}
		EntityGUID* GetEntityGuid() { return nullptr; }
		IEntity* GetEntity() { return nullptr; }
		void SetEntityGuidTarget(const EntityGUID &guid) {}
		void SetEntityGuidSource(const EntityGUID &guid) {}
		IMovieSystem* GetMovieSystem() { return gEnv->pMovieSystem; };
		void SetPos(float time, const Vec3 &pos) {}
		void SetRotate(float time, const Quat &quat) {}
		void SetScale(float time, const Vec3 &scale) {}
		Vec3 GetPos() { return {0.0f,0.0f,0.0f}; }
		Quat GetRotate() { return Quat(); }
		Vec3 GetScale() { return {1.0f,1.0f,1.0f}; }
		bool SetParamValue(float time, CAnimParamType param, float value) { return false; }
		bool SetParamValue(float time, CAnimParamType param, const Vec3 &value) { return false; }
		bool SetParamValue(float time, CAnimParamType param, const Vec4 &value) { return false; }
		bool GetParamValue(float time, CAnimParamType param, float &value) { return false; }
		bool GetParamValue(float time, CAnimParamType param, Vec3 &value) { return false; }
		bool GetParamValue(float time, CAnimParamType param, Vec4 &value) { return false; }
		void StillUpdate() {}
		unsigned int GetParamCount() const { return 0; }
		CAnimParamType GetParamType(unsigned int nIndex) const { return CAnimParamType(eAnimParamType_User); }
		bool IsParamValid(const CAnimParamType &paramType) const { return false; }
		const char *GetParamName(const CAnimParamType &paramType) const { return ""; }
		EAnimValue GetParamValueType(const CAnimParamType &paramType) const { return eAnimValue_Unknown; }
		ESupportedParamFlags GetParamFlags(const CAnimParamType &paramType) const { return eSupportedParamFlags_MultipleTracks; }
		int GetTrackCount() const { return 0; }
		IAnimTrack* GetTrackByIndex(int nIndex) const { return nullptr; }
		IAnimTrack* GetTrackForParameter(const CAnimParamType &paramType) const { return nullptr; }
		IAnimTrack* GetTrackForParameter(const CAnimParamType &paramType, uint32 index) const { return nullptr; }
		uint32 GetTrackParamIndex(const IAnimTrack *pTrack) const { return 0; }
		IAnimTrack* CreateTrack(const CAnimParamType &paramType) { return nullptr; }
		void InitializeTrackDefaultValue(IAnimTrack *pTrack, const CAnimParamType &paramType) {}
		void SetTrack(const CAnimParamType &paramType, IAnimTrack *track) {}
		void AddTrack(IAnimTrack *pTrack) {}
		bool RemoveTrack(IAnimTrack *pTrack) { return false; }
		void CreateDefaultTracks() {}
		void SetNodeOwner(IAnimNodeOwner *pOwner) {}
		IAnimNodeOwner* GetNodeOwner() { return nullptr; }
		void Serialize(XmlNodeRef &xmlNode, bool bLoading, bool bLoadEmptyTracks) {}
		void SetParent(IAnimNode* pParent) {}
		IAnimNode* GetParent() { return nullptr; }
		IAnimNode * HasDirectorAsParent() { return nullptr; }
		void UpdateDynamicParams() {}
		void MarkAsModified() {}*/
	};

	WX_ENABLE_(4264)

	/*
	due to the lumberyard's movie system always casting sequences
	as CAnimSequence and that Animate and Render are NOT virtual functions
	this class is to redirect those two functions so that
	DivaSequence's Animate and Render functions are called.
	*/
	class DivaSequenceRedirect : public CAnimNode {
	public:
		DivaSequenceRedirect(DivaSequence *seq) : CAnimNode(-1), m_seq(seq) {}
	public:
		void Animate(SAnimContext &ec) { m_seq->Animate(ec); }
		void Render() { m_seq->Render(); }
	public:
		CAnimParamType GetParamType(unsigned int nIndex) const { return NULL; }
		EAnimNodeType GetType() const { return eAnimNodeType_Comment; }
	private:
		DivaSequence *m_seq;
	};
}

#endif