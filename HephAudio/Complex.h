#pragma once
#include "framework.h"

namespace HephAudio
{
	struct Complex
	{
		HEPHAUDIO_DOUBLE real;
		HEPHAUDIO_DOUBLE imaginary;
		Complex();
		Complex(HEPHAUDIO_DOUBLE real, HEPHAUDIO_DOUBLE imaginary);
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
		Complex operator+(const HEPHAUDIO_DOUBLE& rhs) const;
		Complex& operator+=(const HEPHAUDIO_DOUBLE& rhs);
		Complex operator-(const HEPHAUDIO_DOUBLE& rhs) const;
		Complex& operator-=(const HEPHAUDIO_DOUBLE& rhs);
		Complex operator*(const HEPHAUDIO_DOUBLE& rhs) const;
		Complex& operator*=(const HEPHAUDIO_DOUBLE& rhs);
		Complex operator/(const HEPHAUDIO_DOUBLE& rhs) const;
		Complex& operator/=(const HEPHAUDIO_DOUBLE& rhs);
		bool operator==(const HEPHAUDIO_DOUBLE& rhs) const;
		bool operator!=(const HEPHAUDIO_DOUBLE& rhs) const;
		HEPHAUDIO_DOUBLE Magnitude() const noexcept;
		HEPHAUDIO_DOUBLE MagnitudeSquared() const noexcept;
		HEPHAUDIO_DOUBLE Phase() const noexcept;
	};
}