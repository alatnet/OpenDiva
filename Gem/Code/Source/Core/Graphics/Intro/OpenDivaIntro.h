#pragma once

#ifndef _H_OPENDIVAINTRO
#define _H_OPENDIVAINTRO

#include "IIntro.h"

namespace OpenDiva {
	class OpenDivaIntro : public IIntro {
	public:
		OpenDivaIntro();
		~OpenDivaIntro();
	public:
		void SetState(EIntroState state);
		void Render();
	private:
		AZStd::string m_str;
		IDraw2d::TextOptions m_options;
		AZStd::mutex m_mutex;
	};
}

#endif