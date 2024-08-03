#include "Complex.h"

namespace HephCommon
{
	Complex::Complex() : Complex(0, 0) {}
	Complex::Complex(double real, double imag) : std::complex<double>(real, imag) {}
	Complex Complex::operator+() const
	{
		return Complex(this->real(), this->imag());
	}
	Complex Complex::operator-() const
	{
		return Complex(-this->real(), -this->imag());
	}
	Complex Complex::operator+(double rhs) const
	{
		return Complex(this->real() + rhs, this->imag());
	}
	Complex Complex::operator+(const Complex& rhs) const
	{
		return Complex(this->real() + rhs.real(), this->imag() + rhs.imag());
	}
	Complex& Complex::operator+=(double rhs)
	{
		this->real(this->real() + rhs);
		return *this;
	}
	Complex& Complex::operator+=(const Complex& rhs)
	{
		this->real(this->real() + rhs.real());
		this->imag(this->imag() + rhs.imag());
		return *this;
	}
	Complex Complex::operator-(double rhs) const
	{
		return Complex(this->real() - rhs, this->imag());
	}
	Complex Complex::operator-(const Complex& rhs) const
	{
		return Complex(this->real() - rhs.real(), this->imag() - rhs.imag());
	}
	Complex& Complex::operator-=(double rhs)
	{
		this->real(this->real() - rhs);
		return *this;
	}
	Complex& Complex::operator-=(const Complex& rhs)
	{
		this->real(this->real() - rhs.real());
		this->imag(this->imag() - rhs.imag());
		return *this;
	}
	Complex Complex::operator*(double rhs) const
	{
		return Complex(this->real() * rhs, this->imag() * rhs);
	}
	Complex Complex::operator*(const Complex& rhs) const
	{
		return Complex(this->real() * rhs.real() - this->imag() * rhs.imag(), this->imag() * rhs.real() + this->real() * rhs.imag());
	}
	Complex& Complex::operator*=(double rhs)
	{
		this->real(this->real() * rhs);
		this->imag(this->imag() * rhs);
		return *this;
	}
	Complex& Complex::operator*=(const Complex& rhs)
	{
		const double newReal = this->real() * rhs.real() - this->imag() * rhs.imag();
		this->imag(this->imag() * rhs.real() + this->real() * rhs.imag());
		this->real(newReal);
		return *this;
	}
	Complex Complex::operator/(double rhs) const
	{
		return Complex(this->real() / rhs, this->imag() / rhs);
	}
	Complex Complex::operator/(const Complex& rhs) const
	{
		Complex result;
		const double denomiter = rhs.real() * rhs.real() + rhs.imag() * rhs.imag();
		result.real((this->real() * rhs.real() + this->imag() * rhs.imag()) / denomiter);
		result.imag((this->imag() * rhs.real() - this->real() * rhs.imag()) / denomiter);
		return result;
	}
	Complex& Complex::operator/=(double rhs)
	{
		this->real(this->real() / rhs);
		this->imag(this->imag() / rhs);
		return *this;
	}
	Complex& Complex::operator/=(const Complex& rhs)
	{
		const double denomiter = rhs.real() * rhs.real() + rhs.imag() * rhs.imag();
		const double newReal = (this->real() * rhs.real() + this->imag() * rhs.imag()) / denomiter;
		this->imag((this->imag() * rhs.real() - this->real() * rhs.imag()) / denomiter);
		this->real(newReal);
		return *this;
	}
	bool Complex::operator==(const Complex& rhs) const
	{
		return this->real() == rhs.real() && this->imag() == rhs.imag();
	}
	bool Complex::operator!=(const Complex& rhs) const
	{
		return this->real() != rhs.real() || this->imag() != rhs.imag();
	}
	Complex Complex::Conjugate() const
	{
		return Complex(this->real(), -this->imag());
	}
	double Complex::MagnitudeSquared() const
	{
		return this->real() * this->real() + this->imag() * this->imag();
	}
	double Complex::Magnitude() const
	{
		return std::sqrt(this->MagnitudeSquared());
	}
	double Complex::Phase() const
	{
		return std::atan2(this->imag(), this->real());
	}
}