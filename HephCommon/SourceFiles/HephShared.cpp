#include "HephShared.h"

namespace Heph
{
	static const char* const __version = HEPH_VERSION;

	const char* GetVersion()
	{
		return __version;
	}

	unsigned int GetVersionMajor()
	{
		return HEPH_VERSION_MAJOR;
	}

	unsigned int GetVersionMinor()
	{
		return HEPH_VERSION_MINOR;
	}

	unsigned int GetVersionPatch()
	{
		return HEPH_VERSION_PATCH;
	}

	Endian GetSystemEndian()
	{
		uint16_t n = 0x0001;
		return (*(uint8_t*)&n == 1) ? Endian::Little : Endian::Big;
	}

	void ChangeEndian(uint8_t* pData, uint8_t dataSize)
	{
		const uint8_t halfDataSize = dataSize / 2;
		for (size_t i = 0; i < halfDataSize; ++i)
		{
			const uint8_t temp = pData[i];
			pData[i] = pData[dataSize - i - 1];
			pData[dataSize - i - 1] = temp;
		}
	}

	Endian systemEndian = Heph::GetSystemEndian();
}