#include "AudioFile.h"
#include "AudioException.h"
#include <errno.h>

namespace HephAudio
{
	AudioFile::AudioFile()
	{
		pFile = nullptr;
		fileSize = 0u;
		dataBuffer = nullptr;
		filePath = L"";
	}
	AudioFile::AudioFile(StringBuffer filePath) : AudioFile()
	{
		this->Read(filePath);
	}
	AudioFile::~AudioFile()
	{
		Release(true, true);
	}
	uint32_t AudioFile::Size() const noexcept
	{
		return fileSize;
	}
	StringBuffer AudioFile::Name() const noexcept
	{
		return GetFileName(filePath);
	}
	StringBuffer AudioFile::Extension() const noexcept
	{
		return GetFileExtension(filePath);
	}
	void* AudioFile::GetInnerBufferAddress() const noexcept
	{
		return dataBuffer;
	}
	bool AudioFile::IsEmpty() const noexcept
	{
		return pFile == nullptr && dataBuffer == nullptr && fileSize == 0u;
	}
	void AudioFile::Read(StringBuffer filePath)
	{
		Release(true, true);
#if defined(__ANDROID__)
		pFile = fopen(filePath.fc_str(), "rb");
#else
		pFile = filePath.GetStringType() == StringType::Normal ? fopen(filePath, "rb") : _wfopen(filePath, L"rb"); // open file for read.
#endif
		if (pFile == nullptr)
		{
			throw AudioException(errno, L"AudioFile::Read", L"An error occurred whilst opening the file.");
		}
		this->filePath = filePath;
		// obtain the file size.
		fseek(pFile, 0, SEEK_END);
		this->fileSize = ftell(pFile);
		rewind(pFile);
		// file size obtained.
		this->dataBuffer = malloc(fileSize);
		if (dataBuffer == nullptr)
		{
			Release(true, false);
			throw AudioException(E_FAIL, L"AudioFile::Read", L"Insufficient memory.");
		}
		size_t result = fread(dataBuffer, 1, fileSize, pFile);
		if (result != fileSize)
		{
			Release(true, true);
			throw AudioException(errno, L"AudioFile::Read", L"An error occurred whilst reading the file.");
		}
		Release(true, false);
	}
	void AudioFile::Write(void* dataBuffer, size_t fileSize)
	{
		rewind(pFile);
		size_t result = fwrite(dataBuffer, 1, fileSize, pFile);
		if (result != fileSize)
		{
			throw AudioException(errno, L"AudioFile::Write", L"An error occurred whilst writing to the file.");
		}
	}
	void AudioFile::Release(bool releaseFile, bool releaseDataBuffer)
	{
		if (releaseFile && pFile != nullptr)
		{
			fclose(pFile);
			pFile = nullptr;
		}
		if (releaseDataBuffer && dataBuffer != nullptr)
		{
			free(dataBuffer);
			dataBuffer = nullptr;
		}
		if (releaseFile && releaseDataBuffer)
		{
			fileSize = 0;
			filePath = L"";
		}
	}
	bool AudioFile::FileExists(StringBuffer filePath)
	{
#if defined(__ANDROID__)
		FILE* pFile = fopen(filePath.fc_str(), "rb");
#else
		FILE* pFile = filePath.GetStringType() == StringType::Normal ? fopen(filePath, "rb") : _wfopen(filePath, L"rb"); // open file for read operations (b = open as a binary file).
#endif
		if (pFile != nullptr)
		{
			fclose(pFile);
			return true;
		}
		return false;
	}
	std::shared_ptr<AudioFile> AudioFile::CreateNew(StringBuffer filePath, bool overwrite)
	{
		if (!overwrite && FileExists(filePath))
		{
			return nullptr;
		}
		std::shared_ptr<AudioFile> newFile = std::shared_ptr<AudioFile>(new AudioFile());
		newFile->filePath = filePath;
		if (overwrite) // open file for write operations (b = open as a binary file, x = don't overwrite if the file already exists).
		{
#if defined(__ANDROID__)
			newFile->pFile = fopen(filePath.fc_str(), "wb");
#else
			newFile->pFile = filePath.GetStringType() == StringType::Normal ? fopen(filePath, "wb") : _wfopen(filePath, L"wb");
#endif
		}
		else
		{
#if defined(__ANDROID__)
			newFile->pFile = fopen(filePath.fc_str(), "wbx");
#else
			newFile->pFile = filePath.GetStringType() == StringType::Normal ? fopen(filePath, "wbx") : _wfopen(filePath, L"wbx");
#endif
		}
		if (newFile->pFile == nullptr)
		{
			throw AudioException(errno, L"AudioFile::CreateNew", L"An error occurred whilst creating the file.");
		}
		return newFile;
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
}