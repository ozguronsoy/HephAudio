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
		/// <summary>
		/// Calculates the magnitude of the complex number.
		/// </summary>
		/// <returns>The magnitude of the complex number.</returns>
		hephaudio_float Magnitude() const noexcept;
		/// <summary>
		/// Gets the phase angle in radians.
		/// </summary>
		/// <returns>The phase angle in radians.</returns>
		hephaudio_float Phase() const noexcept;
	};
}