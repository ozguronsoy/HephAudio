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
		StringBuffer(const char* const& str);
		StringBuffer(const wchar_t* const& str);
		StringBuffer(const StringBuffer& rhs);
		StringBuffer(const StringBuffer& rhs, StringType stringType);
		~StringBuffer();
		operator char* () const;
		operator wchar_t* () const;
		StringBuffer& operator=(const char* const& rhs);
		StringBuffer& operator=(const wchar_t* const& rhs);
		StringBuffer& operator=(const StringBuffer& rhs);
		StringBuffer operator+(const char* const& rhs) const;
		StringBuffer operator+(const wchar_t* const& rhs) const;
		StringBuffer operator+(const StringBuffer& rhs) const;
		StringBuffer& operator+=(const char* const& rhs);
		StringBuffer& operator+=(const wchar_t* const& rhs);
		StringBuffer& operator+=(const StringBuffer& rhs);
		bool operator==(const char* const& rhs) const;
		bool operator==(const wchar_t* const& rhs) const;
		bool operator==(const StringBuffer& rhs) const;
		bool operator!=(const char* const& rhs) const;
		bool operator!=(const wchar_t* const& rhs) const;
		bool operator!=(const StringBuffer& rhs) const;
		char* c_str() const;
		wchar_t* wc_str() const;
		size_t Size() const noexcept;
		StringType GetStringType() const noexcept;
		void SetStringType(StringType newType);
		bool CompareContent(const char* const& rhs) const;
		bool CompareContent(const wchar_t* const& rhs) const;
		bool CompareContent(const StringBuffer& rhs) const;
		char at(const size_t& index) const;
		wchar_t wat(const size_t& index) const;
		size_t Find(const char& c, const size_t& offset) const;
		size_t Find(const wchar_t& wc, const size_t& offset) const;
		size_t Find(const char* const& str, const size_t& offset) const;
		size_t Find(const wchar_t* const& wstr, const size_t& offset) const;
		size_t Find(const StringBuffer& str, const size_t& offset) const;
		void ToLower();
		void ToUpper();
		void* Begin() const noexcept;
		void* End() const;
	private:
		size_t TotalSize() const noexcept;
	};
}