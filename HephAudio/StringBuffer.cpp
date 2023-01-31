#include "StringBuffer.h"
#include "AudioException.h"
#include <cstring>
#include <iostream>

namespace HephAudio
{
	StringBuffer::StringBuffer()
	{
		this->charSize = sizeof(char);
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
	StringBuffer::StringBuffer(const char* const& str)
	{
		this->charSize = sizeof(char);
		this->size = 0;
		if (str == nullptr)
		{
			this->pData = nullptr;
			return;
		}

		while (str[this->size] != '\0') // find the character count
		{
			this->size++;
		}

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
	StringBuffer::StringBuffer(const wchar_t* const& str)
	{
		this->charSize = sizeof(wchar_t);
		this->size = 0;
		if (str == nullptr)
		{
			this->pData = nullptr;
			return;
		}

		while ((wchar_t)str[this->size] != L'\0') // find the character count
		{
			this->size++;
		}

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
	StringBuffer::StringBuffer(const StringBuffer& rhs) : StringBuffer(rhs, rhs.charSize == sizeof(char) ? StringType::Normal : StringType::Wide) {}
	StringBuffer::StringBuffer(const StringBuffer& rhs, StringType stringType)
	{
		this->size = rhs.size;
		this->charSize = stringType == StringType::Normal ? sizeof(char) : sizeof(wchar_t);

		if (rhs.size > 0)
		{
			this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
			if (this->pData != nullptr)
			{
				if (this->charSize != rhs.charSize)
				{
					if (this->charSize == sizeof(char))
					{
						wcstombs(this->pData, (wchar_t*)rhs.pData, rhs.size + 1);
					}
					else
					{
						mbstowcs((wchar_t*)this->pData, rhs.pData, rhs.size + 1);
					}
				}
				else
				{
					memcpy(this->pData, rhs.pData, this->TotalSize() + this->charSize);
				}
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"StringBuffer::StringBuffer", L"Insufficient memory.");
			}
		}
		else
		{
			this->pData = nullptr;
		}
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
	StringBuffer& StringBuffer::operator=(const char* const& rhs)
	{
		this->~StringBuffer();

		this->charSize = sizeof(char);
		this->size = 0;
		if (rhs == nullptr)
		{
			this->pData = nullptr;
			return *this;
		}

		while (rhs[this->size] != '\0') // find the character count
		{
			this->size++;
		}

		this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
		if (this->pData != nullptr)
		{
			memcpy(this->pData, rhs, this->TotalSize() + this->charSize);
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator=", L"Insufficient memory.");
		}

		return *this;
	}
	StringBuffer& StringBuffer::operator=(const wchar_t* const& rhs)
	{
		this->~StringBuffer();

		this->charSize = sizeof(wchar_t);
		this->size = 0;
		if (rhs == nullptr)
		{
			this->pData = nullptr;
			return *this;
		}

		while ((wchar_t)rhs[this->size] != L'\0') // find the character count
		{
			this->size++;
		}

		this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
		if (this->pData != nullptr)
		{
			memcpy(this->pData, rhs, this->TotalSize() + this->charSize);
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator=", L"Insufficient memory.");
		}

		return *this;
	}
	StringBuffer& StringBuffer::operator=(const StringBuffer& rhs)
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
			this->pData = nullptr;
		}

		return *this;
	}
	StringBuffer StringBuffer::operator+(const char& rhs) const
	{
		StringBuffer result;
		result.charSize = this->charSize;
		result.size = this->size + 1;

		result.pData = (char*)malloc(result.TotalSize() + result.charSize);
		if (result.pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+", L"Insufficient memory.");
		}

		memcpy(result.pData, this->pData, this->TotalSize()); // copy *this except the \0 char

		if (result.charSize == sizeof(char)) // copy the rhs
		{
			memcpy(result.pData + this->TotalSize(), &rhs, sizeof(char));
			result.pData[result.size] = '\0';
		}
		else
		{
			mbstowcs((wchar_t*)(result.pData + this->TotalSize()), &rhs, 1);
			((wchar_t*)result.pData)[result.size] = L'\0';
		}

		return result;
	}
	StringBuffer StringBuffer::operator+(const wchar_t& rhs) const
	{
		StringBuffer result;
		result.charSize = this->charSize;
		result.size = this->size + 1;

		result.pData = (char*)malloc(result.TotalSize() + result.charSize);
		if (result.pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+", L"Insufficient memory.");
		}

		memcpy(result.pData, this->pData, this->TotalSize()); // copy *this except the \0 char

		if (result.charSize == sizeof(wchar_t)) // copy the rhs
		{
			memcpy(result.pData + this->TotalSize(), &rhs, sizeof(wchar_t));
			((wchar_t*)result.pData)[result.size] = L'\0';
		}
		else
		{
			wcstombs(result.pData + this->TotalSize(), &rhs, 1);
			result.pData[result.size] = '\0';
		}

		return result;
	}
	StringBuffer StringBuffer::operator+(const char* const& rhs) const
	{
		size_t rhsSize = 0;
		while (rhs[rhsSize] != '\0') // find the character count
		{
			rhsSize++;
		}

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
		size_t rhsSize = 0;
		while (rhs[rhsSize] != L'\0') // find the character count
		{
			rhsSize++;
		}

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
	StringBuffer StringBuffer::operator+=(const char& rhs)
	{
		const size_t newTotalSize = this->TotalSize() + this->charSize;

		char* tempPtr = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (tempPtr == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+=", L"Insufficient memory.");
		}

		if (this->charSize == sizeof(char))
		{
			memcpy(tempPtr + this->TotalSize(), &rhs, sizeof(char));
			tempPtr[this->size + 1] = '\0';
		}
		else
		{
			mbstowcs((wchar_t*)(tempPtr + this->TotalSize()), &rhs, 1);
			((wchar_t*)tempPtr)[this->size + 1] = L'\0';
		}

		this->pData = tempPtr;
		this->size += 1;

		return *this;
	}
	StringBuffer StringBuffer::operator+=(const wchar_t& rhs)
	{
		const size_t newTotalSize = this->TotalSize() + this->charSize;

		char* tempPtr = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (tempPtr == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+=", L"Insufficient memory.");
		}

		if (this->charSize == sizeof(wchar_t))
		{
			memcpy((wchar_t*)(tempPtr + this->TotalSize()), &rhs, sizeof(wchar_t));
			((wchar_t*)tempPtr)[this->size + 1] = L'\0';
		}
		else
		{
			wcstombs(tempPtr + this->TotalSize(), &rhs, 1);
			tempPtr[this->size + 1] = '\0';
		}

		this->pData = tempPtr;
		this->size += 1;

		return *this;
	}
	StringBuffer& StringBuffer::operator+=(const char* const& rhs)
	{
		size_t rhsSize = 0;
		while (rhs[rhsSize] != '\0') // find the character count
		{
			rhsSize++;
		}
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
		size_t rhsSize = 0;
		while (rhs[rhsSize] != L'\0') // find the character count
		{
			rhsSize++;
		}
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
	wchar_t* StringBuffer::wc_str() const
	{
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
	bool StringBuffer::CompareContent(const char* const& rhs) const
	{
		if (this->charSize == sizeof(char))
		{
			return *this == rhs;
		}

		const StringBuffer tempBuffer = StringBuffer(*this, StringType::Normal);
		return tempBuffer == rhs;
	}
	bool StringBuffer::CompareContent(const wchar_t* const& rhs) const
	{
		if (this->charSize == sizeof(wchar_t))
		{
			return *this == rhs;
		}

		const StringBuffer tempBuffer = StringBuffer(*this, StringType::Wide);
		return tempBuffer == rhs;
	}
	bool StringBuffer::CompareContent(const StringBuffer& rhs) const
	{
		if (this->charSize == rhs.charSize)
		{
			return *this == rhs;
		}

		const StringBuffer tempBuffer = StringBuffer(rhs, this->GetStringType());
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
	wchar_t StringBuffer::wat(const size_t& index) const
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

		void* tempPtr = (void*)malloc(subStringTotalSize);
		if (tempPtr == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::SubString", L"Insufficient memory.");
		}

		memcpy(tempPtr, this->pData + startIndex * this->charSize, subStringTotalSize);

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
	size_t StringBuffer::Find(const char& c, const size_t& offset) const
	{
		if (offset >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::Find", L"Offset out of bounds.");
		}

		if (this->charSize == sizeof(char))
		{
			const char* const charPos = strchr(this->pData + offset, c);
			return charPos != nullptr ? (charPos - this->pData - offset) : StringBuffer::npos;
		}

		wchar_t wc = L'\0';
		mbstowcs(&wc, &c, 1);

		const wchar_t* const charPos = wcschr((wchar_t*)this->pData + offset, wc);
		return charPos != nullptr ? (charPos - (wchar_t*)this->pData - offset) : StringBuffer::npos;
	}
	size_t StringBuffer::Find(const wchar_t& wc, const size_t& offset) const
	{
		if (offset >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::Find", L"Offset out of bounds.");
		}

		if (this->charSize == sizeof(wchar_t))
		{
			const wchar_t* const charPos = wcschr((wchar_t*)this->pData + offset, wc);
			return charPos != nullptr ? (charPos - (wchar_t*)this->pData - offset) : StringBuffer::npos;
		}

		char c = '\0';
		wcstombs(&c, (wchar_t*)this->pData + offset, 1);

		const char* const charPos = strchr(this->pData + offset, c);
		return charPos != nullptr ? (charPos - this->pData - offset) : StringBuffer::npos;
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
			return charPos != nullptr ? (charPos - this->pData - offset) : StringBuffer::npos;
		}

		size_t strSize = 0;
		while (str[strSize] != '\0')
		{
			strSize++;
		}

		wchar_t* wstr = (wchar_t*)malloc(strSize * sizeof(wchar_t) + sizeof(wchar_t));
		if (wstr == nullptr)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::Find", L"Insufficient memory.");
		}
		mbstowcs(wstr, str, strSize + 1);

		const wchar_t* const charPos = wcsstr(((wchar_t*)this->pData) + offset, wstr);
		const size_t index = charPos != nullptr ? (charPos - (wchar_t*)this->pData - offset) : StringBuffer::npos;

		free(wstr);

		return index;
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
			return charPos != nullptr ? (charPos - (wchar_t*)this->pData - offset) : StringBuffer::npos;
		}

		size_t strSize = 0;
		while (wstr[strSize] != L'\0')
		{
			strSize++;
		}

		char* str = (char*)malloc(strSize * sizeof(char) + sizeof(char));
		if (str == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::Find", L"Insufficient memory.");
		}
		wcstombs(str, wstr, strSize + 1);

		const char* const charPos = strstr(this->pData + offset, str);
		const size_t index = charPos != nullptr ? (charPos - this->pData - offset) : StringBuffer::npos;

		free(str);

		return index;
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
		if (this->charSize == sizeof(char))
		{
			this->pData[index] = c;
		}
		else
		{
			wchar_t wc = L'\0';
			mbstowcs(&wc, &c, 1);
			((wchar_t*)this->pData)[index] = wc;
		}
	}
	void StringBuffer::ReplaceAt(size_t index, const wchar_t& wc)
	{
		if (this->charSize == sizeof(wchar_t))
		{
			((wchar_t*)this->pData)[index] = wc;
		}
		else
		{
			char c = '\0';
			wcstombs(&c, &wc, 1);
			this->pData[index] = c;
		}
	}
	void StringBuffer::ReplaceAt(size_t index, const char* const& str)
	{
		if (index >= this->size)
		{
			throw AudioException(E_INVALIDARG, L"StringBuffer::ReplaceAt", L"Index out of bounds.");
		}

		size_t strSize = 0;
		while (str[strSize] != '\0')
		{
			strSize++;
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

		size_t strSize = 0;
		while (wstr[strSize] != L'\0')
		{
			strSize++;
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
}