#pragma once
#include "HephCommonFramework.h"
#include "Complex.h"
#include "FloatBuffer.h"
#include <initializer_list>

namespace HephCommon
{
	class ComplexBuffer final
	{
	private:
		size_t frameCount;
		Complex* pData;
	public:
		ComplexBuffer();
		ComplexBuffer(size_t frameCount);
		ComplexBuffer(const std::initializer_list<heph_float>& rhs);
		ComplexBuffer(const std::initializer_list<Complex>& rhs);
		ComplexBuffer(const ComplexBuffer& rhs);
		ComplexBuffer(const FloatBuffer& rhs);
		ComplexBuffer(ComplexBuffer&& rhs) noexcept;
		~ComplexBuffer();
		Complex& operator[](size_t frameIndex) const noexcept;
		ComplexBuffer operator-() const noexcept;
		ComplexBuffer& operator=(const std::initializer_list<heph_float>& rhs);
		ComplexBuffer& operator=(const std::initializer_list<Complex>& rhs);
		ComplexBuffer& operator=(const ComplexBuffer& rhs);
		ComplexBuffer& operator=(const FloatBuffer& rhs);
		ComplexBuffer& operator=(ComplexBuffer&& rhs) noexcept;
		ComplexBuffer operator+(heph_float rhs) const noexcept;
		ComplexBuffer operator+(const Complex& rhs) const noexcept;
		ComplexBuffer operator+(const ComplexBuffer& rhs) const;
		ComplexBuffer operator+(const FloatBuffer& rhs) const;
		ComplexBuffer& operator+=(heph_float rhs);
		ComplexBuffer& operator+=(const Complex& rhs);
		ComplexBuffer& operator+=(const ComplexBuffer& rhs);
		ComplexBuffer& operator+=(const FloatBuffer& rhs);
		ComplexBuffer operator-(heph_float rhs) const noexcept;
		ComplexBuffer operator-(const Complex& rhs) const noexcept;
		ComplexBuffer operator-(const ComplexBuffer& rhs) const;
		ComplexBuffer operator-(const FloatBuffer& rhs) const;
		ComplexBuffer& operator-=(heph_float rhs);
		ComplexBuffer& operator-=(const Complex& rhs);
		ComplexBuffer& operator-=(const ComplexBuffer& rhs);
		ComplexBuffer& operator-=(const FloatBuffer& rhs);
		ComplexBuffer operator*(heph_float rhs) const noexcept;
		ComplexBuffer operator*(const Complex& rhs) const noexcept;
		ComplexBuffer operator*(const ComplexBuffer& rhs) const;
		ComplexBuffer operator*(const FloatBuffer& rhs) const;
		ComplexBuffer& operator*=(heph_float rhs) noexcept;
		ComplexBuffer& operator*=(const Complex& rhs) noexcept;
		ComplexBuffer& operator*=(const ComplexBuffer& rhs);
		ComplexBuffer& operator*=(const FloatBuffer& rhs);
		ComplexBuffer operator/(heph_float rhs) const noexcept;
		ComplexBuffer operator/(const Complex& rhs) const noexcept;
		ComplexBuffer operator/(const ComplexBuffer& rhs) const;
		ComplexBuffer operator/(const FloatBuffer& rhs) const;
		ComplexBuffer& operator/=(heph_float rhs) noexcept;
		ComplexBuffer& operator/=(const Complex& rhs) noexcept;
		ComplexBuffer& operator/=(const ComplexBuffer& rhs);
		ComplexBuffer& operator/=(const FloatBuffer& rhs);
		ComplexBuffer operator<<(size_t rhs) const noexcept;
		ComplexBuffer& operator<<=(size_t rhs) noexcept;
		ComplexBuffer operator>>(size_t rhs) const noexcept;
		ComplexBuffer& operator>>=(size_t rhs) noexcept;
		bool operator==(std::nullptr_t rhs) const noexcept;
		bool operator==(const ComplexBuffer& rhs) const noexcept;
		bool operator!=(std::nullptr_t rhs) const noexcept;
		bool operator!=(const ComplexBuffer& rhs) const noexcept;
		size_t Size() const noexcept;
		size_t FrameCount() const noexcept;
		Complex& At(size_t frameIndex) const;
		ComplexBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const ComplexBuffer& buffer);
		void Insert(const ComplexBuffer& buffer, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const ComplexBuffer& buffer, size_t frameIndex);
		void Replace(const ComplexBuffer& buffer, size_t frameIndex, size_t frameCount);
		void Reset() noexcept;
		void Resize(size_t newFrameCount);
		void Empty() noexcept;
		ComplexBuffer Convolution(const ComplexBuffer& h) const;
		Complex* Begin() const noexcept;
		Complex* End() const noexcept;
	};
};
HephCommon::FloatBuffer abs(const HephCommon::ComplexBuffer& rhs);
HephCommon::FloatBuffer phase(const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator+(heph_float lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator-(heph_float lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator*(heph_float lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator/(heph_float lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs);