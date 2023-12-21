#include "Guid.h"
#include <random>


namespace HephCommon
{
	Guid Guid::GenerateNew()
	{
		Guid guid;
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

		guid.data1 = dist(mt);
		guid.data2 = dist(mt) % (UINT16_MAX + 1);
		guid.data3 = dist(mt) % (UINT16_MAX + 1);
		for (size_t i = 0; i < 8; i++)
		{
			guid.data4[i] = dist(mt) % (UINT8_MAX + 1);
		}

		return guid;
	}
}