#pragma once
#include "HephAudioShared.h"
#include "AudioFormatInfo.h"
#include "FloatBuffer.h"

namespace HephAudio
{
	class AudioBuffer final
	{
		friend class AudioProcessor;
	private:
		AudioFormatInfo formatInfo;
		size_t frameCount;
		heph_audio_sample_t* pData;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, const AudioFormatInfo& formatInfo);
		AudioBuffer(size_t frameCount, const AudioChannelLayout& channelLayout, uint32_t sampleRate);
		AudioBuffer(std::nullptr_t rhs);
		AudioBuffer(const AudioBuffer& rhs);
		AudioBuffer(AudioBuffer&& rhs) noexcept;
		~AudioBuffer();
		heph_audio_sample_t* operator[](size_t frameIndex) const;
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
		const AudioFormatInfo& FormatInfo() const;
		void SetFormatInfo(const AudioChannelLayout& channelLayout, uint32_t sampleRate);
		void SetFormatInfo(const AudioFormatInfo& audioFormatInfo);
		void SetChannelLayout(const AudioChannelLayout& channelLayout);
		void SetSampleRate(uint32_t sampleRate);
		void SetBitsPerSample(uint16_t bitsPerSample);
		void SetBitRate(uint32_t bitRate);
		AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const AudioBuffer& buffer);
		void Insert(const AudioBuffer& buffer, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const AudioBuffer& buffer, size_t frameIndex);
		void Replace(const AudioBuffer& buffer, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		void Release();
		heph_audio_sample_t Min() const;
		heph_audio_sample_t Max() const;
		heph_audio_sample_t AbsMax() const;
		heph_float Rms() const;
		AudioBuffer Convolve(const HephCommon::FloatBuffer& h) const;
		AudioBuffer Convolve(const HephCommon::FloatBuffer& h, HephCommon::ConvolutionMode convolutionMode) const;
		AudioBuffer Convolve(const AudioBuffer& h) const;
		AudioBuffer Convolve(const AudioBuffer& h, HephCommon::ConvolutionMode convolutionMode) const;
		void* Begin() const;
		void* End() const;
	};
}
HephAudio::AudioBuffer operator+(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator+(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator-(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator-(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator*(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator*(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator/(heph_float lhs, const HephAudio::AudioBuffer& rhs);
HephAudio::AudioBuffer operator/(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs);