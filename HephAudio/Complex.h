#pragma once
#include "framework.h"

namespace HephAudio
{
	struct Complex
	{
		hephaudio_float real;
		hephaudio_float imaginary;
		Complex();
		Complex(hephaudio_float real, hephaudio_float imaginary);
		Complex(const Complex& rhs);
		Complex(Complex&& rhs) noexcept;
		virtual ~Complex() = default;
		Complex& operator=(const Complex& rhs);
		Complex& operator=(Complex&& rhs) noexcept;
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
		Complex operator+(const hephaudio_float& rhs) const;
		Complex& operator+=(const hephaudio_float& rhs);
		Complex operator-(const hephaudio_float& rhs) const;
		Complex& operator-=(const hephaudio_float& rhs);
		Complex operator*(const hephaudio_float& rhs) const;
		Complex& operator*=(const hephaudio_float& rhs);
		Complex operator/(const hephaudio_float& rhs) const;
		Complex& operator/=(const hephaudio_float& rhs);
		bool operator==(const hephaudio_float& rhs) const;
		bool operator!=(const hephaudio_float& rhs) const;
		hephaudio_float Magnitude() const noexcept;
		hephaudio_float MagnitudeSquared() const noexcept;
		hephaudio_float Phase() const noexcept;
	};
}