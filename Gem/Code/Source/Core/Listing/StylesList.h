#ifndef _H_STYLESLIST
#define _H_STYLESLIST

#pragma once
namespace OpenDiva {
	class StylesList {
	public:
		static void Refresh();
	private:
		static AZStd::mutex m_mutex;
	};
}

#endif