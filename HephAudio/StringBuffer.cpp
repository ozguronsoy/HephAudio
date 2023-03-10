#include "StringBuffer.h"
#include "AudioException.h"
#include <cstring>
#include <ctype.h>
#include <memory>

#define TO_STRING_BUFFER_SIZE 512

namespace HephAudio
{
	StringBuffer::StringBuffer(size_t size, size_t charSize)
		: charSize(charSize), size(size)
	{
		this->pData = (char*)malloc(this->TotalSize() + this->charSize);
		if (this->pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
		}
		memset(this->pData + this->TotalSize(), 0, this->charSize);
	}
	StringBuffer::StringBuffer()
		: charSize(sizeof(char)), size(0)
	{
		this->pData = (char*)malloc(this->charSize);
		if (this->pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
		}
		this->pData[0] = '\0';
	}
	StringBuffer::StringBuffer(const char& c)
		: charSize(sizeof(char)), size(1)
	{
		this->pData = (char*)malloc(this->charSize * 2);
		if (this->pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
		}

		this->pData[0] = c;
		this->pData[1] = '\0';
	}
	StringBuffer::StringBuffer(const wchar_t& wc)
		: charSize(sizeof(wchar_t)), size(0)
	{
		this->pData = (char*)malloc(this->charSize * 2);
		if (this->pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
		}

		((wchar_t*)this->pData)[0] = wc;
		((wchar_t*)this->pData)[1] = L'\0';
	}
	StringBuffer::StringBuffer(const char* const& str)
		: charSize(sizeof(char))
	{
		if (str == nullptr)
		{
			this->size = 0;
			this->pData = (char*)malloc(this->charSize);
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
			}
			memset(this->pData, 0, this->charSize);
			return;
		}

		this->size = strlen(str);

		this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
		if (this->pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
		}
		memcpy(this->pData, str, this->TotalSize() + this->charSize);
	}
	StringBuffer::StringBuffer(const wchar_t* const& wstr)
		: charSize(sizeof(wchar_t))
	{
		if (wstr == nullptr)
		{
			this->size = 0;
			this->pData = (char*)malloc(this->charSize);
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
			}
			memset(this->pData, 0, this->charSize);
			return;
		}

		this->size = wcslen(wstr);

		this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
		if (this->pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
		}
		memcpy(this->pData, wstr, this->TotalSize() + this->charSize);
	}
	StringBuffer::StringBuffer(const StringBuffer& str) : StringBuffer(str, str.charSize == sizeof(char) ? StringType::ASCII : StringType::Wide) {}
	StringBuffer::StringBuffer(const StringBuffer& str, StringType stringType)
		: size(str.size)
	{
		this->charSize = stringType == StringType::ASCII ? sizeof(char) : sizeof(wchar_t);

		if (str.size > 0)
		{
			this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
			}

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
			this->pData = (char*)malloc(this->charSize);
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
			}
			memset(this->pData, 0, this->charSize);
		}
	}
	StringBuffer::StringBuffer(StringBuffer&& str) noexcept
		: charSize(str.charSize), size(str.size), pData(str.pData)
	{
		str.charSize = 0;
		str.size = 0;
		str.pData = nullptr;
	}
	StringBuffer::~StringBuffer()
	{
		this->size = 0;
		this->charSize = 0;
		if (this->pData != nullptr)
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
				if (this->pData == nullptr)
				{
					throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
				}
				memset(this->pData, 0, this->charSize);

				return *this;
			}

			this->size = strlen(rhs);

			this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::operator=", "Insufficient memory.");
			}
			memcpy(this->pData, rhs, this->TotalSize() + this->charSize);
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
				if (this->pData == nullptr)
				{
					throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
				}
				memset(this->pData, 0, this->charSize);

				return *this;
			}

			this->size = wcslen(rhs);

			this->pData = (char*)malloc(this->TotalSize() + this->charSize); // allocate buffer and copy the string
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::operator=", "Insufficient memory.");
			}
			memcpy(this->pData, rhs, this->TotalSize() + this->charSize);
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
					throw AudioException(E_OUTOFMEMORY, "StringBuffer::operator=", "Insufficient memory.");
				}
			}
			else
			{
				this->size = 0;
				this->pData = (char*)malloc(this->charSize);
				if (this->pData == nullptr)
				{
					throw AudioException(E_OUTOFMEMORY, "StringBuffer::StringBuffer", "Insufficient memory.");
				}
				memset(this->pData, 0, this->charSize);
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

		StringBuffer result = StringBuffer(this->size + rhsSize, this->charSize);

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

		StringBuffer result = StringBuffer(this->size + rhsSize, this->charSize);

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

		char* pTemp = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (pTemp == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::operator+=", "Insufficient memory.");
		}

		if (this->charSize == sizeof(char))
		{
			memcpy(pTemp + this->TotalSize(), rhs, rhsSize + sizeof(char));
		}
		else
		{
			mbstowcs((wchar_t*)(pTemp + this->TotalSize()), rhs, rhsSize + 1);
		}

		this->pData = pTemp;
		this->size += rhsSize;

		return *this;
	}
	StringBuffer& StringBuffer::operator+=(const wchar_t* const& rhs)
	{
		const size_t rhsSize = wcslen(rhs);
		const size_t newTotalSize = this->TotalSize() + rhsSize * this->charSize;

		char* pTemp = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (pTemp == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::operator+=", L"Insufficient memory.");
		}

		if (this->charSize == sizeof(wchar_t))
		{
			memcpy(pTemp + this->TotalSize(), rhs, rhsSize * sizeof(wchar_t) + sizeof(wchar_t));
		}
		else
		{
			wcstombs(pTemp + this->TotalSize(), rhs, rhsSize + 1);
		}

		this->pData = pTemp;
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
	StringBuffer StringBuffer::operator*(const uint32_t& rhs) const
	{
		StringBuffer result = StringBuffer(this->size * rhs, this->charSize);

		const size_t resultTotalSize = result.TotalSize();
		const size_t thisTotalSize = this->TotalSize();

		for (size_t i = 0; i < resultTotalSize; i += thisTotalSize)
		{
			memcpy(result.pData + i, this->pData, thisTotalSize);
		}

		return result;
	}
	StringBuffer& StringBuffer::operator*=(const uint32_t& rhs)
	{
		const size_t oldTotalSize = this->TotalSize();
		this->size *= rhs;
		const size_t newTotalSize = this->TotalSize();

		char* pTemp = (char*)realloc(this->pData, newTotalSize + this->charSize);
		if (pTemp == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::operator*=", "Insufficient memory.");
		}

		for (size_t i = 0; i < newTotalSize; i += oldTotalSize)
		{
			memcpy(pTemp + i, this->pData, oldTotalSize);
		}
		memset(pTemp + newTotalSize, 0, this->charSize);

		this->pData = pTemp;

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
		return rhs != nullptr && this->charSize == sizeof(char) && this->size == strlen(rhs) && strncmp(this->pData, rhs, this->size) == 0;
	}
	bool StringBuffer::operator==(const wchar_t* const& rhs) const
	{
		return rhs != nullptr && this->charSize == sizeof(wchar_t) && this->size == wcslen(rhs) && wcsncmp((wchar_t*)this->pData, rhs, this->size) == 0;
	}
	bool StringBuffer::operator==(const StringBuffer& rhs) const
	{
		return rhs.charSize == sizeof(char) ? this->operator==(rhs.c_str()) : this->operator==(rhs.wc_str());
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
		return rhs == nullptr || this->charSize != sizeof(char) || this->size != strlen(rhs) || strncmp(this->pData, rhs, this->size) != 0;
	}
	bool StringBuffer::operator!=(const wchar_t* const& rhs) const
	{
		return rhs == nullptr || this->charSize != sizeof(wchar_t) || this->size != wcslen(rhs) || wcsncmp((wchar_t*)this->pData, (wchar_t*)rhs, this->size) != 0;
	}
	bool StringBuffer::operator!=(const StringBuffer& rhs) const
	{
		return rhs.charSize == sizeof(char) ? this->operator!=(rhs.c_str()) : this->operator!=(rhs.wc_str());
	}
	char* StringBuffer::c_str() const
	{
		return (char*)this->pData;
	}
	char* StringBuffer::fc_str()
	{
		this->SetStringType(StringType::ASCII);
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
		return this->charSize == sizeof(char) ? StringType::ASCII : StringType::Wide;
	}
	void StringBuffer::SetStringType(StringType newType)
	{
		const size_t newCharSize = newType == StringType::ASCII ? sizeof(char) : sizeof(wchar_t);
		if (this->charSize != newCharSize)
		{
			char* pTemp = (char*)malloc(this->size * newCharSize + newCharSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::ChangeStringType", "Insufficient memory.");
			}

			if (this->charSize == sizeof(char))
			{
				mbstowcs((wchar_t*)pTemp, this->pData, this->size + 1);
			}
			else
			{
				wcstombs(pTemp, (wchar_t*)this->pData, this->size + 1);
			}

			free(this->pData);
			this->pData = pTemp;
			this->charSize = newCharSize;
		}
	}
	bool StringBuffer::CompareContent(char c) const
	{
		char buffer[2] = { c, '\0' };
		return this->CompareContent(buffer);
	}
	bool StringBuffer::CompareContent(wchar_t wc) const
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->CompareContent(buffer);
	}
	bool StringBuffer::CompareContent(const char* str) const
	{
		if (this->charSize == sizeof(char))
		{
			return *this == str;
		}

		const StringBuffer tempBuffer = StringBuffer(*this, StringType::ASCII);
		return tempBuffer == str;
	}
	bool StringBuffer::CompareContent(const wchar_t* wstr) const
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
	char StringBuffer::at(size_t index) const
	{
		if (this->charSize == sizeof(char))
		{
			return this->pData[index];
		}

		char c = '\0';
		wcstombs(&c, (wchar_t*)this->pData + index, 1);

		return c;
	}
	wchar_t StringBuffer::w_at(size_t index) const
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
			throw AudioException(E_INVALIDARG, "StringBuffer::SubString", "Index out of bounds.");
		}

		if (startIndex + size > this->size)
		{
			size = this->size - startIndex;
		}

		StringBuffer subString = StringBuffer(size, this->charSize);
		const size_t subStringTotalSize = subString.TotalSize();

		memcpy(subString.pData, this->pData + startIndex * this->charSize, subStringTotalSize);
		memset((char*)subString.pData + subStringTotalSize, 0, this->charSize);

		return subString;
	}
	StringBuffer StringBuffer::SubString(size_t startIndex, size_t size, StringType stringType) const
	{
		StringBuffer subString = this->SubString(startIndex, size);
		subString.SetStringType(stringType);
		return subString;
	}
	size_t StringBuffer::Find(char c) const
	{
		return this->Find(c, 0);
	}
	size_t StringBuffer::Find(char c, size_t offset) const
	{
		char buffer[2] = { c, '\0' };
		return this->Find(buffer, offset);
	}
	size_t StringBuffer::Find(wchar_t wc) const
	{
		return this->Find(wc, 0);
	}
	size_t StringBuffer::Find(wchar_t wc, size_t offset) const
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->Find(buffer, offset);
	}
	size_t StringBuffer::Find(const char* str) const
	{
		return this->Find(str, 0);
	}
	size_t StringBuffer::Find(const char* str, size_t offset) const
	{
		if (offset >= this->size)
		{
			throw AudioException(E_INVALIDARG, "StringBuffer::Find", "Offset out of bounds.");
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
			throw AudioException(E_INVALIDARG, "StringBuffer::Find", "Insufficient memory.");
		}
		mbstowcs(wstr, str, strSize + 1);

		const wchar_t* const charPos = wcsstr((wchar_t*)this->pData + offset, wstr);
		const size_t index = charPos != nullptr ? (charPos - (wchar_t*)this->pData) : StringBuffer::npos;

		free(wstr);

		return index;
	}
	size_t StringBuffer::Find(const wchar_t* wstr) const
	{
		return this->Find(wstr, 0);
	}
	size_t StringBuffer::Find(const wchar_t* wstr, size_t offset) const
	{
		if (offset >= this->size)
		{
			throw AudioException(E_INVALIDARG, "StringBuffer::Find", "Offset out of bounds.");
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
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::Find", "Insufficient memory.");
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
	size_t StringBuffer::Find(const StringBuffer& str, size_t offset) const
	{
		return str.charSize == sizeof(char) ? this->Find(str.pData, offset) : this->Find((wchar_t*)str.pData, offset);
	}
	bool StringBuffer::Contains(char c) const
	{
		return this->Find(c, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(wchar_t wc) const
	{
		return this->Find(wc, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(const char* str) const
	{
		return this->Find(str, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(const wchar_t* wstr) const
	{
		return this->Find(wstr, 0) != StringBuffer::npos;
	}
	bool StringBuffer::Contains(StringBuffer str) const
	{
		return this->Find(str, 0) != StringBuffer::npos;
	}
	std::vector<StringBuffer> StringBuffer::Split(char c) const
	{
		char buffer[2] = { c, '\0' };
		return this->Split(buffer);
	}
	std::vector<StringBuffer> StringBuffer::Split(wchar_t wc) const
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->Split(buffer);
	}
	std::vector<StringBuffer> StringBuffer::Split(const char* str) const
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
	std::vector<StringBuffer> StringBuffer::Split(const wchar_t* wstr) const
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
		if (str.charSize == sizeof(char))
		{
			return this->Split(str.c_str());
		}
		return this->Split(str.wc_str());
	}
	StringBuffer& StringBuffer::RemoveAt(size_t index)
	{
		return this->RemoveAt(index, 1);
	}
	StringBuffer& StringBuffer::RemoveAt(size_t index, size_t size)
	{
		if (index >= this->size)
		{
			throw AudioException(E_INVALIDARG, "StringBuffer::RemoveAt", "Index out of bounds.");
		}

		if (index + size > this->size)
		{
			size = this->size - index;
		}

		char* pTemp = (char*)malloc(this->TotalSize());
		if (pTemp == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::RemoveAt", "Insufficient memory.");
		}
		const size_t firstPartTotalSize = index * this->charSize;
		const size_t removedTotalSize = size * this->charSize;

		memcpy(pTemp, this->pData, firstPartTotalSize);
		memcpy(pTemp + firstPartTotalSize, this->pData + firstPartTotalSize + removedTotalSize, this->TotalSize() - firstPartTotalSize - removedTotalSize + this->charSize);

		this->size -= size;

		if (this->charSize == sizeof(char))
		{
			pTemp[this->size] = '\0';
		}
		else
		{
			((wchar_t*)pTemp)[this->size] = L'\0';
		}

		free(this->pData);
		this->pData = pTemp;

		return *this;
	}
	StringBuffer& StringBuffer::Remove(char c)
	{
		char buffer[2] = { c, '\0' };
		return this->Remove(buffer);
	}
	StringBuffer& StringBuffer::Remove(wchar_t wc)
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->Remove(buffer);
	}
	StringBuffer& StringBuffer::Remove(const char* str)
	{
		const size_t index = this->Find(str);
		if (index != StringBuffer::npos)
		{
			return this->RemoveAt(index, strlen(str));
		}
		return *this;
	}
	StringBuffer& StringBuffer::Remove(const wchar_t* wstr)
	{
		const size_t index = this->Find(wstr);
		if (index != StringBuffer::npos)
		{
			return this->RemoveAt(index, wcslen(wstr));
		}
		return *this;
	}
	StringBuffer& StringBuffer::Remove(const StringBuffer& str)
	{
		if (str.charSize == sizeof(char))
		{
			return this->Remove(str.c_str());
		}
		else
		{
			return this->Remove(str.wc_str());
		}
	}

	StringBuffer& StringBuffer::ReplaceAt(size_t index, char c)
	{
		char buffer[2] = { c, '\0' };
		return this->ReplaceAt(index, buffer);
	}
	StringBuffer& StringBuffer::ReplaceAt(size_t index, wchar_t wc)
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->ReplaceAt(index, buffer);
	}
	StringBuffer& StringBuffer::ReplaceAt(size_t index, const char* str)
	{
		if (index >= this->size)
		{
			throw AudioException(E_INVALIDARG, "StringBuffer::ReplaceAt", "Index out of bounds.");
		}

		const size_t strSize = strlen(str);

		if (strSize == 0)
		{
			return this->RemoveAt(index);
		}

		if (index + strSize >= this->size)
		{
			const size_t newTotalSize = (index + strSize) * this->charSize;

			char* pTemp = (char*)realloc(this->pData, newTotalSize + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::ReplaceAt", "Insufficient memory.");
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

		return *this;
	}
	StringBuffer& StringBuffer::ReplaceAt(size_t index, const wchar_t* wstr)
	{
		if (index >= this->size)
		{
			throw AudioException(E_INVALIDARG, "StringBuffer::ReplaceAt", "Index out of bounds.");
		}

		const size_t strSize = wcslen(wstr);

		if (strSize == 0)
		{
			return this->RemoveAt(index);
		}

		if (index + strSize >= this->size)
		{
			const size_t newTotalSize = (index + strSize) * this->charSize;

			char* pTemp = (char*)realloc(this->pData, newTotalSize + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::ReplaceAt", "Insufficient memory.");
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

		return *this;
	}
	StringBuffer& StringBuffer::ReplaceAt(size_t index, const StringBuffer& str)
	{
		if (str.charSize == sizeof(char))
		{
			return this->ReplaceAt(index, str.c_str());
		}
		else
		{
			return this->ReplaceAt(index, str.wc_str());
		}
	}
	StringBuffer& StringBuffer::TrimStart(char c)
	{
		char buffer[2] = { c, '\0' };
		return this->TrimStart(buffer);
	}
	StringBuffer& StringBuffer::TrimStart(wchar_t wc)
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->TrimStart(buffer);
	}
	StringBuffer& StringBuffer::TrimStart(const char* str)
	{
		const size_t strSize = strlen(str);
		size_t iStart = 0;

		if (this->charSize == sizeof(char))
		{
			while (strncmp(this->pData + iStart, str, strSize) == 0)
			{
				iStart += strSize;
			}
		}
		else
		{
			wchar_t* wstr = (wchar_t*)malloc(strSize * sizeof(wchar_t) + sizeof(wchar_t));
			if (wstr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimStart", "Insufficient memory.");
			}

			mbstowcs(wstr, str, strSize);
			wstr[strSize] = L'\0';

			while (wcsncmp((wchar_t*)this->pData + iStart, wstr, strSize) == 0)
			{
				iStart += strSize;
			}

			free(wstr);
		}

		if (iStart > 0)
		{
			this->size -= iStart;

			char* pTemp = (char*)malloc(this->TotalSize() + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimStart", "Insufficient memory.");
			}

			memcpy(pTemp, this->pData + iStart * this->charSize, this->TotalSize() + this->charSize);

			free(this->pData);
			this->pData = pTemp;
		}

		return *this;
	}
	StringBuffer& StringBuffer::TrimStart(const wchar_t* wstr)
	{
		const size_t strSize = wcslen(wstr);
		size_t iStart = 0;

		if (this->charSize == sizeof(char))
		{
			char* str = (char*)malloc(strSize * sizeof(char) + sizeof(char));
			if (str == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimStart", "Insufficient memory.");
			}

			wcstombs(str, wstr, strSize);
			str[strSize] = '\0';

			while (strncmp(this->pData + iStart, str, strSize) == 0)
			{
				iStart += strSize;
			}

			free(str);
		}
		else
		{
			while (wcsncmp((wchar_t*)this->pData + iStart, wstr, strSize) == 0)
			{
				iStart += strSize;
			}
		}

		if (iStart > 0)
		{
			this->size -= iStart;

			char* pTemp = (char*)malloc(this->TotalSize() + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimStart", "Insufficient memory.");
			}

			memcpy(pTemp, this->pData + iStart * this->charSize, this->TotalSize() + this->charSize);

			free(this->pData);
			this->pData = pTemp;
		}

		return *this;
	}
	StringBuffer& StringBuffer::TrimStart(const StringBuffer& str)
	{
		if (str.charSize == sizeof(char))
		{
			return this->TrimStart(str.c_str());
		}
		return this->TrimStart(str.wc_str());
	}
	StringBuffer& StringBuffer::TrimEnd(char c)
	{
		char buffer[2] = { c, '\0' };
		return this->TrimEnd(buffer);
	}
	StringBuffer& StringBuffer::TrimEnd(wchar_t wc)
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->TrimEnd(buffer);
	}
	StringBuffer& StringBuffer::TrimEnd(const char* str)
	{
		const size_t strSize = strlen(str);
		size_t iEnd = this->size - strSize;

		if (this->charSize == sizeof(char))
		{
			while (strncmp(this->pData + iEnd, str, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;
		}
		else
		{
			wchar_t* wstr = (wchar_t*)malloc(strSize * sizeof(wchar_t) + sizeof(wchar_t));
			if (wstr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimEnd", "Insufficient memory.");
			}

			mbstowcs(wstr, str, strSize);
			wstr[strSize] = L'\0';

			while (wcsncmp(((wchar_t*)this->pData) + iEnd, wstr, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;

			free(wstr);
		}

		if (iEnd < this->size)
		{
			this->size = iEnd;

			char* pTemp = (char*)realloc(this->pData, this->TotalSize() + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimEnd", "Insufficient memory.");
			}
			memset(pTemp + this->TotalSize(), 0, this->charSize);

			this->pData = pTemp;
		}

		return *this;
	}
	StringBuffer& StringBuffer::TrimEnd(const wchar_t* wstr)
	{
		const size_t strSize = wcslen(wstr);
		size_t iEnd = this->size - strSize;

		if (this->charSize == sizeof(char))
		{
			char* str = (char*)malloc(strSize * sizeof(char) + sizeof(char));
			if (str == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimEnd", "Insufficient memory.");
			}

			wcstombs(str, wstr, strSize);
			str[strSize] = L'\0';

			while (strncmp(this->pData + iEnd, str, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;

			free(str);
		}
		else
		{
			while (wcsncmp((wchar_t*)this->pData + iEnd, wstr, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;
		}

		if (iEnd < this->size)
		{
			this->size = iEnd;

			char* pTemp = (char*)realloc(this->pData, this->TotalSize() + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::TrimEnd", "Insufficient memory.");
			}
			memset(pTemp + this->TotalSize(), 0, this->charSize);

			this->pData = pTemp;
		}

		return *this;
	}
	StringBuffer& StringBuffer::TrimEnd(const StringBuffer& str)
	{
		if (str.charSize == sizeof(char))
		{
			return this->TrimEnd(str.c_str());
		}
		return this->TrimEnd(str.wc_str());
	}
	StringBuffer& StringBuffer::Trim(char c)
	{
		char buffer[2] = { c, '\0' };
		return this->Trim(buffer);
	}
	StringBuffer& StringBuffer::Trim(wchar_t wc)
	{
		wchar_t buffer[2] = { wc, L'\0' };
		return this->Trim(buffer);
	}
	StringBuffer& StringBuffer::Trim(const char* str)
	{
		const size_t strSize = strlen(str);
		size_t iStart = 0;
		size_t iEnd = this->size - strSize;

		if (this->charSize == sizeof(char))
		{
			while (strncmp(this->pData + iStart, str, strSize) == 0)
			{
				iStart += strSize;
			}

			while (strncmp(this->pData + iEnd, str, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;
		}
		else
		{
			wchar_t* wstr = (wchar_t*)malloc(strSize * sizeof(wchar_t) + sizeof(wchar_t));
			if (wstr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::Trim", "Insufficient memory.");
			}

			mbstowcs(wstr, str, strSize);
			wstr[strSize] = L'\0';

			while (wcsncmp((wchar_t*)this->pData + iStart, wstr, strSize) == 0)
			{
				iStart += strSize;
			}

			while (wcsncmp((wchar_t*)this->pData + iEnd, wstr, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;

			free(wstr);
		}

		if (iStart > 0 || iEnd < this->size)
		{
			this->size = iEnd - iStart;

			char* pTemp = (char*)malloc(this->TotalSize() + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::Trim", "Insufficient memory.");
			}

			memcpy(pTemp, this->pData + iStart * this->charSize, this->TotalSize());
			memset(pTemp + this->TotalSize(), 0, this->charSize);

			free(this->pData);
			this->pData = pTemp;
		}

		return *this;
	}
	StringBuffer& StringBuffer::Trim(const wchar_t* wstr)
	{
		const size_t strSize = wcslen(wstr);
		size_t iStart = 0;
		size_t iEnd = this->size - strSize;

		if (this->charSize == sizeof(char))
		{
			char* str = (char*)malloc(strSize * sizeof(char) + sizeof(char));
			if (wstr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::Trim", "Insufficient memory.");
			}

			wcstombs(str, wstr, strSize);
			str[strSize] = '\0';

			while (strncmp(this->pData + iStart, str, strSize) == 0)
			{
				iStart += strSize;
			}

			while (strncmp(this->pData + iEnd, str, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;

			free(str);
		}
		else
		{
			while (wcsncmp((wchar_t*)this->pData + iStart, wstr, strSize) == 0)
			{
				iStart += strSize;
			}

			while (wcsncmp((wchar_t*)this->pData + iEnd, wstr, strSize) == 0)
			{
				iEnd -= strSize;
			}
			iEnd += strSize;
		}

		if (iStart > 0 || iEnd < this->size)
		{
			this->size = iEnd - iStart;

			char* pTemp = (char*)malloc(this->TotalSize() + this->charSize);
			if (pTemp == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "StringBuffer::Trim", "Insufficient memory.");
			}

			memcpy(pTemp, this->pData + iStart * this->charSize, this->TotalSize());
			memset(pTemp + this->TotalSize(), 0, this->charSize);

			free(this->pData);
			this->pData = pTemp;
		}

		return *this;
	}
	StringBuffer& StringBuffer::Trim(const StringBuffer& str)
	{
		if (str.charSize == sizeof(char))
		{
			return this->Trim(str.c_str());
		}
		return this->Trim(str.wc_str());
	}
	StringBuffer& StringBuffer::Clear()
	{
		const size_t charSize = this->charSize;
		this->~StringBuffer();
		this->charSize = charSize;

		this->pData = (char*)malloc(this->charSize);
		if (this->pData == nullptr)
		{
			throw AudioException(E_OUTOFMEMORY, "StringBuffer::Clear", "Insufficient memory.");
		}
		memset(this->pData, 0, this->charSize);

		return *this;
	}
	StringBuffer& StringBuffer::Reverse()
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

		return *this;
	}
	StringBuffer& StringBuffer::ToLower()
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
		return *this;
	}
	StringBuffer& StringBuffer::ToUpper()
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
		return *this;
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
	StringBuffer StringBuffer::ToString(int16_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%hd", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(uint16_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%hu", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(int32_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%d", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(uint32_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%u", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(int64_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%lld", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(uint64_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "%llu", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToString(double value)
	{
		return StringBuffer::ToString(value, 4);
	}
	StringBuffer StringBuffer::ToString(double value, size_t precision)
	{
		char format[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(format, "%c.%dlf", '%', precision);

		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, format, value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(int8_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%02X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(uint8_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%02X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(int16_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%04X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(uint16_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%04X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(int32_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%08X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(uint32_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%08X", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(int64_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%016llX", value);

		StringBuffer result = buffer;

		return result;
	}
	StringBuffer StringBuffer::ToHexString(uint64_t value)
	{
		char buffer[TO_STRING_BUFFER_SIZE]{ 0 };
		sprintf(buffer, "0x%016llX", value);

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
	double StringBuffer::StringToDouble(StringBuffer string)
	{
		double number = 0;
		sscanf(string.fc_str(), "%lf", &number);
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
	StringBuffer StringBuffer::Join(char separator, const std::vector<StringBuffer>& strings)
	{
		char buffer[2] = { separator, '\0' };
		return StringBuffer::Join(buffer, strings);
	}
	StringBuffer StringBuffer::Join(wchar_t separator, const std::vector<StringBuffer>& strings)
	{
		wchar_t buffer[2] = { separator, L'\0' };
		return StringBuffer::Join(buffer, strings);
	}
	StringBuffer StringBuffer::Join(const char* separator, const std::vector<StringBuffer>& strings)
	{
		if (strings.size() > 0)
		{
			constexpr auto copyString = [](char* pTemp, const StringBuffer& str) -> void
			{
				if (str.pData != nullptr)
				{
					if (str.charSize == sizeof(char))
					{
						memcpy(pTemp, str.pData, str.size * sizeof(char));
					}
					else
					{
						wcstombs(pTemp, (wchar_t*)str.pData, str.size);
					}
				}
			};

			const size_t separatorSize = strlen(separator);
			const size_t separatorTotalSize = separatorSize * sizeof(char);
			size_t resultSize = separatorSize * (strings.size() - 1);

			for (size_t i = 0; i < strings.size(); i++)
			{
				resultSize += strings.at(i).size;
			}

			StringBuffer result = StringBuffer(resultSize, sizeof(char));

			copyString(result.pData, strings.at(0));
			size_t cursor = strings.at(0).size;

			for (size_t i = 1; i < strings.size(); i++)
			{
				memcpy(result.pData + cursor, separator, separatorTotalSize);
				cursor += separatorSize;

				copyString(result.pData + cursor, strings.at(i));
				cursor += strings.at(i).size;
			}

			return result;
		}

		return "";
	}
	StringBuffer StringBuffer::Join(const wchar_t* separator, const std::vector<StringBuffer>& strings)
	{
		if (strings.size() > 0)
		{
			constexpr auto copyString = [](wchar_t* pTemp, const StringBuffer& str) -> void
			{
				if (str.pData != nullptr)
				{
					if (str.charSize == sizeof(wchar_t))
					{
						memcpy(pTemp, str.pData, str.size * sizeof(wchar_t));
					}
					else
					{
						mbstowcs(pTemp, str.pData, str.size);
					}
				}
			};

			const size_t separatorSize = wcslen(separator);
			const size_t separatorTotalSize = separatorSize * sizeof(wchar_t);
			size_t resultSize = separatorSize * (strings.size() - 1);

			for (size_t i = 0; i < strings.size(); i++)
			{
				resultSize += strings.at(i).size;
			}

			StringBuffer result = StringBuffer(resultSize, sizeof(wchar_t));

			copyString((wchar_t*)result.pData, strings.at(0));
			size_t cursor = strings.at(0).size;

			for (size_t i = 1; i < strings.size(); i++)
			{
				memcpy((wchar_t*)result.pData + cursor, separator, separatorTotalSize);
				cursor += separatorSize;

				copyString((wchar_t*)result.pData + cursor, strings.at(i));
				cursor += strings.at(i).size;
			}

			return result;
		}

		return L"";
	}
	StringBuffer StringBuffer::Join(const StringBuffer& separator, const std::vector<StringBuffer>& strings)
	{
		if (separator.charSize == sizeof(char))
		{
			return StringBuffer::Join(separator.c_str(), strings);
		}
		return StringBuffer::Join(separator.wc_str(), strings);
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