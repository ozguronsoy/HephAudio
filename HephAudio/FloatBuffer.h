#pragma once
#include "framework.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	class FloatBuffer final
	{
	private:
		size_t frameCount;
		hephaudio_float* pData;
	public:
		FloatBuffer();
		FloatBuffer(size_t frameCount);
		FloatBuffer(const FloatBuffer& rhs);
		FloatBuffer(FloatBuffer&& rhs) noexcept;
		~FloatBuffer();
		explicit operator AudioBuffer() const;
		hephaudio_float& operator[](const size_t& frameIndex) const;
		FloatBuffer operator-() const;
		FloatBuffer& operator=(const FloatBuffer& rhs);
		FloatBuffer& operator=(FloatBuffer&& rhs) noexcept;
		FloatBuffer operator*(const hephaudio_float& rhs) const;
		FloatBuffer& operator*=(const hephaudio_float& rhs);
		FloatBuffer operator/(const hephaudio_float& rhs) const;
		FloatBuffer& operator/=(const hephaudio_float& rhs);
		FloatBuffer operator<<(const size_t rhs) const;
		FloatBuffer& operator<<=(const size_t rhs);
		FloatBuffer operator>>(const size_t rhs) const;
		FloatBuffer& operator>>=(const size_t rhs);
		bool operator!=(const FloatBuffer& rhs) const;
		bool operator==(const FloatBuffer& rhs) const;
		size_t Size() const noexcept;
		const size_t& FrameCount() const noexcept;
		FloatBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Join(const FloatBuffer& rhs);
		void Insert(const FloatBuffer& rhs, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const FloatBuffer& rhs, size_t frameIndex);
		void Replace(const FloatBuffer& rhs, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		hephaudio_float Min() const noexcept;
		hephaudio_float Max() const noexcept;
		hephaudio_float AbsMax() const noexcept;
		hephaudio_float* const& Begin() const noexcept;
		hephaudio_float* End() const noexcept;
	};
}