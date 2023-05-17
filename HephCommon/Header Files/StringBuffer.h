#pragma once
#include "HephCommonFramework.h"
#include <cinttypes>
#include <vector>

namespace HephCommon
{
	enum class StringType : uint8_t
	{
		ASCII,
		Wide
	};
	class StringBuffer final
	{
	public:
		static constexpr size_t npos = -1;
	public:
		class CharAccessor final
		{
			friend class StringBuffer;
		private:
			const StringBuffer* pParent;
			size_t index;
			size_t charSize;
			CharAccessor(const StringBuffer* pParent, size_t index, size_t charSize);
		public:
			operator char() const;
			operator wchar_t() const;
			CharAccessor& operator=(const char& c);
			CharAccessor& operator=(const wchar_t& wc);
			bool operator==(const char& c);
			bool operator==(const wchar_t& wc);
			bool operator!=(const char& c);
			bool operator!=(const wchar_t& wc);
		};
	private:
		size_t charSize;
		size_t size;
		char* pData;
	private:
		StringBuffer(size_t size, size_t charSize);
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
		explicit operator char* () const;
		explicit operator wchar_t* () const;
		CharAccessor operator[](const size_t& index) const;
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
		StringBuffer operator*(const uint32_t& rhs) const;
		StringBuffer& operator*=(const uint32_t& rhs);
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
		size_t ByteSize() const noexcept;
		StringType GetStringType() const noexcept;
		void SetStringType(StringType newType);
		bool CompareContent(char c) const;
		bool CompareContent(wchar_t wc) const;
		bool CompareContent(const char* str) const;
		bool CompareContent(const wchar_t* wstr) const;
		bool CompareContent(const StringBuffer& str) const;
		CharAccessor at(size_t index) const;
		char c_at(size_t index) const;
		wchar_t w_at(size_t index) const;
		StringBuffer SubString(size_t startIndex, size_t size) const;
		StringBuffer SubString(size_t startIndex, size_t size, StringType stringType) const;
		size_t Find(char c) const;
		size_t Find(char c, size_t offset) const;
		size_t Find(wchar_t wc) const;
		size_t Find(wchar_t wc, size_t offset) const;
		size_t Find(const char* str) const;
		size_t Find(const char* str, size_t offset) const;
		size_t Find(const wchar_t* wstr) const;
		size_t Find(const wchar_t* wstr, size_t offset) const;
		size_t Find(const StringBuffer& str) const;
		size_t Find(const StringBuffer& str, size_t offset) const;
		bool Contains(char c) const;
		bool Contains(wchar_t wc) const;
		bool Contains(const char* str) const;
		bool Contains(const wchar_t* wstr) const;
		bool Contains(StringBuffer str) const;
		std::vector<StringBuffer> Split(char c) const;
		std::vector<StringBuffer> Split(wchar_t wc) const;
		std::vector<StringBuffer> Split(const char* str) const;
		std::vector<StringBuffer> Split(const wchar_t* wstr) const;
		std::vector<StringBuffer> Split(const StringBuffer& str) const;
		StringBuffer& RemoveAt(size_t index);
		StringBuffer& RemoveAt(size_t index, size_t size);
		StringBuffer& Remove(char c);
		StringBuffer& Remove(wchar_t wc);
		StringBuffer& Remove(const char* str);
		StringBuffer& Remove(const wchar_t* wstr);
		StringBuffer& Remove(const StringBuffer& str);
		StringBuffer& ReplaceAt(size_t index, char c);
		StringBuffer& ReplaceAt(size_t index, wchar_t wc);
		StringBuffer& ReplaceAt(size_t index, const char* str);
		StringBuffer& ReplaceAt(size_t index, const wchar_t* wstr);
		StringBuffer& ReplaceAt(size_t index, const StringBuffer& str);
		StringBuffer& Insert(size_t index, char c);
		StringBuffer& Insert(size_t index, wchar_t wc);
		StringBuffer& Insert(size_t index, const char* str);
		StringBuffer& Insert(size_t index, const wchar_t* wstr);
		StringBuffer& Insert(size_t index, const StringBuffer& str);
		StringBuffer& TrimStart(char c);
		StringBuffer& TrimStart(wchar_t wc);
		StringBuffer& TrimStart(const char* str);
		StringBuffer& TrimStart(const wchar_t* wstr);
		StringBuffer& TrimStart(const StringBuffer& str);
		StringBuffer& TrimEnd(char c);
		StringBuffer& TrimEnd(wchar_t wc);
		StringBuffer& TrimEnd(const char* str);
		StringBuffer& TrimEnd(const wchar_t* wstr);
		StringBuffer& TrimEnd(const StringBuffer& str);
		StringBuffer& Trim(char c);
		StringBuffer& Trim(wchar_t wc);
		StringBuffer& Trim(const char* str);
		StringBuffer& Trim(const wchar_t* wstr);
		StringBuffer& Trim(const StringBuffer& str);
		StringBuffer& Clear();
		StringBuffer& Reverse();
		StringBuffer& ToLower();
		StringBuffer& ToUpper();
		void* Begin() const noexcept;
		void* End() const;
	public:
		static StringBuffer ToString(int16_t value);
		static StringBuffer ToString(uint16_t value);
		static StringBuffer ToString(int32_t value);
		static StringBuffer ToString(uint32_t value);
		static StringBuffer ToString(int64_t value);
		static StringBuffer ToString(uint64_t value);
		static StringBuffer ToString(double value);
		static StringBuffer ToString(double value, size_t precision);
		static StringBuffer ToHexString(int8_t value);
		static StringBuffer ToHexString(uint8_t value);
		static StringBuffer ToHexString(int16_t value);
		static StringBuffer ToHexString(uint16_t value);
		static StringBuffer ToHexString(int32_t value);
		static StringBuffer ToHexString(uint32_t value);
		static StringBuffer ToHexString(int64_t value);
		static StringBuffer ToHexString(uint64_t value);
		static int16_t StringToI16(StringBuffer string);
		static uint16_t StringToUI16(StringBuffer string);
		static int32_t StringToI32(StringBuffer string);
		static uint32_t StringToUI32(StringBuffer string);
		static int64_t StringToI64(StringBuffer string);
		static uint64_t StringToUI64(StringBuffer string);
		static double StringToDouble(StringBuffer string);
		static int16_t HexStringToI16(StringBuffer hexString);
		static uint16_t HexStringToUI16(StringBuffer hexString);
		static int32_t HexStringToI32(StringBuffer hexString);
		static uint32_t HexStringToUI32(StringBuffer hexString);
		static int64_t HexStringToI64(StringBuffer hexString);
		static uint64_t HexStringToUI64(StringBuffer hexString);
		static StringBuffer Join(char separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(wchar_t separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(const char* separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(const wchar_t* separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(const StringBuffer& separator, const std::vector<StringBuffer>& strings);
	};

}
HephCommon::StringBuffer operator+(const char& lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator+(const wchar_t& lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator+(const char* const& lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator+(const wchar_t* const& lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator""b(char c);
HephCommon::StringBuffer operator""B(char c);
HephCommon::StringBuffer operator""b(wchar_t wc);
HephCommon::StringBuffer operator""B(wchar_t wc);
HephCommon::StringBuffer operator""b(const char* str, size_t);
HephCommon::StringBuffer operator""B(const char* str, size_t);
HephCommon::StringBuffer operator""b(const wchar_t* wstr, size_t);
HephCommon::StringBuffer operator""B(const wchar_t* wstr, size_t);