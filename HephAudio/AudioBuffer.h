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
		size_t frameCount;
		void* pAudioData;
		AudioFormatInfo wfx;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, AudioFormatInfo waveFormat);
		AudioBuffer(const AudioBuffer& rhs);
		AudioBuffer& operator=(const AudioBuffer& rhs);
		AudioBuffer operator+(const AudioBuffer& rhs) const;
		// Joins the rhs buffer to the end of the current buffer.
		AudioBuffer& operator+=(const AudioBuffer& rhs);
		~AudioBuffer();
		// Buffer size in byte.
		size_t Size() const noexcept;
		size_t FrameCount() const noexcept;
		int32_t GetAsInt32(size_t frameIndex, uint8_t channel) const;
		// Gets normalized sample from the buffer. (for frameIndex = 0 and channel = 0, get sample from the first frames first channel)
		double Get(size_t frameIndex, uint8_t channel) const;
		// value must be between -1 and 1.
		void Set(double value, size_t frameIndex, uint8_t channel);
		AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		// Joins the given buffer to the end of the current buffer.
		void Join(AudioBuffer buffer);
		void Insert(size_t frameIndex, AudioBuffer buffer);
		void Cut(size_t frameIndex, size_t frameCount);
		// Sets all samples in the buffer to 0.
		void Reset();
		void Resize(size_t newFrameCount);
		// Calculates the duration of the buffer in seconds.
		double CalculateDuration() const noexcept;
		AudioFormatInfo GetFormat() const noexcept;
		void SetFormat(AudioFormatInfo newFormat);
		void* GetAudioDataAddress() const noexcept;
	private:
		double GetMin() const noexcept;
		double GetMax() const noexcept;
	public:
		// Calculates the duration of the buffer in seconds.
		static double CalculateDuration(size_t frameCount, AudioFormatInfo waveFormat) noexcept;
	};
}