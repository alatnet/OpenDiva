#ifndef _H_DIVAEVENTS
#define _H_DIVAEVENTS

#pragma once

namespace OpenDiva {
	typedef AZStd::function<void (void*)> DivaEventFunc;

	class DivaEventSystem {
	public:
		DivaEventSystem() : currEvent(0) {}
		~DivaEventSystem() { this->Clear(); }
	public:
		void AddEvent(float time, DivaEventFunc func, void* userdata);
		void Update(float currTime);
		void Clear();
		void Reset() { currEvent = 0; }

	private:
		AZStd::vector<
			AZStd::pair<
				float, //time [i](first)
				AZStd::vector< //functions [i](second)
					AZStd::pair<
						DivaEventFunc, //function [i](second[i2].first)
						void * //userdata [i](second[i2].second)
					>
				>
			>
		> m_events;

		unsigned long long currEvent;

		static bool eventSort(
			const AZStd::pair<float, AZStd::vector<AZStd::pair<DivaEventFunc, void *>>> a,
			const AZStd::pair<float, AZStd::vector<AZStd::pair<DivaEventFunc, void *>>> b
		) {
			return a.first < b.first; //sort by time.
		}
	};
}

#endif