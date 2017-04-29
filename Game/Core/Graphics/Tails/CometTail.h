#ifndef _H_COMETTAIL_
#define _H_COMETTAIL_
#pragma once
#include "../../Graphics/OD_Draw2d.h"

namespace LYGame {
	#define COMET_SIZE 1 //width of the comet tail.
	#define RCOMET_SIZE 4 //width of the rainbow comet tail.
	#define COMET_SEP 7 //distance between two comet tails.
	#define COMET_ROT 15 //from 0-360, how much step inbetween iterations.
	#define RCOMET_SHIFT 15 //how fast to shift through the colors

	class CometTail {
	public:
		CometTail(ColorF color) : m_PackCol(color.pack_argb8888()), m_Color(color), m_size(1.0f), m_scale(1.0f,1.0f) {}
		~CometTail() {}
	public:
		//bcurve
		void Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		void Draw(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, int numPoints = 20, bool isolatedDraw = false); //automatically creates stepping

		//bcurve
		static void DrawRainbow(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		static void DrawRainbow(Vec2 p1, Vec2 p2, Vec2 p3, float tStart, float tEnd, int numPoints = 20, bool isolatedDraw = false); //automatically creates stepping

		//line
		void Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		void Draw(Vec2 p1, Vec2 p2, float tStart, float tEnd, int numPoints = 20, bool isolatedDraw = false); //automatically creates stepping

		//line
		static void DrawRainbow(Vec2 p1, Vec2 p2, float tStart, float tEnd, float step = 0.05f, bool isolatedDraw = false, bool pointsSpecified = false);
		static void DrawRainbow(Vec2 p1, Vec2 p2, float tStart, float tEnd, int numPoints = 20, bool isolatedDraw = false); //automatically creates stepping

	public:
		void setSize(float size = 1.0f) { this->m_size = size; }
		static void setRSize(float size = 1.0f) { CometTail::m_rsize = size; }

		float getSize() { return this->m_size; }
		static float getRSize() { return CometTail::m_rsize; }

		void setScale(Vec2 scale = Vec2(1.0f, 1.0f)) { this->m_scale = scale; }
		static void setRScale(Vec2 scale = Vec2(1.0f, 1.0f)) { CometTail::m_rscale = scale; }
	public:
		void SetColor(ColorF color) { this->m_PackCol = color.pack_argb8888(); this->m_Color = color; }
	private:
		uint32 m_PackCol;
		ColorF m_Color;

		float m_size;
		static float m_rsize;

		Vec2 m_scale;
		static Vec2 m_rscale;
	};
}

#endif

/*
5%		95%
fade
here   solid color
+----+-----------+
|    |           |
+----+-----------+
*/

/*
Trail from F2nd seems to be one straight and one that circles the straight one.
---\          /-----\         /-------
----\--------/-------\-------/--------
\------/         \-----/
so with this in mind proabably just use a sin/cos wave based on distance and position?

High ho! High ho! It's Beizer Curve Derivative we go!

DrawQuad is only 4 points...
1   2
+---+
|   |
+---+
3   4

SUCCESS! figured out how to do a line strip.  can now figure out how to do a triangle strip based on indicies.
that'll speed up rendering and also allow for a host of other stuff.

Triangle Strip, how it works:
1    4    6    8
+----+----+----+
|\ 2 |\ 4 |\ 6 |
| \  | \  | \  |
|  \ |  \ |  \ |
| 1 \| 3 \| 5 \|
+----+----+----+
2    3    5    7

Indices: 1,2,3,4,5,6,7,8

comet tail design
variable               variable
|   variable          |
|   |   |-Precalc-|   |

+---+---+---------+---+
|   |   |         |   |
+---+---+---------+---+
| ^ |---solid color---|
| |
| auto fades
white color with alpha fade

rainbow comet tail can be precalculated. ending and starting variable is going to be annoying to deal with though.

Hold note trail design.
Project Diva:
variable          variable
|   |-Precalc-|   |

+---+---------+---+
|   |  white  |   | <- 2 or 5 px width?
+---+---------+---+
|   |  black  |   | <- 2 or 5 px width?
+---+---------+---+
|   |  color  |   |
+---+---------+---+
|   |light col|   |
+---+---------+---+
|   |  color  |   |
+---+---------+---+
|   |  black  |   | <- 2 or 5 px width?
+---+---------+---+
|   |  white  |   | <- 2 or 5 px width?
+---+---------+---+

alt version:
might be better to use this for a more visual appeal.
variable          variable
|   |-Precalc-|   |

+---+----------+---+
|   |   white  |   | <- 2 or 5 px width?
+---+----------+---+
|   |   black  |   | <- 2 or 5 px width?
+---+----------+---+ <- color at this position
|   | gradient |   | <- 10 or 15 px width?
+---+----------+---+ <- light color at this position
|   | gradient |   | <- 10 or 15 px width?
+---+----------+---+ <- color at this position
|   |   black  |   | <- 2 or 5 px width?
+---+----------+---+
|   |   white  |   | <- 2 or 5 px width?
+---+----------+---+

alt version 2:
personal design for opendiva
variable          variable
|   |-Precalc-|   |

+---+----------+---+ <- white at this position
|   | gradient |   |
+---+----------+---+ <- color/black at this position
|   | gradient |   |
+---+----------+---+ <- light color at this position
|   | gradient |   |
+---+----------+---+ <- color/black at this position
|   | gradient |   |
+---+----------+---+ <- white at this position

use an image instead for hold notes? would have to calculate the texture coords...

Lightning hold notes for arcade version (2 or more notes): http://gamedevelopment.tutsplus.com/tutorials/how-to-generate-shockingly-good-2d-lightning-effects--gamedev-2681
*/