#pragma once
#include "HephAudioShared.h"
#include "AudioFormatInfo.h"
#include "../HephCommon/HeaderFiles/FloatBuffer.h"

namespace HephAudio
{
	class AudioBuffer final
	{
		friend class AudioProcessor;
	private:
		AudioFormatInfo formatInfo;
		size_t frameCount;
		heph_audio_sample* pData;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo);
		AudioBuffer(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		AudioBuffer(std::nullptr_t rhs);
		AudioBuffer(const AudioBuffer& rhs);
		AudioBuffer(AudioBuffer&& rhs) noexcept;
		~AudioBuffer();
		heph_audio_sample* operator[](size_t frameIndex) const;
		AudioBuffer operator-() const;
		AudioBuffer& operator=(std::nullptr_t rhs);
		AudioBuffer& operator=(const AudioBuffer& rhs);
		AudioBuffer& operator=(AudioBuffer&& rhs) noexcept;
		AudioBuffer operator+(heph_float rhs) const;
		AudioBuffer operator+(const HephCommon::FloatBuffer& rhs) const;
		AudioBuffer operator+(const AudioBuffer& rhs) const;
		AudioBuffer& operator+=(heph_float rhs);
		AudioBuffer& operator+=(const HephCommon::FloatBuffer& rhs);
		AudioBuffer& operator+=(const AudioBuffer& rhs);
		AudioBuffer operator-(heph_float rhs) const;
		AudioBuffer operator-(const HephCommon::FloatBuffer& rhs) const;
		AudioBuffer operator-(const AudioBuffer& rhs) const;
		AudioBuffer& operator-=(heph_float rhs);
		AudioBuffer& operator-=(const HephCommon::FloatBuffer& rhs);
		AudioBuffer& operator-=(const AudioBuffer& rhs);
		AudioBuffer operator*(heph_float rhs) const;
		AudioBuffer operator*(const HephCommon::FloatBuffer& rhs) const;
		AudioBuffer operator*(const AudioBuffer& rhs) const;
		AudioBuffer& operator*=(heph_float rhs);
		AudioBuffer& operator*=(const HephCommon::FloatBuffer& rhs);
		AudioBuffer& operator*=(const AudioBuffer& rhs);
		AudioBuffer operator/(heph_float rhs) const;
		AudioBuffer operator/(const HephCommon::FloatBuffer& rhs) const;
		AudioBuffer operator/(const AudioBuffer& rhs) const;
		AudioBuffer& operator/=(heph_float rhs);
		AudioBuffer& operator/=(const HephCommon::FloatBuffer& rhs);
		AudioBuffer& operator/=(const AudioBuffer& rhs);
		AudioBuffer operator<<(size_t rhs) const;
		AudioBuffer& operator<<=(size_t rhs);
		AudioBuffer operator>>(size_t rhs) const;
		AudioBuffer& operator>>=(size_t rhs);
		bool operator==(std::nullptr_t rhs) const;
		bool operator==(const AudioBuffer& rhs) const;
		bool operator!=(std::nullptr_t rhs) const;
		bool operator!=(const AudioBuffer& rhs) const;
		size_t Size() const;
		size_t FrameCount() const;
		AudioFormatInfo FormatInfo() const;
		AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const AudioBuffer& buffer);
		void Insert(const AudioBuffer& buffer, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const AudioBuffer& buffer, size_t frameIndex);
		void Replace(const AudioBuffer& buffer, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		void Empty();
		heph_audio_sample Min() const;
		heph_audio_sample Max() const;
		heph_audio_sample AbsMax() const;
		heph_float Rms() const;
		AudioBuffer Convolve(const HephCommon::FloatBuffer& h) const;
		AudioBuffer Convolve(const HephCommon::FloatBuffer& h, HephCommon::ConvolutionMode convolutionMode) const;
		AudioBuffer Convolve(const AudioBuffer& h) const;
		AudioBuffer Convolve(const AudioBuffer& h, HephCommon::ConvolutionMode convolutionMode) const;
		heph_float CalculateDuration() const;
		size_t CalculateFrameIndex(heph_float t_s) const;
		void* Begin() const;
		void* End() const;
	public:
		static heph_float CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo);
		static size_t CalculateFrameIndex(heph_float t_s, AudioFormatInfo formatInfo);
	};
}
HephAudio::AudioBuffer abs(const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator+(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator+(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator-(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator-(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator*(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator*(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator/(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator/(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);