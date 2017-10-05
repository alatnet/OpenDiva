#pragma once

#ifndef _H_IINTRO
#define _H_IINTRO

namespace OpenDiva {
	enum EIntroState {
		eIS_Section1,
		eIS_Section2,
		eIS_Section3,
		eIS_Section4,
		eIS_Section5,
		eIS_Section6
	};

	class IIntro {
	public:
		virtual void SetState(EIntroState state) = 0;
		virtual void Render() = 0;
	};
}

#endif