#ifndef _H_COMMONENUMS_
#define _H_COMMONENUMS_
#pragma once

namespace LYGame {
	enum ENoteType {
		eNT_Cross,
		eNT_Circle,
		eNT_Square,
		eNT_Triangle,

		eNT_Left,
		eNT_Right,
		eNT_Up,
		eNT_Down,

		eNT_Star,
		eNT_BigStar,

		eNT_SwipeL,
		eNT_SwipeR,

		eNT_Count
	};

	enum ESwipeEXNoteType {
		eSEXNT_Left,
		eSEXNT_LeftTick,
		eSEXNT_Right,
		eSEXNT_RightTick,
		eSEXNT_Count
	};

	enum ENoteHitType {
		//input
		eNHT_Single,  //single hit note (Project Diva/Arcade)
		//eNHT_Double,  //double hit note (Arcade) //may not use
		//eNHT_Triple,  //triple hit note (Arcade) //may not use
		//eNHT_Quad,    //quadruple hit note (Arcade) //may not use
		eNHT_Hold, //hold note (Project Diva)
        eNHT_Arrow,
		eNHT_Star,
		eNHT_BigStar,
		eNHT_Swipe,
		eNHT_SwipeHold,
        eNHT_Rush,
		//flow control
		eNHT_BPM,
		//eNHT_TECH,
		//eNHT_CHANCE,
		eNHT_INVALID
	};

	enum EHitScore {
		eHS_Cool,
		eHS_Fine,
		eHS_Safe,
		eHS_Sad,
		eHS_Worst,
		eHS_None
	};

	//may not use
	enum ENoteHitHold {
		eNHH_Start, //hold has started
		eNHH_End, //hold has ended
		eNHH_None //there is no hold
	};

	//future expansion
	//may not use
	enum ENoteHitArrow {
		eNHA_Wait, //waiting for second button press
		eNHA_Confirm, //second button press has happened
		eNHA_None //there is no arrow
	};

	enum EEffectType {
		eET_Cross,
		eET_Diffuse,
		eET_RingFlare,
		eET_Loop,
		eET_NoNote,
		eET_Note,
		eET_Ripple,
		eET_Count
	};

	enum EDivaJudgeCompletion {
		eDJC_Perfect,
		eDJC_Excellent,
		eDJC_Great,
		eDJC_Standard,
		eDJC_Cheap
	};
}

#endif //_H_COMMONENUMS_