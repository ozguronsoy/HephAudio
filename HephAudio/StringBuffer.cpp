#include "StringBuffer.h"
#include "AudioException.h"
#include <cstring>
#include <iostream>

namespace HephAudio
{
	StringBuffer::StringBuffer()
	{
		this->charSize = 0;
		this->size = 0;
		this->pData = nullptr;
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

		if (result.charSize != sizeof(char)) // copy the rhs
		{
			mbstowcs((wchar_t*)(result.pData + this->TotalSize()), rhs, rhsSize + 1);
		}
		else
		{
			memcpy(result.pData + this->TotalSize(), rhs, rhsSize + sizeof(char));
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

		if (result.charSize != sizeof(wchar_t)) // copy the rhs
		{
			wcstombs(result.pData + this->TotalSize(), rhs, rhsSize + 1);
		}
		else
		{
			memcpy(result.pData + this->TotalSize(), rhs, rhsSize * sizeof(wchar_t) + sizeof(wchar_t));
		}

		return result;
	}
	StringBuffer StringBuffer::operator+(const StringBuffer& rhs) const
	{
		StringBuffer result;
		result.charSize = this->charSize;
		result.size = this->size + rhs.size;

		result.pData = (char*)malloc(result.TotalSize() + result.charSize); // allocate the result buffer
		if (result.pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+", L"Insufficient memory.");
		}

		memcpy(result.pData, this->pData, this->TotalSize()); // copy *this except the \0 char


		if (result.charSize != rhs.charSize) // copy the rhs.
		{
			if (result.charSize == sizeof(char))
			{
				wcstombs(result.pData + this->TotalSize(), (wchar_t*)rhs.pData, rhs.size + 1);
			}
			else
			{
				mbstowcs((wchar_t*)(result.pData + this->TotalSize()), rhs.pData, rhs.size + 1);
			}
		}
		else
		{
			memcpy(result.pData + this->TotalSize(), rhs.pData, rhs.TotalSize() + rhs.charSize);
		}

		return result;
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

		if (this->charSize != sizeof(char))
		{
			mbstowcs((wchar_t*)(tempPtr + this->TotalSize()), rhs, rhsSize + 1);
		}
		else
		{
			memcpy(tempPtr + this->TotalSize(), rhs, rhsSize + sizeof(char));
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

		if (this->charSize != sizeof(wchar_t))
		{
			wcstombs(tempPtr + this->TotalSize(), rhs, rhsSize + 1);
		}
		else
		{
			memcpy(tempPtr + this->TotalSize(), rhs, rhsSize * sizeof(wchar_t) + sizeof(wchar_t));
		}

		this->pData = tempPtr;
		this->size += rhsSize;

		return *this;
	}
	StringBuffer& StringBuffer::operator+=(const StringBuffer& rhs)
	{
		const size_t newTotalSize = this->TotalSize() + rhs.size * this->charSize;

		char* tempPtr = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (tempPtr == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, L"StringBuffer::operator+=", L"Insufficient memory.");
		}

		if (this->charSize != rhs.charSize)
		{
			if (this->charSize == sizeof(char))
			{
				wcstombs(tempPtr + this->TotalSize(), (wchar_t*)rhs.pData, rhs.size + 1);
			}
			else
			{
				mbstowcs((wchar_t*)(tempPtr + this->TotalSize()), rhs.pData, rhs.size + 1);
			}
		}
		else
		{
			memcpy(tempPtr + this->TotalSize(), rhs.pData, rhs.TotalSize() + rhs.charSize);
		}

		this->pData = tempPtr;
		this->size += rhs.size;

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