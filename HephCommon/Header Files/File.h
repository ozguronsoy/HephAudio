#pragma once
#include "HephCommonFramework.h"
#include "StringBuffer.h"
#include <memory>

namespace HephCommon
{
	enum class FileOpenMode : uint8_t
	{
		Read = 1,
		Write = 2,
		WriteOverride = 4
	};
	class File final
	{
	private:
		static Endian systemEndian;
	private:
		FILE* pFile;
		uint64_t fileSize;
		StringBuffer filePath;
	public:
		File();
		File(StringBuffer filePath, FileOpenMode openMode);
		File(const File&) = delete;
		File& operator=(const File&) = delete;
		~File();
		uint64_t FileSize() const noexcept;
		StringBuffer FilePath() const;
		StringBuffer FileName() const;
		StringBuffer FileExtension() const;
		uint64_t GetOffset() const;
		void SetOffset(uint64_t offset) const;
		void IncreaseOffset(uint64_t offset) const;
		void DecreaseOffset(uint64_t offset) const;
		void Read(void* pData, uint8_t dataSize, Endian endian) const;
		void ReadToBuffer(void* pBuffer, uint8_t elementSize, uint32_t elementCount) const;
		void Write(const void* pData, uint8_t dataSize, Endian endian) const;
		void WriteToBuffer(const void* pData, uint8_t elementSize, uint32_t elementCount) const;
	private:
		void OpenFile(FileOpenMode openMode);
	public:
		static bool FileExists(StringBuffer filePath);
		static StringBuffer GetFileName(StringBuffer filePath);
		static StringBuffer GetFileExtension(StringBuffer filePath);
		static Endian GetSystemEndian();
		static void ChangeEndian(uint8_t* pData, uint8_t dataSize);
	private:
		static Endian FindSystemEndian();
	};
}