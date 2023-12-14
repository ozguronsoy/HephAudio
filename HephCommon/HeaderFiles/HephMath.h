#pragma once
#include "HephCommonFramework.h"
#include <cmath>
#include <cinttypes>
#include <float.h>

#define HEPH_PI 3.14159265358979323846
#define HEPH_E 2.71828182845904523536

namespace HephCommon
{
	class Math final
	{
	public:
		static HEPH_CONSTEXPR long double pi_l = 3.14159265358979323846L;
		static HEPH_CONSTEXPR double pi = Math::pi_l;
		static HEPH_CONSTEXPR float pi_f = Math::pi_l;
		static HEPH_CONSTEXPR heph_float pi_hf = Math::pi_l;
		static HEPH_CONSTEXPR long double e_l = 2.71828182845904523536L;
		static HEPH_CONSTEXPR double e = Math::e_l;
		static HEPH_CONSTEXPR float e_f = Math::e_l;
		static HEPH_CONSTEXPR heph_float e_hf = Math::e_l;
	public:
		Math() = delete;
		Math(const Math&) = delete;
		Math& operator=(const Math&) = delete;
	public:
#pragma region Sgn
		static HEPH_CONSTEXPR float Sgn(float x) noexcept
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
		static HEPH_CONSTEXPR double Sgn(double x) noexcept
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
		static HEPH_CONSTEXPR long double Sgn(long double x) noexcept
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
		static HEPH_CONSTEXPR int32_t Sgn(int32_t x) noexcept
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
		static HEPH_CONSTEXPR int64_t Sgn(int64_t x) noexcept
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
		static HEPH_CONSTEXPR float Max(float a, float b) noexcept
		{
			return a > b ? a : b;
		}
		static HEPH_CONSTEXPR double Max(double a, double b) noexcept
		{
			return a > b ? a : b;
		}
		static HEPH_CONSTEXPR long double Max(long double a, long double b) noexcept
		{
			return a > b ? a : b;
		}
		static HEPH_CONSTEXPR int32_t Max(int32_t a, int32_t b) noexcept
		{
			return a > b ? a : b;
		}
		static HEPH_CONSTEXPR uint32_t Max(uint32_t a, uint32_t b) noexcept
		{
			return a > b ? a : b;
		}
		static HEPH_CONSTEXPR int64_t Max(int64_t a, int64_t b) noexcept
		{
			return a > b ? a : b;
		}
		static HEPH_CONSTEXPR uint64_t Max(uint64_t a, uint64_t b) noexcept
		{
			return a > b ? a : b;
		}
#pragma endregion
#pragma region Min
		static HEPH_CONSTEXPR float Min(float a, float b) noexcept
		{
			return a > b ? b : a;
		}
		static HEPH_CONSTEXPR double Min(double a, double b) noexcept
		{
			return a > b ? b : a;
		}
		static HEPH_CONSTEXPR long double Min(long double a, long double b) noexcept
		{
			return a > b ? b : a;
		}
		static HEPH_CONSTEXPR int32_t Min(int32_t a, int32_t b) noexcept
		{
			return a > b ? b : a;
		}
		static HEPH_CONSTEXPR uint32_t Min(uint32_t a, uint32_t b) noexcept
		{
			return a > b ? b : a;
		}
		static HEPH_CONSTEXPR int64_t Min(int64_t a, int64_t b) noexcept
		{
			return a > b ? b : a;
		}
		static HEPH_CONSTEXPR uint64_t Min(uint64_t a, uint64_t b) noexcept
		{
			return a > b ? b : a;
		}
#pragma endregion
#pragma region Abs
		static HEPH_CONSTEXPR float Abs(float x) noexcept
		{
			return x < 0 ? -x : x;
		}
		static HEPH_CONSTEXPR double Abs(double x) noexcept
		{
			return x < 0 ? -x : x;
		}
		static HEPH_CONSTEXPR long double Abs(long double x) noexcept
		{
			return x < 0 ? -x : x;
		}
		static HEPH_CONSTEXPR int32_t Abs(int32_t x) noexcept
		{
			return x < 0 ? -x : x;
		}
		static HEPH_CONSTEXPR int64_t Abs(int64_t x) noexcept
		{
			return x < 0 ? -x : x;
		}
#pragma endregion
#pragma region Round
		static HEPH_CONSTEXPR float Round(float x) noexcept
		{
			return (((int32_t)(x * 10.0f)) % 10) < 5 ? (int32_t)x : (int32_t)(x + 1.0f);
		}
		static HEPH_CONSTEXPR double Round(double x) noexcept
		{
			return (((int64_t)(x * 10.0)) % 10) < 5 ? (int64_t)x : (int64_t)(x + 1.0);
		}
		static HEPH_CONSTEXPR long double Round(long double x) noexcept
		{
			return (((int64_t)(x * 10.0L)) % 10) < 5 ? (int64_t)x : (int64_t)(x + 1.0L);
		}
#pragma endregion
#pragma region Floor
		static HEPH_CONSTEXPR float Floor(float x) noexcept
		{
			if (x < 0)
			{
				return x == ((int32_t)x) ? (int32_t)x : (int32_t)(x - 1.0f);
			}
			return (int32_t)x;
		}
		static HEPH_CONSTEXPR double Floor(double x) noexcept
		{
			if (x < 0)
			{
				return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x - 1.0);
			}
			return (int64_t)x;
		}
		static HEPH_CONSTEXPR long double Floor(long double x) noexcept
		{
			if (x < 0)
			{
				return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x - 1.0L);
			}
			return (int64_t)x;
		}
#pragma endregion
#pragma region Ceil
		static HEPH_CONSTEXPR float Ceil(float x) noexcept
		{
			if (x < 0)
			{
				return -((int32_t)(-x));
			}
			return x == ((int32_t)x) ? (int32_t)x : (int32_t)(x + 1.0f);
		}
		static HEPH_CONSTEXPR double Ceil(double x) noexcept
		{
			if (x < 0)
			{
				return -((int64_t)(-x));
			}
			return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x + 1.0);
		}
		static HEPH_CONSTEXPR long double Ceil(long double x) noexcept
		{
			if (x < 0)
			{
				return -((int64_t)(-x));
			}
			return x == ((int64_t)x) ? (int64_t)x : (int64_t)(x + 1.0L);
		}
#pragma endregion
#pragma region Mod
		static HEPH_CONSTEXPR float Mod(float a, float b) noexcept
		{
			if ((a < 0 && b > 0) || (a > 0 && b < 0))
			{
				const double div = (double)-a / (double)b;
				return (((int32_t)div) - div) * (double)b;
			}
			const double div = (double)a / (double)b;
			return (div - ((int32_t)div)) * (double)b;
		}
		static HEPH_CONSTEXPR double Mod(double a, double b) noexcept
		{
			if ((a < 0 && b > 0) || (a > 0 && b < 0))
			{
				const double div = -a / b;
				return (((int64_t)div) - div) * b;
			}
			const double div = a / b;
			return (div - ((int64_t)div)) * b;
		}
		static HEPH_CONSTEXPR long double Mod(long double a, long double b) noexcept
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
		static HEPH_CONSTEXPR float RadToDeg(float x) noexcept
		{
			return x * 180.0f / Math::pi_f;
		}
		static HEPH_CONSTEXPR double RadToDeg(double x) noexcept
		{
			return x * 180.0 / Math::pi;
		}
		static HEPH_CONSTEXPR long double RadToDeg(long double x) noexcept
		{
			return x * 180.0L / Math::pi_l;
		}
		static HEPH_CONSTEXPR double RadToDeg(int32_t x) noexcept
		{
			return x * 180.0 / Math::pi;
		}
		static HEPH_CONSTEXPR double RadToDeg(uint32_t x) noexcept
		{
			return x * 180.0 / Math::pi;
		}
		static HEPH_CONSTEXPR long double RadToDeg(int64_t x) noexcept
		{
			return x * 180.0L / Math::pi_l;
		}
		static HEPH_CONSTEXPR long double RadToDeg(uint64_t x) noexcept
		{
			return x * 180.0L / Math::pi_l;
		}
#pragma endregion
#pragma region DegToRad
		static HEPH_CONSTEXPR float DegToRad(float x) noexcept
		{
			return x * Math::pi_f / 180.0f;
		}
		static HEPH_CONSTEXPR double DegToRad(double x) noexcept
		{
			return x * Math::pi / 180.0;
		}
		static HEPH_CONSTEXPR long double DegToRad(long double x) noexcept
		{
			return x * Math::pi_l / 180.0L;
		}
		static HEPH_CONSTEXPR double DegToRad(int32_t x) noexcept
		{
			return x * Math::pi / 180.0;
		}
		static HEPH_CONSTEXPR double DegToRad(uint32_t x) noexcept
		{
			return x * Math::pi / 180.0;
		}
		static HEPH_CONSTEXPR long double DegToRad(int64_t x) noexcept
		{
			return x * Math::pi_l / 180.0L;
		}
		static HEPH_CONSTEXPR long double DegToRad(uint64_t x) noexcept
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