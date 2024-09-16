#pragma once
#include "HephCommonShared.h"
#include <string>
#include <memory>

/** @file */

namespace HephCommon
{
	/**
	 * @brief file open mode, can be combined with bitwise OR.
	 * 
	 */
	enum class FileOpenMode
	{
		Read = 1,
		Write = 2,
		Append = 4,
		Overwrite = 8
	};

	/**
	 * @brief class for managing files.
	 * 
	 */
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

		/**
		 * opens the file at the provided path.
		 * 
		 * @throws InvalidOperationException
		 * @throws ExternalException
		 */
		void Open(const std::string& filePath, FileOpenMode openMode);

		/**
		 * closes the file.
		 * 
		 */
		void Close();

		/**
		 * flushes the file.
		 * 
		 */
		void Flush();

		/**
		 * checks whether a file is open.
		 * 
		 * @return true if a file is currently open, otherwise false.
		 */
		bool IsOpen() const;

		/**
		 * gets the C FILE* object.
		 * 
		 */
		FILE* GetInternalFilePtr() const;

		/**
		 * gets the file size in bytes.
		 * 
		 */
		uint64_t FileSize() const;

		/**
		 * gets the full path of the file that's currently open.
		 * 
		 */
		std::string FilePath() const;

		/**
		 * gets the name of the file that's currently open.
		 * 
		 */
		std::string FileName() const;

		/**
		 * gets the extension of the file that's currently open.
		 * 
		 */
		std::string FileExtension() const;
		
		/**
		 * gets the current file position in bytes.
		 * 
		 */
		uint64_t GetOffset() const;

		/**
		 * sets the current file position in bytes.
		 * 
		 */
		void SetOffset(uint64_t offset) const;

		/**
		 * increases the current file position.
		 * 
		 * @param offset in bytes.
		 */
		void IncreaseOffset(uint64_t offset) const;

		/**
		 * decreases the current file position.
		 * 
		 * @param offset in bytes.
		 */
		void DecreaseOffset(uint64_t offset) const;

		/**
		 * reads data from the file.
		 * 
		 * @param pData pointer to the instance the data will be read to.
		 * @param dataSize number of bytes to read.
		 * @param endian endianness of the data that will be read.
		 */
		void Read(void* pData, uint8_t dataSize, Endian endian) const;

		/**
		 * reads data from the file into a buffer.
		 * 
		 * @param pBuffer pointer to the instance the data will be read to.
		 * @param elementSize size of each element in bytes.
		 * @param elementCount number of elements to read.
		 */
		void ReadToBuffer(void* pBuffer, uint8_t elementSize, uint32_t elementCount) const;

		/**
		 * writes data to the file.
		 * 
		 * @param pData pointer to the data to write.
		 * @param dataSize number of bytes to write.
		 * @param endian endianness of the data that will be written.
		 * 
		 * @throws InsufficientMemoryException
		 */
		void Write(const void* pData, uint8_t dataSize, Endian endian) const;

		/**
		 * writes data to the file from a buffer.
		 * 
		 * @param pBuffer pointer to the buffer.
		 * @param elementSize size of each element in bytes.
		 * @param elementCount number of elements to write.
		 */
		void WriteFromBuffer(const void* pData, uint8_t elementSize, uint32_t elementCount) const;
	
	private:
		void Open(FileOpenMode openMode);
	
	public:
		/**
		 * checks whether the file exists at the provided path.
		 * 
		 * @return true if a file exists at the provided path, otherwise false.
		 */
		static bool FileExists(const std::string& filePath);

		/**
		 * extracts the file name from the path.
		 * 
		 */
		static std::string GetFileName(const std::string& filePath);

		/**
		 * extracts the file extension from the path.
		 * 
		 */
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