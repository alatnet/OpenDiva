#ifndef _H_DIVANOTEHOLDNODE
#define _H_DIVANOTEHOLDNODE

#pragma once

namespace LYGame {
	//struct IDivaNodeCallback;
	//class DivaNoteBaseNode;

	class DivaNoteHoldNode : public DivaNoteBaseNode {
	REGISTER_DIVA_NOTE_NODE("hold", DivaNoteHoldNode, NoteEntryHold)
	public:
		DivaNoteHoldNode(ResourceCollection *rc, NoteEntryHold* entry/*, NoteEntryBPM bpm*/);
		~DivaNoteHoldNode();
	public:
		void Init(NoteEntryBPM bpm);
		void Reset();
		//Range GetTimeRange() { return this->m_timeRange; }
	public:
		void CreateDefaultTracks();
		void ConfigureEffects(float start1, float start2, float end1, float end2);
	public:
		const bool NeedToRender();
		const bool NeedToRender(float time);
		void Animate(SAnimContext &ec);
		void Render();
	public: //Input
		void OnCross(LyInputEventType mode, float value);
		void OnCircle(LyInputEventType mode, float value);
		void OnSquare(LyInputEventType mode, float value);
		void OnTriangle(LyInputEventType mode, float value);

		void OnLeft(LyInputEventType mode, float value);
		void OnRight(LyInputEventType mode, float value);
		void OnUp(LyInputEventType mode, float value);
		void OnDown(LyInputEventType mode, float value);

		void OnSwipeL(LyInputEventType mode, float value);
		void OnSwipeR(LyInputEventType mode, float value);

		void OnStar(LyInputEventType mode, float value);
	private:
		void DrawNoteE2(AZ::Vector2 pos, ENoteType type);
	public:
		void GetMemoryUsage(ICrySizer *pSizer) const;
	private:
		NoteEntryHold* m_NoteEntry;
	private:
		IAnimTrack *m_ptimeArrowTrack2;// , //time arrow rotation
				   //*m_pNoteEScaleTrack2, //ending note scale effect
				   //*m_pRippleTrack2; //ending note ripple effect
		float m_timeArrow2 = 0.0f;// , m_noteScale2 = 1.0f;
		//Vec3 m_Ripple2;

		AZStd::vector<ENoteTrack *> m_pENoteTracks2;
		Range m_eNote2Range;
		bool m_drawENote2;
	private:
		AZ::Vector2 m_posStart, m_posCtrl, m_currPos1, m_currPos2;
		IAnimTrack *m_pBeizerTrack1, *m_pBeizerTrack2;
		float m_bTime1, m_bTime2;
		//Range m_timeRange;
		bool m_renderOut;
		unsigned int m_holdMult;
		float m_holdTick;
	private:
		bool m_faceHeld, m_arrowHeld;
		bool m_holdEnabled;
	private:
		bool m_hitscoreWrong;
		EHitScore m_hitScore;
	};
}

#endif