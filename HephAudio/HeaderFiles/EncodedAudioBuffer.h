#pragma once
#include "HephAudioShared.h"
#include "Buffers/BufferBase.h"
#include "AudioFormatInfo.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief stores encoded audio data.
	 * 
	 */
	class EncodedAudioBuffer : protected HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>
	{
	public:
		using HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>::Size;
		using HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>::Resize;
		using HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>::begin;
		using HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>::end;

	protected:
		/**
		 * format of the encoded audio data.
		 * 
		 */
		AudioFormatInfo formatInfo;

	public:
		EncodedAudioBuffer();
		explicit EncodedAudioBuffer(const AudioFormatInfo& formatInfo);
		EncodedAudioBuffer(size_t size, const AudioFormatInfo& formatInfo);
		EncodedAudioBuffer(const uint8_t* pData, size_t size, const AudioFormatInfo& formatInfo);
		EncodedAudioBuffer(const EncodedAudioBuffer&) = delete;
		EncodedAudioBuffer(EncodedAudioBuffer&& rhs) noexcept;
		virtual ~EncodedAudioBuffer() = default;
		EncodedAudioBuffer& operator=(const EncodedAudioBuffer&) = delete;
		EncodedAudioBuffer& operator=(EncodedAudioBuffer&& rhs) noexcept;
		virtual void Release() override;

		/**
		 * gets the \link HephAudio::AudioFormatInfo AudioFormatInfo \endlink of the buffer.
		 * 
		 */
		const AudioFormatInfo& GetAudioFormatInfo() const;

		/**
		 * sets the \link HephAudio::AudioFormatInfo AudioFormatInfo \endlink of the buffer.
		 * 
		 */
		void SetAudioFormatInfo(const AudioFormatInfo& newFormatInfo);

		/**
		 * adds encoded audio data to the end of the buffer.
		 * 
		 * @param pData pointer to the encoded data that will be added.
		 * @param size number of bytes that will be added.
		 */
		void Add(const uint8_t* pData, size_t size);
	};
}