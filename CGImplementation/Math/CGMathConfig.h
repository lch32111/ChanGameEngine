#ifndef __CG_MATH_CONFIG_H__
#define __CG_MATH_CONFIG_H__

#include <CGPrecompiled.h>

#include <cmath>

#undef max

#include <cstdlib>
#include <cfloat>

namespace CG
{
	template <typename Scalar>
	struct ScalarOp {};

	template<>
	struct ScalarOp<float> {
		static float TwoTimesPi() { return 6.283185307179586232f; }
		static float Epsilon() { return FLT_EPSILON; }
		static float Max() { return FLT_MAX; }

		// static float random() { return float(GEN_rand()) / float(GEN_RAND_MAX); }
		static float Sqrt(float x) { return ::sqrtf(x); }
		static float Abs(float x) { return ::fabsf(x); }

		static float Cos(float x) { return ::cosf(x); }
		static float Sin(float x) { return ::sinf(x); }
		static float Tan(float x) { return ::tanf(x); }

		static float Acos(float x) { return ::acosf(x); }
		static float Asin(float x) { return ::asinf(x); }
		static float Atan(float x) { return ::atanf(x); }
		static float Atan2(float x, float y) { return ::atan2f(x, y); }

		static float Exp(float x) { return ::expf(x); }
		static float Log(float x) { return ::logf(x); }
		static float Pow(float x, float y) { return ::powf(x, y); }

		static float Radian(float angle) { return 0.01745329251994329576923690768489f * angle; }
	};

	template<>
	struct ScalarOp<double> {
		static double TwoTimesPi() { return 6.283185307179586232; }
		static double Epsilon() { return DBL_EPSILON; }
		static double Max() { return DBL_MAX; }

		// static double random() { return double(GEN_rand()) / double(GEN_RAND_MAX); }
		static double Sqrt(double x) { return ::sqrt(x); }
		static double Abs(double x) { return ::fabs(x); }

		static double Cos(double x) { return ::cos(x); }
		static double Sin(double x) { return ::sin(x); }
		static double Tan(double x) { return ::tan(x); }

		static double Acos(double x) { return ::acos(x); }
		static double Asin(double x) { return ::asin(x); }
		static double Atan(double x) { return ::atan(x); }
		static double Atan2(double x, double y) { return ::atan2(x, y); }

		static double Exp(double x) { return ::exp(x); }
		static double Log(double x) { return ::log(x); }
		static double Pow(double x, double y) { return ::pow(x, y); }

		static double Radian(double angle) { return 0.01745329251994329576923690768489 * angle; }
	};

	template<typename T>
	T Max(const T& A, const T& B)
	{
		return A > B ? A : B;
	}

	template<typename T>
	T Min(const T& A, const T& B)
	{
		return A < B ? A : B;
	}
}

#endif
