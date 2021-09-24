#include "AudioFile.h"
#include "AudioException.h"
#include <errno.h>

using namespace HephAudio::Structs;

namespace HephAudio
{
	AudioFile::AudioFile()
	{
		pFile = nullptr;
		fileSize = 0u;
		dataBuffer = nullptr;
		filePath = L"";
	}
	AudioFile::AudioFile(std::wstring filePath) : AudioFile()
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
	std::wstring AudioFile::Name() const noexcept
	{
		return GetFileName(filePath);
	}
	std::wstring AudioFile::Extension() const noexcept
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
	void AudioFile::Read(std::wstring filePath)
	{
		Release(true, true);
		pFile = fopen(std::string(filePath.begin(), filePath.end()).c_str(), "rb"); // open file for read.
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
	bool AudioFile::FileExists(std::wstring filePath)
	{
		FILE* pFile = fopen(std::string(filePath.begin(), filePath.end()).c_str(), "rb"); // open file for read operations (b = open as a binary file).
		if (pFile != nullptr)
		{
			fclose(pFile);
			return true;
		}
		return false;
	}
	std::shared_ptr<AudioFile> AudioFile::CreateNew(std::wstring filePath, bool overwrite)
	{
		if (!overwrite && FileExists(filePath))
		{
			return nullptr;
		}
		std::shared_ptr<AudioFile> newFile = std::shared_ptr<AudioFile>(new AudioFile());
		newFile->filePath = filePath;
		if (overwrite) // open file for write operations (b = open as a binary file, x = don't overwrite if the file already exists).
		{
			newFile->pFile = fopen(std::string(filePath.begin(), filePath.end()).c_str(), "wb");
		}
		else
		{
			newFile->pFile = fopen(std::string(filePath.begin(), filePath.end()).c_str(), "wbx");
		}
		if (newFile->pFile == nullptr)
		{
			throw AudioException(errno, L"AudioFile::CreateNew", L"An error occurred whilst creating the file.");
		}
		return newFile;
	}
	std::wstring AudioFile::GetFileName(std::wstring filePath)
	{
		uint32_t cursor = 0;
		while (true)
		{
			const size_t pos = filePath.find(L"\\", cursor);
			if (pos == std::wstring::npos)
			{
				break;
			}
			cursor = pos + 1;
		}
		return filePath.substr(cursor, filePath.size() - cursor);
	}
	std::wstring AudioFile::GetFileExtension(std::wstring filePath)
	{
		uint32_t cursor = 0;
		while (true)
		{
			const size_t pos = filePath.find(L".", cursor);
			if (pos == std::wstring::npos)
			{
				break;
			}
			cursor = pos + 1;
		}
		return filePath.substr(cursor - 1, filePath.size() - cursor + 1);
	}
}