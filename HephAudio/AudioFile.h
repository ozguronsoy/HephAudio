#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <memory>

namespace HephAudio
{
	class AudioFile final
	{
	private:
		FILE* pFile;
		uint32_t fileSize;
		void* dataBuffer;
		StringBuffer filePath;
	public:
		AudioFile();
		AudioFile(StringBuffer filePath);
		AudioFile(const AudioFile&) = delete;
		AudioFile& operator=(const AudioFile&) = delete;
		~AudioFile();
		uint32_t Size() const noexcept;
		StringBuffer Name() const noexcept;
		StringBuffer Extension() const noexcept;
		void* GetInnerBufferAddress() const noexcept;
		bool IsEmpty() const noexcept;
		void Read(StringBuffer filePath);
		void Write(void* dataBuffer, size_t fileSize);
	private:
		void Release(bool releaseFile, bool releaseDataBuffer);
	public:
		static bool FileExists(StringBuffer filePath);
		static std::shared_ptr<AudioFile> CreateNew(StringBuffer filePath, bool overwrite);
		static StringBuffer GetFileName(StringBuffer filePath);
		static StringBuffer GetFileExtension(StringBuffer filePath);
	};
}