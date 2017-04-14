#ifndef _H_DSFGEVENTLISTENER
#define _H_DSFGEVENTLISTENER

#pragma once

namespace LYGame {
	//Diva Sequence FlowGraph Event Listener
	//used to notify the flowgraph of zone events
	class DSFGEventListener : public ITrackEventListener {
		void OnTrackEvent(IAnimSequence* pSequence, int reason, const char* event, void* pUserData);
		void GetMemoryUsage(ICrySizer* pSizer) const { pSizer->AddObject(this, sizeof(*this)); };
	};
}

#endif