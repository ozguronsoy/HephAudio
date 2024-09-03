#pragma once
#include "HephAudioShared.h"
#include "AudioFormatInfo.h"
#include <Buffers/ArithmeticBuffer.h>
#include "Buffers/DoubleBuffer.h"

namespace HephAudio
{
	class AudioBuffer final : public HephCommon::SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>
	{
		friend class AudioProcessor;
	private:
		size_t frameCount;
		AudioFormatInfo formatInfo;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, const AudioFormatInfo& formatInfo);
		AudioBuffer(size_t frameCount, const AudioFormatInfo& formatInfo, HephCommon::BufferFlags flags);
		AudioBuffer(size_t frameCount, const AudioChannelLayout& channelLayout, uint32_t sampleRate);
		AudioBuffer(size_t frameCount, const AudioChannelLayout& channelLayout, uint32_t sampleRate, HephCommon::BufferFlags flags);
		AudioBuffer(const AudioBuffer& rhs);
		AudioBuffer(AudioBuffer&& rhs) noexcept;
		~AudioBuffer();
		AudioBuffer& operator=(const AudioBuffer& rhs);
		AudioBuffer& operator=(AudioBuffer&& rhs) noexcept;
		AudioBuffer operator<<(size_t rhs) const override;
		AudioBuffer& operator<<=(size_t rhs) override;
		AudioBuffer operator>>(size_t rhs) const override;
		AudioBuffer& operator>>=(size_t rhs) override;
		bool operator==(const AudioBuffer& rhs) const override;
		heph_audio_sample_t* operator[](size_t frameIndex) const;
		void Release() override;
		AudioBuffer SubBuffer(size_t frameIndex, size_t frameCount) const override;
		void Insert(const AudioBuffer& rhs, size_t frameIndex) override;
		void Cut(size_t frameIndex, size_t frameCount) override;
		void Replace(const AudioBuffer& rhs, size_t frameIndex, size_t frameCount) override;
		void Resize(size_t newFrameCount) override;
		void Reverse() override;
		size_t FrameCount() const;
		const AudioFormatInfo& FormatInfo() const;
		void SetFormatInfo(const AudioChannelLayout& channelLayout, uint32_t sampleRate);
		void SetFormatInfo(const AudioFormatInfo& audioFormatInfo);
		void SetChannelLayout(const AudioChannelLayout& channelLayout);
		void SetSampleRate(uint32_t sampleRate);
		void SetBitsPerSample(uint16_t bitsPerSample);
		void SetBitRate(uint32_t bitRate);
	private:
		static inline bool ADD_EVENT_HANDLERS = false;
		static void AddEventHandlers();
		static void ResultCreatedEventHandler(const HephCommon::EventParams& params);
		static void ResultCreatedEventHandlerBuffer(const HephCommon::EventParams& params);
	};
}