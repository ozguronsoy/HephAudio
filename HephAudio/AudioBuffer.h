#pragma once
#include "HephAudioFramework.h"
#include "AudioFormatInfo.h"
#include "FloatBuffer.h"

namespace HephAudio
{
	class AudioBuffer final
	{
		friend class AudioProcessor;
		friend class AudioStream;
	private:
		AudioFormatInfo formatInfo;
		size_t frameCount;
		void* pAudioData;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo);
		AudioBuffer(const AudioBuffer& rhs);
		AudioBuffer(AudioBuffer&& rhs) noexcept;
		~AudioBuffer();
		heph_float* operator[](size_t frameIndex) const;
		AudioBuffer operator-() const;
		AudioBuffer& operator=(const AudioBuffer& rhs);
		AudioBuffer& operator=(AudioBuffer&& rhs) noexcept;
		AudioBuffer operator+(heph_float rhs) const noexcept;
		AudioBuffer operator+(const HephCommon::FloatBuffer& rhs) const;
		AudioBuffer operator+(const AudioBuffer& rhs) const;
		AudioBuffer& operator+=(heph_float rhs) noexcept;
		AudioBuffer& operator+=(const HephCommon::FloatBuffer& rhs);
		AudioBuffer& operator+=(const AudioBuffer& rhs);
		AudioBuffer operator-(heph_float rhs) const noexcept;
		AudioBuffer operator-(const HephCommon::FloatBuffer& rhs) const;
		AudioBuffer operator-(const AudioBuffer& rhs) const;
		AudioBuffer& operator-=(heph_float rhs) noexcept;
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
		bool operator==(std::nullptr_t rhs) const noexcept;
		bool operator==(const AudioBuffer& rhs) const;
		bool operator!=(std::nullptr_t rhs) const noexcept;
		bool operator!=(const AudioBuffer& rhs) const;
		size_t Size() const noexcept;
		size_t FrameCount() const noexcept;
		AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Append(const AudioBuffer& buffer);
		void Insert(const AudioBuffer& buffer, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const AudioBuffer& buffer, size_t frameIndex);
		void Replace(const AudioBuffer& buffer, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		void Empty() noexcept;
		heph_float Min() const noexcept;
		heph_float Max() const noexcept;
		heph_float AbsMax() const noexcept;
		heph_float Rms() const noexcept;
		heph_float CalculateDuration() const noexcept;
		size_t CalculateFrameIndex(heph_float t_s) const noexcept;
		const AudioFormatInfo& FormatInfo() const noexcept;
		void SetFormat(AudioFormatInfo newFormat);
		void SetChannelCount(uint16_t newChannelCount);
		void SetSampleRate(uint32_t newSampleRate);
		void* Begin() const noexcept;
		void* End() const noexcept;
	public:
		static heph_float CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept;
		static size_t CalculateFrameIndex(heph_float t_s, AudioFormatInfo formatInfo) noexcept;
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