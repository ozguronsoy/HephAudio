#include "gtest/gtest.h"
#include "Buffers/ComplexBuffer.h"

using namespace Heph;

void PrintTo(const ComplexBuffer& b, std::ostream* os)
{
	*os << "{";

	for (size_t i = 0; i < b.Size(); ++i)
	{
		*os << "(" << b[i].real
			<< ((b[i].imag < 0) ? (" - ") : (" + "))
			<< b[i].imag << "j)";

		if (i != b.Size() - 1)
		{
			*os << ", ";
		}
	}

	*os << "}";
}

TEST(ComplexBufferTest, Constructors)
{
	{
		ComplexBuffer b;
		EXPECT_EQ(b.Size(), 0);
		EXPECT_TRUE(b.begin() == nullptr);
	}

	{
		ComplexBuffer b(5);
		EXPECT_EQ(b.Size(), 5);

		for (const Complex& data : b)
			EXPECT_EQ(data, Complex());
	}

	{
		ComplexBuffer b(5, BufferFlags::AllocUninitialized);
		EXPECT_EQ(b.Size(), 5);
		EXPECT_FALSE(b.begin() == nullptr);
	}

	{
		ComplexBuffer b = { 1, 5, 3, 4, 7 };
		Complex expected[5] = { Complex(1, 0), Complex(5, 0), Complex(3, 0), Complex(4, 0), Complex(7, 0) };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		ComplexBuffer b = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		Complex expected[5] = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}
}

TEST(ComplexBufferTest, Copy)
{
	{
		ComplexBuffer b = ComplexBuffer();
		Complex expected[5] = { Complex(1, 0), Complex(2, 0), Complex(3, 0), Complex(4, 0), Complex(5, 0) };

		b = { 1, 2, 3, 4, 5 };

		EXPECT_EQ(b.Size(), 5);
		EXPECT_FALSE(b.begin() == nullptr);

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		ComplexBuffer b = ComplexBuffer();
		Complex expected[5] = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };

		b = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };

		EXPECT_EQ(b.Size(), 5);
		EXPECT_FALSE(b.begin() == nullptr);

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		ComplexBuffer b1 = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		ComplexBuffer b2(b1);
		Complex expected[5] = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };

		EXPECT_EQ(b1, b2);
	}

	{
		ComplexBuffer b1 = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		ComplexBuffer b2 = ComplexBuffer();
		Complex expected[5] = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };

		b2 = b1;

		EXPECT_EQ(b1, b2);
	}

	{
		ComplexBuffer b = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		Complex expected[5] = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };

		EXPECT_NO_THROW(b = b);

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}
}

TEST(ComplexBufferTest, Move)
{
	{
		ComplexBuffer b1 = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		
		const size_t b2_expectedSize = b1.Size();
		const Complex* b2_expectedBegin = b1.begin();
		
		ComplexBuffer b2(std::move(b1));

		EXPECT_TRUE(b1.begin() == nullptr);
		EXPECT_EQ(b1.Size(), 0);
		EXPECT_EQ(b2.Size(), b2_expectedSize);
		EXPECT_EQ(b2.begin(), b2_expectedBegin);
	}

	{
		ComplexBuffer b1 = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		ComplexBuffer b2 = ComplexBuffer();

		const size_t b2_expectedSize = b1.Size();
		const Complex* b2_expectedBegin = b1.begin();

		b2 = std::move(b1);

		EXPECT_TRUE(b1.begin() == nullptr);
		EXPECT_EQ(b1.Size(), 0);
		EXPECT_EQ(b2.Size(), b2_expectedSize);
		EXPECT_EQ(b2.begin(), b2_expectedBegin);
	}

	{
		ComplexBuffer b = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		
		const size_t expectedSize = b.Size();
		const Complex* expectedBegin = b.begin();
		
		b = std::move(b);

		EXPECT_EQ(b.Size(), expectedSize);
		EXPECT_EQ(b.begin(), expectedBegin);
	}
}

TEST(ComplexBufferTest, Invert)
{
	{
		ComplexBuffer b = { Complex(1, -3), Complex(5, 6), Complex(3, -9), Complex(4, 12), Complex(7, -15) };
		Complex expected[5] = { Complex(-1, 3), Complex(-5, -6), Complex(-3, 9), Complex(-4, -12), Complex(-7, 15) };

		b.Invert();

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}
}