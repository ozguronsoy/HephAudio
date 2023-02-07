#pragma once
#include "framework.h"
#include <cinttypes>
#include <vector>

namespace HephAudio
{
	enum class StringType
	{
		Normal,
		Wide
	};
	class StringBuffer final
	{
	public:
		static constexpr size_t npos = -1;
	private:
		size_t charSize;
		size_t size;
		char* pData;
	public:
		StringBuffer();
		StringBuffer(const char& c);
		StringBuffer(const wchar_t& wc);
		StringBuffer(const char* const& str);
		StringBuffer(const wchar_t* const& wstr);
		StringBuffer(const StringBuffer& str);
		StringBuffer(const StringBuffer& str, StringType stringType);
		StringBuffer(StringBuffer&& str) noexcept;
		~StringBuffer();
		operator char* () const;
		operator wchar_t* () const;
		StringBuffer& operator=(const char& rhs);
		StringBuffer& operator=(const wchar_t& rhs);
		StringBuffer& operator=(const char* const& rhs);
		StringBuffer& operator=(const wchar_t* const& rhs);
		StringBuffer& operator=(const StringBuffer& rhs);
		StringBuffer& operator=(StringBuffer&& rhs) noexcept;
		StringBuffer operator+(const char& rhs) const;
		StringBuffer operator+(const wchar_t& rhs) const;
		StringBuffer operator+(const char* const& rhs) const;
		StringBuffer operator+(const wchar_t* const& rhs) const;
		StringBuffer operator+(const StringBuffer& rhs) const;
		StringBuffer& operator+=(const char& rhs);
		StringBuffer& operator+=(const wchar_t& rhs);
		StringBuffer& operator+=(const char* const& rhs);
		StringBuffer& operator+=(const wchar_t* const& rhs);
		StringBuffer& operator+=(const StringBuffer& rhs);
		bool operator==(const char& rhs) const;
		bool operator==(const wchar_t& rhs) const;
		bool operator==(const char* const& rhs) const;
		bool operator==(const wchar_t* const& rhs) const;
		bool operator==(const StringBuffer& rhs) const;
		bool operator!=(const char& rhs) const;
		bool operator!=(const wchar_t& rhs) const;
		bool operator!=(const char* const& rhs) const;
		bool operator!=(const wchar_t* const& rhs) const;
		bool operator!=(const StringBuffer& rhs) const;
		char* c_str() const;
		char* fc_str();
		wchar_t* wc_str() const;
		wchar_t* fwc_str();
		size_t Size() const noexcept;
		StringType GetStringType() const noexcept;
		void SetStringType(StringType newType);
		bool CompareContent(const char& c) const;
		bool CompareContent(const wchar_t& wc) const;
		bool CompareContent(const char* const& str) const;
		bool CompareContent(const wchar_t* const& wstr) const;
		bool CompareContent(const StringBuffer& str) const;
		char at(const size_t& index) const;
		wchar_t w_at(const size_t& index) const;
		StringBuffer SubString(size_t startIndex, size_t size) const;
		StringBuffer SubString(size_t startIndex, size_t size, StringType stringType) const;
		size_t Find(const char& c) const;
		size_t Find(const char& c, const size_t& offset) const;
		size_t Find(const wchar_t& wc) const;
		size_t Find(const wchar_t& wc, const size_t& offset) const;
		size_t Find(const char* const& str) const;
		size_t Find(const char* const& str, const size_t& offset) const;
		size_t Find(const wchar_t* const& wstr) const;
		size_t Find(const wchar_t* const& wstr, const size_t& offset) const;
		size_t Find(const StringBuffer& str) const;
		size_t Find(const StringBuffer& str, const size_t& offset) const;
		bool Contains(const char& c) const;
		bool Contains(const wchar_t& wc) const;
		bool Contains(const char* const& str) const;
		bool Contains(const wchar_t* const& wstr) const;
		bool Contains(const StringBuffer& str) const;
		std::vector<StringBuffer> Split(const char& c) const;
		std::vector<StringBuffer> Split(const wchar_t& wc) const;
		std::vector<StringBuffer> Split(const char* const& str) const;
		std::vector<StringBuffer> Split(const wchar_t* const& wstr) const;
		std::vector<StringBuffer> Split(const StringBuffer& str) const;
		void RemoveAt(size_t index);
		void RemoveAt(size_t index, size_t size);
		void Remove(const char& c);
		void Remove(const wchar_t& wc);
		void Remove(const char* const& str);
		void Remove(const wchar_t* const& wstr);
		void Remove(const StringBuffer& str);
		void ReplaceAt(size_t index, const char& c);
		void ReplaceAt(size_t index, const wchar_t& wc);
		void ReplaceAt(size_t index, const char* const& str);
		void ReplaceAt(size_t index, const wchar_t* const& wstr);
		void ReplaceAt(size_t index, const StringBuffer& str);
		void Clear();
		void Reverse();
		void ToLower();
		void ToUpper();
		void* Begin() const noexcept;
		void* End() const;
	private:
		size_t TotalSize() const noexcept;
	public:
		static StringBuffer ToString(const int16_t& value);
		static StringBuffer ToString(const uint16_t& value);
		static StringBuffer ToString(const int32_t& value);
		static StringBuffer ToString(const uint32_t& value);
		static StringBuffer ToString(const int64_t& value);
		static StringBuffer ToString(const uint64_t& value);
		static StringBuffer ToString(const double& value);
		static StringBuffer ToString(const double& value, size_t precision);
		static StringBuffer ToHexString(const int16_t& value);
		static StringBuffer ToHexString(const uint16_t& value);
		static StringBuffer ToHexString(const int32_t& value);
		static StringBuffer ToHexString(const uint32_t& value);
		static StringBuffer ToHexString(const int64_t& value);
		static StringBuffer ToHexString(const uint64_t& value);
		static int16_t StringToI16(StringBuffer string);
		static uint16_t StringToUI16(StringBuffer string);
		static int32_t StringToI32(StringBuffer string);
		static uint32_t StringToUI32(StringBuffer string);
		static int64_t StringToI64(StringBuffer string);
		static uint64_t StringToUI64(StringBuffer string);
		static int16_t HexStringToI16(StringBuffer hexString);
		static uint16_t HexStringToUI16(StringBuffer hexString);
		static int32_t HexStringToI32(StringBuffer hexString);
		static uint32_t HexStringToUI32(StringBuffer hexString);
		static int64_t HexStringToI64(StringBuffer hexString);
		static uint64_t HexStringToUI64(StringBuffer hexString);
	};
}
HephAudio::StringBuffer operator+(const char& lhs, const HephAudio::StringBuffer& rhs);
HephAudio::StringBuffer operator+(const wchar_t& lhs, const HephAudio::StringBuffer& rhs);
HephAudio::StringBuffer operator+(const char* const& lhs, const HephAudio::StringBuffer& rhs);
HephAudio::StringBuffer operator+(const wchar_t* const& lhs, const HephAudio::StringBuffer& rhs);