#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <memory>

namespace HephAudio
{
	enum class AudioFileOpenMode : uint8_t
	{
		Read = 1,
		Write = 2,
		WriteOverride = 4
	};
	class AudioFile final
	{
	private:
		static Endian systemEndian;
	private:
		FILE* pFile;
		uint64_t fileSize;
		HephCommon::StringBuffer filePath;
	public:
		AudioFile();
		AudioFile(HephCommon::StringBuffer filePath, AudioFileOpenMode openMode);
		AudioFile(const AudioFile&) = delete;
		AudioFile& operator=(const AudioFile&) = delete;
		~AudioFile();
		uint64_t FileSize() const noexcept;
		HephCommon::StringBuffer FilePath() const;
		HephCommon::StringBuffer FileName() const;
		HephCommon::StringBuffer FileExtension() const;
		uint64_t GetOffset() const;
		void SetOffset(uint64_t offset) const;
		void IncreaseOffset(uint64_t offset) const;
		void DecreaseOffset(uint64_t offset) const;
		void Read(void* pData, uint8_t dataSize, Endian endian) const;
		void ReadToBuffer(void* pBuffer, uint8_t elementSize, uint32_t elementCount) const;
		void Write(const void* pData, uint8_t dataSize, Endian endian) const;
		void WriteToBuffer(const void* pData, uint8_t elementSize, uint32_t elementCount) const;
	private:
		void OpenFile(AudioFileOpenMode openMode);
	public:
		static bool FileExists(HephCommon::StringBuffer filePath);
		static HephCommon::StringBuffer GetFileName(HephCommon::StringBuffer filePath);
		static HephCommon::StringBuffer GetFileExtension(HephCommon::StringBuffer filePath);
		static Endian GetSystemEndian();
		static void ChangeEndian(uint8_t* pData, uint8_t dataSize);
	private:
		static Endian FindSystemEndian();
	};
}