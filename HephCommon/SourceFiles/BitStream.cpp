#include "BitStream.h"
#include "HephException.h"

#define BYTE_OFFSET(x) (x / 8)
#define BIT_OFFSET(x) (x % 8)

namespace HephCommon
{
	BitStream::BitStream(void* pData, size_t dataSize_bit) : offset(0), bufferSize_bit(dataSize_bit), pBuffer((uint8_t*)pData) {}
	BitStream::BitStream(const BitStream& rhs) : offset(rhs.offset), bufferSize_bit(rhs.bufferSize_bit), pBuffer(rhs.pBuffer) {}
	BitStream& BitStream::operator=(const BitStream& rhs)
	{
		this->offset = rhs.offset;
		this->bufferSize_bit = rhs.bufferSize_bit;
		this->pBuffer = rhs.pBuffer;
		return *this;
	}
	size_t BitStream::GetOffset() const
	{
		return this->offset;
	}
	StreamStatus BitStream::SetOffset(size_t offset)
	{
		if (offset >= this->bufferSize_bit)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::SetOffset", "New offset exceeds the buffer size."));
			return StreamStatus::Fail;
		}

		this->offset = offset;
		return StreamStatus::Success;
	}
	StreamStatus BitStream::IncreaseOffset(size_t x)
	{
		if ((this->offset + x) >= this->bufferSize_bit)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::IncreaseOffset", "New offset exceeds the buffer size."));
			return StreamStatus::Fail;
		}

		this->offset += x;
		return StreamStatus::Success;
	}
	StreamStatus BitStream::DecreaseOffset(size_t x)
	{
		if (x > this->offset)
		{
			this->offset = 0;
			return StreamStatus::Success;
		}


		this->offset -= x;
		return StreamStatus::Success;
	}
	StreamStatus BitStream::Read(void* pOutData)
	{
		if (pOutData == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::Read", "pOutData cannot be null."));
			return StreamStatus::Fail;
		}

		if (this->offset == this->bufferSize_bit)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::Read", "Offset exceeds the buffer size."));
			return StreamStatus::Fail;
		}

		(*(uint8_t*)pOutData) = (this->pBuffer[BYTE_OFFSET(this->offset)] >> BIT_OFFSET(this->offset)) & 1;
		this->offset++;
		return (this->offset == this->bufferSize_bit) ? StreamStatus::EndOfStream : StreamStatus::Success;
	}
	StreamStatus BitStream::Read(void* pOutData, size_t nBits)
	{
		if (pOutData == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::Read", "pOutData cannot be null."));
			return StreamStatus::Fail;
		}

		if (this->offset == this->bufferSize_bit)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::Read", "Offset exceeds the buffer size."));
			return StreamStatus::Fail;
		}

		if ((this->offset + nBits) >= this->bufferSize_bit)
		{
			nBits = this->bufferSize_bit - this->offset;
		}

		for (size_t i = 0; i < nBits; i++, this->offset++)
		{
			const size_t bitOffset_i = BIT_OFFSET(i);

			if (bitOffset_i == 0) // if new byte, clear it.
			{
				((uint8_t*)pOutData)[BYTE_OFFSET(i)] = (this->pBuffer[BYTE_OFFSET(this->offset)] >> BIT_OFFSET(this->offset)) & 1;
			}
			else
			{
				((uint8_t*)pOutData)[BYTE_OFFSET(i)] |= ((this->pBuffer[BYTE_OFFSET(this->offset)] >> BIT_OFFSET(this->offset)) & 1) << bitOffset_i;
			}
		}

		return (this->offset == this->bufferSize_bit) ? StreamStatus::EndOfStream : StreamStatus::Success;
	}
	StreamStatus BitStream::Write(uint8_t bit)
	{
		if (this->offset == this->bufferSize_bit)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::Write", "Offset exceeds the buffer size."));
			return StreamStatus::Fail;
		}

		this->pBuffer[BYTE_OFFSET(this->offset)] =  bit << BIT_OFFSET(this->offset);
		this->offset++;
		return (this->offset == this->bufferSize_bit) ? StreamStatus::EndOfStream : StreamStatus::Success;
	}
	StreamStatus BitStream::Write(void* pBits, size_t nBits)
	{
		if (pBits == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::Write", "pBits cannot be null."));
			return StreamStatus::Fail;
		}

		if (this->offset == this->bufferSize_bit)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BitStream::Write", "Offset exceeds the buffer size."));
			return StreamStatus::Fail;
		}

		if ((this->offset + nBits) >= this->bufferSize_bit)
		{
			nBits = this->bufferSize_bit - this->offset;
		}

		for (size_t i = 0; i < nBits; i++, this->offset++)
		{
			const size_t bitOffset_o = BIT_OFFSET(this->offset);

			if (bitOffset_o == 0) // if new byte, clear it.
			{
				this->pBuffer[BYTE_OFFSET(this->offset)] = (((uint8_t*)pBits)[BYTE_OFFSET(i)] >> BIT_OFFSET(i)) & 1;
			}
			else
			{
				this->pBuffer[BYTE_OFFSET(this->offset)] |= ((((uint8_t*)pBits)[BYTE_OFFSET(i)] >> BIT_OFFSET(i)) & 1) << bitOffset_o;
			}
		}
	
		return (this->offset == this->bufferSize_bit) ? StreamStatus::EndOfStream : StreamStatus::Success;
	}
}