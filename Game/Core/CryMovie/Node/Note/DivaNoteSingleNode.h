#ifndef _H_DIVANOTESINGLENODE
#define _H_DIVANOTESINGLENODE

#pragma once

namespace LYGame {
	//struct IDivaNodeCallback;
	//class DivaNoteBaseNode;

	class DivaNoteSingleNode : public DivaNoteBaseNode {
	REGISTER_DIVA_NOTE_NODE("note", DivaNoteSingleNode, NoteEntrySingle)
	public:
		DivaNoteSingleNode(ResourceCollection *rc, IDivaJudge *divaJudge, NoteEntrySingle* entry/*, NoteEntryBPM bpm*/);
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
		void OnCross(int mode, float value);
		void OnCircle(int mode, float value);
		void OnSquare(int mode, float value);
		void OnTriangle(int mode, float value);

		void OnLeft(int mode, float value);
		void OnRight(int mode, float value);
		void OnUp(int mode, float value);
		void OnDown(int mode, float value);
		
		void OnSwipeL(int mode, float value);
		void OnSwipeR(int mode, float value);

		void OnStar(int mode, float value);
	public:
		void GetMemoryUsage(ICrySizer *pSizer) const;
	private:
		NoteEntrySingle *m_NoteEntry;
		Vec2 m_posStart, m_posCtrl, m_currPos;
		IAnimTrack *m_pBeizerTrack;
		float m_bTime;
		//Range m_timeRange;
		bool m_renderOut;
		bool m_noteHit;
	};
}

#endif