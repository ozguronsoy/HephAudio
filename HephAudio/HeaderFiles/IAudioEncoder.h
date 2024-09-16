#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "EncodedAudioBuffer.h"
#include <string>

/** @file */

namespace HephAudio
{
	/**
	 * @brief interface for the audio encoders.
	 * 
	 */
	class IAudioEncoder
	{
	protected:
		/**
		 * path of the file that will store the encoded audio data.
		 * 
		 */
		std::string filePath;

	public:
		virtual ~IAudioEncoder() = default;

		/**
		 * changes the file that will store the encoded audio data.
		 * 
		 * @param overwrite indicates whether to write over the file if it already exists.
		 */
		virtual void ChangeFile(const std::string& newAudioFilePath, bool overwrite) = 0;

		/**
		 * closes the file and releases the resources allocated for encoding.
		 * 
		 */
		virtual void CloseFile() = 0;

		/**
		 * checks whether a file is currently open.
		 * 
		 * @return true if a file is currently open, otherwise false.
		 */
		virtual bool IsFileOpen() const = 0;

		/**
		 * encodes the provided audio data and writes it to the file.
		 * 
		 */
		virtual void Encode(const AudioBuffer& bufferToEncode) = 0;

		/**
		 * encodes the provided audio data and writes it to the \a outputBuffer.
		 * 
		 * @param inputBuffer the buffer that contains the audio data that will be encoded.
		 * @param outputBuffer the buffer that will store the encoded audio data.
		 */
		virtual void Encode(const AudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) = 0;

		/**
		 * transcodes the provided audio data and writes it to the \a outputBuffer.
		 * 
		 * @param inputBuffer the buffer that contains the audio data that will be transcoded.
		 * @param outputBuffer the buffer that will store the transcoded audio data.
		 */
		virtual void Transcode(const EncodedAudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) = 0;
	};
}