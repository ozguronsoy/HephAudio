#pragma once
#include "HephCommonFramework.h"
#include "HephMath.h"

namespace HephCommon
{
	struct Complex final
	{
		heph_float real;
		heph_float imaginary;
		constexpr Complex() : real(0.0f), imaginary(0.0f) {}
		constexpr Complex(heph_float real, heph_float imaginary) : real(real), imaginary(imaginary) {}
		constexpr Complex(const Complex& rhs) : real(rhs.real), imaginary(rhs.imaginary) {}
		constexpr Complex& operator=(const Complex& rhs)
		{
			this->real = rhs.real;
			this->imaginary = rhs.imaginary;
			return *this;
		}
		constexpr Complex operator+() const
		{
			return Complex(this->real, this->imaginary);
		}
		constexpr Complex operator-() const
		{
			return Complex(-this->real, -this->imaginary);
		}
		constexpr Complex operator+(const Complex& rhs) const
		{
			return Complex(this->real + rhs.real, this->imaginary + rhs.imaginary);
		}
		constexpr Complex& operator+=(const Complex& rhs)
		{
			this->real += rhs.real;
			this->imaginary += rhs.imaginary;
			return *this;
		}
		constexpr Complex operator-(const Complex& rhs) const
		{
			return Complex(this->real - rhs.real, this->imaginary - rhs.imaginary);
		}
		constexpr Complex& operator-=(const Complex& rhs)
		{
			this->real -= rhs.real;
			this->imaginary -= rhs.imaginary;
			return *this;
		}
		constexpr Complex operator*(const Complex& rhs) const
		{
			return Complex(this->real * rhs.real - this->imaginary * rhs.imaginary, this->imaginary * rhs.real + this->real * rhs.imaginary);
		}
		constexpr Complex& operator*=(const Complex& rhs)
		{
			const heph_float newReal = this->real * rhs.real - this->imaginary * rhs.imaginary;
			this->imaginary = this->imaginary * rhs.real + this->real * rhs.imaginary;
			this->real = newReal;
			return *this;
		}
		constexpr Complex operator/(const Complex& rhs) const
		{
			Complex result;
			const heph_float denomiter = rhs.real * rhs.real + rhs.imaginary * rhs.imaginary;
			result.real = (this->real * rhs.real + this->imaginary * rhs.imaginary) / denomiter;
			result.imaginary = (this->imaginary * rhs.real - this->real * rhs.imaginary) / denomiter;
			return result;
		}
		constexpr Complex& operator/=(const Complex& rhs)
		{
			const heph_float denomiter = rhs.real * rhs.real + rhs.imaginary * rhs.imaginary;
			const heph_float newReal = (this->real * rhs.real + this->imaginary * rhs.imaginary) / denomiter;
			this->imaginary = (this->imaginary * rhs.real - this->real * rhs.imaginary) / denomiter;
			this->real = newReal;
			return *this;
		}
		constexpr bool operator==(const Complex& rhs) const
		{
			return this->real == rhs.real && this->imaginary == rhs.imaginary;
		}
		constexpr bool operator!=(const Complex& rhs) const
		{
			return this->real != rhs.real || this->imaginary != rhs.imaginary;
		}
		constexpr Complex operator+(heph_float rhs) const
		{
			return Complex(this->real + rhs, this->imaginary);
		}
		constexpr Complex& operator+=(heph_float rhs)
		{
			this->real += rhs;
			return *this;
		}
		constexpr Complex operator-(heph_float rhs) const
		{
			return Complex(this->real - rhs, this->imaginary);
		}
		constexpr Complex& operator-=(heph_float rhs)
		{
			this->real -= rhs;
			return *this;
		}
		constexpr Complex operator*(heph_float rhs) const
		{
			return Complex(this->real * rhs, this->imaginary * rhs);
		}
		constexpr Complex& operator*=(heph_float rhs)
		{
			this->real *= rhs;
			this->imaginary *= rhs;
			return *this;
		}
		constexpr Complex operator/(heph_float rhs) const
		{
			return Complex(this->real / rhs, this->imaginary / rhs);
		}
		constexpr Complex& operator/=(heph_float rhs)
		{
			this->real /= rhs;
			this->imaginary /= rhs;
			return *this;
		}
		constexpr Complex Conjugate() const
		{
			return Complex(this->real, -this->imaginary);
		}
		constexpr heph_float MagnitudeSquared() const
		{
			return this->real * this->real + this->imaginary * this->imaginary;
		}
		heph_float Magnitude() const
		{
			return std::sqrt(this->MagnitudeSquared());
		}
		heph_float Phase() const
		{
			return std::atan2(this->imaginary, this->real);
		}
	};
}
inline heph_float abs(const HephCommon::Complex& rhs)
{
	return rhs.Magnitude();
}
inline heph_float phase(const HephCommon::Complex& rhs)
{
	return rhs.Phase();
}
inline constexpr HephCommon::Complex operator+(heph_float lhs, const HephCommon::Complex& rhs)
{
	return rhs + lhs;
}
inline constexpr HephCommon::Complex operator-(heph_float lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs - rhs.real, -rhs.imaginary);
}
inline constexpr HephCommon::Complex operator*(heph_float lhs, const HephCommon::Complex& rhs)
{
	return rhs * lhs;
}
inline constexpr HephCommon::Complex operator/(heph_float lhs, const HephCommon::Complex& rhs)
{
	return HephCommon::Complex(lhs, 0) / rhs;
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_j(unsigned long long int x)
{
	return HephCommon::Complex(0, x);
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_j(long double x)
{
	return HephCommon::Complex(0, x);
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_J(unsigned long long int x)
{
	return HephCommon::Complex(0, x);
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_J(long double x)
{
	return HephCommon::Complex(0, x);
}