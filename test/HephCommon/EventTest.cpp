#include "gtest/gtest.h"
#include "Event.h"
#include "Complex.h"
#include <string>

using namespace Heph;

class EventTest : public testing::Test
{
protected:
	static constexpr const char* THIS_KEY = "this";
	static constexpr const char* PARAM1_KEY = "param1";
	static constexpr const char* PARAM2_KEY = "param2";
	static constexpr const char* PARAM3_KEY = "param3";

protected:
	Event event;

	std::string param1;
	int param2;
	Complex param3;

	bool handler1;
	bool handler2;
	bool handler3;
	bool handler4;

	EventArgs* pArgs;
	EventResult* pResult;

protected:
	EventTest()
	{
		handler1 = false;
		handler2 = false;
		handler3 = false;
		handler4 = false;

		param1 = "Hello World!";
		param2 = 37;
		param3 = Complex(10, 20);

		pArgs = nullptr;
		pResult = nullptr;
	}

	static void Handler1(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userEventArgs[THIS_KEY];

		EXPECT_TRUE(pThis != nullptr);
		EXPECT_TRUE(params.userEventArgs.Exists(THIS_KEY));
		EXPECT_EQ(params.userEventArgs[PARAM1_KEY], &pThis->param1);
		EXPECT_EQ(params.userEventArgs[PARAM2_KEY], &pThis->param2);
		EXPECT_EQ(params.userEventArgs[PARAM3_KEY], &pThis->param3);
		EXPECT_EQ(params.userEventArgs["DOES NOT EXIST"], nullptr);
		EXPECT_FALSE(params.userEventArgs.Exists("DOES NOT EXIST"));

		pThis->handler1 = true;
	}

	static void Handler2(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userEventArgs[THIS_KEY];
		pThis->handler2 = true;
		params.pResult->isHandled = true;
	}

	static void Handler3(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userEventArgs[THIS_KEY];
		pThis->handler3 = true;
	}

	static void Handler4(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userEventArgs[THIS_KEY];

		EXPECT_EQ(params.pArgs, pThis->pArgs);
		EXPECT_EQ(params.pResult, pThis->pResult);

		pThis->handler4 = true;
	}
};

TEST_F(EventTest, HandlerMethods)
{
	EXPECT_EQ(event.EventHandlerCount(), 0);
	EXPECT_FALSE(event);

	event = &Handler1;
	EXPECT_EQ(event.EventHandlerCount(), 1);
	EXPECT_TRUE(event);
	EXPECT_TRUE(event.EventHandlerExists(&Handler1));
	EXPECT_FALSE(event.EventHandlerExists(&Handler2));

	event = &Handler2;
	EXPECT_EQ(event.EventHandlerCount(), 1);
	EXPECT_TRUE(event.EventHandlerExists(&Handler2));
	EXPECT_FALSE(event.EventHandlerExists(&Handler1));

	event.SetEventHandler(&Handler3);
	EXPECT_EQ(event.EventHandlerCount(), 1);
	EXPECT_TRUE(event.EventHandlerExists(&Handler3));
	EXPECT_FALSE(event.EventHandlerExists(&Handler1));
	EXPECT_FALSE(event.EventHandlerExists(&Handler2));

	event += &Handler1;
	event.AddEventHandler(&Handler2);
	EXPECT_EQ(event.EventHandlerCount(), 3);
	EXPECT_TRUE(event.EventHandlerExists(&Handler1));
	EXPECT_TRUE(event.EventHandlerExists(&Handler2));
	EXPECT_TRUE(event.EventHandlerExists(&Handler3));
	EXPECT_EQ(event.GetEventHandler(0), &Handler3);
	EXPECT_EQ(event.GetEventHandler(1), &Handler1);
	EXPECT_EQ(event.GetEventHandler(2), &Handler2);

	event.InsertEventHandler(&Handler4, 2);
	EXPECT_EQ(event.GetEventHandler(0), &Handler3);
	EXPECT_EQ(event.GetEventHandler(1), &Handler1);
	EXPECT_EQ(event.GetEventHandler(2), &Handler4);
	EXPECT_EQ(event.GetEventHandler(3), &Handler2);

	event.ClearEventHandlers();
	EXPECT_EQ(event.EventHandlerCount(), 0);
	EXPECT_FALSE(event);

	event = &Handler1;
	event += &Handler2;
	event.ClearAll();
	EXPECT_EQ(event.EventHandlerCount(), 0);
	EXPECT_EQ(event.userEventArgs.Size(), 0);
	EXPECT_FALSE(event);
}

TEST_F(EventTest, Invoke)
{
	event = &Handler1;
	event += &Handler2;
	event += &Handler3;
	event += &Handler4;

	EXPECT_EQ(event.userEventArgs.Size(), 0);
	event.userEventArgs.Add(THIS_KEY, this);
	event.userEventArgs.Add(PARAM1_KEY, &param1);
	event.userEventArgs.Add(PARAM2_KEY, &param2);
	event.userEventArgs.Add(PARAM3_KEY, &param3);
	EXPECT_EQ(event.userEventArgs.Size(), 4);

	event.userEventArgs.Remove(PARAM3_KEY);
	EXPECT_EQ(event.userEventArgs.Size(), 3);
	EXPECT_FALSE(event.userEventArgs.Exists(PARAM3_KEY));
	event.userEventArgs.Add(PARAM3_KEY, &param3);

	event.Invoke(nullptr, nullptr);

	EXPECT_TRUE(handler1);
	EXPECT_TRUE(handler2);
	EXPECT_FALSE(handler3);
	EXPECT_FALSE(handler4);

	handler1 = false;
	handler2 = false;
	handler3 = false;
	handler4 = false;

	event.RemoveEventHandler(1);
	event.RemoveEventHandler(&Handler3);

	EventArgs args;
	EventResult result;

	pArgs = &args;
	pResult = &result;

	event(&args, &result);

	EXPECT_TRUE(handler1);
	EXPECT_FALSE(handler2);
	EXPECT_FALSE(handler3);
	EXPECT_TRUE(handler4);
}