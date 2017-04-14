#include <StdAfx.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Util.h"

namespace LYGame {

	//because /WX is fucking with us...
	#ifdef WX_DISABLE
		#pragma warning(disable : 4244)
	#endif

	//could be slow so better to use the ones with defined parameters than this one.
	Vec2 Util::BCurve(float time, Vec2 * p, int numPoints) {
		Vec2 point = (factorial(numPoints) / factorial(0))*factorial(numPoints - 0)*pow(time, 0)*pow((1 - time), (numPoints - 0))*p[0];
		for (int i = 1; i < numPoints; i++)
			point += (factorial(numPoints) / factorial(i))*factorial(numPoints - i)*pow(time, i)*pow((1 - time), (numPoints - i))*p[i];

		return point;
	}

	Vec2 Util::BCurvePerpPoint(float time, Vec2 p1, Vec2 p2, Vec2 p3, float distance) {
		Vec2 bCurve = BCurve(time, p1, p2, p3);  //Bx,By (B)

		if (distance == 0) return bCurve;

		Vec2 Derivative = BCurveDeriv(time, p1, p2, p3); //Bdx, Bdy (Bd)

		float DerivSlope = 0, PerpDerivSlope = 0;
		bool DerivSlopeInf = false, PerpDerivSlopeInf = false, DerivSlopeUndef = false, PerpDerivSlopeUndef = false;

		//make sure we're not dividing by zero.
		if (Derivative.x == 0 && Derivative.y == 0) {
			DerivSlopeUndef = PerpDerivSlopeUndef = true;
		} else if (Derivative.x == 0) DerivSlopeInf = true;
		else if (Derivative.y == 0) PerpDerivSlopeInf = true;
		else {
			DerivSlope = Derivative.y / Derivative.x; //md = Bdy/Bdx
			PerpDerivSlope = Derivative.x / (-Derivative.y); //mpd = Bdx/-Bdy
		}

		float Dx = bCurve.x, Dy = bCurve.y;

		//past vertical md <  mpd //swapped
		//past horizontal md <  mpd //is same as normal
		//normal md > mpd

		if (!DerivSlopeInf && !PerpDerivSlopeInf && !DerivSlopeUndef && !PerpDerivSlopeUndef) {
			if (DerivSlope == 0) { //Special Case - if the slope is 0;
				Dy += distance;
			} else if (DerivSlope < PerpDerivSlope) { //special case.
				Dx += ((-distance) / sqrt(1 + square(PerpDerivSlope)));
				Dy += (((-distance)*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
			} else {
				//B+D/sqrt(1+mpd^2)
				Dx += (distance / sqrt(1 + square(PerpDerivSlope)));
				Dy += ((distance*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
			}
		}

		//todo: special cases.
		//Special Case 1 - slope is past vertical
		//Special Case 2 - slope is past horizontal

		return Vec2(Dx, Dy);
	}

	Vec2* Util::BCurvePerp2Point(float time, Vec2 p1, Vec2 p2, Vec2 p3, float distance) {
		Vec2 bCurve = BCurve(time, p1, p2, p3);  //Bx,By (B)

		if (distance == 0) {
			static Vec2 ret[2];
			ret[0] = ret[1] = bCurve;
			return ret;
		}

		Vec2 Derivative = BCurveDeriv(time, p1, p2, p3); //Bdx, Bdy (Bd)

		float DerivSlope = 0, PerpDerivSlope = 0;
		bool DerivSlopeInf = false, PerpDerivSlopeInf = false, DerivSlopeUndef = false, PerpDerivSlopeUndef = false;

		//make sure we're not dividing by zero.
		if (Derivative.x == 0 && Derivative.y == 0) {
			DerivSlopeUndef = PerpDerivSlopeUndef = true;
		} else if (Derivative.x == 0) DerivSlopeInf = true;
		else if (Derivative.y == 0) PerpDerivSlopeInf = true;
		else {
			DerivSlope = Derivative.y / Derivative.x; //md = Bdy/Bdx
			PerpDerivSlope = Derivative.x / (-Derivative.y); //mpd = Bdx/-Bdy
		}

		float Dx1 = bCurve.x, Dy1 = bCurve.y;
		float Dx2 = bCurve.x, Dy2 = bCurve.y;

		//past vertical md <  mpd //swapped
		//past horizontal md <  mpd //is same as normal
		//normal md > mpd

		if (!DerivSlopeInf && !PerpDerivSlopeInf && !DerivSlopeUndef && !PerpDerivSlopeUndef && distance != 0) {
			if (DerivSlope == 0) { //Special Case - if the slope is 0;
				Dy1 += distance;
				Dy2 += -distance;
			} else if (DerivSlope < PerpDerivSlope) { //special case.
				Dx1 += ((-distance) / sqrt(1 + square(PerpDerivSlope)));
				Dy1 += (((-distance)*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
				Dx2 += (distance / sqrt(1 + square(PerpDerivSlope)));
				Dy2 += ((distance*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
			} else {
				//B+D/sqrt(1+mpd^2)
				Dx1 += (distance / sqrt(1 + square(PerpDerivSlope)));
				Dy1 += ((distance*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
				Dx2 += ((-distance) / sqrt(1 + square(PerpDerivSlope)));
				Dy2 += (((-distance)*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
			}
		}

		static Vec2 ret[2];
		ret[0] = Vec2(Dx1, Dy1);
		ret[1] = Vec2(Dx2, Dy2);

		return ret;
	}

	Vec2* Util::BCurvePerpPoint(float time, Vec2 p1, Vec2 p2, Vec2 p3, float * distance, const int numDists, bool openmp) {
		Vec2 bCurve = BCurve(time, p1, p2, p3);  //Bx,By (B)
		Vec2 Derivative = BCurveDeriv(time, p1, p2, p3); //Bdx, Bdy (Bd)

		float DerivSlope = 0, PerpDerivSlope = 0;
		bool DerivSlopeInf = false, PerpDerivSlopeInf = false, DerivSlopeUndef = false, PerpDerivSlopeUndef = false;

		//make sure we're not dividing by zero.
		if (Derivative.x == 0 && Derivative.y == 0) {
			DerivSlopeUndef = PerpDerivSlopeUndef = true;
		} else if (Derivative.x == 0) DerivSlopeInf = true;
		else if (Derivative.y == 0) PerpDerivSlopeInf = true;
		else {
			DerivSlope = Derivative.y / Derivative.x; //md = Bdy/Bdx
			PerpDerivSlope = Derivative.x / (-Derivative.y); //mpd = Bdx/-Bdy
		}

		static Vec2 * ret = new Vec2[numDists + 1];
		ret[0] = bCurve;

		//past vertical md <  mpd //swapped
		//past horizontal md <  mpd //is same as normal
		//normal md > mpd
		//if (openmp) { //use only for data loading.
		#pragma omp parallel for if(openmp)
		for (int i = 0; i < numDists; i++) {
			//ret[i + 1] = Vec2();
			float Dx = bCurve.x, Dy = bCurve.y;
			if (!DerivSlopeInf && !PerpDerivSlopeInf && !DerivSlopeUndef && !PerpDerivSlopeUndef && distance[i] != 0) {
				if (DerivSlope == 0) { //Special Case - if the slope is 0;
					Dy += distance[i];
				} else if (DerivSlope < PerpDerivSlope) { //special case.
					Dx += ((-distance[i]) / sqrt(1 + square(PerpDerivSlope)));
					Dy += (((-distance[i])*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
				} else {
					//B+D/sqrt(1+mpd^2)
					Dx += (distance[i] / sqrt(1 + square(PerpDerivSlope)));
					Dy += ((distance[i] * PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
				}
			}
			ret[i + 1] = Vec2(Dx, Dy);
		}
		/*} else {
			for (int i = 0; i < numDists; i++) {
			//ret[i + 1] = Vec2();
			float Dx = bCurve.x, Dy = bCurve.y;
			if (!DerivSlopeInf && !PerpDerivSlopeInf && !DerivSlopeUndef && !PerpDerivSlopeUndef && distance[i] != 0) {
			if (DerivSlope == 0) { //Special Case - if the slope is 0;
			Dy += distance[i];
			} else if (DerivSlope < PerpDerivSlope) { //special case.
			Dx += ((-distance[i]) / sqrt(1 + square(PerpDerivSlope)));
			Dy += (((-distance[i])*PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
			} else {
			//B+D/sqrt(1+mpd^2)
			Dx += (distance[i] / sqrt(1 + square(PerpDerivSlope)));
			Dy += ((distance[i] * PerpDerivSlope) / sqrt(1 + square(PerpDerivSlope)));
			}
			}
			ret[i + 1] = Vec2(Dx, Dy);
			}
			}*/

		return ret;
	}

	ColorF Util::HSVToRGB(HSV in, float alpha) {
		/*ColorF out;
		out.fromHSV(in.h, in.s, in.v);
		out.a = alpha;
		return out;*/

		double hh, p, q, t, ff;
		long i;
		ColorF out;

		if (in.s <= 0.0) { // < is bogus, just shuts up warnings
			out.r = in.v;
			out.g = in.v;
			out.b = in.v;
			return out;
		}
		hh = in.h;
		if (hh >= 360.0) hh = 0.0;
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = in.v * (1.0 - in.s);
		q = in.v * (1.0 - (in.s * ff));
		t = in.v * (1.0 - (in.s * (1.0 - ff)));

		switch (i) {
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
		}

		out.a = alpha;

		return out;
	}

	float Util::BCurveLen(Vec2 p1, Vec2 p2, Vec2 p3) {
		Vec2 a, b;
		a.x = p1.x - 2 * p2.x + p3.x;
		a.y = p1.y - 2 * p2.y + p3.y;
		b.x = 2 * p2.x - 2 * p1.x;
		b.y = 2 * p2.y - 2 * p1.y;

		float A = 4 * (a.x*a.x + a.y*a.y);
		float B = 4 * (a.x*b.x + a.y*b.y);
		float C = b.x*b.x + b.y*b.y;

		float Sabc = 2 * sqrt(A + B + C);
		float A_2 = sqrt(A);
		float A_32 = 2 * A*A_2;
		float C_2 = 2 * sqrt(C);
		float BA = B / A_2;

		return (A_32*Sabc +
				A_2*B*(Sabc - C_2) +
				(4 * C*A - B*B)*log((2 * A_2 + BA + Sabc) / (BA + C_2))
				) / (4 * A_32);
	}

	Vec2 Util::PerpPointDist(Vec2 start, Vec2 end, float dist1, float dist2) {
		//make sure that dist1 is between 0 and 1;
		/*if (dist1 <= 0) dist1 = 0;
		if (dist1 >= 1) dist1 = 1;*/

		float m = -1;
		if ((end.x - start.x) != 0) m = ((end.y - start.y) / (end.x - start.x));

		Vec2 d = Vec2((1 - dist1)*start.x + dist1*end.x, (1 - dist1)*start.y + dist1*end.y);
		float mp = -(1 / ((end.y - start.y) / (end.x - start.x)));

		if (m == 0) {
			return Vec2(
				d.x + (dist2 / (sqrt(1 + square(mp)))),
				d.y + dist2
				);
		} else {
			if (d.y < 0) {
				//ret.x = d.x + (dist2 / (sqrt(1 + (mp*mp))));
				//ret.y = d.y + ((dist2*mp) / (sqrt(1 + (mp*mp))));

				return Vec2(
					d.x + (dist2 / (sqrt(1 + square(mp)))),
					d.y + ((dist2*mp) / (sqrt(1 + square(mp))))
					);
			}
			else {
				//ret.x = d.x + ((-dist2) / (sqrt(1 + (mp*mp))));
				//ret.y = d.y + ((-(dist2*mp)) / (sqrt(1 + (mp*mp))));

				return Vec2(
					d.x + ((-dist2) / (sqrt(1 + square(mp)))),
					d.y + ((-(dist2*mp)) / (sqrt(1 + square(mp))))
					);
			}
		}

		return Vec2(0, 0);
	}

	Vec2 * Util::PerpPointDist(Vec2 start, Vec2 end, float * dist1, float * dist2, const int numDists) {
		static Vec2 * ret = new Vec2[numDists];
		//make sure that dist1 is between 0 and 1;

		float m = -1;
		if ((end.x - start.x) != 0) m = ((end.y - start.y) / (end.x - start.x));
		float mp = -(1 / ((end.y - start.y) / (end.x - start.x)));

		for (int i = 0; i < numDists; i++) {
			/*if (dist1[i] <= 0) dist1[i] = 0;
			if (dist1[i] >= 1) dist1[i] = 1;*/

			Vec2 d = Vec2((1 - dist1[i])*start.x + dist1[i] * end.x, (1 - dist1[i])*start.y + dist1[i] * end.y);

			if (m == 0) {
				ret[i] = Vec2(
					d.x + (dist2[i] / (sqrt(1 + square(mp)))),
					d.y + dist2[i]
					);
			}else {
				if (d.y < 0) {
					//ret.x = d.x + (dist2 / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((dist2*mp) / (sqrt(1 + (mp*mp))));

					ret[i] = Vec2(
						d.x + (dist2[i] / (sqrt(1 + square(mp)))),
						d.y + ((dist2[i] * mp) / (sqrt(1 + square(mp))))
						);
				}
				else {
					//ret.x = d.x + ((-dist2) / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((-(dist2*mp)) / (sqrt(1 + (mp*mp))));

					ret[i] = Vec2(
						d.x + ((-dist2[i]) / (sqrt(1 + square(mp)))),
						d.y + ((-(dist2[i] * mp)) / (sqrt(1 + square(mp))))
						);
				}
			}

			ret[i] = Vec2(0, 0);
		}
		return ret;
	}

	Vec2 * Util::PerpPointDist(Vec2 start, Vec2 end, float dist1, float * dist2, const int numDists) {
		static Vec2 * ret = new Vec2[numDists];
		//make sure that dist1 is between 0 and 1;

		float m = -1;
		if ((end.x - start.x) != 0) m = ((end.y - start.y) / (end.x - start.x));
		Vec2 d = Vec2((1 - dist1)*start.x + dist1 * end.x, (1 - dist1)*start.y + dist1 * end.y);
		float mp = -(1 / ((end.y - start.y) / (end.x - start.x)));

		for (int i = 0; i < numDists; i++) {
			/*if (dist1[i] <= 0) dist1[i] = 0;
			if (dist1[i] >= 1) dist1[i] = 1;*/


			if (m == 0) {
				ret[i] = Vec2(
					d.x + (dist2[i] / (sqrt(1 + square(mp)))),
					d.y + dist2[i]
					);
			} else {
				if (d.y < 0) {
					//ret.x = d.x + (dist2 / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((dist2*mp) / (sqrt(1 + (mp*mp))));

					ret[i] = Vec2(
						d.x + (dist2[i] / (sqrt(1 + square(mp)))),
						d.y + ((dist2[i] * mp) / (sqrt(1 + square(mp))))
						);
				}
				else {
					//ret.x = d.x + ((-dist2) / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((-(dist2*mp)) / (sqrt(1 + (mp*mp))));

					ret[i] = Vec2(
						d.x + ((-dist2[i]) / (sqrt(1 + square(mp)))),
						d.y + ((-(dist2[i] * mp)) / (sqrt(1 + square(mp))))
						);
				}
			}
		}
		return ret;
	}

	Vec2 Util::GetEdgePoint(Vec2 point, float angle, float x_min, float x_max, float y_min, float y_max) {
		if (angle > 0.0f) while (angle >= 360.0f) angle -= 360.0f;
		else while (angle <= -360.0f) angle += 360.0f;

		if (angle == 0.0f){
			return Vec2(x_max, point.y);
		}else if (angle == 90.0f) {
			return Vec2(point.x, y_max);
		} else if (angle == 180.0f) {
			return Vec2(x_min, point.y);
		} else if (angle == 270.0f) {
			return Vec2(point.x, y_min);
		} else {
			float thetaR = M_PI * (angle / 180.0f);

			Vec2 circleVec(
				cos(thetaR)+point.x,
				sin(thetaR) + point.y
			);

			float slope = (circleVec.y - point.y) / (circleVec.x - point.x);
			float yintercept = circleVec.y - (slope * (circleVec.x));

			Vec2 vecMax(
				(x_max-yintercept+(y_max-x_max))/slope,
				(slope*x_max)+yintercept
			);
			Vec2 vecMin(
				(x_min-yintercept+(y_min-x_min))/slope,
				(slope*x_min)+yintercept
			);

			Vec2 point1(vecMax.x, y_max);
			Vec2 point2(x_max, vecMax.y);
			Vec2 point3(vecMin.x, y_min);
			Vec2 point4(x_min, vecMin.y);

			if (angle > 0.0f && angle < 90.0f) {
				if (vecMax.x <= vecMax.y) return point1;
				else return point2;
			} else if (angle > 90.0f && angle < 180.0f) {
				if (vecMax.x >= vecMax.y) return point1;
				else return point4;
			} else if (angle > 180.0f && angle < 270.0f) {
				if (vecMin.x <= vecMin.y) return point3;
				else return point4;
			} else if (angle > 270.0f && angle < 360.0f) {
				if (vecMin.x >= vecMin.y) return point2;
				else return point3;
			}
		}

		return Vec2();
	}

	#ifdef WX_DISABLE
		#pragma warning(default : 4244)
	#endif
}