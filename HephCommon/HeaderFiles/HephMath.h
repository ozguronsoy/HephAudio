#pragma once
#include "HephCommonShared.h"
#include <cmath>
#include <cinttypes>
#include <float.h>

#define HEPH_PI 3.14159265358979323846l
#define HEPH_E 2.71828182845904523536l

namespace HephCommon
{
	class Math final
	{
	public:
		static constexpr long double pi_l = HEPH_PI;
		static constexpr double pi = HEPH_PI;
		static constexpr float pi_f = HEPH_PI;
		static constexpr heph_float pi_hf = HEPH_PI;
		static constexpr long double e_l = HEPH_E;
		static constexpr double e = HEPH_E;
		static constexpr float e_f = HEPH_E;
		static constexpr heph_float e_hf = HEPH_E;
	public:
		Math() = delete;
		Math(const Math&) = delete;
		Math& operator=(const Math&) = delete;
	public:
#pragma region Sgn
		static constexpr float Sgn(float x)
		{
			if (x > 0.0f)
			{
				return 1.0f;
			}
			else if (x < 0.0f)
			{
				return -1.0f;
			}
			return 0.0f;
		}
		static constexpr double Sgn(double x)
		{
			if (x > 0.0)
			{
				return 1.0;
			}
			else if (x < 0.0)
			{
				return -1.0;
			}
			return 0.0;
		}
		static constexpr long double Sgn(long double x)
		{
			if (x > 0.0L)
			{
				return 1.0L;
			}
			else if (x < 0.0L)
			{
				return -1.0L;
			}
			return 0.0L;
		}
		static constexpr int32_t Sgn(int32_t x)
		{
			if (x > 0)
			{
				return 1;
			}
			else if (x < 0)
			{
				return -1;
			}
			return 0;
		}
		static constexpr int64_t Sgn(int64_t x)
		{
			if (x > 0)
			{
				return 1;
			}
			else if (x < 0)
			{
				return -1;
			}
			return 0;
		}
#pragma endregion
#pragma region Max
		static constexpr float Max(float a, float b)
		{
			return a > b ? a : b;
		}
		static constexpr double Max(double a, double b)
		{
			return a > b ? a : b;
		}
		static constexpr long double Max(long double a, long double b)
		{
			return a > b ? a : b;
		}
		static constexpr int32_t Max(int32_t a, int32_t b)
		{
			return a > b ? a : b;
		}
		static constexpr uint32_t Max(uint32_t a, uint32_t b)
		{
			return a > b ? a : b;
		}
		static constexpr int64_t Max(int64_t a, int64_t b)
		{
			return a > b ? a : b;
		}
		static constexpr uint64_t Max(uint64_t a, uint64_t b)
		{
			return a > b ? a : b;
		}
#pragma endregion
#pragma region Min
		static constexpr float Min(float a, float b)
		{
			return a > b ? b : a;
		}
		static constexpr double Min(double a, double b)
		{
			return a > b ? b : a;
		}
		static constexpr long double Min(long double a, long double b)
		{
			return a > b ? b : a;
		}
		static constexpr int32_t Min(int32_t a, int32_t b)
		{
			return a > b ? b : a;
		}
		static constexpr uint32_t Min(uint32_t a, uint32_t b)
		{
			return a > b ? b : a;
		}
		static constexpr int64_t Min(int64_t a, int64_t b)
		{
			return a > b ? b : a;
		}
		static constexpr uint64_t Min(uint64_t a, uint64_t b)
		{
			return a > b ? b : a;
		}
#pragma endregion
#pragma region Abs
		static constexpr float Abs(float x)
		{
			return x < 0 ? -x : x;
		}
		static constexpr double Abs(double x)
		{
			return x < 0 ? -x : x;
		}
		static constexpr long double Abs(long double x)
		{
			return x < 0 ? -x : x;
		}
		static constexpr int32_t Abs(int32_t x)
		{
			return x < 0 ? -x : x;
		}
		static constexpr int64_t Abs(int64_t x)
		{
			return x < 0 ? -x : x;
		}
#pragma endregion
#pragma region Round
		static constexpr float Round(float x)
		{
			return (((int32_t)(x * 10.0f)) % 10) < 5 ? (int32_t)x : (int32_t)(x + 1.0f);
		}
		static constexpr double Round(double x)
		{
			return (((int64_t)(x * 10.0)) % 10) < 5 ? (int64_t)x : (int64_t)(x + 1.0);
		}
		static constexpr long double Round(long double x)
		{
			return (((int64_t)(x * 10.0L)) % 10) < 5 ? (int64_t)x : (int64_t)(x + 1.0L);
		}
#pragma endregion
#pragma region Floor
		static constexpr float Floor(float x)
		{
			if (x < 0)
			{
				return x == ((int32_t)x) ? (int32_t)x : (int32_t)(x - 1.0f);
			}
			return (int32_t)x;
		}
		static constexpr double Floor(double x)
		{
			if (x < 0)
			{
				return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x - 1.0);
			}
			return (int64_t)x;
		}
		static constexpr long double Floor(long double x)
		{
			if (x < 0)
			{
				return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x - 1.0L);
			}
			return (int64_t)x;
		}
#pragma endregion
#pragma region Ceil
		static constexpr float Ceil(float x)
		{
			if (x < 0)
			{
				return -((int32_t)(-x));
			}
			return x == ((int32_t)x) ? (int32_t)x : (int32_t)(x + 1.0f);
		}
		static constexpr double Ceil(double x)
		{
			if (x < 0)
			{
				return -((int64_t)(-x));
			}
			return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x + 1.0);
		}
		static constexpr long double Ceil(long double x)
		{
			if (x < 0)
			{
				return -((int64_t)(-x));
			}
			return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x + 1.0L);
		}
#pragma endregion
#pragma region Mod
		static constexpr float Mod(float a, float b)
		{
			if ((a < 0 && b > 0) || (a > 0 && b < 0))
			{
				const double div = (double)-a / (double)b;
				return (((int32_t)div) - div) * (double)b;
			}
			const double div = (double)a / (double)b;
			return (div - ((int32_t)div)) * (double)b;
		}
		static constexpr double Mod(double a, double b)
		{
			if ((a < 0 && b > 0) || (a > 0 && b < 0))
			{
				const double div = -a / b;
				return (((int64_t)div) - div) * b;
			}
			const double div = a / b;
			return (div - ((int64_t)div)) * b;
		}
		static constexpr long double Mod(long double a, long double b)
		{
			if ((a < 0 && b > 0) || (a > 0 && b < 0))
			{
				const long double div = -a / b;
				return (((int64_t)div) - div) * b;
			}
			const long double div = a / b;
			return (div - ((int64_t)div)) * b;
		}
#pragma endregion
#pragma region RadToDeg
		static constexpr float RadToDeg(float x)
		{
			return x * 180.0f / Math::pi_f;
		}
		static constexpr double RadToDeg(double x)
		{
			return x * 180.0 / Math::pi;
		}
		static constexpr long double RadToDeg(long double x)
		{
			return x * 180.0L / Math::pi_l;
		}
		static constexpr double RadToDeg(int32_t x)
		{
			return x * 180.0 / Math::pi;
		}
		static constexpr double RadToDeg(uint32_t x)
		{
			return x * 180.0 / Math::pi;
		}
		static constexpr long double RadToDeg(int64_t x)
		{
			return x * 180.0L / Math::pi_l;
		}
		static constexpr long double RadToDeg(uint64_t x)
		{
			return x * 180.0L / Math::pi_l;
		}
#pragma endregion
#pragma region DegToRad
		static constexpr float DegToRad(float x)
		{
			return x * Math::pi_f / 180.0f;
		}
		static constexpr double DegToRad(double x)
		{
			return x * Math::pi / 180.0;
		}
		static constexpr long double DegToRad(long double x)
		{
			return x * Math::pi_l / 180.0L;
		}
		static constexpr double DegToRad(int32_t x)
		{
			return x * Math::pi / 180.0;
		}
		static constexpr double DegToRad(uint32_t x)
		{
			return x * Math::pi / 180.0;
		}
		static constexpr long double DegToRad(int64_t x)
		{
			return x * Math::pi_l / 180.0L;
		}
		static constexpr long double DegToRad(uint64_t x)
		{
			return x * Math::pi_l / 180.0L;
		}
#pragma endregion
	};
}
#pragma region Literals
inline HEPH_CONSTEVAL long double operator""_rad(unsigned long long int x)
{
	return HephCommon::Math::RadToDeg((uint64_t)x);
}
inline HEPH_CONSTEVAL long double operator""_rad(long double x)
{
	return HephCommon::Math::RadToDeg(x);
}
inline HEPH_CONSTEVAL long double operator""_deg(unsigned long long int x)
{
	return HephCommon::Math::DegToRad((uint64_t)x);
}
inline HEPH_CONSTEVAL long double operator""_deg(long double x)
{
	return HephCommon::Math::DegToRad(x);
}
#pragma endregion