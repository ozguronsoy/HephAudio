#pragma once
#include "framework.h"
#include "int24.h"
#include "AudioFormatInfo.h"
#include <vector>

namespace HephAudio
{
#pragma region Audio Frame
	class AudioBuffer;
	struct AudioFrame final
	{
		friend class AudioBuffer;
	private:
		double* pAudioData;
		size_t frameIndex;
		size_t channelCount;
		AudioFrame(void* pAudioData, size_t frameIndex, size_t channelCount);
	public:
		double& operator[](const size_t& channel) const;
	};
#pragma endregion
#pragma region Audio Buffer
	class AudioBuffer final
	{
		friend class AudioProcessor;
	private:
		AudioFormatInfo formatInfo;
		size_t frameCount;
		void* pAudioData;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo);
		AudioBuffer(const AudioBuffer& rhs);
		~AudioBuffer();
		// Only for WAVE_FORMAT_HEPHAUDIO.
		AudioFrame operator[](const size_t& frameIndex) const;
		AudioBuffer operator-() const;
		AudioBuffer& operator=(const AudioBuffer& rhs);
		// Joins the rhs buffer to the end of the current buffer and returns it as a new audio buffer.
		AudioBuffer operator+(const AudioBuffer& rhs) const;
		// Joins the rhs buffer to the end of the current buffer.
		AudioBuffer& operator+=(const AudioBuffer& rhs);
		// Multiplies all the samples in the current buffer by rhs, then returns the result as a new audio buffer.
		AudioBuffer operator*(const double& rhs) const;
		// Multiplies all the samples in the current buffer by rhs.
		AudioBuffer& operator*=(const double& rhs);
		// Divides all the samples in the current buffer by rhs, then returns the result as a new audio buffer.
		AudioBuffer operator/(const double& rhs) const;
		// Divides all the samples in the current buffer by rhs.
		AudioBuffer& operator/=(const double& rhs);
		bool operator!=(const AudioBuffer& rhs) const;
		bool operator==(const AudioBuffer& rhs) const;
		// Buffer size in byte.
		size_t Size() const noexcept;
		const size_t& FrameCount() const noexcept;
		// Gets normalized sample from the buffer.
		double Get(size_t frameIndex, uint8_t channel) const;
		// value must be between -1 and 1.
		void Set(double value, size_t frameIndex, uint8_t channel);
		AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		// Joins the given buffer to the end of the current buffer.
		void Join(AudioBuffer buffer);
		void Insert(AudioBuffer buffer, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(AudioBuffer buffer, size_t frameIndex);
		void Replace(AudioBuffer buffer, size_t frameIndex, size_t frameCount);
		// Sets all samples in the buffer to 0.
		void Reset();
		void Resize(size_t newFrameCount);
		// Calculates the duration of the buffer in seconds.
		double CalculateDuration() const noexcept;
		size_t CalculateFrameIndex(double ts) const noexcept;
		const AudioFormatInfo& FormatInfo() const noexcept;
		void SetFormat(AudioFormatInfo newFormat);
		void* const& Begin() const noexcept;
		void* End() const noexcept;
	public:
		// Calculates the duration of the buffer in seconds.
		static double CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept;
		static size_t CalculateFrameIndex(double ts, AudioFormatInfo formatInfo) noexcept;
	};
#pragma endregion
}