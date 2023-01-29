#pragma once
#include "framework.h"

namespace HephAudio
{
	enum class StringType
	{
		Normal,
		Wide
	};
	class StringBuffer final
	{
	private:
		size_t charSize;
		size_t size;
		char* pData;
	public:
		StringBuffer();
		StringBuffer(const char* str);
		StringBuffer(const wchar_t* str);
		StringBuffer(const StringBuffer& rhs);
		StringBuffer(const StringBuffer& rhs, StringType stringType);
		~StringBuffer();
		operator char* () const;
		operator wchar_t* () const;
		StringBuffer& operator=(const char* rhs);
		StringBuffer& operator=(const wchar_t* rhs);
		StringBuffer& operator=(const StringBuffer& rhs);
		StringBuffer operator+(const char* rhs) const;
		StringBuffer operator+(const wchar_t* rhs) const;
		StringBuffer operator+(const StringBuffer& rhs) const;
		StringBuffer& operator+=(const char* rhs);
		StringBuffer& operator+=(const wchar_t* rhs);
		StringBuffer& operator+=(const StringBuffer& rhs);
		char* c_str() const;
		wchar_t* wc_str() const;
		size_t Size() const noexcept;
		StringType GetStringType() const noexcept;
		void SetStringType(StringType newType);
		void* Begin() const noexcept;
		void* End() const;
	private:
		size_t TotalSize() const noexcept;
	};
}