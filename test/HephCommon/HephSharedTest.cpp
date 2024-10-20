#include "gtest/gtest.h"
#include "HephShared.h"

using namespace Heph;

namespace Heph
{
	void PrintTo(const Endian& endian, std::ostream* os)
	{
		switch (endian)
		{
		case Endian::Little:
			*os << "Little Endian";
			break;
		case Endian::Big:
			*os << "Big Endian";
			break;
		default:
			*os << "Unknown Endian";
			break;
		}
	}
}

TEST(HephSharedTest, NotOperator)
{
	EXPECT_EQ(!Endian::Big, Endian::Little);
	EXPECT_EQ(!Endian::Little, Endian::Big);
	EXPECT_EQ(!Endian::Unknown, Endian::Unknown);
}

TEST(HephSharedTest, ChangeEndian)
{
	uint16_t x = 0xABCD;
	HEPH_CHANGE_ENDIAN(&x, sizeof(uint16_t));
	EXPECT_EQ(x, 0xCDAB);
}