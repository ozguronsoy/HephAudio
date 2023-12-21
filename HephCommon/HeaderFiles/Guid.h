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
		HEPH_CONSTEXPR Guid() : data1(0), data2(0), data3(0), data4{ 0, 0, 0, 0, 0, 0, 0, 0 } {}
		HEPH_CONSTEXPR bool operator==(const Guid& rhs)
		{
			if (this->data1 == rhs.data1 && this->data2 == rhs.data2 && this->data3 == rhs.data3)
			{
				for (size_t i = 0; i < 8; i++)
				{
					if (this->data4[i] != rhs.data4[i])
					{
						return false;
					}
				}
				return true;
			}
			return false;
		}
		HEPH_CONSTEXPR bool operator!=(const Guid& rhs)
		{
			return !(this->operator==(rhs));
		}
		static Guid GenerateNew();
	};
}