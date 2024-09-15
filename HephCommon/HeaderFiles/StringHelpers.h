#pragma once
#include "HephCommonShared.h"
#include "Guid.h"
#include <string>
#include <vector>

namespace HephCommon
{
	class StringHelpers final
	{
	public:
		StringHelpers() = delete;
		StringHelpers(const StringHelpers&) = delete;
		StringHelpers& operator=(const StringHelpers&) = delete;
		
	public:
		static std::wstring StrToWide(const std::string& str);
		static std::string WideToStr(const std::wstring& wstr);
		static std::vector<std::string> Split(const std::string& str, const std::string& separator);
		static std::vector<std::wstring> Split(const std::wstring& wstr, const std::wstring& separator);
		static std::string ToString(int16_t value);
		static std::string ToString(uint16_t value);
		static std::string ToString(int32_t value);
		static std::string ToString(uint32_t value);
		static std::string ToString(int64_t value);
		static std::string ToString(uint64_t value);
		static std::string ToString(double value);
		static std::string ToString(double value, size_t precision);
		static std::string ToString(const Guid& guid);
		static std::string ToHexString(int8_t value);
		static std::string ToHexString(uint8_t value);
		static std::string ToHexString(int16_t value);
		static std::string ToHexString(uint16_t value);
		static std::string ToHexString(int32_t value);
		static std::string ToHexString(uint32_t value);
		static std::string ToHexString(int64_t value);
		static std::string ToHexString(uint64_t value);
		static int16_t StringToS16(const std::string& string);
		static uint16_t StringToU16(const std::string& string);
		static int32_t StringToS32(const std::string& string);
		static uint32_t StringToU32(const std::string& string);
		static int64_t StringToS64(const std::string& string);
		static uint64_t StringToU64(const std::string& string);
		static double StringToDouble(const std::string& string);
		static Guid StringToGuid(const std::string& string);
		static int16_t HexStringToS16(const std::string& hexString);
		static uint16_t HexStringToU16(const std::string& hexString);
		static int32_t HexStringToS32(const std::string& hexString);
		static uint32_t HexStringToU32(const std::string& hexString);
		static int64_t HexStringToS64(const std::string& hexString);
		static uint64_t HexStringToU64(const std::string& hexString);
	};
}