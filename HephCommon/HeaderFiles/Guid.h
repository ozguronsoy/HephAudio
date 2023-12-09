#pragma once
#include "HephCommonFramework.h"
#include <cinttypes>

namespace HephCommon
{
	struct Guid
	{
		uint32_t data1;
		uint16_t data2;
		uint16_t data3;
		uint8_t data4[8];
		constexpr Guid(): data1(0), data2(0), data3(0), data4({0, 0, 0, 0, 0, 0, 0, 0}) {}
	};
}