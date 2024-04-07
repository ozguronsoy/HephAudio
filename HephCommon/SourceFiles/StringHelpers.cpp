#include "StringHelpers.h"
#include "HephException.h"
#include <cstdlib>

#define HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE 512

namespace HephCommon
{
	std::wstring StringHelpers::StrToWide(const std::string& str)
	{
		if (str.size() > 0)
		{
			const size_t strSize_byte = str.size() * sizeof(wchar_t) + sizeof(wchar_t);
			wchar_t* pTemp = (wchar_t*)malloc(strSize_byte);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "StringHelpers::StrToWide", "Insufficient memory."));
			}
			(void)memset(pTemp, 0, strSize_byte);

			mbstowcs(pTemp, str.c_str(), str.size());

			const std::wstring wstr = pTemp;
			free(pTemp);

			return wstr;
		}
		return L"";
	}
	std::string StringHelpers::WideToStr(const std::wstring& wstr)
	{
		if (wstr.size() > 0)
		{
			const size_t strSize_byte = wstr.size() * sizeof(char) + sizeof(char);
			char* pTemp = (char*)malloc(strSize_byte);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "StringHelpers::WideToStr", "Insufficient memory."));
			}
			(void)memset(pTemp, 0, strSize_byte);

			(void)wcstombs(pTemp, wstr.c_str(), wstr.size());

			const std::string str = pTemp;
			free(pTemp);

			return str;
		}
		return "";
	}
	std::vector<std::string> StringHelpers::Split(const std::string& str, const std::string& separator)
	{
		size_t pos = str.find_first_of(separator);
		if (pos == std::string::npos)
		{
			return { str };
		}

		std::vector<std::string> result;
		size_t lastPos = 0;
		
		do
		{
			result.push_back(str.substr(lastPos, pos - lastPos));
			lastPos = pos + 1;
			pos = str.find_first_of(separator, lastPos);
		} while (pos != std::string::npos);
		result.push_back(str.substr(lastPos));

		return result;
	}
	std::vector<std::wstring> StringHelpers::Split(const std::wstring& str, const std::wstring& separator)
	{
		size_t pos = str.find_first_of(separator);
		if (pos == std::wstring::npos)
		{
			return { str };
		}

		std::vector<std::wstring> result;
		size_t lastPos = 0;

		do
		{
			result.push_back(str.substr(lastPos, pos - lastPos));
			lastPos = pos + 1;
			pos = str.find_first_of(separator, lastPos);
		} while (pos != std::wstring::npos);
		result.push_back(str.substr(lastPos));

		return result;
	}
	std::string StringHelpers::ToString(int16_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%hd", value);
		return buffer;
	}
	std::string StringHelpers::ToString(uint16_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%hu", value);
		return buffer;
	}
	std::string StringHelpers::ToString(int32_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%d", value);
		return buffer;
	}
	std::string StringHelpers::ToString(uint32_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%u", value);
		return buffer;
	}
	std::string StringHelpers::ToString(int64_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%lld", value);
		return buffer;
	}
	std::string StringHelpers::ToString(uint64_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%llu", value);
		return buffer;
	}
	std::string StringHelpers::ToString(double value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%lf", value);
		return buffer;
	}
	std::string StringHelpers::ToString(double value, size_t precision)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%.*lf", (unsigned int)precision, value);
		return buffer;
	}
	std::string StringHelpers::ToString(const Guid& guid)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", guid.data1, guid.data2, guid.data3,
			guid.data4[0], guid.data4[1], guid.data4[2], guid.data4[3],
			guid.data4[4], guid.data4[5], guid.data4[6], guid.data4[7]);
		return buffer;
	}
	std::string StringHelpers::ToHexString(int8_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%02X", value);
		return buffer;
	}
	std::string StringHelpers::ToHexString(uint8_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%02X", value);
		return buffer;
	}
	std::string StringHelpers::ToHexString(int16_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%04X", value);
		return buffer;
	}
	std::string StringHelpers::ToHexString(uint16_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%04X", value);
		return buffer;
	}
	std::string StringHelpers::ToHexString(int32_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%08X", value);
		return buffer;
	}
	std::string StringHelpers::ToHexString(uint32_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%08X", value);
		return buffer;
	}
	std::string StringHelpers::ToHexString(int64_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%016llX", value);
		return buffer;
	}
	std::string StringHelpers::ToHexString(uint64_t value)
	{
		char buffer[HEPH_STRING_HELPERS_TO_STRING_BUFFER_SIZE]{ 0 };
		(void)sprintf(buffer, "0x%016llX", value);
		return buffer;
	}
	int16_t StringHelpers::StringToS16(const std::string& string)
	{
		int16_t number = 0;
		(void)sscanf(string.c_str(), "%hd", &number);
		return number;
	}
	uint16_t StringHelpers::StringToU16(const std::string& string)
	{
		uint16_t number = 0;
		(void)sscanf(string.c_str(), "%hu", &number);
		return number;
	}
	int32_t StringHelpers::StringToS32(const std::string& string)
	{
		int32_t number = 0;
		(void)sscanf(string.c_str(), "%d", &number);
		return number;
	}
	uint32_t StringHelpers::StringToU32(const std::string& string)
	{
		uint32_t number = 0;
		(void)sscanf(string.c_str(), "%u", &number);
		return number;
	}
	int64_t StringHelpers::StringToS64(const std::string& string)
	{
		int64_t number = 0;
		(void)sscanf(string.c_str(), "%lld", &number);
		return number;
	}
	uint64_t StringHelpers::StringToU64(const std::string& string)
	{
		uint64_t number = 0;
		(void)sscanf(string.c_str(), "%llu", &number);
		return number;
	}
	double StringHelpers::StringToDouble(const std::string& string)
	{
		double number = 0;
		(void)sscanf(string.c_str(), "%lf", &number);
		return number;
	}
	Guid StringHelpers::StringToGuid(const std::string& string)
	{
		Guid guid;
		const std::vector<std::string> dataStr = StringHelpers::Split(string, "-");
		(void)sscanf(dataStr[0].c_str(), "%x", &guid.data1);
		(void)sscanf(dataStr[1].c_str(), "%04x", &guid.data2);
		(void)sscanf(dataStr[2].c_str(), "%04x", &guid.data3);
		(void)sscanf(dataStr[3].c_str(), "%02x", guid.data4);
		(void)sscanf(dataStr[3].c_str() + 2, "%02x", guid.data4 + 1);
		(void)sscanf(dataStr[4].c_str(), "%02x%02x%02x%02x%02x%02x", guid.data4 + 2, guid.data4 + 3, guid.data4 + 4, guid.data4 + 5, guid.data4 + 6, guid.data4 + 7);
		return guid;
	}
	int16_t StringHelpers::HexStringToS16(const std::string& hexString)
	{
		int16_t number = 0;
		(void)sscanf(hexString.c_str(), "%hX", &number);
		return number;
	}
	uint16_t StringHelpers::HexStringToU16(const std::string& hexString)
	{
		uint16_t number = 0;
		(void)sscanf(hexString.c_str(), "%hX", &number);
		return number;
	}
	int32_t StringHelpers::HexStringToS32(const std::string& hexString)
	{
		int32_t number = 0;
		(void)sscanf(hexString.c_str(), "%X", &number);
		return number;
	}
	uint32_t StringHelpers::HexStringToU32(const std::string& hexString)
	{
		uint32_t number = 0;
		(void)sscanf(hexString.c_str(), "%X", &number);
		return number;
	}
	int64_t StringHelpers::HexStringToS64(const std::string& hexString)
	{
		int64_t number = 0;
		(void)sscanf(hexString.c_str(), "%llX", &number);
		return number;
	}
	uint64_t StringHelpers::HexStringToU64(const std::string& hexString)
	{
		uint64_t number = 0;
		(void)sscanf(hexString.c_str(), "%llX", &number);
		return number;
	}
}