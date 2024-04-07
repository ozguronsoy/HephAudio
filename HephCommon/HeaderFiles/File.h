#pragma once
#include "HephCommonShared.h"
#include <string>
#include <memory>

namespace HephCommon
{
	enum class FileOpenMode
	{
		Read = 1,
		Write = 2,
		Append = 4,
		Overwrite = 8
	};
	class File final
	{
	private:
		FILE* pFile;
		uint64_t fileSize;
		std::string filePath;
	public:
		File();
		File(const std::string& filePath, FileOpenMode openMode);
		File(const File&) = delete;
		File(File&& rhs) noexcept;
		~File();
		File& operator=(const File&) = delete;
		File& operator=(File&& rhs) noexcept;
		void Open(const std::string& filePath, FileOpenMode openMode);
		void Close();
		void Flush();
		bool IsOpen() const;
		FILE* GetInternalFilePtr() const;
		uint64_t FileSize() const;
		std::string FilePath() const;
		std::string FileName() const;
		std::string FileExtension() const;
		uint64_t GetOffset() const;
		void SetOffset(uint64_t offset) const;
		void IncreaseOffset(uint64_t offset) const;
		void DecreaseOffset(uint64_t offset) const;
		void Read(void* pData, uint8_t dataSize, Endian endian) const;
		void ReadToBuffer(void* pBuffer, uint8_t elementSize, uint32_t elementCount) const;
		void Write(const void* pData, uint8_t dataSize, Endian endian) const;
		void WriteFromBuffer(const void* pData, uint8_t elementSize, uint32_t elementCount) const;
	private:
		void Open(FileOpenMode openMode);
	public:
		static bool FileExists(const std::string& filePath);
		static std::string GetFileName(const std::string& filePath);
		static std::string GetFileExtension(const std::string& filePath);
	};
}
constexpr HephCommon::FileOpenMode operator|(const HephCommon::FileOpenMode& lhs, const HephCommon::FileOpenMode& rhs)
{
	return static_cast<HephCommon::FileOpenMode>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr HephCommon::FileOpenMode operator&(const HephCommon::FileOpenMode& lhs, const HephCommon::FileOpenMode& rhs)
{
	return static_cast<HephCommon::FileOpenMode>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr HephCommon::FileOpenMode operator^(const HephCommon::FileOpenMode& lhs, const HephCommon::FileOpenMode& rhs)
{
	return static_cast<HephCommon::FileOpenMode>(static_cast<uint8_t>(lhs) ^ static_cast<uint8_t>(rhs));
}