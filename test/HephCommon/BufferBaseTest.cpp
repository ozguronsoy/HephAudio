#include "gtest/gtest.h"
#include "Buffers/BufferBase.h"
#include <cinttypes>

using namespace Heph;
using test_data_t = uint32_t;

class TestBuffer : public BufferBase<TestBuffer, test_data_t>
{
public:
	TestBuffer() : BufferBase() {}

	explicit TestBuffer(size_t size) : BufferBase(size) {}

	TestBuffer(size_t size, BufferFlags flags) : BufferBase(size, flags) {}

	TestBuffer(const std::initializer_list<test_data_t>& rhs) : BufferBase(rhs) {}

	TestBuffer(const TestBuffer& rhs) : BufferBase(rhs) {}

	TestBuffer(TestBuffer&& rhs) noexcept : BufferBase(std::move(rhs)) {}

	TestBuffer& operator=(const std::initializer_list<test_data_t>& rhs)
	{
		this->Release();

		if (rhs.size() > 0)
		{
			this->size = rhs.size();
			const size_t size_byte = this->SizeAsByte();

			this->pData = BufferBase::AllocateUninitialized(size_byte);
			(void)std::memcpy(this->pData, rhs.begin(), size_byte);
		}

		return *this;
	}

	TestBuffer& operator=(const TestBuffer& rhs)
	{
		this->Release();

		if (this != &rhs && !rhs.IsEmpty())
		{
			const size_t size_byte = rhs.SizeAsByte();

			this->pData = BufferBase::AllocateUninitialized(size_byte);
			(void)std::memcpy(this->pData, rhs.pData, size_byte);

			this->size = rhs.size;
		}

		return *this;
	}

	TestBuffer& operator=(TestBuffer&& rhs) noexcept
	{
		this->Release();

		if (this != &rhs)
		{
			this->pData = rhs.pData;
			this->size = rhs.size;

			rhs.pData = nullptr;
			rhs.size = 0;
		}

		return *this;
	}
};

void PrintTo(const TestBuffer& b, std::ostream* os)
{
	*os << "{";

	for (size_t i = 0; i < b.Size(); ++i)
	{
		*os << b[i];
		if (i != b.Size() - 1)
		{
			*os << ", ";
		}
	}

	*os << "}";
}

TEST(BufferBaseTest, Constructors)
{
	{
		TestBuffer b;
		EXPECT_EQ(b.Size(), 0);
		EXPECT_TRUE(b.begin() == nullptr);
	}

	{
		TestBuffer b(5);
		EXPECT_EQ(b.Size(), 5);

		for (const test_data_t& data : b)
			EXPECT_EQ(data, 0);
	}

	{
		TestBuffer b(5, BufferFlags::AllocUninitialized);
		EXPECT_EQ(b.Size(), 5);
		EXPECT_FALSE(b.begin() == nullptr);
	}

	{
		TestBuffer b = { 1, 5, 3, 4, 7 };
		test_data_t expected[5] = { 1, 5, 3, 4, 7 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}
}

TEST(BufferBaseTest, Copy)
{
	TestBuffer b1 = { 1, 2, 3, 4, 5 };
	TestBuffer b2(b1);

	EXPECT_EQ(b1.Size(), b2.Size());

	for (size_t i = 0; i < b1.Size(); ++i)
		EXPECT_EQ(b1[i], b2[i]);
}

TEST(BufferBaseTest, Move)
{
	TestBuffer b1 = { 1, 2, 3, 4, 5 };

	test_data_t* b1_begin = b1.begin();
	size_t b1_size = b1.Size();

	TestBuffer b2(std::move(b1));

	EXPECT_EQ(b1.Size(), 0);
	EXPECT_TRUE(b1.begin() == nullptr);
	EXPECT_EQ(b2.Size(), b1_size);
	EXPECT_EQ(b2.begin(), b1_begin);
}

TEST(BufferBaseTest, LeftShift)
{
	TestBuffer b1 = { 1, 2, 3, 4, 5 };

	{
		TestBuffer b2(b1 << 2);
		test_data_t expected[] = { 3, 4, 5, 0, 0 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2(b1 << 0);
		test_data_t expected[] = { 1, 2, 3, 4, 5 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2(b1 << 10);
		test_data_t expected[] = { 0, 0, 0, 0, 0 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2 = b1;
		b2 <<= 2;
		test_data_t expected[] = { 3, 4, 5, 0, 0 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2 = b1;
		b2 <<= 0;
		test_data_t expected[] = { 1, 2, 3, 4, 5 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2 = b1;
		b2 <<= 10;
		test_data_t expected[] = { 0, 0, 0, 0, 0 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}
}

TEST(BufferBaseTest, RightShift)
{
	TestBuffer b1 = { 1, 2, 3, 4, 5 };

	{
		TestBuffer b2(b1 >> 2);
		test_data_t expected[] = { 0, 0, 1, 2, 3 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2(b1 >> 0);
		test_data_t expected[] = { 1, 2, 3, 4, 5 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2(b1 >> 10);
		test_data_t expected[] = { 0, 0, 0, 0, 0 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2 = b1;
		b2 >>= 2;
		test_data_t expected[] = { 0, 0, 1, 2, 3 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2 = b1;
		b2 >>= 0;
		test_data_t expected[] = { 1, 2, 3, 4, 5 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}

	{
		TestBuffer b2 = b1;
		b2 >>= 10;
		test_data_t expected[] = { 0, 0, 0, 0, 0 };

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b2[i], expected[i]);
	}
}

TEST(BufferBaseTest, Compare)
{
	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 1, 2, 3, 4, 5 };

		EXPECT_EQ(b1, b2);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 1, 2, 3, 4, 70 };

		EXPECT_NE(b1, b2);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 1, 2, 3 };

		EXPECT_NE(b1, b2);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 1, 2, 3, 4, 5, 6, 7, 8 };

		EXPECT_NE(b1, b2);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2;

		EXPECT_NE(b1, b2);
	}
}

TEST(BufferBaseTest, SubscriptOperator)
{
	TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };

	EXPECT_EQ(b[0], 1);
	EXPECT_EQ(b[1], 2);
	EXPECT_EQ(b[4], 5);
	EXPECT_EQ(b[6], 7);
}

TEST(BufferBaseTest, Size)
{
	TestBuffer b;

	EXPECT_EQ(b.Size(), 0);

	b = { 1, 2, 3, 4, 5, 6, 7, 8 };

	EXPECT_EQ(b.Size(), 8);
}

TEST(BufferBaseTest, SizeAsByte)
{
	TestBuffer b;

	EXPECT_EQ(b.Size(), 0);
	EXPECT_EQ(b.SizeAsByte(), 0);

	b = { 1, 2, 3, 4, 5, 6, 7, 8 };

	EXPECT_EQ(b.Size(), 8);
	EXPECT_EQ(b.SizeAsByte(), 8 * sizeof(test_data_t));
}

TEST(BufferBaseTest, At)
{
	TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };

	EXPECT_EQ(b.At(0), 1);
	EXPECT_EQ(b.At(1), 2);
	EXPECT_EQ(b.At(4), 5);
	EXPECT_EQ(b.At(6), 7);

	EXPECT_THROW(b.At(b.Size() + 1), InvalidArgumentException);
}

TEST(BufferBaseTest, IsEmpty)
{
	TestBuffer b;

	EXPECT_TRUE(b.IsEmpty());

	b = { 1, 2, 3, 4, 5 };

	EXPECT_FALSE(b.IsEmpty());
}

TEST(BufferBaseTest, Reset)
{
	TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };

	b.Reset();

	for (size_t i = 0; i < b.Size(); ++i)
		EXPECT_EQ(b[i], 0);
}

TEST(BufferBaseTest, Release)
{
	TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };

	b.Release();

	EXPECT_TRUE(b.begin() == nullptr);
	EXPECT_EQ(b.Size(), 0);

	// test if actually freed?
}

TEST(BufferBaseTest, Subbuffer)
{
	{
		TestBuffer b = { 1, 2, 3, 4, 5 };

		{
			TestBuffer sb = b.SubBuffer(2, 2);
			test_data_t expected[] = { 3, 4 };

			EXPECT_EQ(sb.Size(), 2);

			for (size_t i = 0; i < 2; ++i)
				EXPECT_EQ(sb[i], expected[i]);
		}

		{
			TestBuffer sb = b.SubBuffer(3, 5);
			test_data_t expected[] = { 4, 5, 0, 0, 0 };

			EXPECT_EQ(sb.Size(), 5);

			for (size_t i = 0; i < 3; ++i)
				EXPECT_EQ(sb[i], expected[i]);
		}

		EXPECT_THROW(b.SubBuffer(b.Size() + 1, 5), InvalidArgumentException);

		{
			TestBuffer sb = b.SubBuffer(3, 0);
			EXPECT_EQ(sb.Size(), 0);
		}
	}


	{
		TestBuffer b;
		EXPECT_THROW(b.SubBuffer(0, 3), InvalidArgumentException);
	}
}

TEST(BufferBaseTest, Prepend)
{
	{
		TestBuffer b1 = { 4, 5, 6 };
		TestBuffer b2 = { 1, 2, 3 };
		test_data_t expected[] = { 1, 2, 3, 4, 5, 6 };
		size_t expectedSize = b1.Size() + b2.Size();

		b1.Prepend(b2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < b1.Size(); ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1 = { 4, 5, 6 };
		TestBuffer b2;
		test_data_t expected[] = { 4, 5, 6 };
		size_t expectedSize = b1.Size();

		b1.Prepend(b2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < b1.Size(); ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1;
		TestBuffer b2 = { 1, 2, 3 };
		test_data_t expected[] = { 1, 2, 3 };
		size_t expectedSize = b2.Size();

		b1.Prepend(b2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < b1.Size(); ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1;
		TestBuffer b2;

		b1.Prepend(b2);
		EXPECT_EQ(b1.Size(), 0);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3, 4, 5, 1, 2, 3, 4, 5 };
		size_t expectedSize = b.Size() * 2;

		b.Prepend(b);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}
}

TEST(BufferBaseTest, Append)
{
	{
		TestBuffer b1 = { 4, 5, 6 };
		TestBuffer b2 = { 1, 2, 3 };
		test_data_t expected[] = { 4, 5, 6, 1, 2, 3 };
		size_t expectedSize = b1.Size() + b2.Size();

		b1.Append(b2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < b1.Size(); ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1 = { 4, 5, 6 };
		TestBuffer b2;
		test_data_t expected[] = { 4, 5, 6 };
		size_t expectedSize = b1.Size();

		b1.Append(b2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < b1.Size(); ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1;
		TestBuffer b2 = { 1, 2, 3 };
		test_data_t expected[] = { 1, 2, 3 };
		size_t expectedSize = b2.Size();

		b1.Append(b2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < b1.Size(); ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1;
		TestBuffer b2;

		b1.Append(b2);
		EXPECT_EQ(b1.Size(), 0);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3, 4, 5, 1, 2, 3, 4, 5 };
		size_t expectedSize = b.Size() * 2;

		b.Append(b);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}
}

TEST(BufferBaseTest, Insert)
{
	{
		TestBuffer b1 = { 1, 2, 3, 4 };
		TestBuffer b2 = { 5, 6, 7, 8 };
		test_data_t expected[] = { 1, 2, 5, 6, 7, 8, 3, 4 };
		size_t expectedSize = b1.Size() + b2.Size();

		b1.Insert(b2, 2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4 };
		TestBuffer b2 = { 5, 6, 7, 8 };
		test_data_t expected[] = { 5, 6, 7, 8, 1, 2, 3, 4 };
		size_t expectedSize = b1.Size() + b2.Size();

		b1.Insert(b2, 0);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4 };
		TestBuffer b2 = { 5, 6, 7, 8 };
		test_data_t expected[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		size_t expectedSize = b1.Size() + b2.Size();

		b1.Insert(b2, b1.Size());

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4 };
		TestBuffer b2 = { 5, 6, 7, 8 };
		test_data_t expected[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

		EXPECT_THROW(b1.Insert(b2, b1.Size() + 1), InvalidArgumentException);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4 };
		TestBuffer b2;
		test_data_t expected[] = { 1, 2, 3, 4 };
		size_t expectedSize = b1.Size();

		b1.Insert(b2, 0);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1;
		TestBuffer b2 = { 1, 2, 3, 4 };
		test_data_t expected[] = { 1, 2, 3, 4 };
		size_t expectedSize = b2.Size();

		b1.Insert(b2, 0);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1;
		TestBuffer b2;

		b1.Insert(b2, 0);
		EXPECT_EQ(b1.Size(), 0);
	}

	{
		TestBuffer b = { 1, 2, 3, 4 };
		test_data_t expected[] = { 1, 2, 1, 2, 3, 4, 3, 4 };
		size_t expectedSize = b.Size() * 2;

		b.Insert(b, 2);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}
}

TEST(BufferBaseTest, Cut)
{
	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 5 };
		size_t expectedSize = 3;

		b.Cut(2, 2);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 4, 5 };
		size_t expectedSize = 2;

		b.Cut(0, 3);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3 };
		size_t expectedSize = 3;

		b.Cut(3, 10);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 4, 5 };
		size_t expectedSize = 2;

		b.Cut(0, 3);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3, 4, 5 };
		size_t expectedSize = b.Size();

		b.Cut(2, 0);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b;
		b.Cut(0, 0);
		EXPECT_EQ(b.Size(), 0);
	}
}

TEST(BufferBaseTest, Replace)
{
	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 6, 7, 8 };
		test_data_t expected[] = { 6, 7, 3, 4, 5 };
		size_t expectedSize = b1.Size();

		b1.Replace(b2, 0, 2);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 6, 7, 8 };

		EXPECT_THROW(b1.Replace(b2, 0, b2.Size() + 1), InvalidArgumentException);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 6, 7, 8 };
		test_data_t expected[] = { 1, 2, 3, 6, 7 };
		size_t expectedSize = b1.Size();

		b1.Replace(b2, 3);

		EXPECT_EQ(b1.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2 = { 6, 7, 8 };

		EXPECT_THROW(b1.Replace(b2, b1.Size()), InvalidArgumentException);
	}

	{
		TestBuffer b1 = { 1, 2, 3, 4, 5 };
		TestBuffer b2;
		test_data_t expected[] = { 1, 2, 3, 4, 5 };

		b1.Replace(b2, 0);

		for (size_t i = 0; i < 5; ++i)
			EXPECT_EQ(b1[i], expected[i]);
	}

	{
		TestBuffer b1;
		TestBuffer b2 = { 1, 2, 3, 4, 5 };

		b1.Replace(b2, 0);

		EXPECT_EQ(b1.Size(), 0);
	}

	{
		TestBuffer b1;
		TestBuffer b2;

		b1.Replace(b2, 0);

		EXPECT_EQ(b1.Size(), 0);
	}
}

TEST(BufferBaseTest, Resize)
{
	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3, 4, 5, 0, 0, 0 };
		size_t expectedSize = 8;

		b.Resize(expectedSize);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3 };
		size_t expectedSize = 3;

		b.Resize(expectedSize);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3, 4, 5 };
		size_t expectedSize = b.Size();

		b.Resize(expectedSize);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		
		b.Resize(0);

		EXPECT_EQ(b.Size(), 0);
		EXPECT_TRUE(b.begin() == nullptr);
	}
}

TEST(BufferBaseTest, Reverse)
{
	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 5, 4, 3, 2, 1 };
		size_t expectedSize = b.Size();

		b.Reverse();

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b;
		b.Reverse();
		EXPECT_EQ(b.Size(), 0);
		EXPECT_TRUE(b.begin() == nullptr);
	}
}

TEST(BufferBaseTest, begin)
{
	{
		TestBuffer b;
		EXPECT_TRUE(b.begin() == nullptr);
	}
	
	{
		TestBuffer b(1);
		EXPECT_FALSE(b.begin() == nullptr);
	}
}

TEST(BufferBaseTest, end)
{
	{
		TestBuffer b;
		EXPECT_TRUE(b.end() == nullptr);
	}

	{
		TestBuffer b(1);
		test_data_t* expected = b.begin() + b.Size();

		EXPECT_EQ(b.end(), expected);
	}
}