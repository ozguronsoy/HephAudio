#include "AudioFile.h"
#include "AudioException.h"
#ifdef _WIN32
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#endif

using namespace HephAudio::Structs;

namespace HephAudio
{
	AudioFile::AudioFile()
	{
		hFile = nullptr;
		fileSize = 0u;
		dataBuffer = nullptr;
		filePath = L"";
	}
	AudioFile::AudioFile(std::wstring filePath) : AudioFile()
	{
#ifdef _WIN32
		WReadFile(filePath);
#endif
#ifdef __ANDROID__
		AReadFile(filePath);
#endif
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
		return hFile == nullptr && dataBuffer == nullptr && fileSize == 0u;
	}
	void AudioFile::Write(void* dataBuffer, size_t fileSize)
	{
#ifdef _WIN32
		WWrite(dataBuffer, fileSize);
#endif
	}
	void AudioFile::Release(bool releaseFile, bool releaseDataBuffer)
	{
		if (releaseFile && hFile != nullptr)
		{
#ifdef _WIN32
			BOOL result = CloseHandle(hFile);
			if (!result)
			{
				throw AudioException(E_HANDLE, L"", L"An error occurred whilst closing the audio file handle.");
			}
#endif
			hFile = nullptr;
		}
		if (releaseDataBuffer && dataBuffer != nullptr)
		{
			free(dataBuffer);
			dataBuffer = nullptr;
		}
	}
	bool AudioFile::FileExists(std::wstring filePath)
	{
#ifdef _WIN32
		return WFileExists(filePath);
#endif
#ifdef __ANDROID__
		return AFileExists(filePath);
#endif
		return false;
	}
	std::shared_ptr<AudioFile> AudioFile::CreateNew(std::wstring filePath, bool overwrite)
	{
		if (!overwrite && FileExists(filePath))
		{
			return nullptr;
		}
#ifdef _WIN32
		return WCreateNew(filePath, overwrite);
#endif
		return nullptr;
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
#ifdef _WIN32
	void AudioFile::WReadFile(std::wstring& filePath)
	{
		if (!PathFileExistsW(filePath.c_str()))
		{
			throw AudioException(E_FAIL, L"", L"File not found.");
		}
		this->filePath = filePath;
		hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			throw AudioException(E_FAIL, L"", L"An error occurred whilst opening the file.");
		}
		fileSize = GetFileSize(hFile, nullptr);
		if (fileSize == INVALID_FILE_SIZE)
		{
			Release(true, false);
			throw AudioException(E_FAIL, L"", L"An error occurred whilst reading the file size.");
		}
		dataBuffer = malloc(fileSize);
		if (dataBuffer == nullptr)
		{
			Release(true, false);
			throw AudioException(E_FAIL, L"", L"An error occurred whilst reading the file.");
		}
		DWORD bytesRead = 0;
		BOOL readResult = ReadFile(hFile, dataBuffer, fileSize, &bytesRead, nullptr);
		if (!readResult)
		{
			Release(true, true);
			throw AudioException(E_FAIL, L"", L"An error occurred whilst reading the file.");
		}
		Release(true, false);
	}
	void AudioFile::WWrite(void* dataBuffer, size_t fileSize)
	{
		DWORD bytesWritten;
		if (!WriteFile(hFile, dataBuffer, fileSize, &bytesWritten, nullptr))
		{
			throw AudioException(E_FAIL, L"", L"An error occurred whilst writing to the file.");
		}
	}
	bool AudioFile::WFileExists(std::wstring& filePath)
	{
		return PathFileExistsW(filePath.c_str());
	}
	std::shared_ptr<AudioFile> AudioFile::WCreateNew(std::wstring& filePath, bool overwrite)
	{
		std::shared_ptr<AudioFile> newFile = std::shared_ptr<AudioFile>(new AudioFile());
		newFile->filePath = filePath;
		newFile->hFile = CreateFileW(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, overwrite ? CREATE_ALWAYS : CREATE_NEW, 0, nullptr);
		if (newFile->hFile == INVALID_HANDLE_VALUE)
		{
			throw AudioException(E_FAIL, L"", L"An error occurred whilst opening the file.");
		}
		return newFile;
	}
#endif
#ifdef __ANDROID__
	void AudioFile::AReadFile(std::wstring& filePath)
	{
		FILE* pFile = fopen(std::string(filePath.begin(), filePath.end()).c_str(), "r"); // open file for read.
		if (pFile == nullptr)
		{
			throw AudioException(*__errno(), L"", L"An error occurred whilst opening the file.");
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
			fclose(pFile);
			throw AudioException(E_FAIL, L"", L"Insufficient memory.");
		}
		size_t result = fread(dataBuffer, 1, fileSize, pFile);
		if (result != fileSize)
		{
			free(dataBuffer);
			fclose(pFile);
			throw AudioException(E_FAIL, L"", L"An error occurred whilst reading the file.");
		}
		fclose(pFile);
	}
	bool AudioFile::AFileExists(std::wstring& filePath)
	{
		FILE* pFile = fopen(std::string(filePath.begin(), filePath.end()).c_str(), "r"); // open file for read operations.
		bool result = pFile != nullptr;
		fclose(pFile);
		return result;
	}
#endif
}