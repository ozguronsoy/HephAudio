#pragma once
#include "framework.h"
#include "int24.h"
#include "AudioFormatInfo.h"
#include <vector>

using namespace HephAudio::Structs;

namespace HephAudio
{
	class AudioBuffer final
	{
		friend class AudioProcessor;
	private:
		std::vector<uint8_t> buffer;
		AudioFormatInfo wfx;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, AudioFormatInfo waveFormat);
		// Buffer size in byte.
		size_t Size() const noexcept;
		size_t FrameCount() const noexcept;
		int32_t GetAsInt32(uint32_t frameIndex, uint8_t channel) const;
		// Gets normalized sample from the buffer. (for frameIndex = 0 and channel = 0, get sample from the first frames first channel)
		double Get(uint32_t frameIndex, uint8_t channel) const;
		// value must be between -1 and 1.
		void Set(double value, uint32_t frameIndex, uint8_t channel);
		AudioBuffer GetSubBuffer(uint32_t frameIndex, size_t frameCount) const;
		void Join(AudioBuffer b);
		void Cut(uint32_t frameIndex, size_t frameCount);
		// Sets all samples in the buffer to 0.
		void Reset();
		// Calculates the duration of the buffer in seconds.
		double CalculateDuration() const noexcept;
		AudioFormatInfo GetFormat() const noexcept;
		void* GetInnerBufferAddress() const noexcept;
	private:
		double GetMin() const noexcept;
		double GetMax() const noexcept;
	public:
		AudioBuffer& operator+=(const AudioBuffer& rhs); // Joins buffers.
	public:
		// Calculates the duration of the buffer in seconds.
		static double CalculateDuration(size_t frameCount, AudioFormatInfo waveFormat) noexcept;
	};
}