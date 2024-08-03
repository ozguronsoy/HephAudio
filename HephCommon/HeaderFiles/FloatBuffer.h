#pragma once
#include "HephCommonShared.h"
#include <initializer_list>

namespace HephCommon
{
	class FloatBuffer final
	{
	private:
		size_t frameCount;
		double* pData;
	public:
		FloatBuffer();
		FloatBuffer(size_t frameCount);
		FloatBuffer(const std::initializer_list<double>& rhs);
		FloatBuffer(std::nullptr_t rhs);
		FloatBuffer(const FloatBuffer& rhs);
		FloatBuffer(FloatBuffer&& rhs) noexcept;
		~FloatBuffer();
		double& operator[](size_t frameIndex) const;
		FloatBuffer operator-() const;
		FloatBuffer& operator=(const std::initializer_list<double>& rhs);
		FloatBuffer& operator=(std::nullptr_t rhs);
		FloatBuffer& operator=(const FloatBuffer& rhs);
		FloatBuffer& operator=(FloatBuffer&& rhs) noexcept;
		FloatBuffer operator+(double rhs) const;
		FloatBuffer operator+(const FloatBuffer& rhs) const;
		FloatBuffer& operator+=(double rhs);
		FloatBuffer& operator+=(const FloatBuffer& rhs);
		FloatBuffer operator-(double rhs) const;
		FloatBuffer operator-(const FloatBuffer& rhs) const;
		FloatBuffer& operator-=(double rhs);
		FloatBuffer& operator-=(const FloatBuffer& rhs);
		FloatBuffer operator*(double rhs) const;
		FloatBuffer operator*(const FloatBuffer& rhs) const;
		FloatBuffer& operator*=(double rhs);
		FloatBuffer& operator*=(const FloatBuffer& rhs);
		FloatBuffer operator/(double rhs) const;
		FloatBuffer operator/(const FloatBuffer& rhs) const;
		FloatBuffer& operator/=(double rhs);
		FloatBuffer& operator/=(const FloatBuffer& rhs);
		FloatBuffer operator<<(size_t rhs) const;
		FloatBuffer& operator<<=(size_t rhs);
		FloatBuffer operator>>(size_t rhs) const;
		FloatBuffer& operator>>=(size_t rhs);
		bool operator==(std::nullptr_t rhs) const;
		bool operator==(const FloatBuffer& rhs) const;
		bool operator!=(std::nullptr_t rhs) const;
		bool operator!=(const FloatBuffer& rhs) const;
		size_t Size() const;
		size_t FrameCount() const;
		double& At(size_t frameIndex) const;
		FloatBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const FloatBuffer& rhs);
		void Insert(const FloatBuffer& rhs, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const FloatBuffer& rhs, size_t frameIndex);
		void Replace(const FloatBuffer& rhs, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		void Release();
		double Min() const;
		double Max() const;
		double AbsMax() const;
		double Rms() const;
		FloatBuffer Convolve(const FloatBuffer& h) const;
		FloatBuffer Convolve(const FloatBuffer& h, ConvolutionMode convolutionMode) const;
		double* Begin() const;
		double* End() const;
	};
}
HephCommon::FloatBuffer operator+(double lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::FloatBuffer operator-(double lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::FloatBuffer operator*(double lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::FloatBuffer operator/(double lhs, const HephCommon::FloatBuffer& rhs);