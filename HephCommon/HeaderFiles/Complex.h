#pragma once
#include "HephCommonShared.h"
#include "HephMath.h"
#include <complex>

namespace HephCommon
{
	struct Complex final : public std::complex<heph_float>
	{
		Complex();
		Complex(heph_float real, heph_float imag);
		Complex operator+() const;
		Complex operator-() const;
		Complex operator+(heph_float rhs) const;
		Complex operator+(const Complex& rhs) const;
		Complex& operator+=(heph_float rhs);
		Complex& operator+=(const Complex& rhs);
		Complex operator-(heph_float rhs) const;
		Complex operator-(const Complex& rhs) const;
		Complex& operator-=(heph_float rhs);
		Complex& operator-=(const Complex& rhs);
		Complex operator*(heph_float rhs) const;
		Complex operator*(const Complex& rhs) const;
		Complex& operator*=(heph_float rhs);
		Complex& operator*=(const Complex& rhs);
		Complex operator/(heph_float rhs) const;
		Complex operator/(const Complex& rhs) const;
		Complex& operator/=(heph_float rhs);
		Complex& operator/=(const Complex& rhs);
		bool operator==(const Complex& rhs) const;
		bool operator!=(const Complex& rhs) const;
		Complex Conjugate() const;
		heph_float MagnitudeSquared() const;
		heph_float Magnitude() const;
		heph_float Phase() const;
	};
}
inline HephCommon::Complex operator+(heph_float lhs, const HephCommon::Complex& rhs)
{
	return rhs + lhs;
}
inline HephCommon::Complex operator-(heph_float lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs - rhs.real(), -rhs.imag());
}
inline HephCommon::Complex operator*(heph_float lhs, const HephCommon::Complex& rhs)
{
	return rhs * lhs;
}
inline HephCommon::Complex operator/(heph_float lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs, 0) / rhs;
}
inline HephCommon::Complex operator""_j(unsigned long long int x)
{
	return HephCommon::Complex(0, x);
}
inline HephCommon::Complex operator""_j(long double x)
{
	return HephCommon::Complex(0, x);
}
inline HephCommon::Complex operator""_J(unsigned long long int x)
{
	return HephCommon::Complex(0, x);
}
inline HephCommon::Complex operator""_J(long double x)
{
	return HephCommon::Complex(0, x);
}