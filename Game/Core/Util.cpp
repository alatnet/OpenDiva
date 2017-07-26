#include <StdAfx.h>
//#define _USE_MATH_DEFINES
//#include <math.h>

#ifndef M_PI
	#define M_PI 3.14159265358979323846   // pi
#endif

#include "Util.h"

namespace LYGame {

	//because /WX is fucking with us...
	#ifdef WX_DISABLE
		#pragma warning(disable : 4244)
	#endif

	//could be slow so better to use the ones with defined parameters than this one.
	AZ::Vector2 Util::BCurve(float time, AZ::Vector2 * p, int numPoints) {
		AZ::Vector2 point = (factorial(numPoints) / factorial(0))*factorial(numPoints - 0)*pow(time, 0)*pow((1 - time), (numPoints - 0))*p[0];
		for (int i = 1; i < numPoints; i++)
			point += (factorial(numPoints) / factorial(i))*factorial(numPoints - i)*pow(time, i)*pow((1 - time), (numPoints - i))*p[i];

		return point;
	}

	AZ::Vector2 Util::BCurvePerpPoint(float time, AZ::Vector2 p1, AZ::Vector2 p2, AZ::Vector2 p3, float distance) {
		AZ::Vector2 bCurve = BCurve(time, p1, p2, p3);  //Bx,By (B)

		if (distance == 0) return bCurve;

		AZ::Vector2 Derivative = BCurveDeriv(time, p1, p2, p3); //Bdx, Bdy (Bd)

		float DerivSlope = 0, PerpDerivSlope = 0;
		bool DerivSlopeInf = false, PerpDerivSlopeInf = false, DerivSlopeUndef = false, PerpDerivSlopeUndef = false;

		//make sure we're not dividing by zero.
		if (Derivative.GetX() == 0 && Derivative.GetY() == 0) {
			DerivSlopeUndef = PerpDerivSlopeUndef = true;
		} else if (Derivative.GetX() == 0) DerivSlopeInf = true;
		else if (Derivative.GetY() == 0) PerpDerivSlopeInf = true;
		else {
			DerivSlope = Derivative.GetY() / Derivative.GetX(); //md = Bdy/Bdx
			PerpDerivSlope = Derivative.GetX() / (-Derivative.GetY()); //mpd = Bdx/-Bdy
		}

		float Dx = bCurve.GetX(), Dy = bCurve.GetY();

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

		return AZ::Vector2(Dx, Dy);
	}

	AZ::Vector2* Util::BCurvePerp2Point(float time, AZ::Vector2 p1, AZ::Vector2 p2, AZ::Vector2 p3, float distance) {
		AZ::Vector2 bCurve = BCurve(time, p1, p2, p3);  //Bx,By (B)

		if (distance == 0) {
			static AZ::Vector2 ret[2];
			ret[0] = ret[1] = bCurve;
			return ret;
		}

		AZ::Vector2 Derivative = BCurveDeriv(time, p1, p2, p3); //Bdx, Bdy (Bd)

		float DerivSlope = 0, PerpDerivSlope = 0;
		bool DerivSlopeInf = false, PerpDerivSlopeInf = false, DerivSlopeUndef = false, PerpDerivSlopeUndef = false;

		//make sure we're not dividing by zero.
		if (Derivative.GetX() == 0 && Derivative.GetY() == 0) {
			DerivSlopeUndef = PerpDerivSlopeUndef = true;
		} else if (Derivative.GetX() == 0) DerivSlopeInf = true;
		else if (Derivative.GetY() == 0) PerpDerivSlopeInf = true;
		else {
			DerivSlope = Derivative.GetY() / Derivative.GetX(); //md = Bdy/Bdx
			PerpDerivSlope = Derivative.GetX() / (-Derivative.GetY()); //mpd = Bdx/-Bdy
		}

		float Dx1 = bCurve.GetX(), Dy1 = bCurve.GetY();
		float Dx2 = bCurve.GetX(), Dy2 = bCurve.GetY();

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

		static AZ::Vector2 ret[2];
		ret[0] = AZ::Vector2(Dx1, Dy1);
		ret[1] = AZ::Vector2(Dx2, Dy2);

		return ret;
	}

	AZ::Vector2* Util::BCurvePerpPoint(float time, AZ::Vector2 p1, AZ::Vector2 p2, AZ::Vector2 p3, float * distance, const int numDists, bool openmp) {
		AZ::Vector2 bCurve = BCurve(time, p1, p2, p3);  //Bx,By (B)
		AZ::Vector2 Derivative = BCurveDeriv(time, p1, p2, p3); //Bdx, Bdy (Bd)

		float DerivSlope = 0, PerpDerivSlope = 0;
		bool DerivSlopeInf = false, PerpDerivSlopeInf = false, DerivSlopeUndef = false, PerpDerivSlopeUndef = false;

		//make sure we're not dividing by zero.
		if (Derivative.GetX() == 0 && Derivative.GetY() == 0) {
			DerivSlopeUndef = PerpDerivSlopeUndef = true;
		} else if (Derivative.GetX() == 0) DerivSlopeInf = true;
		else if (Derivative.GetY() == 0) PerpDerivSlopeInf = true;
		else {
			DerivSlope = Derivative.GetY() / Derivative.GetX(); //md = Bdy/Bdx
			PerpDerivSlope = Derivative.GetX() / (-Derivative.GetY()); //mpd = Bdx/-Bdy
		}

		static AZ::Vector2 * ret = new AZ::Vector2[numDists + 1];
		ret[0] = bCurve;

		//past vertical md <  mpd //swapped
		//past horizontal md <  mpd //is same as normal
		//normal md > mpd
		//if (openmp) { //use only for data loading.
		#pragma omp parallel for if(openmp)
		for (int i = 0; i < numDists; i++) {
			//ret[i + 1] = Vec2();
			float Dx = bCurve.GetX(), Dy = bCurve.GetY();
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
			ret[i + 1] = AZ::Vector2(Dx, Dy);
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

	float Util::BCurveLen(AZ::Vector2 p1, AZ::Vector2 p2, AZ::Vector2 p3) {
		AZ::Vector2 a, b;
		a = p1 - 2 * p2 + p3;
		b = 2 * p2 - 2 * p1;

		float A = 4 * (a.GetX()*a.GetX() + a.GetY()*a.GetY());
		float B = 4 * (a.GetX()*b.GetX() + a.GetY()*b.GetY());
		float C = b.GetX()*b.GetX() + b.GetY()*b.GetY();

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

	AZ::Vector2 Util::PerpPointDist(AZ::Vector2 start, AZ::Vector2 end, float dist1, float dist2) {
		//make sure that dist1 is between 0 and 1;
		/*if (dist1 <= 0) dist1 = 0;
		if (dist1 >= 1) dist1 = 1;*/

		float m = -1;
		if ((end.GetX() - start.GetX()) != 0) m = ((end.GetY() - start.GetY()) / (end.GetX() - start.GetX()));

		AZ::Vector2 d = AZ::Vector2((1 - dist1)*start.GetX() + dist1*end.GetX(), (1 - dist1)*start.GetY() + dist1*end.GetY());
		float mp = -(1 / ((end.GetY() - start.GetY()) / (end.GetX() - start.GetX())));

		if (m == 0) {
			return AZ::Vector2(
				d.GetX() + (dist2 / (sqrt(1 + square(mp)))),
				d.GetY() + dist2
				);
		} else {
			if (d.GetY() < 0) {
				//ret.x = d.x + (dist2 / (sqrt(1 + (mp*mp))));
				//ret.y = d.y + ((dist2*mp) / (sqrt(1 + (mp*mp))));

				return AZ::Vector2(
					d.GetX() + (dist2 / (sqrt(1 + square(mp)))),
					d.GetY() + ((dist2*mp) / (sqrt(1 + square(mp))))
					);
			}
			else {
				//ret.x = d.x + ((-dist2) / (sqrt(1 + (mp*mp))));
				//ret.y = d.y + ((-(dist2*mp)) / (sqrt(1 + (mp*mp))));

				return AZ::Vector2(
					d.GetX() + ((-dist2) / (sqrt(1 + square(mp)))),
					d.GetY() + ((-(dist2*mp)) / (sqrt(1 + square(mp))))
					);
			}
		}

		return AZ::Vector2(0, 0);
	}

	AZ::Vector2 * Util::PerpPointDist(AZ::Vector2 start, AZ::Vector2 end, float * dist1, float * dist2, const int numDists) {
		static AZ::Vector2 * ret = new AZ::Vector2[numDists];
		//make sure that dist1 is between 0 and 1;

		float m = -1;
		if ((end.GetX() - start.GetX()) != 0) m = ((end.GetY() - start.GetY()) / (end.GetX() - start.GetX()));
		float mp = -(1 / ((end.GetY() - start.GetY()) / (end.GetX() - start.GetX())));

		for (int i = 0; i < numDists; i++) {
			/*if (dist1[i] <= 0) dist1[i] = 0;
			if (dist1[i] >= 1) dist1[i] = 1;*/

			AZ::Vector2 d = AZ::Vector2((1 - dist1[i])*start.GetX() + dist1[i] * end.GetX(), (1 - dist1[i])*start.GetY() + dist1[i] * end.GetY());

			if (m == 0) {
				ret[i] = AZ::Vector2(
					d.GetX() + (dist2[i] / (sqrt(1 + square(mp)))),
					d.GetY() + dist2[i]
					);
			}else {
				if (d.GetY() < 0) {
					//ret.x = d.x + (dist2 / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((dist2*mp) / (sqrt(1 + (mp*mp))));

					ret[i] = AZ::Vector2(
						d.GetX() + (dist2[i] / (sqrt(1 + square(mp)))),
						d.GetY() + ((dist2[i] * mp) / (sqrt(1 + square(mp))))
						);
				}
				else {
					//ret.x = d.x + ((-dist2) / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((-(dist2*mp)) / (sqrt(1 + (mp*mp))));

					ret[i] = AZ::Vector2(
						d.GetX() + ((-dist2[i]) / (sqrt(1 + square(mp)))),
						d.GetY() + ((-(dist2[i] * mp)) / (sqrt(1 + square(mp))))
						);
				}
			}

			ret[i] = AZ::Vector2(0, 0);
		}
		return ret;
	}

	AZ::Vector2 * Util::PerpPointDist(AZ::Vector2 start, AZ::Vector2 end, float dist1, float * dist2, const int numDists) {
		static AZ::Vector2 * ret = new AZ::Vector2[numDists];
		//make sure that dist1 is between 0 and 1;

		float m = -1;
		if ((end.GetX() - start.GetX()) != 0) m = ((end.GetY() - start.GetY()) / (end.GetX() - start.GetX()));
		AZ::Vector2 d = AZ::Vector2((1 - dist1)*start.GetX() + dist1 * end.GetX(), (1 - dist1)*start.GetY() + dist1 * end.GetY());
		float mp = -(1 / ((end.GetY() - start.GetY()) / (end.GetX() - start.GetX())));

		for (int i = 0; i < numDists; i++) {
			/*if (dist1[i] <= 0) dist1[i] = 0;
			if (dist1[i] >= 1) dist1[i] = 1;*/


			if (m == 0) {
				ret[i] = AZ::Vector2(
					d.GetX() + (dist2[i] / (sqrt(1 + square(mp)))),
					d.GetY() + dist2[i]
					);
			} else {
				if (d.GetY() < 0) {
					//ret.x = d.x + (dist2 / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((dist2*mp) / (sqrt(1 + (mp*mp))));

					ret[i] = AZ::Vector2(
						d.GetX() + (dist2[i] / (sqrt(1 + square(mp)))),
						d.GetY() + ((dist2[i] * mp) / (sqrt(1 + square(mp))))
						);
				}
				else {
					//ret.x = d.x + ((-dist2) / (sqrt(1 + (mp*mp))));
					//ret.y = d.y + ((-(dist2*mp)) / (sqrt(1 + (mp*mp))));

					ret[i] = AZ::Vector2(
						d.GetX() + ((-dist2[i]) / (sqrt(1 + square(mp)))),
						d.GetY() + ((-(dist2[i] * mp)) / (sqrt(1 + square(mp))))
						);
				}
			}
		}
		return ret;
	}

	AZ::Vector2 Util::GetEdgePoint(AZ::Vector2 point, float angle, float x_min, float x_max, float y_min, float y_max) {
		if (angle > 0.0f) while (angle >= 360.0f) angle -= 360.0f;
		else while (angle <= -360.0f) angle += 360.0f;

		if (angle == 0.0f){
			return AZ::Vector2(x_max, point.GetY());
		}else if (angle == 90.0f) {
			return AZ::Vector2(point.GetX(), y_max);
		} else if (angle == 180.0f) {
			return AZ::Vector2(x_min, point.GetY());
		} else if (angle == 270.0f) {
			return AZ::Vector2(point.GetX(), y_min);
		} else {
			float thetaR = M_PI * (angle / 180.0f);

			AZ::Vector2 circleVec(
				cos(thetaR)+point.GetX(),
				sin(thetaR) + point.GetY()
			);

			float slope = (circleVec.GetY() - point.GetY()) / (circleVec.GetX() - point.GetX());
			float yintercept = circleVec.GetY() - (slope * (circleVec.GetX()));

			AZ::Vector2 vecMax(
				(x_max-yintercept+(y_max-x_max))/slope,
				(slope*x_max)+yintercept
			);
			AZ::Vector2 vecMin(
				(x_min-yintercept+(y_min-x_min))/slope,
				(slope*x_min)+yintercept
			);

			AZ::Vector2 point1(vecMax.GetX(), y_max);
			AZ::Vector2 point2(x_max, vecMax.GetY());
			AZ::Vector2 point3(vecMin.GetX(), y_min);
			AZ::Vector2 point4(x_min, vecMin.GetY());

			if (angle > 0.0f && angle < 90.0f) {
				if (vecMax.GetX() <= vecMax.GetY()) return point1;
				else return point2;
			} else if (angle > 90.0f && angle < 180.0f) {
				if (vecMax.GetX() >= vecMax.GetY()) return point1;
				else return point4;
			} else if (angle > 180.0f && angle < 270.0f) {
				if (vecMin.GetX() <= vecMin.GetY()) return point3;
				else return point4;
			} else if (angle > 270.0f && angle < 360.0f) {
				if (vecMin.GetX() >= vecMin.GetY()) return point2;
				else return point3;
			}
		}

		return AZ::Vector2();
	}

	bool Util::CopyToCache(AZStd::string src, AZStd::string cacheDest) {
		return gEnv->pFileIO->Copy(src.c_str(), GetCachePath(src,cacheDest).c_str()) == AZ::IO::ResultCode::Success;
	}

	bool Util::ClearCache(AZStd::string cacheDest) {
		AZ::IO::FileIOBase* fileIO = gEnv->pFileIO;
		AZStd::string cacheAlias(fileIO->GetAlias("@cache@"));
		cacheAlias += "/" + cacheDest;
		return fileIO->DestroyPath(cacheAlias.c_str()) == AZ::IO::ResultCode::Success;
	}

	AZStd::string Util::GetCachePath(AZStd::string src, AZStd::string cacheDest) {
		return gEnv->pFileIO->GetAlias("@cache@") + AZStd::string("/") + cacheDest + PathUtil::GetFile(src.c_str());
	}

	#ifdef WX_DISABLE
		#pragma warning(default : 4244)
	#endif
}