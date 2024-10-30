#include "gtest/gtest.h"
#include "Buffers/DoubleBuffer.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

TEST(ArithmeticBufferTest, Min)
{
	DoubleBuffer b = { 3.27, 5.61, -9.38, -30.56, 60.267, 2.51, -5.63 };
	const double expected = -30.56;
	EXPECT_EQ(b.Min(), expected);
}

TEST(ArithmeticBufferTest, Max)
{
	DoubleBuffer b = { 3.27, 5.61, -9.38, -30.56, 60.267, 2.51, -5.63 };
	const double expected = 60.267;
	EXPECT_EQ(b.Max(), expected);
}

TEST(ArithmeticBufferTest, Rms)
{
	DoubleBuffer b = { 3.27, 5.61, -9.38, -30.56, 60.267, 2.51, -5.63 };
	const double expected = 26.006;
	EXPECT_NEAR(b.Rms(), expected, 0.001);
}

TEST(ArithmeticBufferTest, Invert)
{
	DoubleBuffer b = { 3.27, 5.61, -9.38, -30.56, 60.267, 2.51, -5.63 };
	DoubleBuffer expected = { -3.27, -5.61, 9.38, 30.56, -60.267, -2.51, 5.63 };

	b.Invert();
	EXPECT_EQ(b, expected);
}

TEST(ArithmeticBufferTest, UnaryMinusOperator)
{
	DoubleBuffer b = { 3.27, 5.61, -9.38, -30.56, 60.267, 2.51, -5.63 };
	DoubleBuffer expected = { -3.27, -5.61, 9.38, 30.56, -60.267, -2.51, 5.63 };

	EXPECT_EQ(-b, expected);
}

TEST(ArithmeticBufferTest, AbsMax)
{
	DoubleBuffer b = { 3.27, 5.61, -9.38, -30.56, 60.267, -120.36, 2.51, -5.63 };
	const double expected = 120.36;

	EXPECT_EQ(b.AbsMax(), expected);
}

TEST(ArithmeticBufferTest, AdditionOperator)
{
	DoubleBuffer b1 = { 1, 2, 3, 4 };
	DoubleBuffer b2 = { 5, 6, 7, 8 };
	DoubleBuffer expected = { 6, 8, 10, 12 };

	EXPECT_EQ(b1 + b2, expected);

	b1 += b2;
	EXPECT_EQ(b1, expected);

	DoubleBuffer b3 = { 1 };
	EXPECT_THROW(b1 + b3, InvalidArgumentException);
}

TEST(ArithmeticBufferTest, SubtractionOperator)
{
	DoubleBuffer b1 = { 1, 2, 3, 4 };
	DoubleBuffer b2 = { 5, 10, 15, 20 };
	DoubleBuffer expected = { -4, -8, -12, -16 };

	EXPECT_EQ(b1 - b2, expected);

	b1 -= b2;
	EXPECT_EQ(b1, expected);

	DoubleBuffer b3 = { 1 };
	EXPECT_THROW(b1 - b3, InvalidArgumentException);
}

TEST(ArithmeticBufferTest, MultiplicationOperator)
{
	DoubleBuffer b1 = { 1, 2, 3, 4 };
	DoubleBuffer b2 = { 5, 6, 7, 8 };
	DoubleBuffer expected = { 5, 12, 21, 32 };

	EXPECT_EQ(b1 * b2, expected);

	b1 *= b2;
	EXPECT_EQ(b1, expected);

	DoubleBuffer b3 = { 1 };
	EXPECT_THROW(b1 * b3, InvalidArgumentException);
}

TEST(ArithmeticBufferTest, DivisionOperator)
{
	DoubleBuffer b1 = { 1, 2, 3, 4 };
	DoubleBuffer b2 = { 5, 6, 7, 8 };
	DoubleBuffer expected = { 1.0/5, 2.0/6, 3.0/7, 4.0/8 };

	EXPECT_EQ(b1 / b2, expected);

	b1 /= b2;
	EXPECT_EQ(b1, expected);

	DoubleBuffer b3 = { 1 };
	EXPECT_THROW(b1 / b3, InvalidArgumentException);
}