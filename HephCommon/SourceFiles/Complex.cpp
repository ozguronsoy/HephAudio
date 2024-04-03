#include "Complex.h"

namespace HephCommon
{
	Complex::Complex() : Complex(0, 0) {}
	Complex::Complex(heph_float real, heph_float imag) : std::complex<heph_float>(real, imag) {}
	Complex Complex::operator+() const
	{
		return Complex(this->real(), this->imag());
	}
	Complex Complex::operator-() const
	{
		return Complex(-this->real(), -this->imag());
	}
	Complex Complex::operator+(heph_float rhs) const
	{
		return Complex(this->real() + rhs, this->imag());
	}
	Complex Complex::operator+(const Complex& rhs) const
	{
		return Complex(this->real() + rhs.real(), this->imag() + rhs.imag());
	}
	Complex& Complex::operator+=(heph_float rhs)
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
	Complex Complex::operator-(heph_float rhs) const
	{
		return Complex(this->real() - rhs, this->imag());
	}
	Complex Complex::operator-(const Complex& rhs) const
	{
		return Complex(this->real() - rhs.real(), this->imag() - rhs.imag());
	}
	Complex& Complex::operator-=(heph_float rhs)
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
	Complex Complex::operator*(heph_float rhs) const
	{
		return Complex(this->real() * rhs, this->imag() * rhs);
	}
	Complex Complex::operator*(const Complex& rhs) const
	{
		return Complex(this->real() * rhs.real() - this->imag() * rhs.imag(), this->imag() * rhs.real() + this->real() * rhs.imag());
	}
	Complex& Complex::operator*=(heph_float rhs)
	{
		this->real(this->real() * rhs);
		this->imag(this->imag() * rhs);
		return *this;
	}
	Complex& Complex::operator*=(const Complex& rhs)
	{
		const heph_float newReal = this->real() * rhs.real() - this->imag() * rhs.imag();
		this->imag(this->imag() * rhs.real() + this->real() * rhs.imag());
		this->real(newReal);
		return *this;
	}
	Complex Complex::operator/(heph_float rhs) const
	{
		return Complex(this->real() / rhs, this->imag() / rhs);
	}
	Complex Complex::operator/(const Complex& rhs) const
	{
		Complex result;
		const heph_float denomiter = rhs.real() * rhs.real() + rhs.imag() * rhs.imag();
		result.real((this->real() * rhs.real() + this->imag() * rhs.imag()) / denomiter);
		result.imag((this->imag() * rhs.real() - this->real() * rhs.imag()) / denomiter);
		return result;
	}
	Complex& Complex::operator/=(heph_float rhs)
	{
		this->real(this->real() / rhs);
		this->imag(this->imag() / rhs);
		return *this;
	}
	Complex& Complex::operator/=(const Complex& rhs)
	{
		const heph_float denomiter = rhs.real() * rhs.real() + rhs.imag() * rhs.imag();
		const heph_float newReal = (this->real() * rhs.real() + this->imag() * rhs.imag()) / denomiter;
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
	heph_float Complex::MagnitudeSquared() const
	{
		return this->real() * this->real() + this->imag() * this->imag();
	}
	heph_float Complex::Magnitude() const
	{
		return std::sqrt(this->MagnitudeSquared());
	}
	heph_float Complex::Phase() const
	{
		return std::atan2(this->imag(), this->real());
	}
}