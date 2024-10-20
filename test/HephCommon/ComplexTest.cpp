#include "gtest/gtest.h"
#include "Complex.h"
#include "HephMath.h"
#include <ostream>

using namespace Heph;

namespace Heph
{
	void PrintTo(const Complex& c, std::ostream* os)
	{
		if (c.imag >= 0)
			*os << c.real << " + " << c.imag << "j";
		else
			*os << c.real << " - " << -c.imag << "j";
	}
}

TEST(ComplexTest, Compare)
{
	Complex c(5, 3);
	
	EXPECT_TRUE(c == Complex(5, 3));
	EXPECT_FALSE(c == Complex(-5, 3));
	EXPECT_FALSE(c == Complex(5, -3));
	EXPECT_FALSE(c == Complex(-5, -3));

	EXPECT_FALSE(c != Complex(5, 3));
	EXPECT_TRUE(c != Complex(-5, 3));
	EXPECT_TRUE(c != Complex(5, -3));
	EXPECT_TRUE(c != Complex(-5, -3));
}

TEST(ComplexTest, Add)
{
	Complex c1(5, 3);
	Complex c2(8, -20);

	EXPECT_EQ(c1 + c2, Complex(13, -17));
	EXPECT_EQ(c1 + 10, Complex(15, 3));
	EXPECT_EQ(10 + c1, Complex(15, 3));

	c1 += c2;
	EXPECT_EQ(c1, Complex(13, -17));

	c2 += 10;
	EXPECT_EQ(c2, Complex(18, -20));
}

TEST(ComplexTest, Subtract)
{
	Complex c1(5, 3);
	Complex c2(8, -20);

	EXPECT_EQ(c1 - c2, Complex(-3, 23));
	EXPECT_EQ(c1 - 10, Complex(-5, 3));
	EXPECT_EQ(10 - c1, Complex(5, -3));

	c1 -= c2;
	EXPECT_EQ(c1, Complex(-3, 23));

	c2 -= 10;
	EXPECT_EQ(c2, Complex(-2, -20));
}

TEST(ComplexTest, Multiply)
{
	Complex c1(5, 3);
	Complex c2(8, -10);

	EXPECT_EQ(c1 * c2, Complex(70, -26));
	EXPECT_EQ(c1 * 10, Complex(50, 30));
	EXPECT_EQ(10 * c1, Complex(50, 30));

	c1 *= c2;
	EXPECT_EQ(c1, Complex(70, -26));

	c2 *= 10;
	EXPECT_EQ(c2, Complex(80, -100));
}

TEST(ComplexTest, Divide)
{
	Complex c1(5, 3);
	Complex c2(8, -10);

	EXPECT_EQ(c1 / c2, Complex(5.0 / 82, 37.0 / 82));
	EXPECT_EQ(c1 / 10, Complex(0.5, 0.3));
	EXPECT_EQ(10 / c1, Complex(25.0 / 17, -15.0 / 17));

	c1 /= c2;
	EXPECT_EQ(c1, Complex(5.0 / 82, 37.0 / 82));

	c2 /= 10;
	EXPECT_EQ(c2, Complex(0.8, -1));
}

TEST(ComplexTest, Conjugate)
{
	Complex c(5, 3);

	EXPECT_EQ(c.Conjugate(), Complex(5, -3));
}

TEST(ComplexTest, Magnitude)
{
	Complex c(3, 4);

	EXPECT_EQ(c.MagnitudeSquared(), 25);
	EXPECT_EQ(c.Magnitude(), 5);
}

TEST(ComplexTest, Phase)
{
	EXPECT_EQ(Complex(1, 1).Phase(), HEPH_MATH_PI / 4);
	EXPECT_EQ(Complex(1, sqrt(3)).Phase(), HEPH_MATH_PI / 3);
}