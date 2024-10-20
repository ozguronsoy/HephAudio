#include "gtest/gtest.h"
#include "HephMath.h"

TEST(HephMathTest, Sgn)
{
	EXPECT_EQ(HEPH_MATH_SGN(53), 1);
	EXPECT_EQ(HEPH_MATH_SGN(-37), -1);
	EXPECT_EQ(HEPH_MATH_SGN(0), 0);
}

TEST(HephMathTest, Min)
{
	EXPECT_EQ(HEPH_MATH_MIN(10, 20), 10);
	EXPECT_EQ(HEPH_MATH_MIN(20, 10), 10);
	EXPECT_EQ(HEPH_MATH_MIN(-10, 8), -10);
	EXPECT_EQ(HEPH_MATH_MIN(0, 8), 0);
	EXPECT_EQ(HEPH_MATH_MIN(-7, 0), -7);
}

TEST(HephMathTest, Max)
{
	EXPECT_EQ(HEPH_MATH_MAX(10, 20), 20);
	EXPECT_EQ(HEPH_MATH_MAX(20, 10), 20);
	EXPECT_EQ(HEPH_MATH_MAX(-10, 8), 8);
	EXPECT_EQ(HEPH_MATH_MAX(0, 8), 8);
	EXPECT_EQ(HEPH_MATH_MAX(-7, 0), 0);
}

TEST(HephMathTest, Rad2Deg)
{
	EXPECT_EQ(HEPH_MATH_RAD_TO_DEG(HEPH_MATH_PI), 180);
	EXPECT_EQ(HEPH_MATH_RAD_TO_DEG(-HEPH_MATH_PI / 4), -45);
}

TEST(HephMathTest, Deg2Rad)
{
	EXPECT_EQ(HEPH_MATH_DEG_TO_RAD(180), HEPH_MATH_PI);
	EXPECT_EQ(HEPH_MATH_DEG_TO_RAD(-45), -HEPH_MATH_PI / 4);
}