#pragma once
#include "HephAudioShared.h"
#include "AudioFormatInfo.h"
#include <Buffers/ArithmeticBuffer.h>
#include "Buffers/DoubleBuffer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief class for storing the audio samples in internal format.
	 * 
	 */
	class HEPH_API AudioBuffer final : public HephCommon::SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>
	{
		friend class AudioProcessor;

	private:
		size_t frameCount;
		AudioFormatInfo formatInfo;

	public:
		/** @copydoc default_constructor */
		AudioBuffer();

		/**
		 * @copydoc constructor
		 * 
		 * @param frameCount number of frames the buffer stores.
		 * @param channelLayout channel layout of the buffer.
		 * @param sampleRate sample rate of the buffer.
		 */
		AudioBuffer(size_t frameCount, const AudioChannelLayout& channelLayout, uint32_t sampleRate);

		/**
		 * @copydoc AudioBuffer(size_t,const AudioChannelLayout&,uint32_t)
		 * 
		 * @param flags flags.
		 */
		AudioBuffer(size_t frameCount, const AudioChannelLayout& channelLayout, uint32_t sampleRate, HephCommon::BufferFlags flags);
		
		/** @copydoc copy_constructor */
		AudioBuffer(const AudioBuffer& rhs);
		
		/** @copydoc move_constructor */
		AudioBuffer(AudioBuffer&& rhs) noexcept;
		
		/** @copydoc destructor */
		~AudioBuffer();
		
		AudioBuffer& operator=(const AudioBuffer& rhs);
		AudioBuffer& operator=(AudioBuffer&& rhs) noexcept;
		AudioBuffer operator<<(size_t rhs) const override;
		AudioBuffer& operator<<=(size_t rhs) override;
		AudioBuffer operator>>(size_t rhs) const override;
		AudioBuffer& operator>>=(size_t rhs) override;
		bool operator==(const AudioBuffer& rhs) const override;

		/**
		 * gets the pointer to the first sample of the audio frame at the provided index.
		 * 
		 */
		heph_audio_sample_t* operator[](size_t frameIndex) const;
		
		void Release() override;
		AudioBuffer SubBuffer(size_t frameIndex, size_t frameCount) const override;
		void Prepend(const AudioBuffer& rhs) override;
		void Append(const AudioBuffer& rhs) override;
		void Insert(const AudioBuffer& rhs, size_t frameIndex) override;
		void Cut(size_t frameIndex, size_t frameCount) override;
		void Replace(const AudioBuffer& rhs, size_t frameIndex, size_t frameCount) override;
		void Resize(size_t newFrameCount) override;
		void Reverse() override;

		/**
		 * gets the number of audio frames the buffer stores.
		 * 
		 */
		size_t FrameCount() const;

		/**
		 * gets the \link HephAudio::AudioFormatInfo AudioFormatInfo \endlink of the buffer.
		 * 
		 */
		const AudioFormatInfo& FormatInfo() const;

		/**
		 * changes the channel layout of the buffer.
		 * 
		 */
		void SetChannelLayout(const AudioChannelLayout& channelLayout);

		/**
		 * changes the sample rate of the buffer.
		 * 
		 */
		void SetSampleRate(uint32_t sampleRate);

	private:
		static inline bool ADD_EVENT_HANDLERS = false;
		static void AddEventHandlers();
		static void ResultCreatedEventHandler(const HephCommon::EventParams& params);
		static void ResultCreatedEventHandlerBuffer(const HephCommon::EventParams& params);
	};

	// explicit instantiate for DLLs.
	template class HEPH_API HephCommon::BufferBase<AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API HephCommon::ArithmeticBuffer<AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API HephCommon::SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>;
}