#include "Complex.h"

namespace HephCommon
{
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
	double Complex::Magnitude() const
	{
		return std::sqrt(this->MagnitudeSquared());
	}
	double Complex::Phase() const
	{
		return std::atan2(this->imag(), this->real());
	}
}