#include "Guid.h"

#if CPP_VERSION >= CPP_VERSION_11

#include <random>

#else

#include <stdlib.h>

#if RAND_MAX == INT16_MAX

#define HEPH_GEN_RAND_UINT16 ((rand()) + (rand()) + (rand() % 3))
#define HEPH_GEN_RAND_UINT32 ((HEPH_GEN_RAND_UINT16 << 16) | HEPH_GEN_RAND_UINT16)

#elif RAND_MAX == UINT16_MAX

#define HEPH_GEN_RAND_UINT16 rand()
#define HEPH_GEN_RAND_UINT32 ((HEPH_GEN_RAND_UINT16 << 16) | HEPH_GEN_RAND_UINT16)

#elif RAND_MAX == INT32_MAX

#define HEPH_GEN_RAND_UINT16 (rand() % (UINT16_MAX + 1))
#define HEPH_GEN_RAND_UINT32 ((rand()) + (rand()) + (rand() % 3))

#elif RAND_MAX == UINT32_MAX

#define HEPH_GEN_RAND_UINT16 (rand() % (UINT16_MAX + 1))
#define HEPH_GEN_RAND_UINT32 rand()

#else

#define HEPH_GEN_RAND_UINT16 (rand() % (UINT16_MAX + 1))
#define HEPH_GEN_RAND_UINT32 (rand() % (UINT32_MAX + 1))

#endif

#endif

namespace HephCommon
{
	Guid Guid::GenerateNew()
	{
		Guid guid;

#if CPP_VERSION >= CPP_VERSION_11

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

#else
		
		guid.data1 = HEPH_GEN_RAND_UINT32;
		guid.data2 = HEPH_GEN_RAND_UINT16;
		guid.data3 = HEPH_GEN_RAND_UINT16;
		for (size_t i = 0; i < 8; i++)
		{
			guid.data4[i] = rand() % (UINT8_MAX + 1);
		}

#endif

		return guid;
	}
}