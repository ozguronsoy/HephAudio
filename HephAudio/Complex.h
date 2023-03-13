#pragma once
#include "framework.h"

namespace HephAudio
{
	/// <summary>
	/// Struct for representing complex numbers.
	/// </summary>
	struct Complex final
	{
		/// <summary>
		/// The real part of the complex number.
		/// </summary>
		hephaudio_float real;
		/// <summary>
		/// The imaginary part of the complex number.
		/// </summary>
		hephaudio_float imaginary;
		/// <summary>
		/// Creates and initializes a Complex instance with default values.
		/// </summary>
		Complex();
		/// <summary>
		/// Creates and initializes a Complex instance with the provided values.
		/// </summary>
		/// <param name="real">The real part of the complex number.</param>
		/// <param name="imaginary">The imaginary part of the complex number.</param>
		Complex(hephaudio_float real, hephaudio_float imaginary);
		/// <summary>
		/// Creates and initializes a Complex instance with rhs's values.
		/// </summary>
		Complex(const Complex& rhs);
		Complex& operator=(const Complex& rhs);
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
		/// <summary>
		/// Creates the complex conjugate of the current complex number.
		/// </summary>
		/// <returns>The complex conjugate of the current complex number.</returns>
		Complex Conjugate() const noexcept;
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
hephaudio_float abs(const HephAudio::Complex& rhs);
hephaudio_float phase(const HephAudio::Complex& rhs);