#ifndef _H_IDIVASEQUENCE
#define _H_IDIVASEQUENCE

#pragma once

#include "../../../Graphics/Particles.h"

namespace LYGame {
	/*struct IDivaSequence {
		virtual void pushbackRating(Vec2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong) = 0;
		virtual void pushbackEffect(Vec2 pos, EEffectList eff) = 0;
		virtual void pushbackHoldMulti(Vec2 pos, unsigned int score, EHitScore hitscore, bool wrong) = 0;
		virtual void updateNoteHit() = 0;
	};*/

	struct IDivaSequenceJudge {
		virtual void updateNoteHit() = 0;
	};

	struct IDivaSequenceEffects {
		virtual void pushbackRating(Vec2 pos, ENoteType nType, EHitScore hitscore, unsigned int combo, bool wrong) = 0;
		virtual void pushbackEffect(Vec2 pos, EEffectList eff) = 0;
		virtual void pushbackHoldMulti(Vec2 pos, unsigned int score, EHitScore hitscore, bool wrong) = 0;
	};
}

#endif