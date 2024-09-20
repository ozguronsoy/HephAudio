#pragma once
#include "HephAudioShared.h"
#include "EncodedAudioBuffer.h"
#include "FFmpegAudioShared.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief stores the audio data that's encoded via [FFmpeg](https://www.ffmpeg.org/).
	 * 
	 */
	class HEPH_API FFmpegEncodedAudioBuffer final : public EncodedAudioBuffer
	{
	private:
		using EncodedAudioBuffer::Resize;

	private:
		size_t frameCount;
		size_t extraDataSize;
		void* extraData;
		size_t blockAlign;

	public:
		/** @copydoc default_constructor */
		FFmpegEncodedAudioBuffer();

		/** 
		 * @copydoc constructor 
		 * 
		 * @param formatInfo audio format info of the encoded data.
		 */
		explicit FFmpegEncodedAudioBuffer(const AudioFormatInfo& formatInfo);

		FFmpegEncodedAudioBuffer(const FFmpegEncodedAudioBuffer&) = delete;

		/** @copydoc move_constructor */
		FFmpegEncodedAudioBuffer(FFmpegEncodedAudioBuffer&& rhs) noexcept;

		/** @copydoc destructor */
		~FFmpegEncodedAudioBuffer();

		FFmpegEncodedAudioBuffer& operator=(const FFmpegEncodedAudioBuffer&) = delete;
		FFmpegEncodedAudioBuffer& operator=(FFmpegEncodedAudioBuffer&& rhs) noexcept;
		
		/**
		 * gets the packet at the provided index.
		 * 
		 * @param index index of the packet.
		 */
		AVPacket*& operator[](size_t index) const;

		void Release() override;

		/**
		 * gets the frame count.
		 * 
		 */
		size_t GetFrameCount() const;

		/**
		 * gets the block align.
		 * 
		 */
		size_t GetBlockAlign() const;

		/**
		 * sets the block align.
		 * 
		 */
		void SetBlockAlign(size_t blockAlign);

		/**
		 * gets the extra data size.
		 * 
		 */
		size_t GetExtraDataSize() const;

		/**
		 * gets the pointer to extra data.
		 * 
		 */
		void* GetExtraData() const;

		/**
		 * sets the extra data.
		 * 
		 */
		void SetExtraData(void* pExtraData, size_t extraDataSize);

		/**
		 * adds a new packet to the buffer.
		 * 
		 * @param frameCount number of frames the packet contains.
		 */
		void Add(AVPacket* packet, size_t frameCount);

		/**
		 * gets the pointer to the first packet.
		 * 
		 */
		AVPacket** begin() const;

		/**
		 * gets the pointer to the end of the last packet.
		 * 
		 */
		AVPacket** end() const;
	};
}