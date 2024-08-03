#pragma once
#include "HephCommonShared.h"
#include "HephMath.h"
#include <complex>

namespace HephCommon
{
	struct Complex final : public std::complex<double>
	{
		Complex();
		Complex(double real, double imag);
		Complex operator+() const;
		Complex operator-() const;
		Complex operator+(double rhs) const;
		Complex operator+(const Complex& rhs) const;
		Complex& operator+=(double rhs);
		Complex& operator+=(const Complex& rhs);
		Complex operator-(double rhs) const;
		Complex operator-(const Complex& rhs) const;
		Complex& operator-=(double rhs);
		Complex& operator-=(const Complex& rhs);
		Complex operator*(double rhs) const;
		Complex operator*(const Complex& rhs) const;
		Complex& operator*=(double rhs);
		Complex& operator*=(const Complex& rhs);
		Complex operator/(double rhs) const;
		Complex operator/(const Complex& rhs) const;
		Complex& operator/=(double rhs);
		Complex& operator/=(const Complex& rhs);
		bool operator==(const Complex& rhs) const;
		bool operator!=(const Complex& rhs) const;
		Complex Conjugate() const;
		double MagnitudeSquared() const;
		double Magnitude() const;
		double Phase() const;
	};
}
inline HephCommon::Complex operator+(double lhs, const HephCommon::Complex& rhs)
{
	return rhs + lhs;
}
inline HephCommon::Complex operator-(double lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs - rhs.real(), -rhs.imag());
}
inline HephCommon::Complex operator*(double lhs, const HephCommon::Complex& rhs)
{
	return rhs * lhs;
}
inline HephCommon::Complex operator/(double lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs, 0) / rhs;
}