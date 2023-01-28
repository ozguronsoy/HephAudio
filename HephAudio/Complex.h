#pragma once
#include "framework.h"

namespace HephAudio
{
	struct Complex
	{
		double real;
		double imaginary;
		Complex();
		Complex(double real, double imaginary);
		virtual ~Complex() = default;
		Complex operator-() const;
		Complex operator+(const Complex& rhs) const;
		Complex& operator+=(const Complex& rhs);
		Complex operator-(const Complex& rhs) const;
		Complex& operator-=(const Complex& rhs);
		Complex operator*(const Complex& rhs) const;
		Complex& operator*=(const Complex& rhs);
		Complex operator/(const Complex& rhs) const;
		Complex& operator/=(const Complex& rhs);
		bool operator==(const Complex& rhs) const;
		bool operator!=(const Complex& rhs) const;
		Complex operator+(const double& rhs) const;
		Complex& operator+=(const double& rhs);
		Complex operator-(const double& rhs) const;
		Complex& operator-=(const double& rhs);
		Complex operator*(const double& rhs) const;
		Complex& operator*=(const double& rhs);
		Complex operator/(const double& rhs) const;
		Complex& operator/=(const double& rhs);
		bool operator==(const double& rhs) const;
		bool operator!=(const double& rhs) const;
		double Magnitude() const noexcept;
		double MagnitudeSquared() const noexcept;
		double Phase() const noexcept;
	};
}