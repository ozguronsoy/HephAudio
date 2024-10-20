#include "gtest/gtest.h"
#include "Exceptions/Exception.h"
#include "Exceptions/ExceptionEventArgs.h"
#include "Exceptions/InsufficientMemoryException.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/InvalidOperationException.h"
#include "Exceptions/NotFoundException.h"
#include "Exceptions/NotImplementedException.h"
#include "Exceptions/NotSupportedException.h"
#include "Exceptions/TimeoutException.h"
#include <vector>
#include <thread>

#define CHECK_EX_TYPE(pEx, type) { const type* _ex = dynamic_cast<const type*>(pEx); EXPECT_TRUE(_ex != nullptr); }

using namespace Heph;

class ExceptionTest : public testing::Test
{
protected:
	static constexpr const char* METHOD = "Method";
	static constexpr const char* MESSAGE = "Message";

protected:
	ExceptionTest()
	{
		Exception::GetExceptions().clear();
		Exception::OnException.ClearAll();
	}

	static void Handler1(const EventParams& params)
	{
		ExceptionEventArgs* pArgs = dynamic_cast<ExceptionEventArgs*>(params.pArgs);
		EXPECT_TRUE((&pArgs->exception) != nullptr);
		EXPECT_EQ(pArgs->exception.GetMethod(), ExceptionTest::METHOD);
		EXPECT_EQ(pArgs->exception.GetMessage(), ExceptionTest::MESSAGE);
	}

	static void Handler2(const EventParams& params)
	{
		ExceptionEventArgs* pArgs = (ExceptionEventArgs*)params.pArgs;
		CHECK_EX_TYPE(&pArgs->exception, InvalidArgumentException);
	}

	static void TestThread()
	{
		EXPECT_EQ(Exception::GetExceptions().size(), 0);
		HEPH_RAISE_EXCEPTION(nullptr, Exception());
		HEPH_RAISE_EXCEPTION(nullptr, InvalidArgumentException());
		HEPH_RAISE_EXCEPTION(nullptr, Exception());
		EXPECT_EQ(Exception::GetExceptions().size(), 3);
	}
};

TEST_F(ExceptionTest, Handler)
{
	Exception::OnException = &Handler1;
	HEPH_RAISE_EXCEPTION(nullptr, Exception(ExceptionTest::METHOD, ExceptionTest::MESSAGE));
}

TEST_F(ExceptionTest, GetName)
{
	std::vector<std::string> names;

	auto test = [&names](const std::string& name)
		{
			bool exists = false;
			
			for (const std::string& n : names)
			{
				if (n == name)
				{
					exists = true;
					EXPECT_NE(n, name) << "Duplicate exception name \"" << n << "\"";
				}
			}
			
			if (!exists)
				names.push_back(name);
		};

	test(Exception().GetName());
	test(InsufficientMemoryException().GetName());
	test(InvalidArgumentException().GetName());
	test(InvalidOperationException().GetName());
	test(NotFoundException().GetName());
	test(NotImplementedException().GetName());
	test(NotSupportedException().GetName());
	test(TimeoutException().GetName());
}

TEST_F(ExceptionTest, GetExceptions)
{
	// test thread_local
	EXPECT_EQ(Exception::GetExceptions().size(), 0);
	HEPH_RAISE_EXCEPTION(nullptr, Exception());
	EXPECT_EQ(Exception::GetExceptions().size(), 1);

	std::thread t(&TestThread);
	t.join();

	EXPECT_EQ(Exception::GetExceptions().size(), 1);
}

TEST_F(ExceptionTest, GetLastException)
{
	EXPECT_TRUE(Exception::GetLastException() == nullptr);

	HEPH_RAISE_EXCEPTION(nullptr, Exception());
	EXPECT_TRUE(Exception::GetLastException() != nullptr);

	Exception::OnException = &Handler2;
	HEPH_RAISE_EXCEPTION(nullptr, InvalidArgumentException());
	CHECK_EX_TYPE(Exception::GetLastException().get(), InvalidArgumentException);
}