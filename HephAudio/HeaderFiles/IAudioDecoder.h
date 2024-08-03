#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include <string>

namespace HephAudio
{
	class IAudioDecoder
	{
	protected:
		std::string filePath;
	protected:
		IAudioDecoder();
		IAudioDecoder(const std::string& filePath);
	public:
		virtual ~IAudioDecoder() = default;
		virtual void ChangeFile(const std::string& newFilePath) = 0;
		virtual void CloseFile() = 0;
		virtual bool IsFileOpen() const = 0;
		virtual AudioFormatInfo GetOutputFormatInfo() const = 0;
		virtual size_t GetFrameCount() const = 0;
		virtual bool Seek(size_t frameIndex) = 0;
		virtual AudioBuffer Decode() = 0;
		virtual AudioBuffer Decode(size_t frameCount) = 0;
		virtual AudioBuffer Decode(size_t frameIndex, size_t frameCount) = 0;
	};
}