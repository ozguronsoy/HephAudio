#pragma once
#include "HephCommonShared.h"
#include <initializer_list>

namespace HephCommon
{
	class FloatBuffer final
	{
	private:
		size_t frameCount;
		heph_float* pData;
	public:
		FloatBuffer();
		FloatBuffer(size_t frameCount);
		FloatBuffer(const std::initializer_list<heph_float>& rhs);
		FloatBuffer(std::nullptr_t rhs);
		FloatBuffer(const FloatBuffer& rhs);
		FloatBuffer(FloatBuffer&& rhs) noexcept;
		~FloatBuffer();
		heph_float& operator[](size_t frameIndex) const;
		FloatBuffer operator-() const;
		FloatBuffer& operator=(const std::initializer_list<heph_float>& rhs);
		FloatBuffer& operator=(std::nullptr_t rhs);
		FloatBuffer& operator=(const FloatBuffer& rhs);
		FloatBuffer& operator=(FloatBuffer&& rhs) noexcept;
		FloatBuffer operator+(heph_float rhs) const;
		FloatBuffer operator+(const FloatBuffer& rhs) const;
		FloatBuffer& operator+=(heph_float rhs);
		FloatBuffer& operator+=(const FloatBuffer& rhs);
		FloatBuffer operator-(heph_float rhs) const;
		FloatBuffer operator-(const FloatBuffer& rhs) const;
		FloatBuffer& operator-=(heph_float rhs);
		FloatBuffer& operator-=(const FloatBuffer& rhs);
		FloatBuffer operator*(heph_float rhs) const;
		FloatBuffer operator*(const FloatBuffer& rhs) const;
		FloatBuffer& operator*=(heph_float rhs);
		FloatBuffer& operator*=(const FloatBuffer& rhs);
		FloatBuffer operator/(heph_float rhs) const;
		FloatBuffer operator/(const FloatBuffer& rhs) const;
		FloatBuffer& operator/=(heph_float rhs);
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
		heph_float& At(size_t frameIndex) const;
		FloatBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const FloatBuffer& rhs);
		void Insert(const FloatBuffer& rhs, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const FloatBuffer& rhs, size_t frameIndex);
		void Replace(const FloatBuffer& rhs, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		void Empty();
		heph_float Min() const;
		heph_float Max() const;
		heph_float AbsMax() const;
		heph_float Rms() const;
		FloatBuffer Convolve(const FloatBuffer& h) const;
		FloatBuffer Convolve(const FloatBuffer& h, ConvolutionMode convolutionMode) const;
		heph_float* Begin() const;
		heph_float* End() const;
	};
}
HephCommon::FloatBuffer abs(const HephCommon::FloatBuffer& rhs);
HephCommon::FloatBuffer operator+(heph_float lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::FloatBuffer operator-(heph_float lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::FloatBuffer operator*(heph_float lhs, const HephCommon::FloatBuffer& rhs);
HephCommon::FloatBuffer operator/(heph_float lhs, const HephCommon::FloatBuffer& rhs);