#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "EncodedAudioBuffer.h"
#include <string>

namespace HephAudio
{
	class IAudioEncoder
	{
	protected:
		std::string filePath;
	public:
		virtual ~IAudioEncoder() = default;
		virtual void ChangeFile(const std::string& newAudioFilePath, bool overwrite) = 0;
		virtual void CloseFile() = 0;
		virtual bool IsFileOpen() const = 0;
		virtual void Encode(const AudioBuffer& bufferToEncode) = 0;
		virtual void Encode(const AudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) = 0;
		virtual void Transcode(const EncodedAudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) = 0;
	};
}