#include "AudioFile.h"
#include "AudioException.h"
#include <errno.h>

namespace HephAudio
{
	Endian AudioFile::systemEndian = AudioFile::FindSystemEndian();
	AudioFile::AudioFile()
		: pFile(nullptr), fileSize(0), filePath("") { }
	AudioFile::AudioFile(StringBuffer filePath, AudioFileOpenMode openMode)
		: pFile(nullptr), fileSize(0), filePath(filePath)
	{
		this->OpenFile(openMode);
		if (this->pFile == nullptr)
		{
			throw AudioException(errno, L"AudioFile::AudioFile", L"An error occurred whilst opening the file.");
		}

		fseek(this->pFile, 0, SEEK_END);
		this->fileSize = ftell(this->pFile);
		fseek(this->pFile, 0, SEEK_SET);
	}
	AudioFile::~AudioFile()
	{
		if (this->pFile != nullptr)
		{
			fclose(this->pFile);
			this->pFile = nullptr;
		}
	}
	uint64_t AudioFile::FileSize() const noexcept
	{
		return this->fileSize;
	}
	StringBuffer AudioFile::FilePath() const
	{
		return this->filePath;
	}
	StringBuffer AudioFile::FileName() const
	{
		return AudioFile::GetFileName(this->filePath);
	}
	StringBuffer AudioFile::FileExtension() const
	{
		return AudioFile::GetFileExtension(this->filePath);
	}
	uint64_t AudioFile::GetOffset() const
	{
		return ftell(this->pFile);
	}
	void AudioFile::SetOffset(uint64_t offset) const
	{
		fseek(this->pFile, offset, SEEK_SET);
	}
	void AudioFile::IncreaseOffset(uint64_t offset) const
	{
		fseek(this->pFile, offset, SEEK_CUR);
	}
	void AudioFile::DecreaseOffset(uint64_t offset) const
	{
		fseek(this->pFile, -(int64_t)offset, SEEK_CUR);
	}
	void AudioFile::Read(void* pData, uint8_t dataSize, Endian endian) const
	{
		fread(pData, dataSize, 1, this->pFile);
		if (endian != AudioFile::systemEndian)
		{
			AudioFile::ChangeEndian((uint8_t*)pData, dataSize);
		}
	}
	void AudioFile::ReadToBuffer(void* pBuffer, uint8_t elementSize, uint32_t elementCount)const
	{
		fread(pBuffer, elementSize, elementCount, this->pFile);
	}
	void AudioFile::Write(const void* pData, uint8_t dataSize, Endian endian) const
	{
		void* pTemp = malloc(dataSize);
		if (pTemp == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "AudioFile::Write", "Insufficient memory.");
		}
		memcpy(pTemp, pData, dataSize);

		if (endian != AudioFile::systemEndian)
		{
			AudioFile::ChangeEndian((uint8_t*)pTemp, dataSize);
		}
		fwrite(pTemp, dataSize, 1, this->pFile);
	}
	void AudioFile::WriteToBuffer(const void* pData, uint8_t elementSize, uint32_t elementCount) const
	{
		fwrite(pData, elementSize, elementCount, this->pFile);
	}
	void AudioFile::OpenFile(AudioFileOpenMode openMode)
	{
		StringBuffer strOpenMode = "";
		switch (openMode)
		{
		case AudioFileOpenMode::Read:
			strOpenMode = "rb";
			break;
		case AudioFileOpenMode::Write:
			strOpenMode = "wbx";
			break;
		case AudioFileOpenMode::WriteOverride:
			strOpenMode = "wb";
			break;
		default:
			throw AudioException(E_INVALIDARG, "AudioFile::AudioFile", "Invalid mode.");
		}

#if defined(__ANDROID__)
		this->pFile = fopen(this->filePath.fc_str(), strOpenMode.fc_str());
#else
		this->pFile = this->filePath.GetStringType() == StringType::ASCII ? fopen(this->filePath, strOpenMode.fc_str()) : _wfopen(this->filePath, strOpenMode.fwc_str());
#endif
	}
	bool AudioFile::FileExists(StringBuffer filePath)
	{
		const AudioFile audioFile = AudioFile(filePath, AudioFileOpenMode::Read);
		return audioFile.pFile != nullptr;
	}
	StringBuffer AudioFile::GetFileName(StringBuffer filePath)
	{
#if defined(__ANDROID__)
		std::vector<StringBuffer> sfp = filePath.Split('/');
#else
		std::vector<StringBuffer> sfp = filePath.Split('\\');
#endif
		return sfp.at(sfp.size() - 1);
	}
	StringBuffer AudioFile::GetFileExtension(StringBuffer filePath)
	{
		std::vector<StringBuffer> sfp = filePath.Split('.');
		return '.' + sfp.at(sfp.size() - 1);
	}
	Endian AudioFile::GetSystemEndian()
	{
		return AudioFile::systemEndian;
	}
	void AudioFile::ChangeEndian(uint8_t* pData, uint8_t dataSize)
	{
		const uint8_t halfDataSize = dataSize / 2;
		for (size_t i = 0; i < halfDataSize; i++)
		{
			const uint8_t temp = pData[i];
			pData[i] = pData[dataSize - i - 1];
			pData[dataSize - i - 1] = temp;
		}
	}
	Endian AudioFile::FindSystemEndian()
	{
		uint16_t n = 1;
		return (*(uint8_t*)&n == 1) ? Endian::Little : Endian::Big;
	}
}