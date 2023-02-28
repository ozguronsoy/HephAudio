#include "Complex.h"
#include <cmath>

namespace HephAudio
{
	Complex::Complex() : Complex(0.0, 0.0) {}
	Complex::Complex(HEPHAUDIO_DOUBLE real, HEPHAUDIO_DOUBLE imaginary)
	{
		this->real = real;
		this->imaginary = imaginary;
	}
	Complex::Complex(const Complex& rhs)
	{
		this->real = rhs.real;
		this->imaginary = rhs.imaginary;
	}
	Complex::Complex(Complex&& rhs) noexcept
	{
		this->real = (HEPHAUDIO_DOUBLE&&)rhs.real;
		this->imaginary = (HEPHAUDIO_DOUBLE&&)rhs.imaginary;
	}
	Complex& Complex::operator=(const Complex& rhs)
	{
		this->real = rhs.real;
		this->imaginary = rhs.imaginary;
		return *this;
	}
	Complex& Complex::operator=(Complex&& rhs) noexcept
	{
		this->real = (HEPHAUDIO_DOUBLE&&)rhs.real;
		this->imaginary = (HEPHAUDIO_DOUBLE&&)rhs.imaginary;
		return *this;
	}
	Complex Complex::operator-() const
	{
		return Complex(-this->real, -this->imaginary);
	}
	Complex Complex::operator+(const Complex& rhs) const
	{
		return Complex(this->real + rhs.real, this->imaginary + rhs.imaginary);
	}
	Complex& Complex::operator+=(const Complex& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	Complex Complex::operator-(const Complex& rhs) const
	{
		return Complex(this->real - rhs.real, this->imaginary - rhs.imaginary);
	}
	Complex& Complex::operator-=(const Complex& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	Complex Complex::operator*(const Complex& rhs) const
	{
		return Complex(this->real * rhs.real - this->imaginary * rhs.imaginary, this->imaginary * rhs.real + this->real * rhs.imaginary);
	}
	Complex& Complex::operator*=(const Complex& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	Complex Complex::operator/(const Complex& rhs) const
	{
		Complex result;
		const HEPHAUDIO_DOUBLE denomiter = rhs.real * rhs.real + rhs.imaginary * rhs.imaginary;
		result.real = (this->real * rhs.real + this->imaginary * rhs.imaginary) / denomiter;
		result.imaginary = (this->imaginary * rhs.real - this->real * rhs.imaginary) / denomiter;
		return result;
	}
	Complex& Complex::operator/=(const Complex& rhs)
	{
		*this = *this / rhs;
		return *this;
	}
	bool Complex::operator==(const Complex& rhs) const
	{
		return this->real == rhs.real && this->imaginary == rhs.imaginary;
	}
	bool Complex::operator!=(const Complex& rhs) const
	{
		return !(*this == rhs);
	}
	Complex Complex::operator+(const HEPHAUDIO_DOUBLE& rhs) const
	{
		return Complex(this->real + rhs, this->imaginary);
	}
	Complex& Complex::operator+=(const HEPHAUDIO_DOUBLE& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	Complex Complex::operator-(const HEPHAUDIO_DOUBLE& rhs) const
	{
		return Complex(this->real - rhs, this->imaginary);
	}
	Complex& Complex::operator-=(const HEPHAUDIO_DOUBLE& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	Complex Complex::operator*(const HEPHAUDIO_DOUBLE& rhs) const
	{
		return Complex(this->real * rhs, this->imaginary * rhs);
	}
	Complex& Complex::operator*=(const HEPHAUDIO_DOUBLE& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	Complex Complex::operator/(const HEPHAUDIO_DOUBLE& rhs) const
	{
		return Complex(this->real / rhs, this->imaginary / rhs);
	}
	Complex& Complex::operator/=(const HEPHAUDIO_DOUBLE& rhs)
	{
		*this = *this / rhs;
		return *this;
	}
	bool Complex::operator==(const HEPHAUDIO_DOUBLE& rhs) const
	{
		return this->imaginary == 0 && this->real == rhs;
	}
	bool Complex::operator!=(const HEPHAUDIO_DOUBLE& rhs) const
	{
		return this->imaginary != 0 || this->real != rhs;
	}
	HEPHAUDIO_DOUBLE Complex::Magnitude() const noexcept
	{
		return sqrt(this->MagnitudeSquared());
	}
	HEPHAUDIO_DOUBLE Complex::MagnitudeSquared() const noexcept
	{
		return this->real * this->real + this->imaginary * this->imaginary;
	}
	HEPHAUDIO_DOUBLE Complex::Phase() const noexcept
	{
		return atan2(this->imaginary, this->real);
	}
}