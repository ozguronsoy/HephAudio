#include "gtest/gtest.h"
#include "StringHelpers.h"

using namespace Heph;

TEST(StringHelpersTest, StrToWide)
{
	std::string str = "Hello World!";
	std::wstring wstr = L"Hello World!";

	EXPECT_EQ(StringHelpers::StrToWide(str), wstr);
}

TEST(StringHelpersTest, WideToStr)
{
	std::string str = "Hello World!";
	std::wstring wstr = L"Hello World!";

	EXPECT_EQ(StringHelpers::WideToStr(wstr), str);
}

TEST(StringHelpersTest, Split)
{
	{
		std::string str = "Hello World";
		std::vector<std::string> split_str1 = { "Hello World" };
		std::vector<std::string> split_str2 = { "Hello", "World" };
		std::vector<std::string> split_str3 = { "He", "", "o Wor", "d" };
		std::vector<std::string> split_str4 = { "", "ello World" };
		std::vector<std::string> split_str5 = { "Hello Worl", "" };

		EXPECT_EQ(StringHelpers::Split(str, ""), split_str1);
		EXPECT_EQ(StringHelpers::Split(str, " "), split_str2);
		EXPECT_EQ(StringHelpers::Split(str, "l"), split_str3);
		EXPECT_EQ(StringHelpers::Split(str, "H"), split_str4);
		EXPECT_EQ(StringHelpers::Split(str, "d"), split_str5);
	}

	{
		std::wstring str = L"Hello World";
		std::vector<std::wstring> split_str1 = { L"Hello World" };
		std::vector<std::wstring> split_str2 = { L"Hello", L"World" };
		std::vector<std::wstring> split_str3 = { L"He", L"", L"o Wor", L"d" };
		std::vector<std::wstring> split_str4 = { L"", L"ello World" };
		std::vector<std::wstring> split_str5 = { L"Hello Worl", L"" };

		EXPECT_EQ(StringHelpers::Split(str, L""), split_str1);
		EXPECT_EQ(StringHelpers::Split(str, L" "), split_str2);
		EXPECT_EQ(StringHelpers::Split(str, L"l"), split_str3);
		EXPECT_EQ(StringHelpers::Split(str, L"H"), split_str4);
		EXPECT_EQ(StringHelpers::Split(str, L"d"), split_str5);
	}
}

TEST(StringHelpersTest, ToString)
{
	int16_t		s16 = -16;
	uint16_t	u16 = 16;
	int32_t		s32 = -32;
	uint32_t	u32 = 32;
	int64_t		s64 = -64;
	uint64_t	u64 = 64;
	float		f = 1.23456289;
	double		d = 5.43184214;

	EXPECT_EQ(StringHelpers::ToString(s16), "-16");
	EXPECT_EQ(StringHelpers::ToString(u16), "16");
	EXPECT_EQ(StringHelpers::ToString(s32), "-32");
	EXPECT_EQ(StringHelpers::ToString(u32), "32");
	EXPECT_EQ(StringHelpers::ToString(s64), "-64");
	EXPECT_EQ(StringHelpers::ToString(u64), "64");
	EXPECT_EQ(StringHelpers::ToString(f, 5), "1.23456");
	EXPECT_EQ(StringHelpers::ToString(d, 3), "5.432");

	Guid guid;
	guid.data1 = 0xABCDEF12;
	guid.data2 = 0x3456;
	guid.data3 = 0x7890;
	guid.data4[0] = 0x53;
	guid.data4[1] = 0x78;
	guid.data4[4] = 0x2B;
	guid.data4[7] = 0xA2;

	EXPECT_EQ(StringHelpers::ToString(guid), "ABCDEF12-3456-7890-5378-00002B0000A2");
}

TEST(StringHelpersTest, ToHexString)
{
	int8_t		s8 = 0xAB;
	int16_t		s16 = 0xAB00;
	int32_t		s32 = 0xABCDEF00;
	int64_t		s64 = 0xABCDEF1234567800;

	EXPECT_EQ(StringHelpers::ToHexString(s8), "0xAB");
	EXPECT_EQ(StringHelpers::ToHexString((uint8_t)s8), "0xAB");

	EXPECT_EQ(StringHelpers::ToHexString(s16), "0xAB00");
	EXPECT_EQ(StringHelpers::ToHexString((uint16_t)s16), "0xAB00");

	EXPECT_EQ(StringHelpers::ToHexString(s32), "0xABCDEF00");
	EXPECT_EQ(StringHelpers::ToHexString((uint32_t)s32), "0xABCDEF00");

	EXPECT_EQ(StringHelpers::ToHexString(s64), "0xABCDEF1234567800");
	EXPECT_EQ(StringHelpers::ToHexString((uint64_t)s64), "0xABCDEF1234567800");
}

TEST(StringHelpersTest, FromString)
{
	EXPECT_EQ(StringHelpers::StringToU16("35"), 35);
	EXPECT_EQ(StringHelpers::StringToS16("-35"), -35);

	EXPECT_EQ(StringHelpers::StringToU32("35"), 35);
	EXPECT_EQ(StringHelpers::StringToS32("-35"), -35);

	EXPECT_EQ(StringHelpers::StringToU64("35"), 35);
	EXPECT_EQ(StringHelpers::StringToS64("-35"), -35);

	EXPECT_EQ(StringHelpers::StringToDouble("0.351342"), 0.351342);

	Guid guid = Guid::GenerateNew();
	EXPECT_EQ(StringHelpers::StringToGuid(StringHelpers::ToString(guid)), guid);
}

TEST(StringHelpersTest, FromHexString)
{
	int16_t		s16 = 0x8B00;
	uint16_t	u16 = 0x8B00;
	int32_t		s32 = 0x8BCDEF00;
	uint32_t	u32 = 0x8BCDEF00;
	int64_t		s64 = 0x8BCDEF1234567800;
	uint64_t	u64 = 0x8BCDEF1234567800;

	EXPECT_EQ(StringHelpers::HexStringToS16("8B00"), s16);
	EXPECT_EQ(StringHelpers::HexStringToS16("0x8B00"), s16);
	EXPECT_EQ(StringHelpers::HexStringToU16("8B00"), u16);
	EXPECT_EQ(StringHelpers::HexStringToU16("0x8B00"), u16);

	EXPECT_EQ(StringHelpers::HexStringToS32("8BCDEF00"), s32);
	EXPECT_EQ(StringHelpers::HexStringToS32("0x8BCDEF00"), s32);
	EXPECT_EQ(StringHelpers::HexStringToU32("8BCDEF00"), u32);
	EXPECT_EQ(StringHelpers::HexStringToU32("0x8BCDEF00"), u32);

	EXPECT_EQ(StringHelpers::HexStringToS64("8BCDEF1234567800"), s64);
	EXPECT_EQ(StringHelpers::HexStringToS64("0x8BCDEF1234567800"), s64);
	EXPECT_EQ(StringHelpers::HexStringToU64("8BCDEF1234567800"), u64);
	EXPECT_EQ(StringHelpers::HexStringToU64("0x8BCDEF1234567800"), u64);
}