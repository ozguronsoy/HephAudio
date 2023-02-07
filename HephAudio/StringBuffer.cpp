#include "StringBuffer.h"
#include "AudioException.h"
#include <cstring>
#include <ctype.h>
#include <memory>

#define TO_STRING_BUFFER_SIZE 512

namespace HephAudio
{
	StringBuffer::StringBuffer()
	{
		this->charSize = sizeof(char);
		this->size = 0;
		this->pData = (char*)malloc(this->charSize);
		if (this->pData != nullptr)
		{
			this->pData[0] = '\0';
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
		}
	}
	StringBuffer::StringBuffer(const char& c)
	{
		this->charSize = sizeof(char);
		this->size = 1;
		this->pData = (char*)malloc(this->charSize * 2);
		if (this->pData != nullptr)
		{
			this->pData[0] = c;
			this->pData[1] = '\0';
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
		}
	}
	StringBuffer::StringBuffer(const wchar_t& wc)
	{
		this->charSize = sizeof(wchar_t);
		this->size = 1;
		this->pData = (char*)malloc(this->charSize * 2);
		if (this->pData != nullptr)
		{
			((wchar_t*)this->pData)[0] = wc;
			((wchar_t*)this->pData)[1] = L'\0';
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
		}
	}
	StringBuffer::StringBuffer(const char* const& str)
	{
		this->charSize = sizeof(char);

		if (str == nullptr)
		{
			this->size = 0;
			this->pData = (char*)malloc(this->charSize);
			if (this->pData != nullptr)
			{
				memset(this->pData, 0, this->charSize);
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
			}
			return;
		}

		this->size = strlen(str);

		this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
		if (this->pData != nullptr)
		{
			memcpy(this->pData, str, this->TotalSize() + this->charSize);
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
		}
	}
	StringBuffer::StringBuffer(const wchar_t* const& wstr)
	{
		this->charSize = sizeof(wchar_t);

		if (wstr == nullptr)
		{
			this->size = 0;
			this->pData = (char*)malloc(this->charSize);
			if (this->pData != nullptr)
			{
				memset(this->pData, 0, this->charSize);
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
			}
			return;
		}

		this->size = wcslen(wstr);

		this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
		if (this->pData != nullptr)
		{
			memcpy(this->pData, wstr, this->TotalSize() + this->charSize);
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
		}
	}
	StringBuffer::StringBuffer(const StringBuffer& str) : StringBuffer(str, str.charSize == sizeof(char) ? StringType::Normal : StringType::Wide) {}
	StringBuffer::StringBuffer(const StringBuffer& str, StringType stringType)
	{
		this->size = str.size;
		this->charSize = stringType == StringType::Normal ? sizeof(char) : sizeof(wchar_t);

		if (str.size > 0)
		{
			this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
			if (this->pData != nullptr)
			{
				if (this->charSize != str.charSize)
				{
					if (this->charSize == sizeof(char))
					{
						wcstombs(this->pData, (wchar_t*)str.pData, str.size + 1);
					}
					else
					{
						mbstowcs((wchar_t*)this->pData, str.pData, str.size + 1);
					}
				}
				else
				{
					memcpy(this->pData, str.pData, this->TotalSize() + this->charSize);
				}
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
			}
		}
		else
		{
			this->pData = (char*)malloc(this->charSize);
			if (this->pData != nullptr)
			{
				memset(this->pData, 0, this->charSize);
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
			}
		}
	}
	StringBuffer::StringBuffer(StringBuffer&& str) noexcept
	{
		this->charSize = str.charSize;
		this->size = str.size;
		this->pData = str.pData;

		str.charSize = 0;
		str.size = 0;
		str.pData = nullptr;
	}
	StringBuffer::~StringBuffer()
	{
		this->size = 0;
		this->charSize = 0;
		if (this->pData == nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	StringBuffer::operator char* () const
	{
		return this->c_str();
	}
	StringBuffer::operator wchar_t* () const
	{
		return this->wc_str();
	}
	StringBuffer& StringBuffer::operator=(const char& rhs)
	{
		char buffer[2] = { rhs, '\0' };
		*this = buffer;
		return *this;
	}
	StringBuffer& StringBuffer::operator=(const wchar_t& rhs)
	{
		wchar_t buffer[2] = { rhs, L'\0' };
		*this = buffer;
		return *this;
	}
	StringBuffer& StringBuffer::operator=(const char* const& rhs)
	{
		if (this->pData != rhs)
		{
			this->~StringBuffer();

			this->charSize = sizeof(char);

			if (rhs == nullptr)
			{
				this->size = 0;
				this->pData = (char*)malloc(this->charSize);
				if (this->pData != nullptr)
				{
					memset(this->pData, 0, this->charSize);
				}
				else
				{
					throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
				}
				return *this;
			}

			this->size = strlen(rhs);

			this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
			if (this->pData != nullptr)
			{
				memcpy(this->pData, rhs, this->TotalSize() + this->charSize);
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator=", L"Insufficient memory.");
			}
		}

		return *this;
	}
	StringBuffer& StringBuffer::operator=(const wchar_t* const& rhs)
	{
		if ((wchar_t*)this->pData != rhs)
		{
			this->~StringBuffer();

			this->charSize = sizeof(wchar_t);

			if (rhs == nullptr)
			{
				this->size = 0;
				this->pData = (char*)malloc(this->charSize);
				if (this->pData != nullptr)
				{
					memset(this->pData, 0, this->charSize);
				}
				else
				{
					throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
				}
				return *this;
			}

			this->size = wcslen(rhs);

			this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
			if (this->pData != nullptr)
			{
				memcpy(this->pData, rhs, this->TotalSize() + this->charSize);
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator=", L"Insufficient memory.");
			}
		}

		return *this;
	}
	StringBuffer& StringBuffer::operator=(const StringBuffer& rhs)
	{
		if (this->pData != rhs.pData)
		{
			this->~StringBuffer(); // destroy the current string to prevent memory leaks.

			this->charSize = rhs.charSize;
			this->size = rhs.size;

			if (rhs.size > 0)
			{
				this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
				if (this->pData != nullptr)
				{
					memcpy(this->pData, rhs.pData, this->TotalSize() + this->charSize);
				}
				else
				{
					throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator=", L"Insufficient memory.");
				}
			}
			else
			{
				this->size = 0;
				this->pData = (char*)malloc(this->charSize);
				if (this->pData != nullptr)
				{
					memset(this->pData, 0, this->charSize);
				}
				else
				{
					throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
				}
			}
		}

		return *this;
	}
	StringBuffer& StringBuffer::operator=(StringBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->~StringBuffer();

			this->charSize = rhs.charSize;
			this->size = rhs.size;
			this->pData = rhs.pData;

			rhs.charSize = 0;
			rhs.size = 0;
			rhs.pData = nullptr;
		}

		return *this;
	}
	StringBuffer StringBuffer::operator+(const char& rhs) const
	{
		char buffer[2] = { rhs, '\0' };
		return *this + buffer;
	}
	StringBuffer StringBuffer::operator+(const wchar_t& rhs) const
	{
		wchar_t buffer[2] = { rhs, L'\0' };
		return *this + buffer;
	}
	StringBuffer StringBuffer::operator+(const char* const& rhs) const
	{
		const size_t rhsSize = strlen(rhs);

		StringBuffer result;
		result.charSize = this->charSize;
		result.size = this->size + rhsSize;

		result.pData = (char*)malloc(result.TotalSize() + result.charSize);
		if (result.pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+", L"Insufficient memory.");
		}

		memcpy(result.pData, this->pData, this->TotalSize()); // copy *this except the \0 char

		if (result.charSize == sizeof(char)) // copy the rhs
		{
			memcpy(result.pData + this->TotalSize(), rhs, rhsSize + sizeof(char));
		}
		else
		{
			mbstowcs((wchar_t*)(result.pData + this->TotalSize()), rhs, rhsSize + 1);
		}

		return result;
	}
	StringBuffer StringBuffer::operator+(const wchar_t* const& rhs) const
	{
		const size_t rhsSize = wcslen(rhs);

		StringBuffer result;
		result.charSize = this->charSize;
		result.size = this->size + rhsSize;

		result.pData = (char*)malloc(result.TotalSize() + result.charSize);
		if (result.pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+", L"Insufficient memory.");
		}

		memcpy(result.pData, this->pData, this->TotalSize()); // copy *this except the \0 char

		if (result.charSize == sizeof(wchar_t)) // copy the rhs
		{
			memcpy(result.pData + this->TotalSize(), rhs, rhsSize * sizeof(wchar_t) + sizeof(wchar_t));
		}
		else
		{
			wcstombs(result.pData + this->TotalSize(), rhs, rhsSize + 1);
		}

		return result;
	}
	StringBuffer StringBuffer::operator+(const StringBuffer& rhs) const
	{
		if (rhs.charSize == sizeof(char))
		{
			return *this + rhs.c_str();
		}
		return *this + rhs.wc_str();
	}
	StringBuffer& StringBuffer::operator+=(const char& rhs)
	{
		char buffer[2] = { rhs, '\0' };
		*this += buffer;
		return *this;
	}
	StringBuffer& StringBuffer::operator+=(const wchar_t& rhs)
	{
		wchar_t buffer[2] = { rhs, L'\0' };
		*this += buffer;
		return *this;
	}
	StringBuffer& StringBuffer::operator+=(const char* const& rhs)
	{
		const size_t rhsSize = strlen(rhs);
		const size_t newTotalSize = this->TotalSize() + rhsSize * this->charSize;

		char* tempPtr = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (tempPtr == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+=", L"Insufficient memory.");
		}

		if (this->charSize == sizeof(char))
		{
			memcpy(tempPtr + this->TotalSize(), rhs, rhsSize + sizeof(char));
		}
		else
		{
			mbstowcs((wchar_t*)(tempPtr + this->TotalSize()), rhs, rhsSize + 1);
		}

		this->pData = tempPtr;
		this->size += rhsSize;

		return *this;
	}
	StringBuffer& StringBuffer::operator+=(const wchar_t* const& rhs)
	{
		const size_t rhsSize = wcslen(rhs);
		const size_t newTotalSize = this->TotalSize() + rhsSize * this->charSize;

		char* tempPtr = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (tempPtr == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+=", L"Insufficient memory.");
		}

		if (this->charSize == sizeof(wchar_t))
		{
			memcpy(tempPtr + this->TotalSize(), rhs, rhsSize * sizeof(wchar_t) + sizeof(wchar_t));
		}
		else
		{
			wcstombs(tempPtr + this->TotalSize(), rhs, rhsSize + 1);
		}

		this->pData = tempPtr;
		this->size += rhsSize;

		return *this;
	}
	StringBuffer& StringBuffer::operator+=(const StringBuffer& rhs)
	{
		if (rhs.charSize == sizeof(char))
		{
			*this += rhs.c_str();
		}
		else
		{
			*this += rhs.wc_str();
		}

		return *this;
	}
	bool StringBuffer::operator==(const char& rhs) const
	{
		char buffer[2] = { rhs, '\0' };
		return this->operator==(buffer);
	}
	bool StringBuffer::operator==(const wchar_t& rhs) const
	{
		wchar_t buffer[2] = { rhs, L'\0' };
		return this->operator==(buffer);
	}
	bool StringBuffer::operator==(const char* const& rhs) const
	{
		return this->charSize == sizeof(char) && strncmp(this->pData, rhs, this->size) == 0;
	}
	bool StringBuffer::operator==(const wchar_t* const& rhs) const
	{
		return this->charSize == sizeof(wchar_t) && wcsncmp((wchar_t*)this->pData, (wchar_t*)rhs, this->size) == 0;
	}
	bool StringBuffer::operator==(const StringBuffer& rhs) const
	{
		return this->charSize == rhs.charSize && this->size == rhs.size && memcmp(this->pData, rhs.pData, this->TotalSize()) == 0;
	}
	bool StringBuffer::operator!=(const char& rhs) const
	{
		char buffer[2] = { rhs, '\0' };
		return this->operator!=(buffer);
	}
	bool StringBuffer::operator!=(const wchar_t& rhs) const
	{
		wchar_t buffer[2] = { rhs, L'\0' };
		return this->operator!=(buffer);
	}
	bool StringBuffer::operator!=(const char* const& rhs) const
	{
		return this->charSize != sizeof(char) || strncmp(this->pData, rhs, this->size) != 0;
	}
	bool StringBuffer::operator!=(const wchar_t* const& rhs) const
	{
		return this->charSize != sizeof(wchar_t) || wcsncmp((wchar_t*)this->pData, (wchar_t*)rhs, this->size) != 0;
	}
	bool StringBuffer::operator!=(const StringBuffer& rhs) const
	{
		return this->charSize != rhs.charSize || this->size != rhs.size || memcmp(this->pData, rhs.pData, this->TotalSize()) != 0;
	}
	char* StringBuffer::c_str() const
	{
		return (char*)this->pData;
	}
	char* StringBuffer::fc_str()
	{
		this->SetStringType(StringType::Normal);
		return (char*)this->pData;
	}
	wchar_t* StringBuffer::wc_str() const
	{
		return (wchar_t*)this->pData;
	}
	wchar_t* StringBuffer::fwc_str()
	{
		this->SetStringType(StringType::Wide);
		return (wchar_t*)this->pData;
	}
	size_t StringBuffer::Size() const noexcept
	{
		return this->size;
	}
	StringType StringBuffer::GetStringType() const noexcept
	{
		return this->charSize == sizeof(char) ? StringType::Normal : StringType::Wide;
	}
	void StringBuffer::SetStringType(StringType newType)
	{
		const size_t newCharSize = newType == StringType::Normal ? sizeof(char) : sizeof(wchar_t);
		if (this->charSize != newCharSize)
		{
			char* tempPtr = (char*)malloc(this->size * newCharSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::ChangeStringType", L"Insufficient memory.");
			}

			if (this->charSize == sizeof(wchar_t))
			{
				wcstombs(tempPtr, (wchar_t*)this->pData, this->size + 1);
			}
			else
			{
				mbstowcs((wchar_t*)tempPtr, this->pData, this->size + 1);
			}

			free(this->pData);
			this->pData = tempPtr;
			this->charSize = newCharSize;
		}
	}
	bool StringBuffer::CompareContent(const char& c) const
	{
		char buffer[2] = { c, '\0' };
		return this->CompareContent(buffer);
	}
	bool StringBuffer::CompareContent(const wchar_t& wc) const
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->CompareContent(buffer);
	}
	bool StringBuffer::CompareContent(const char* const& str) const
	{
		if (this->charSize == sizeof(char))
		{
			return *this == str;
		}

		const StringBuffer tempBuffer = StringBuffer(*this, StringType::Normal);
		return tempBuffer == str;
	}
	bool StringBuffer::CompareContent(const wchar_t* const& wstr) const
	{
		if (this->charSize == sizeof(wchar_t))
		{
			return *this == wstr;
		}

		const StringBuffer tempBuffer = StringBuffer(*this, StringType::Wide);
		return tempBuffer == wstr;
	}
	bool StringBuffer::CompareContent(const StringBuffer& str) const
	{
		if (this->charSize == str.charSize)
		{
			return *this == str;
		}

		const StringBuffer tempBuffer = StringBuffer(str, this->GetStringType());
		return *this == tempBuffer;
	}
	char StringBuffer::at(const size_t& index) const
	{
		if (this->charSize == sizeof(char))
		{
			return this->pData[index];
		}

		char c = '\0';
		wcstombs(&c, (wchar_t*)this->pData + index, 1);

		return c;
	}
	wchar_t StringBuffer::w_at(const size_t& index) const
	{
		if (this->charSize == sizeof(wchar_t))
		{
			return ((wchar_t*)this->pData)[index];
		}

		wchar_t wc = L'\0';
		mbstowcs(&wc, this->pData + index, 1);

		return wc;
	}
	StringBuffer StringBuffer::SubString(size_t startIndex, size_t size) const
	{
		if (startIndex >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::SubString", L"Index out of bounds.");
		}

		const size_t subStringTotalSize = size * this->charSize;

		void* tempPtr = (void*)malloc(subStringTotalSize + this->charSize);
		if (tempPtr == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::SubString", L"Insufficient memory.");
		}

		memcpy(tempPtr, this->pData + startIndex * this->charSize, subStringTotalSize);
		memset((char*)tempPtr + subStringTotalSize, 0, this->charSize);

		StringBuffer subString = (this->charSize == sizeof(char)) ? StringBuffer((char*)tempPtr) : StringBuffer((wchar_t*)tempPtr);

		free(tempPtr);

		return subString;
	}
	StringBuffer StringBuffer::SubString(size_t startIndex, size_t size, StringType stringType) const
	{
		StringBuffer subString = this->SubString(startIndex, size);
		subString.SetStringType(stringType);
		return subString;
	}
	size_t StringBuffer::Find(const char& c) const
	{
		return this->Find(c, 0);
	}
	size_t StringBuffer::Find(const char& c, const size_t& offset) const
	{
		char buffer[2] = { c, '\0' };
		return this->Find(buffer, offset);
	}
	size_t StringBuffer::Find(const wchar_t& wc) const
	{
		return this->Find(wc, 0);
	}
	size_t StringBuffer::Find(const wchar_t& wc, const size_t& offset) const
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->Find(buffer, offset);
	}
	size_t StringBuffer::Find(const char* const& str) const
	{
		return this->Find(str, 0);
	}
	size_t StringBuffer::Find(const char* const& str, const size_t& offset) const
	{
		if (offset >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::Find", L"Offset out of bounds.");
		}

		if (this->charSize == sizeof(char))
		{
			const char* const charPos = strstr(this->pData + offset, str);
			return charPos != nullptr ? (charPos - this->pData) : StringBuffer::npos;
		}

		const size_t strSize = strlen(str);

		wchar_t* wstr = (wchar_t*)malloc(strSize * sizeof(wchar_t) + sizeof(wchar_t));
		if (wstr == nullptr)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::Find", L"Insufficient memory.");
		}
		mbstowcs(wstr, str, strSize + 1);

		const wchar_t* const charPos = wcsstr((wchar_t*)this->pData + offset, wstr);
		const size_t index = charPos != nullptr ? (charPos - (wchar_t*)this->pData) : StringBuffer::npos;

		free(wstr);

		return index;
	}
	size_t StringBuffer::Find(const wchar_t* const& wstr) const
	{
		return this->Find(wstr, 0);
	}
	size_t StringBuffer::Find(const wchar_t* const& wstr, const size_t& offset) const
	{
		if (offset >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::Find", L"Offset out of bounds.");
		}

		if (this->charSize == sizeof(wchar_t))
		{
			const wchar_t* const charPos = wcsstr((wchar_t*)this->pData + offset, wstr);
			return charPos != nullptr ? (charPos - (wchar_t*)this->pData) : StringBuffer::npos;
		}

		const size_t strSize = wcslen(wstr);

		char* str = (char*)malloc(strSize * sizeof(char) + sizeof(char));
		if (str == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::Find", L"Insufficient memory.");
		}
		wcstombs(str, wstr, strSize + 1);

		const char* const charPos = strstr(this->pData + offset, str);
		const size_t index = charPos != nullptr ? (charPos - this->pData) : StringBuffer::npos;

		free(str);

		return index;
	}
	size_t StringBuffer::Find(const StringBuffer& str) const
	{
		return this->Find(str, 0);
	}
	size_t StringBuffer::Find(const StringBuffer& str, const size_t& offset) const
	{
		return str.charSize == sizeof(char) ? this->Find(str.pData, offset) : this->Find((wchar_t*)str.pData, offset);
	}
	bool StringBuffer::Contains(const char& c) const
	{
		return this->Find(c, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(const wchar_t& wc) const
	{
		return this->Find(wc, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(const char* const& str) const
	{
		return this->Find(str, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(const wchar_t* const& wstr) const
	{
		return this->Find(wstr, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(const StringBuffer& str) const
	{
		return this->Find(str, 0) != StringBuffer::npos;
	}
	void StringBuffer::Clear()
	{
		const size_t charSize = this->charSize;
		this->~StringBuffer();
		this->charSize = charSize;

		this->pData = (char*)malloc(this->charSize);
		if (this->pData != nullptr)
		{
			memset(this->pData, 0, this->charSize);
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::Clear", L"Insufficient memory.");
		}
	}
	std::vector<StringBuffer> StringBuffer::Split(const char& c) const
	{
		char buffer[2] = { c, '\0' };
		return this->Split(buffer);
	}
	std::vector<StringBuffer> StringBuffer::Split(const wchar_t& wc) const
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->Split(buffer);
	}
	std::vector<StringBuffer> StringBuffer::Split(const char* const& str) const
	{
		std::vector<StringBuffer> result;
		const size_t strSize = strlen(str);
		size_t position = StringBuffer::npos;
		size_t cursor = 0;

		if (strSize > 0)
		{
			do
			{
				position = this->Find(str, cursor);
				if (position == StringBuffer::npos)
				{
					result.push_back(this->SubString(cursor, this->size - cursor));
					return result;
				}
				result.push_back(this->SubString(cursor, position - cursor));
				cursor = position + strSize;
			} while (cursor < this->size);
		}

		return result;
	}
	std::vector<StringBuffer> StringBuffer::Split(const wchar_t* const& wstr) const
	{
		std::vector<StringBuffer> result;
		const size_t strSize = wcslen(wstr);

		if (strSize > 0)
		{
			size_t position = StringBuffer::npos;
			size_t cursor = 0;

			do
			{
				position = this->Find(wstr, cursor);
				if (position == StringBuffer::npos)
				{
					result.push_back(this->SubString(cursor, this->size - cursor));
					return result;
				}
				result.push_back(this->SubString(cursor, position - cursor));
				cursor = position + strSize;
			} while (cursor < this->size);
		}

		return result;
	}
	std::vector<StringBuffer> StringBuffer::Split(const StringBuffer& str) const
	{
		if (this->charSize == sizeof(char))
		{
			return this->Split(str.c_str());
		}
		return this->Split(str.wc_str());
	}
	void StringBuffer::Reverse()
	{
		const size_t hs = this->size * 0.5;
		if (this->charSize == sizeof(char))
		{
			for (size_t i = 0, ir = this->size - 1; i < hs; i++, ir--)
			{
				const char temp = this->pData[i];
				this->pData[i] = this->pData[ir];
				this->pData[ir] = temp;
			}
		}
		else
		{
			for (size_t i = 0, ir = this->size - 1; i < hs; i++, ir--)
			{
				const wchar_t temp = ((wchar_t*)this->pData)[i];
				((wchar_t*)this->pData)[i] = ((wchar_t*)this->pData)[ir];
				((wchar_t*)this->pData)[ir] = temp;
			}
		}
	}
	void StringBuffer::ReplaceAt(size_t index, const char& c)
	{
		char buffer[2] = { c, '\0' };
		this->ReplaceAt(index, buffer);
	}
	void StringBuffer::ReplaceAt(size_t index, const wchar_t& wc)
	{
		wchar_t buffer[2] = { wc, L'\0' };
		this->ReplaceAt(index, buffer);
	}
	void StringBuffer::ReplaceAt(size_t index, const char* const& str)
	{
		if (index >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::ReplaceAt", L"Index out of bounds.");
		}

		const size_t strSize = strlen(str);

		if (strSize == 0)
		{
			char* tempPtr = (char*)malloc(this->TotalSize());
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::ReplaceAt", L"Insufficient memory.");
			}
			const size_t firstPartTotalSize = index * this->charSize;

			memcpy(tempPtr, this->pData, firstPartTotalSize);
			memcpy(tempPtr + firstPartTotalSize, this->pData + firstPartTotalSize + this->charSize, this->TotalSize() - firstPartTotalSize);

			if (this->charSize == sizeof(char))
			{
				tempPtr[this->size] = '\0';
			}
			else
			{
				((wchar_t*)tempPtr)[this->size] = L'\0';
			}

			free(this->pData);
			this->pData = tempPtr;
			this->size--;
		}

		if (index + strSize >= this->size)
		{
			const size_t newTotalSize = (index + strSize) * this->charSize;

			char* tempPtr = (char*)realloc(this->pData, newTotalSize + this->charSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::ReplaceAt", L"Insufficient memory.");
			}

			this->size = index + strSize;
		}

		if (this->charSize == sizeof(char))
		{
			memcpy(this->pData + index, str, strSize * sizeof(char));
			this->pData[this->size] = '\0';
		}
		else
		{
			mbstowcs((wchar_t*)this->pData + index, str, strSize);
			((wchar_t*)this->pData)[this->size] = L'\0';
		}
	}
	void StringBuffer::ReplaceAt(size_t index, const wchar_t* const& wstr)
	{
		if (index >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::ReplaceAt", L"Index out of bounds.");
		}

		const size_t strSize = wcslen(wstr);

		if (strSize == 0)
		{
			char* tempPtr = (char*)malloc(this->TotalSize());
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::ReplaceAt", L"Insufficient memory.");
			}
			const size_t firstPartTotalSize = index * this->charSize;

			memcpy(tempPtr, this->pData, firstPartTotalSize);
			memcpy(tempPtr + firstPartTotalSize, this->pData + firstPartTotalSize + this->charSize, this->TotalSize() - firstPartTotalSize);

			if (this->charSize == sizeof(char))
			{
				tempPtr[this->size] = '\0';
			}
			else
			{
				((wchar_t*)tempPtr)[this->size] = L'\0';
			}

			free(this->pData);
			this->pData = tempPtr;
			this->size--;
		}

		if (index + strSize >= this->size)
		{
			const size_t newTotalSize = (index + strSize) * this->charSize;

			char* tempPtr = (char*)realloc(this->pData, newTotalSize + this->charSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::ReplaceAt", L"Insufficient memory.");
			}

			this->size = index + strSize;
		}

		if (this->charSize == sizeof(wchar_t))
		{
			memcpy((wchar_t*)this->pData + index, wstr, strSize * sizeof(wchar_t));
			((wchar_t*)this->pData)[this->size] = L'\0';
		}
		else
		{
			wcstombs(this->pData + index, wstr, strSize);
			this->pData[this->size] = '\0';
		}
	}
	void StringBuffer::ReplaceAt(size_t index, const StringBuffer& str)
	{
		if (str.charSize == sizeof(char))
		{
			this->ReplaceAt(index, str.c_str());
		}
		else
		{
			this->ReplaceAt(index, str.wc_str());
		}
	}
	void StringBuffer::ToLower()
	{
		if (this->charSize == sizeof(char))
		{
			for (size_t i = 0; i < this->size; i++)
			{
				this->pData[i] = tolower(this->pData[i]);
			}
		}
		else
		{
			for (size_t i = 0; i < this->size; i++)
			{
				((wchar_t*)this->pData)[i] = towlower(((wchar_t*)this->pData)[i]);
			}
		}
	}
	void StringBuffer::ToUpper()
	{
		if (this->charSize == sizeof(char))
		{
			for (size_t i = 0; i < this->size; i++)
			{
				this->pData[i] = toupper(this->pData[i]);
			}
		}
		else
		{
			for (size_t i = 0; i < this->size; i++)
			{
				((wchar_t*)this->pData)[i] = towupper(((wchar_t*)this->pData)[i]);
			}
		}
	}
	void* StringBuffer::Begin() const noexcept
	{
		return this->pData;
	}
	void* StringBuffer::End() const
	{
		return this->pData + this->TotalSize() + this->charSize;
	}
	size_t StringBuffer::TotalSize() const noexcept
	{
		return this->size * this->charSize;
	}
	StringBuffer StringBuffer::ToString(const int16_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%hd", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(const uint16_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%hu", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(const int32_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%d", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(const uint32_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%u", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(const int64_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%lld", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(const uint64_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%llu", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(const double& value)
	{
		return StringBuffer::ToString(value, 4);
	}
	StringBuffer StringBuffer::ToString(const double& value, size_t precision)
	{
		char format[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(format, "%c.%dlf", '%', precision);

		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, format, value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(const int16_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%#hX", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(const uint16_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%#hX", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(const int32_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%#X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(const uint32_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%#X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(const int64_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%#llX", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(const uint64_t& value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%#llX", value);

		StringBuffer result = buffer;

		return result;
	}
	int16_t StringBuffer::StringToI16(StringBuffer string)
	{
		int16_t number = 0;
		sscanf(string.fc_str(), "%hd", &number);
		return number;
	}
	uint16_t StringBuffer::StringToUI16(StringBuffer string)
	{
		uint16_t number = 0;
		sscanf(string.fc_str(), "%hu", &number);
		return number;
	}
	int32_t StringBuffer::StringToI32(StringBuffer string)
	{
		int32_t number = 0;
		sscanf(string.fc_str(), "%d", &number);
		return number;
	}
	uint32_t StringBuffer::StringToUI32(StringBuffer string)
	{
		uint32_t number = 0;
		sscanf(string.fc_str(), "%u", &number);
		return number;
	}
	int64_t StringBuffer::StringToI64(StringBuffer string)
	{
		int64_t number = 0;
		sscanf(string.fc_str(), "%lld", &number);
		return number;
	}
	uint64_t StringBuffer::StringToUI64(StringBuffer string)
	{
		uint64_t number = 0;
		sscanf(string.fc_str(), "%llu", &number);
		return number;
	}
	int16_t StringBuffer::HexStringToI16(StringBuffer hexString)
	{
		int16_t number = 0;
		sscanf(hexString.fc_str(), "%hX", &number);
		return number;
	}
	uint16_t StringBuffer::HexStringToUI16(StringBuffer hexString)
	{
		uint16_t number = 0;
		sscanf(hexString.fc_str(), "%hX", &number);
		return number;
	}
	int32_t StringBuffer::HexStringToI32(StringBuffer hexString)
	{
		int32_t number = 0;
		sscanf(hexString.fc_str(), "%X", &number);
		return number;
	}
	uint32_t StringBuffer::HexStringToUI32(StringBuffer hexString)
	{
		uint32_t number = 0;
		sscanf(hexString.fc_str(), "%X", &number);
		return number;
	}
	int64_t StringBuffer::HexStringToI64(StringBuffer hexString)
	{
		int64_t number = 0;
		sscanf(hexString.fc_str(), "%llX", &number);
		return number;
	}
	uint64_t StringBuffer::HexStringToUI64(StringBuffer hexString)
	{
		uint64_t number = 0;
		sscanf(hexString.fc_str(), "%llX", &number);
		return number;
	}
}
HephAudio::StringBuffer operator+(const char& lhs, const HephAudio::StringBuffer& rhs)
{
	HephAudio::StringBuffer result = "";
	result += lhs;
	result += rhs;
	return result;
}
HephAudio::StringBuffer operator+(const wchar_t& lhs, const HephAudio::StringBuffer& rhs)
{
	HephAudio::StringBuffer result = "";
	result += lhs;
	result += rhs;
	return result;
}
HephAudio::StringBuffer operator+(const char* const& lhs, const HephAudio::StringBuffer& rhs)
{
	HephAudio::StringBuffer result = lhs;
	result += rhs;
	return result;
}
HephAudio::StringBuffer operator+(const wchar_t* const& lhs, const HephAudio::StringBuffer& rhs)
{
	HephAudio::StringBuffer result = lhs;
	result += rhs;
	return result;
}