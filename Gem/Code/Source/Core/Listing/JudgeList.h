#ifndef _H_JUDGELIST
#define _H_JUDGELIST

#pragma once
namespace OpenDiva {
	class JudgeList {
	public:
		static void Refresh();
	private:
		static AZStd::mutex m_mutex;
	};
}

#endif