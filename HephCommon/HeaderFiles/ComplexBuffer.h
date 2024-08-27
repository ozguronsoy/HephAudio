#pragma once
#include "HephCommonShared.h"
#include "Complex.h"
#include "Buffers/DoubleBuffer.h"
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
		ComplexBuffer(const std::initializer_list<double>& rhs);
		ComplexBuffer(const std::initializer_list<Complex>& rhs);
		ComplexBuffer(std::nullptr_t rhs);
		explicit ComplexBuffer(const DoubleBuffer& rhs);
		ComplexBuffer(const ComplexBuffer& rhs);
		ComplexBuffer(ComplexBuffer&& rhs) noexcept;
		~ComplexBuffer();
		Complex& operator[](size_t frameIndex) const;
		ComplexBuffer operator-() const;
		ComplexBuffer& operator=(const std::initializer_list<double>& rhs);
		ComplexBuffer& operator=(const std::initializer_list<Complex>& rhs);
		ComplexBuffer& operator=(std::nullptr_t rhs);
		ComplexBuffer& operator=(const DoubleBuffer& rhs);
		ComplexBuffer& operator=(const ComplexBuffer& rhs);
		ComplexBuffer& operator=(ComplexBuffer&& rhs) noexcept;
		ComplexBuffer operator+(double rhs) const;
		ComplexBuffer operator+(const Complex& rhs) const;
		ComplexBuffer operator+(const ComplexBuffer& rhs) const;
		ComplexBuffer operator+(const DoubleBuffer& rhs) const;
		ComplexBuffer& operator+=(double rhs);
		ComplexBuffer& operator+=(const Complex& rhs);
		ComplexBuffer& operator+=(const ComplexBuffer& rhs);
		ComplexBuffer& operator+=(const DoubleBuffer& rhs);
		ComplexBuffer operator-(double rhs) const;
		ComplexBuffer operator-(const Complex& rhs) const;
		ComplexBuffer operator-(const ComplexBuffer& rhs) const;
		ComplexBuffer operator-(const DoubleBuffer& rhs) const;
		ComplexBuffer& operator-=(double rhs);
		ComplexBuffer& operator-=(const Complex& rhs);
		ComplexBuffer& operator-=(const ComplexBuffer& rhs);
		ComplexBuffer& operator-=(const DoubleBuffer& rhs);
		ComplexBuffer operator*(double rhs) const;
		ComplexBuffer operator*(const Complex& rhs) const;
		ComplexBuffer operator*(const ComplexBuffer& rhs) const;
		ComplexBuffer operator*(const DoubleBuffer& rhs) const;
		ComplexBuffer& operator*=(double rhs);
		ComplexBuffer& operator*=(const Complex& rhs);
		ComplexBuffer& operator*=(const ComplexBuffer& rhs);
		ComplexBuffer& operator*=(const DoubleBuffer& rhs);
		ComplexBuffer operator/(double rhs) const;
		ComplexBuffer operator/(const Complex& rhs) const;
		ComplexBuffer operator/(const ComplexBuffer& rhs) const;
		ComplexBuffer operator/(const DoubleBuffer& rhs) const;
		ComplexBuffer& operator/=(double rhs);
		ComplexBuffer& operator/=(const Complex& rhs);
		ComplexBuffer& operator/=(const ComplexBuffer& rhs);
		ComplexBuffer& operator/=(const DoubleBuffer& rhs);
		ComplexBuffer operator<<(size_t rhs) const;
		ComplexBuffer& operator<<=(size_t rhs);
		ComplexBuffer operator>>(size_t rhs) const;
		ComplexBuffer& operator>>=(size_t rhs);
		bool operator==(std::nullptr_t rhs) const;
		bool operator==(const ComplexBuffer& rhs) const;
		bool operator!=(std::nullptr_t rhs) const;
		bool operator!=(const ComplexBuffer& rhs) const;
		size_t Size() const;
		size_t FrameCount() const;
		Complex& At(size_t frameIndex) const;
		ComplexBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const ComplexBuffer& buffer);
		void Insert(const ComplexBuffer& buffer, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const ComplexBuffer& buffer, size_t frameIndex);
		void Replace(const ComplexBuffer& buffer, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		void Release();
		Complex* Begin() const;
		Complex* End() const;
	};
};
HephCommon::ComplexBuffer operator+(double lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator-(double lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator*(double lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator/(double lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator-(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs);
HephCommon::ComplexBuffer operator/(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs);