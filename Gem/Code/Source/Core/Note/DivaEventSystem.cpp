#include <StdAfx.h>
#include <OpenDivaCommon.h>

#include "DivaEventSystem.h"

#include <AzCore/std/sort.h>

namespace OpenDiva {
	void DivaEventSystem::AddEvent(float time, DivaEventFunc func, void* userdata) {
		//look if there's already a list of events for specified time.
		for (int i=0; i < this->m_events.size(); i++){
			if (this->m_events[i].first == time) { //if there is, add it to the list.
				this->m_events[i].second.push_back({ func, userdata });
				return;
			}
		}

		//otherwise, create a new list of events for that specified time.
		this->m_events.push_back(
			{
				time,
				AZStd::vector<
					AZStd::pair<
						DivaEventFunc,
						void *
					>
				>()
			}
		);

		//and add that event to the list.
		this->m_events.at(this->m_events.size() - 1).second.push_back({ func, userdata });

		//sort the list by time.
		AZStd::sort(
			this->m_events.begin(),
			this->m_events.end(),
			DivaEventSystem::eventSort
		);
	}

	void DivaEventSystem::Update(float currTime) {
		if (currEvent == this->m_events.size()) return; //if we are at the end of the list, do nothing.

		//if the current time is >= to the current event we are tracking.
		while (currTime >= this->m_events[currEvent].first) {
			//for each function, execute it.
			for (auto func : this->m_events[currEvent].second) func.first(func.second);

			currEvent++; //track the next event
			if (currEvent >= this->m_events.size()) { //we are at the end of the list
				currEvent = this->m_events.size();
				break;
			}
		}
	}

	void DivaEventSystem::Clear() {
		for (auto functions : this->m_events) functions.second.clear(); //for each function list, clear it's list.
		this->m_events.clear(); //clear the event list.
		this->m_events.shrink_to_fit(); //free up memory.
		currEvent = 0;
	}
}