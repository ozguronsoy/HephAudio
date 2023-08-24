#include "File.h"
#include "HephException.h"
#include <errno.h>

namespace HephCommon
{
	File::File()
		: pFile(nullptr), fileSize(0), filePath("") { }
	File::File(StringBuffer filePath, FileOpenMode openMode)
		: pFile(nullptr), fileSize(0), filePath(filePath)
	{
		this->OpenFile(openMode);
		if (this->pFile == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(errno, L"File::File", L"An error occurred whilst opening the file."));
		}

		fseek(this->pFile, 0, SEEK_END);
		this->fileSize = ftell(this->pFile);
		fseek(this->pFile, 0, SEEK_SET);
	}
	File::~File()
	{
		if (this->pFile != nullptr)
		{
			fclose(this->pFile);
			this->pFile = nullptr;
		}
	}
	uint64_t File::FileSize() const noexcept
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "File::Write", "Insufficient memory."));
		}
		memcpy(pTemp, pData, dataSize);

		if (endian != HEPH_SYSTEM_ENDIAN)
		{
			HephCommon::ChangeEndian((uint8_t*)pTemp, dataSize);
		}
		fwrite(pTemp, dataSize, 1, this->pFile);
	}
	void File::WriteToBuffer(const void* pData, uint8_t elementSize, uint32_t elementCount) const
	{
		fwrite(pData, elementSize, elementCount, this->pFile);
	}
	void File::OpenFile(FileOpenMode openMode)
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
		case FileOpenMode::WriteOverride:
			strOpenMode = "wb";
			break;
		default:
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "File::File", "Invalid mode."));
		}
		this->pFile = fopen(this->filePath.fc_str(), strOpenMode.fc_str());
	}
	bool File::FileExists(StringBuffer filePath)
	{
		const File audioFile(filePath, FileOpenMode::Read);
		return audioFile.pFile != nullptr;
	}
	StringBuffer File::GetFileName(StringBuffer filePath)
	{
#if defined(__ANDROID__) || defined(__linux__) || defined(__APPLE__)
		std::vector<StringBuffer> sfp = filePath.Split('/');
#else
		std::vector<StringBuffer> sfp = filePath.Split('\\');
#endif
		return sfp.at(sfp.size() - 1);
	}
	StringBuffer File::GetFileExtension(StringBuffer filePath)
	{
		std::vector<StringBuffer> sfp = filePath.Split('.');
		return '.' + sfp.at(sfp.size() - 1);
	}
}