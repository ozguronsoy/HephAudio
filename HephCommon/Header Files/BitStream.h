#pragma once
#include "HephCommonFramework.h"
#include "StreamStatus.h"

namespace HephCommon
{
	class BitStream final
	{
	private:
		size_t offset;
		size_t bufferSize_bit;
		uint8_t* pBuffer;
	public:
		BitStream(void* pData, size_t dataSize_bit);
		BitStream(const BitStream& rhs);
		BitStream& operator=(const BitStream& rhs);
		size_t GetOffset() const noexcept;
		StreamStatus SetOffset(size_t offset);
		StreamStatus IncreaseOffset(size_t x);
		StreamStatus DecreaseOffset(size_t x);
		StreamStatus Read(void* pOutData);
		StreamStatus Read(void* pOutData, size_t nBits);
		StreamStatus Write(uint8_t bit);
		StreamStatus Write(void* pBits, size_t nBits);
	};
}