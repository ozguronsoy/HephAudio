#pragma once
#include "HephCommonShared.h"
#include "Guid.h"
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
			CharAccessor& operator=(char c);
			CharAccessor& operator=(wchar_t wc);
			bool operator==(char c);
			bool operator==(wchar_t wc);
			bool operator!=(char c);
			bool operator!=(wchar_t wc);
		};
	private:
		size_t charSize;
		size_t size;
		char* pData;
	private:
		StringBuffer(size_t size, size_t charSize);
	public:
		StringBuffer();
		StringBuffer(char c);
		StringBuffer(wchar_t wc);
		StringBuffer(const char* const str);
		StringBuffer(const wchar_t* const wstr);
		StringBuffer(std::nullptr_t rhs);
		StringBuffer(const StringBuffer& str);
		StringBuffer(const StringBuffer& str, StringType stringType);
		StringBuffer(StringBuffer&& str) noexcept;
		~StringBuffer();
		explicit operator char* () const;
		explicit operator wchar_t* () const;
		CharAccessor operator[](size_t index) const;
		StringBuffer& operator=(char rhs);
		StringBuffer& operator=(wchar_t rhs);
		StringBuffer& operator=(const char* const rhs);
		StringBuffer& operator=(const wchar_t* const rhs);
		StringBuffer& operator=(std::nullptr_t rhs);
		StringBuffer& operator=(const StringBuffer& rhs);
		StringBuffer& operator=(StringBuffer&& rhs) noexcept;
		StringBuffer operator+(char rhs) const;
		StringBuffer operator+(wchar_t rhs) const;
		StringBuffer operator+(const char* const rhs) const;
		StringBuffer operator+(const wchar_t* const rhs) const;
		StringBuffer operator+(const StringBuffer& rhs) const;
		StringBuffer& operator+=(char rhs);
		StringBuffer& operator+=(wchar_t rhs);
		StringBuffer& operator+=(const char* const rhs);
		StringBuffer& operator+=(const wchar_t* const rhs);
		StringBuffer& operator+=(const StringBuffer& rhs);
		StringBuffer operator*(size_t rhs) const;
		StringBuffer& operator*=(size_t rhs);
		bool operator==(char rhs) const;
		bool operator==(wchar_t rhs) const;
		bool operator==(const char* const rhs) const;
		bool operator==(const wchar_t* const rhs) const;
		bool operator==(const StringBuffer& rhs) const;
		bool operator==(std::nullptr_t rhs) const;
		bool operator!=(char rhs) const;
		bool operator!=(wchar_t rhs) const;
		bool operator!=(const char* const rhs) const;
		bool operator!=(const wchar_t* const rhs) const;
		bool operator!=(const StringBuffer& rhs) const;
		bool operator!=(std::nullptr_t rhs) const;
		char* c_str() const;
		char* fc_str();
		wchar_t* wc_str() const;
		wchar_t* fwc_str();
		size_t Size() const;
		size_t ByteSize() const;
		StringType GetStringType() const;
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
		void RemoveAt(size_t index);
		void RemoveAt(size_t index, size_t size);
		void Remove(char c);
		void Remove(wchar_t wc);
		void Remove(const char* str);
		void Remove(const wchar_t* wstr);
		void Remove(const StringBuffer& str);
		void ReplaceAt(size_t index, char c);
		void ReplaceAt(size_t index, wchar_t wc);
		void ReplaceAt(size_t index, const char* str);
		void ReplaceAt(size_t index, const wchar_t* wstr);
		void ReplaceAt(size_t index, const StringBuffer& str);
		void Insert(size_t index, char c);
		void Insert(size_t index, wchar_t wc);
		void Insert(size_t index, const char* str);
		void Insert(size_t index, const wchar_t* wstr);
		void Insert(size_t index, const StringBuffer& str);
		void TrimStart(char c);
		void TrimStart(wchar_t wc);
		void TrimStart(const char* str);
		void TrimStart(const wchar_t* wstr);
		void TrimStart(const StringBuffer& str);
		void TrimEnd(char c);
		void TrimEnd(wchar_t wc);
		void TrimEnd(const char* str);
		void TrimEnd(const wchar_t* wstr);
		void TrimEnd(const StringBuffer& str);
		void Trim(char c);
		void Trim(wchar_t wc);
		void Trim(const char* str);
		void Trim(const wchar_t* wstr);
		void Trim(const StringBuffer& str);
		void Clear();
		void Reverse();
		void ToLower();
		void ToUpper();
		bool IsNumber() const;
		bool IsNullOrEmpty() const;
		bool IsNullOrWhitespace() const;
		void* Begin() const;
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
		static StringBuffer ToString(const Guid& guid);
		static StringBuffer ToHexString(int8_t value);
		static StringBuffer ToHexString(uint8_t value);
		static StringBuffer ToHexString(int16_t value);
		static StringBuffer ToHexString(uint16_t value);
		static StringBuffer ToHexString(int32_t value);
		static StringBuffer ToHexString(uint32_t value);
		static StringBuffer ToHexString(int64_t value);
		static StringBuffer ToHexString(uint64_t value);
		static int16_t StringToI16(StringBuffer string);
		static uint16_t StringToU16(StringBuffer string);
		static int32_t StringToI32(StringBuffer string);
		static uint32_t StringToU32(StringBuffer string);
		static int64_t StringToI64(StringBuffer string);
		static uint64_t StringToU64(StringBuffer string);
		static double StringToDouble(StringBuffer string);
		static Guid StringToGuid(StringBuffer string);
		static int16_t HexStringToI16(StringBuffer hexString);
		static uint16_t HexStringToU16(StringBuffer hexString);
		static int32_t HexStringToI32(StringBuffer hexString);
		static uint32_t HexStringToU32(StringBuffer hexString);
		static int64_t HexStringToI64(StringBuffer hexString);
		static uint64_t HexStringToU64(StringBuffer hexString);
		static StringBuffer Join(char separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(wchar_t separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(const char* separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(const wchar_t* separator, const std::vector<StringBuffer>& strings);
		static StringBuffer Join(const StringBuffer& separator, const std::vector<StringBuffer>& strings);
	};

}
HephCommon::StringBuffer operator+(char lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator+(wchar_t lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator+(const char* const lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator+(const wchar_t* const lhs, const HephCommon::StringBuffer& rhs);
HephCommon::StringBuffer operator*(size_t lhs, const HephCommon::StringBuffer& rhs);