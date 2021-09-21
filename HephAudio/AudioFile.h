#pragma once
#include "framework.h"
#include <string>
#include <memory>

namespace HephAudio
{
	class HephAudioAPI AudioFile final
	{
	private:
		void* hFile;
		uint32_t fileSize;
		void* dataBuffer;
		std::wstring filePath;
	public:
		AudioFile();
		AudioFile(std::wstring filePath);
		AudioFile(const AudioFile&) = delete;
		AudioFile& operator=(const AudioFile&) = delete;
		~AudioFile();
		uint32_t Size() const noexcept;
		std::wstring Name() const noexcept;
		std::wstring Extension() const noexcept;
		void* GetInnerBufferAddress() const noexcept;
		bool IsEmpty() const noexcept;
		void Write(void* dataBuffer, size_t fileSize);
	private:
		void Release(bool releaseFile, bool releaseDataBuffer);
	public:
		static bool FileExists(std::wstring filePath);
		static std::shared_ptr<AudioFile> CreateNew(std::wstring filePath, bool overwrite);
	public:
		static std::wstring GetFileName(std::wstring filePath);
		static std::wstring GetFileExtension(std::wstring filePath);
#ifdef _WIN32
	private:
		void WReadFile(std::wstring& filePath);
		void WWrite(void* dataBuffer, size_t fileSize);
		static bool WFileExists(std::wstring& filePath);
		static std::shared_ptr<AudioFile> WCreateNew(std::wstring& filePath, bool overwrite);
#endif
#ifdef __ANDROID__
	private:
		void AReadFile(std::wstring& filePath);
		static bool AFileExists(std::wstring& filePath);
#endif
	};
}