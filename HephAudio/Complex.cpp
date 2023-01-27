#include "Complex.h"
#include <cmath>

namespace HephAudio
{
	Complex::Complex() : Complex(0.0, 0.0) {}
	Complex::Complex(double real, double imaginary)
	{
		this->real = real;
		this->imaginary = imaginary;
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
		const double denomiter = rhs.real * rhs.real + rhs.imaginary * rhs.imaginary;
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
	Complex Complex::operator+(const double& rhs) const
	{
		return Complex(this->real + rhs, this->imaginary);
	}
	Complex& Complex::operator+=(const double& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	Complex Complex::operator-(const double& rhs) const
	{
		return Complex(this->real - rhs, this->imaginary);
	}
	Complex& Complex::operator-=(const double& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	Complex Complex::operator*(const double& rhs) const
	{
		return Complex(this->real * rhs, this->imaginary * rhs);
	}
	Complex& Complex::operator*=(const double& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	Complex Complex::operator/(const double& rhs) const
	{
		return Complex(this->real / rhs, this->imaginary / rhs);
	}
	Complex& Complex::operator/=(const double& rhs)
	{
		*this = *this / rhs;
		return *this;
	}
	bool Complex::operator==(const double& rhs) const
	{
		return this->imaginary == 0 && this->real == rhs;
	}
	bool Complex::operator!=(const double& rhs) const
	{
		return this->imaginary != 0 || this->real != rhs;
	}
}