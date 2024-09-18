#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "EncodedAudioBuffer.h"
#include <string>

/** @file */

namespace HephAudio
{
	/**
	 * @brief interface for the audio decoders.
	 * 
	 */
	class IAudioDecoder
	{
	protected:
		/**
		 * path of the file that's being decoded.
		 * 
		 */
		std::string filePath;

	public:
		/** @copydoc destructor */
		virtual ~IAudioDecoder() = default;

		/**
		 * changes the decoded file.
		 * 
		 */
		virtual void ChangeFile(const std::string& newFilePath) = 0;

		/**
		 * closes the file that's being decoded and releases the resources allocated for decoding.
		 * 
		 */
		virtual void CloseFile() = 0;

		/**
		 * checks whether a file is currently open for decoding.
		 * 
		 * @return true if a file is currently open, otherwise false.
		 */
		virtual bool IsFileOpen() const = 0;

		/**
		 * gets the \link HephAudio::AudioFormatInfo AudioFormatInfo \endlink the audio file will be decoded to.
		 * 
		 */
		virtual AudioFormatInfo GetOutputFormatInfo() const = 0;

		/**
		 * gets the number of audio frames the audio file contains.
		 * 
		 */
		virtual size_t GetFrameCount() const = 0;

		/**
		 * seeks an audio frame in the file.
		 * 
		 * @param frameIndex index of the audio frame.
		 * @return true if the frame is found, otherwise false.
		 * 
		 */
		virtual bool Seek(size_t frameIndex) = 0;

		/**
		 * decodes the audio file.
		 * 
		 */
		virtual AudioBuffer Decode() = 0;

		/**
		 * decodes the audio file.
		 * 
		 * @param frameCount number of frames to decode.
		 * 
		 */
		virtual AudioBuffer Decode(size_t frameCount) = 0;

		/**
		 * decodes the specified portion of the audio file.
		 * 
		 * @param frameIndex index of the first frame that will be decoded.
		 * @param frameCount number of frames to decode.
		 * 
		 */
		virtual AudioBuffer Decode(size_t frameIndex, size_t frameCount) = 0;

		/**
		 * decodes the encoded audio data in the memory.
		 * 
		 * @param encodedBuffer the buffer that contains the encoded audio data.
		 * 
		 */
		virtual AudioBuffer Decode(const EncodedAudioBuffer& encodedBuffer) = 0;
	};
}