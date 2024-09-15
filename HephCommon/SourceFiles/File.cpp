#include "File.h"
#include "HephException.h"
#include <errno.h>
#include <cstdio>
#include <stdlib.h>

namespace HephCommon
{
	File::File() : pFile(nullptr), fileSize(0), filePath("") { }
	File::File(const std::string& filePath, FileOpenMode openMode) : pFile(nullptr), fileSize(0), filePath(filePath)
	{
		if (this->filePath != "")
		{
			this->Open(openMode);
			if (this->pFile == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(errno, "File::File", "An error occurred while opening the file.", "stdlib", strerror(errno)));
			}

			fseek(this->pFile, 0, SEEK_END);
			this->fileSize = ftell(this->pFile);
			fseek(this->pFile, 0, SEEK_SET);
		}
	}
	File::File(File&& rhs) noexcept : pFile(rhs.pFile), fileSize(rhs.fileSize), filePath(rhs.filePath)
	{
		rhs.pFile = nullptr;
		rhs.fileSize = 0;
		rhs.filePath = "";
	}
	File::~File()
	{
		this->Close();
	}
	File& File::operator=(File&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->pFile = rhs.pFile;
			this->fileSize = rhs.fileSize;
			this->filePath = rhs.filePath;

			rhs.pFile = nullptr;
			rhs.fileSize = 0;
			rhs.filePath = "";
		}

		return *this;
	}
	void File::Open(const std::string& filePath, FileOpenMode openMode)
	{
		if (filePath != "")
		{
			if (this->pFile != nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_OPERATION, "File::Open", "A file is already open."));
			}

			this->filePath = filePath;
			this->Open(openMode);
			if (this->pFile == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(errno, "File::Open", "An error occurred while opening the file.", "stdlib", strerror(errno)));
			}

			fseek(this->pFile, 0, SEEK_END);
			this->fileSize = ftell(this->pFile);
			fseek(this->pFile, 0, SEEK_SET);
		}
	}
	void File::Close()
	{
		if (this->pFile != nullptr)
		{
			fclose(this->pFile);
			this->pFile = nullptr;
		}
	}
	void File::Flush()
	{
		if (this->pFile != nullptr)
		{
			fflush(this->pFile);
		}
	}
	bool File::IsOpen() const
	{
		return this->pFile != nullptr;
	}
	FILE* File::GetInternalFilePtr() const
	{
		return this->pFile;
	}
	uint64_t File::FileSize() const
	{
		return this->fileSize;
	}
	std::string File::FilePath() const
	{
		return this->filePath;
	}
	std::string File::FileName() const
	{
		return File::GetFileName(this->filePath);
	}
	std::string File::FileExtension() const
	{
		return File::GetFileExtension(this->filePath);
	}
	uint64_t File::GetOffset() const
	{
		return ftell(this->pFile);
	}
	void File::SetOffset(uint64_t offset) const
	{
		fseek(this->pFile, offset, SEEK_SET);
	}
	void File::IncreaseOffset(uint64_t offset) const
	{
		fseek(this->pFile, offset, SEEK_CUR);
	}
	void File::DecreaseOffset(uint64_t offset) const
	{
		fseek(this->pFile, -(int64_t)offset, SEEK_CUR);
	}
	void File::Read(void* pData, uint8_t dataSize, Endian endian) const
	{
		fread(pData, dataSize, 1, this->pFile);
		if (endian != HEPH_SYSTEM_ENDIAN)
		{
			HephCommon::ChangeEndian((uint8_t*)pData, dataSize);
		}
	}
	void File::ReadToBuffer(void* pBuffer, uint8_t elementSize, uint32_t elementCount)const
	{
		fread(pBuffer, elementSize, elementCount, this->pFile);
	}
	void File::Write(const void* pData, uint8_t dataSize, Endian endian) const
	{
		void* pTemp = malloc(dataSize);
		if (pTemp == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "File::Write", "Insufficient memory."));
		}
		memcpy(pTemp, pData, dataSize);

		if (endian != HEPH_SYSTEM_ENDIAN)
		{
			HephCommon::ChangeEndian((uint8_t*)pTemp, dataSize);
		}
		fwrite(pTemp, dataSize, 1, this->pFile);
	}
	void File::WriteFromBuffer(const void* pData, uint8_t elementSize, uint32_t elementCount) const
	{
		fwrite(pData, elementSize, elementCount, this->pFile);
	}
	void File::Open(FileOpenMode openMode)
	{
		std::string strOpenMode = "";
		switch (openMode)
		{
		case FileOpenMode::Read:
			strOpenMode = "rb";
			break;
		case FileOpenMode::Write:
			strOpenMode = "wbx";
			break;
		case FileOpenMode::Append:
			strOpenMode = "ab";
			break;
		case FileOpenMode::Overwrite:
			strOpenMode = "wb";
			break;
		case (FileOpenMode::Read | FileOpenMode::Write):
			strOpenMode = "rb+";
			break;
		case (FileOpenMode::Read | FileOpenMode::Append):
			strOpenMode = "ab+";
			break;
		case (FileOpenMode::Read | FileOpenMode::Overwrite):
			strOpenMode = "wb+";
			break;
		default:
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "File::File", "Invalid mode."));
		}
		this->pFile = fopen(this->filePath.c_str(), strOpenMode.c_str());
	}
	bool File::FileExists(const std::string& filePath)
	{
		FILE* pFile = fopen(filePath.c_str(), "rb");
		if (pFile != nullptr)
		{
			fclose(pFile);
			return true;
		}
		return false;
	}
	std::string File::GetFileName(const std::string& filePath)
	{
#if defined(_WIN32)
		constexpr char delimiter = '\\';
#else
		constexpr char delimiter = '/';
#endif
		const size_t index = filePath.find_last_of(delimiter);
		return index == std::string::npos ? filePath : filePath.substr(index + 1);
	}
	std::string File::GetFileExtension(const std::string& filePath)
	{
		const size_t index = filePath.find_last_of('.');
		return index == std::string::npos ? filePath : filePath.substr(index + 1);
	}
}