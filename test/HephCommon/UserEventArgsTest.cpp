#include "gtest/gtest.h"
#include "UserEventArgs.h"

using namespace Heph;

TEST(UserEventArgsTest, TestAll)
{
	UserEventArgs args;

	int data1 = 30;
	std::string data2 = "Hello World!";
	double data3 = 27.35;

	std::string key1 = "key1";
	std::string key2 = "key2";
	std::string key3 = "key3";

	args.Add(key1, (void*)&data1);
	args.Add(key2, (void*)&data2);

	EXPECT_EQ(args.Size(), 2);
	EXPECT_TRUE(args.Exists(key1));
	EXPECT_TRUE(args.Exists(key2));
	EXPECT_FALSE(args.Exists(key3));

	EXPECT_EQ(args[key1], (void*)&data1);
	EXPECT_EQ(args[key2], (void*)&data2);
	EXPECT_TRUE(args[key3] == nullptr);

	args.Add(key3, (void*)&data3);

	EXPECT_EQ(args.Size(), 3);

	args.Remove(key2);

	EXPECT_EQ(args.Size(), 2);
	EXPECT_TRUE(args.Exists(key1));
	EXPECT_FALSE(args.Exists(key2));
	EXPECT_TRUE(args.Exists(key3));

	args.Clear();
	
	EXPECT_EQ(args.Size(), 0);
}