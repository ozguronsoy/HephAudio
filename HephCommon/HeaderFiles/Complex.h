#pragma once
#include "HephCommonShared.h"

namespace HephCommon
{
	struct Complex
	{
		double real;
		double imag;

		constexpr Complex() : Complex(0, 0) {}
		
		constexpr Complex(double real, double imag) : real(real), imag(imag) {}
		
		constexpr Complex operator-() const 
		{ 
			return Complex(-this->real, -this->imag);
		}
		
		constexpr Complex operator+(double rhs) const
		{
			return Complex(this->real + rhs, this->imag);
		}
		
		constexpr Complex operator+(const Complex& rhs) const
		{
			return Complex(this->real + rhs.real, this->imag + rhs.imag);
		}

		inline constexpr Complex& operator+=(double rhs)
		{
			this->real += rhs;
			return *this;
		}

		inline constexpr Complex& operator+=(const Complex& rhs)
		{
			this->real += rhs.real;
			this->imag += rhs.imag;
			return *this;
		}
		
		constexpr Complex operator-(double rhs) const
		{
			return Complex(this->real - rhs, this->imag);
		}
		
		constexpr Complex operator-(const Complex& rhs) const
		{
			return Complex(this->real - rhs.real, this->imag - rhs.imag);
		}

		inline constexpr Complex& operator-=(double rhs)
		{
			this->real -= rhs;
			return *this;
		}

		inline constexpr Complex& operator-=(const Complex& rhs)
		{
			this->real -= rhs.real;
			this->imag -= rhs.imag;
			return *this;
		}
		
		constexpr Complex operator*(double rhs) const
		{
			return Complex(this->real * rhs, this->imag * rhs);
		}
		
		constexpr Complex operator*(const Complex& rhs) const
		{
			return Complex(this->real * rhs.real - this->imag * rhs.imag, this->imag * rhs.real + this->real * rhs.imag);
		}
		
		inline constexpr Complex& operator*=(double rhs)
		{
			this->real *= rhs;
			this->imag *= rhs;
			return *this;
		}

		inline constexpr Complex& operator*=(const Complex& rhs)
		{
			const double newReal = this->real * rhs.real - this->imag * rhs.imag;
			this->imag = this->imag * rhs.real + this->real * rhs.imag;
			this->real = newReal;
			return *this;
		}

		constexpr Complex operator/(double rhs) const
		{
			return Complex(this->real / rhs, this->imag / rhs);
		}
		
		constexpr Complex operator/(const Complex& rhs) const
		{
			const double denomiter = rhs.real * rhs.real + rhs.imag * rhs.imag;
			return Complex((this->real * rhs.real + this->imag * rhs.imag) / denomiter,
							(this->imag * rhs.real - this->real * rhs.imag) / denomiter);
		}

		inline constexpr Complex& operator/=(double rhs)
		{
			this->real /= rhs;
			this->imag /= rhs;
			return *this;
		}

		inline constexpr Complex& operator/=(const Complex& rhs)
		{
			const double denomiter = rhs.real * rhs.real + rhs.imag * rhs.imag;
			const double newReal = (this->real * rhs.real + this->imag * rhs.imag) / denomiter;
			this->imag = (this->imag * rhs.real - this->real * rhs.imag) / denomiter;
			this->real = newReal;
			return *this;
		}
		
		constexpr bool operator==(const Complex& rhs) const
		{
			return this->real == rhs.real && this->imag == rhs.imag;
		}
		
		constexpr bool operator!=(const Complex& rhs) const
		{
			return this->real != rhs.real || this->imag != rhs.imag;
		}
		
		constexpr Complex Conjugate() const
		{
			return Complex(this->real, -this->imag);
		}
		
		constexpr double MagnitudeSquared() const
		{
			return this->real * this->real + this->imag * this->imag;
		}

		double Magnitude() const;
		double Phase() const;
	};
}

inline constexpr HephCommon::Complex operator+(double lhs, const HephCommon::Complex& rhs)
{
	return rhs + lhs;
}

inline constexpr HephCommon::Complex operator-(double lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs - rhs.real, -rhs.imag);
}

inline constexpr HephCommon::Complex operator*(double lhs, const HephCommon::Complex& rhs)
{
	return rhs * lhs;
}

inline constexpr HephCommon::Complex operator/(double lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs, 0) / rhs;
}