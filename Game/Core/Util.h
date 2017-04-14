#ifndef _H_UTIL_
#define _H_UTIL_
#pragma once

namespace LYGame {

	//Class is used for misc static functions that can be used everywhere or as a dumping ground for functions that dont fit anywhere else.
	class Util {
	public: //Beizer Curve Functions
		static Vec2 BCurve(float time, Vec2 p1, Vec2 p2) { //Linear Beizer Curve, or a straight line.
			return Vec2(
				(1 - time)*p1.x + time*p2.x,
				(1 - time)*p1.y + time*p2.y
			);
		}
		static Vec2 BCurve(float time, Vec2 p1, Vec2 p2, Vec2 p3) { //Quadratic Beizer Curve
			return Vec2(
				square(1 - time)*p1.x + 2 * (1 - time)*time*p2.x + square(time)*p3.x,
				square(1 - time)*p1.y + 2 * (1 - time)*time*p2.y + square(time)*p3.y
			);
		}
		static Vec2 BCurve(float time, Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4) { //Cubic Beizer Curve
			return Vec2(
				cube(1 - time)*p1.x + 3 * square(1 - time)*time*p2.x + 3 * (1 - time)*square(time)*p3.x + cube(time)*p4.x,
				cube(1 - time)*p1.y + 3 * square(1 - time)*time*p2.y + 3 * (1 - time)*square(time)*p3.y + cube(time)*p4.y
			);
		}
		static Vec2 BCurve(float time, Vec2 * p, int numPoints); //doesnt work correctly... XP

		/*
		Quadratic Beizer Curve Equations.
		https://www.desmos.com/calculator/tgvdbsbldc
		*/
	public: //Beizer Curve Derivative Functions
		//Quadratic Beizer Curve Derivative
		static Vec2 BCurveDeriv(float time, Vec2 p1, Vec2 p2, Vec2 p3) {
			return Vec2(
				2 * ((1 - time)*(p2.x - p1.x) + time*(p3.x - p2.x)),
				2 * ((1 - time)*(p2.y - p1.y) + time*(p3.y - p2.y))
				);
		}
	public: //Beizer Curve Misc Functions
		//Point on the perpendicular line of the derivative of the beizer curve.
		static Vec2 BCurvePerpPoint(float time, Vec2 p1, Vec2 p2, Vec2 p3, float distance);
		//returns 2 points in opposite directions.
		static Vec2* BCurvePerp2Point(float time, Vec2 p1, Vec2 p2, Vec2 p3, float distance);
		//returns numDists + 1 points.
		//first one is always the beizer curve point.
		//NOTE! usage of openmp is not for multi-threading rendering or something every frame. use only in loading contexts.
		static Vec2* BCurvePerpPoint(float time, Vec2 p1, Vec2 p2, Vec2 p3, float * distance, const int numDists, bool openmp = false);
		/*
		NOTE!
		Cubic Beizer curve will not have a perpendicular point function until the calculations for it are figure out.
		*/

		/*
		Length of a Quadratic Beizer Curve.
		Yoinked from: http://www.malczak.linuxpl.com/blog/quadratic-bezier-curve-length/
		*/
		static float BCurveLen(Vec2 p1, Vec2 p2, Vec2 p3);
	public: //misc math functions
		//recursive factorial
		static int factorial(int n) { return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n; }
		//Point on a Perpendicular Line based on Distance
		//https://www.desmos.com/calculator/uoxp8malu9
		static Vec2 PerpPointDist(Vec2 start, Vec2 end, float dist1 = 0.5f, float dist2 = 1.0f);
		static Vec2 * PerpPointDist(Vec2 start, Vec2 end, float * dist1, float * dist2, const int numDists);
		static Vec2 * PerpPointDist(Vec2 start, Vec2 end, float dist1, float * dist2, const int numDists);
	public: //color functions
		struct HSV {
			float h, s, v;
			HSV() :
				h(0),
				s(1.0f),
				v(1.0f) {
			};
		};
		static ColorF HSVToRGB(HSV in, float alpha = 1.0f);

		/*
		float f;
		uint8_t i;
		//byte to float
		f =CLAMP(((float)((i &0x0000ff))) /255.0, 0.0, 1.0);
		//float to byte
		i =((uint8_t)(255.0f *CLAMP(f, 0.0, 1.0)));
		*/
		static inline uint8 floatToInt(float in) { return ((uint8)(255.0f * CLAMP(in, 0.0, 1.0))); }
		static inline ColorB floatColToIntCol(ColorF in) { return ColorB(floatToInt(in.r), floatToInt(in.g), floatToInt(in.b), floatToInt(in.a)); }
		//static inline uint8 intToFloat(float in) { return CLAMP((((float)(in & 0x0000ff)) / 255.0), 0.0, 1.0); }
	public:
		//template specialization for checking index ranges.
		//use this as a nicer way of explaining things?
		template <class T> static inline bool CheckBounds(T val, T min, T max) { return ((val >= min) && (val <= max)); }
	public:
		//get a point at the edge of a box based on a point and an angle.
		//https://www.desmos.com/calculator/2ysl6hklqg
		static Vec2 GetEdgePoint(Vec2 point, float angle, float x_min, float x_max, float y_min, float y_max);
	};

}

#endif //_H_UTIL_