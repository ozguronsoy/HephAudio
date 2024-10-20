#include "gtest/gtest.h"
#include "Guid.h"
#include <ostream>

using namespace Heph;

namespace Heph
{
	void PrintTo(const Guid& guid, std::ostream* os)
	{
		*os << std::uppercase;
		os->width(8);
		*os << std::hex << guid.data1 << '-';

		os->width(4);
		*os << std::hex << guid.data2 << '-';

		os->width(4);
		*os << std::hex << guid.data3 << '-';

		os->width(2);
		*os << std::hex
			<< static_cast<short>(guid.data4[0])
			<< static_cast<short>(guid.data4[1])
			<< '-'
			<< static_cast<short>(guid.data4[2])
			<< static_cast<short>(guid.data4[3])
			<< static_cast<short>(guid.data4[4])
			<< static_cast<short>(guid.data4[5])
			<< static_cast<short>(guid.data4[6])
			<< static_cast<short>(guid.data4[7]);
	}
}

TEST(GuidTest, Compare)
{
	Guid g1;
	g1.data1 = 0xABCDEF12;
	g1.data2 = 0x3456;
	g1.data3 = 0x7890;
	g1.data4[0] = 0x53;
	g1.data4[1] = 0x78;
	g1.data4[4] = 0x2B;
	g1.data4[7] = 0xA2;

	Guid g2;
	g2.data1 = 0xABCDEF12;
	g2.data2 = 0x3456;
	g2.data3 = 0x7890;
	g2.data4[0] = 0x53;
	g2.data4[1] = 0x78;
	g2.data4[4] = 0x2B;
	g2.data4[7] = 0xA2;

	EXPECT_EQ(g1, g2);

	g2.data1 = 0;
	EXPECT_NE(g1, g2);
}

TEST(GuidTest, GenerateNew)
{
	constexpr size_t iterationCount = 100;
	for (size_t i = 0; i < iterationCount; ++i)
	{
		const Guid g1 = Guid::GenerateNew();
		const Guid g2 = Guid::GenerateNew();
		EXPECT_NE(g1, g2);
	}
}