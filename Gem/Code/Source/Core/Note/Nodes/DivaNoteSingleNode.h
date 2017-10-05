#ifndef _H_DIVANOTESINGLENODE
#define _H_DIVANOTESINGLENODE

#pragma once

namespace OpenDiva {
	//struct IDivaNodeCallback;
	//class DivaNoteBaseNode;

	class DivaNoteSingleNode : public DivaNoteBaseNode {
	REGISTER_DIVA_NOTE_NODE("note", DivaNoteSingleNode, NoteEntrySingle)
	public:
		DivaNoteSingleNode(ResourceCollection *rc, NoteEntrySingle* entry/*, NoteEntryBPM bpm*/);
		~DivaNoteSingleNode();
	public:
		void Init(NoteEntryBPM bpm);
		void Reset();
		//Range GetTimeRange() { return this->m_timeRange; }
	public:
		void CreateDefaultTracks();
	public:
		const bool NeedToRender() { return true; }
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
	public:
		void GetMemoryUsage(ICrySizer *pSizer) const;
	private:
		NoteEntrySingle *m_NoteEntry;
		AZ::Vector2 m_posStart, m_posCtrl, m_currPos;
		IAnimTrack *m_pBeizerTrack;
		float m_bTime;
		//Range m_timeRange;
		bool m_renderOut;
		bool m_noteHit;
	};
}

#endif