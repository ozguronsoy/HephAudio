#pragma once
#include "HephCommonFramework.h"
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
		FloatBuffer(const FloatBuffer& rhs);
		FloatBuffer(FloatBuffer&& rhs) noexcept;
		~FloatBuffer();
		heph_float& operator[](const size_t& frameIndex) const noexcept;
		FloatBuffer operator-() const noexcept;
		FloatBuffer& operator=(const std::initializer_list<heph_float>& rhs);
		FloatBuffer& operator=(const FloatBuffer& rhs);
		FloatBuffer& operator=(FloatBuffer&& rhs) noexcept;
		FloatBuffer operator*(const heph_float& rhs) const noexcept;
		FloatBuffer& operator*=(const heph_float& rhs) noexcept;
		FloatBuffer operator/(const heph_float& rhs) const noexcept;
		FloatBuffer& operator/=(const heph_float& rhs) noexcept;
		FloatBuffer operator<<(const size_t& rhs) const noexcept;
		FloatBuffer& operator<<=(const size_t& rhs) noexcept;
		FloatBuffer operator>>(const size_t& rhs) const noexcept;
		FloatBuffer& operator>>=(const size_t& rhs) noexcept;
		bool operator==(const FloatBuffer& rhs) const noexcept;
		bool operator!=(const FloatBuffer& rhs) const noexcept;
		size_t Size() const noexcept;
		size_t FrameCount() const noexcept;
		heph_float& At(size_t frameIndex) const;
		FloatBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const FloatBuffer& rhs);
		void Insert(const FloatBuffer& rhs, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const FloatBuffer& rhs, size_t frameIndex);
		void Replace(const FloatBuffer& rhs, size_t frameIndex, size_t frameCount);
		void Reset() noexcept;
		void Resize(size_t newFrameCount);
		void Empty() noexcept;
		heph_float Min() const noexcept;
		heph_float Max() const noexcept;
		heph_float AbsMax() const noexcept;
		heph_float Rms() const noexcept;
		heph_float* Begin() const noexcept;
		heph_float* End() const noexcept;
	};
}
HephCommon::FloatBuffer abs(const HephCommon::FloatBuffer& rhs);