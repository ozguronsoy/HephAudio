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
	class HEPH_API EncodedAudioBuffer : protected Heph::BufferBase<EncodedAudioBuffer, uint8_t>
	{
		friend class Heph::BufferBase<EncodedAudioBuffer, uint8_t>; // BufferBase cannot access its own members from inherited methods for some reason.

	public:
		using Heph::BufferBase<EncodedAudioBuffer, uint8_t>::Size;
		using Heph::BufferBase<EncodedAudioBuffer, uint8_t>::Resize;
		using Heph::BufferBase<EncodedAudioBuffer, uint8_t>::begin;
		using Heph::BufferBase<EncodedAudioBuffer, uint8_t>::end;

	protected:
		/**
		 * format of the encoded audio data.
		 * 
		 */
		AudioFormatInfo formatInfo;

	public:
		/** @copydoc default_constructor */
		EncodedAudioBuffer();

		/**
		 * @copydoc constructor
		 * 
		 * @param formatInfo audio format info of the encoded data.
		 */
		explicit EncodedAudioBuffer(const AudioFormatInfo& formatInfo);

		/**
		 * @copydoc constructor
		 * 
		 * @param size number of bytes the buffer will store.
		 * @param formatInfo audio format info of the encoded data.
		 */
		EncodedAudioBuffer(size_t size, const AudioFormatInfo& formatInfo);

		/**
		 * @copydoc constructor
		 * 
		 * @param pData pointer to the data that the buffer will store (copies the data).
		 * @param size number of bytes the buffer will store.
		 * @param formatInfo audio format info of the encoded data.
		 */
		EncodedAudioBuffer(const uint8_t* pData, size_t size, const AudioFormatInfo& formatInfo);
		
		/** @copydoc copy_constructor */
		EncodedAudioBuffer(const EncodedAudioBuffer&) = delete;
		
		/** @copydoc move_constructor */
		EncodedAudioBuffer(EncodedAudioBuffer&& rhs) noexcept;
		
		/** @copydoc destructor */
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

	template class HEPH_API Heph::BufferBase<EncodedAudioBuffer, uint8_t>;
}