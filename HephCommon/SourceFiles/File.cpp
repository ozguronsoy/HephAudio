#include "File.h"
#include "HephException.h"
#include <errno.h>
#include <cstdio>
#include <stdlib.h>

namespace HephCommon
{
	File::File() : pFile(nullptr), fileSize(0), filePath(nullptr) { }
	File::File(const StringBuffer& filePath, FileOpenMode openMode) : pFile(nullptr), fileSize(0), filePath(filePath)
	{
		if (this->filePath != nullptr && this->filePath != "" && this->filePath != L"")
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
		rhs.filePath = nullptr;
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
			rhs.filePath = nullptr;
		}

		return *this;
	}
	void File::Open(const StringBuffer& filePath, FileOpenMode openMode)
	{
		if (this->pFile == nullptr)
		{
			this->filePath = filePath;
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
	StringBuffer File::FilePath() const
	{
		return this->filePath;
	}
	StringBuffer File::FileName() const
	{
		return File::GetFileName(this->filePath);
	}
	StringBuffer File::FileExtension() const
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
		StringBuffer strOpenMode = "";
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
		this->pFile = fopen(this->filePath.fc_str(), strOpenMode.fc_str());
	}
	bool File::FileExists(StringBuffer filePath)
	{
		FILE* pFile = fopen(filePath.fc_str(), "rb");
		if (pFile != nullptr)
		{
			fclose(pFile);
			return true;
		}
		return false;
	}
	StringBuffer File::GetFileName(const StringBuffer& filePath)
	{
#if defined(__ANDROID__) || defined(__linux__) || defined(__APPLE__)
		std::vector<StringBuffer> sfp = filePath.Split('/');
#else
		std::vector<StringBuffer> sfp = filePath.Split('\\');
#endif
		return sfp.at(sfp.size() - 1);
	}
	StringBuffer File::GetFileExtension(const StringBuffer& filePath)
	{
		std::vector<StringBuffer> sfp = filePath.Split('.');
		return '.' + sfp.at(sfp.size() - 1);
	}
}