#ifndef _H_DIVANOTEBASENODE
#define _H_DIVANOTEBASENODE

#pragma once

#define BIEZER_TACKON 0.01f //both time and percentage, so a 0.05 is 0.05 seconds added to the time with a 5% additional overhang.

#include "../../Files/NoteFileEntries.h"
#include "../../Input/InputSystem.h"
#include "../../Graphics/OD_Draw2d.h"
#include "../../Graphics/Resources/ResourceCollection.h"

#include "../../Judge/IDivaJudge.h"

#include "DivaNoteNodeFactory.h"

namespace OpenDiva {
	//struct IDivaNodeCallback;
	//class DivaNode;

	//because /WX is fucking with us...
	//#ifdef WX_DISABLE
	//	#pragma warning(disable : 4264)
	//#endif

	WX_DISABLE_(4264)

	struct ENoteTrack {
		EffectImage * img; //effect image
		IAnimTrack * posTrack; //position track
		IAnimTrack * sroTrack; //scale, rotation, and opacity track
		AZ::Vector2 pos;
		AZ::Vector3 sro;

		ENoteTrack() : img(nullptr), posTrack(nullptr), sroTrack(nullptr), pos( 0,0 ), sro( 0,0,0 ) {}

		void GetMemoryUsage(ICrySizer *pSizer) const {
			pSizer->AddObject(this, sizeof(*this));
			pSizer->AddObject(posTrack);
			pSizer->AddObject(sroTrack);
		}
	};

	class DivaNoteBaseNode : public IInputSystemListener, public _i_reference_target_t {
	public:
		DivaNoteBaseNode(ResourceCollection *rc);
		virtual ~DivaNoteBaseNode();
	public:
		virtual void Init(NoteEntryBPM bpm) = 0;
		virtual void Reset() = 0;
		Range GetTimeRange() { return this->m_timeRange; }
	public:
		virtual void CreateDefaultTracks();
		virtual void ConfigureEffects(float start, float end);
	public:
		virtual const bool NeedToRender() { return true; }
		virtual const bool NeedToRender(float time) = 0;
		virtual void Animate(SAnimContext &ec);
		virtual void Render() = 0;
	public:
		//void setJudge(IDivaJudge *judge) { this->m_pDivaJudge = judge; } //switch to ebus
		//void setDivaSeqEff(IDivaSequenceEffects * DivaSeqEff) { this->m_pDivaSeqEff = DivaSeqEff; }
	public: //input
		virtual void OnCross(LyInputEventType mode, float value) = 0;
		virtual void OnCircle(LyInputEventType mode, float value) = 0;
		virtual void OnSquare(LyInputEventType mode, float value) = 0;
		virtual void OnTriangle(LyInputEventType mode, float value) = 0;

		virtual void OnLeft(LyInputEventType mode, float value) = 0;
		virtual void OnRight(LyInputEventType mode, float value) = 0;
		virtual void OnUp(LyInputEventType mode, float value) = 0;
		virtual void OnDown(LyInputEventType mode, float value) = 0;

		virtual void OnSwipeL(LyInputEventType mode, float value) = 0;
		virtual void OnSwipeR(LyInputEventType mode, float value) = 0;

		virtual void OnStar(LyInputEventType mode, float value) = 0;
	public: //graphics
		void DrawNoteE(AZ::Vector2 pos, ENoteType type);
	public:
		virtual void GetMemoryUsage(ICrySizer *pSizer) const;
	protected: //graphics
		ResourceCollection * m_pRC;
		//static IDraw2d * iDraw2d;
	private: //animation
		IAnimTrack *m_ptimeArrowTrack;//, //time arrow rotation
		float m_timeArrow = 0.0f;// , m_noteScale = 1.0f;
		AZStd::vector<ENoteTrack *> m_pENoteTracks;
	protected:
		Range m_timeRange;
	protected: //judge
		//IDivaJudge *m_pDivaJudge;
		//IDivaSequenceEffects * m_pDivaSeqEff;
		void pushEffects(IDivaJudgeParams params);
	};

	//#ifdef WX_DISABLE
	//	#pragma warning(default : 4264)
	//#endif

	WX_ENABLE_(4264)
}

#endif