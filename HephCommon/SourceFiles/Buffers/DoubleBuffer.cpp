#include "Buffers/DoubleBuffer.h"

namespace Heph
{
	DoubleBuffer::DoubleBuffer() : SignedArithmeticBuffer<DoubleBuffer, double>() {}
	DoubleBuffer::DoubleBuffer(size_t size) : SignedArithmeticBuffer<DoubleBuffer, double>(size) {}
	DoubleBuffer::DoubleBuffer(size_t size, BufferFlags flags) : SignedArithmeticBuffer<DoubleBuffer, double>(size, flags) {}
	DoubleBuffer::DoubleBuffer(const std::initializer_list<double>& rhs) : SignedArithmeticBuffer<DoubleBuffer, double>(rhs) {}
	DoubleBuffer::DoubleBuffer(const DoubleBuffer& rhs) : SignedArithmeticBuffer<DoubleBuffer, double>(rhs) {}
	DoubleBuffer::DoubleBuffer(DoubleBuffer&& rhs) noexcept : SignedArithmeticBuffer<DoubleBuffer, double>(std::move(rhs)) {}

	DoubleBuffer& DoubleBuffer::operator=(const std::initializer_list<double>& rhs)
	{
		this->Release();

		this->size = rhs.size();
		if (this->size > 0)
		{
			const size_t size_byte = this->SizeAsByte();
			this->pData = DoubleBuffer::AllocateUninitialized(size_byte);
			(void)std::memcpy(this->pData, rhs.begin(), size_byte);
		}

		return *this;
	}

	DoubleBuffer& DoubleBuffer::operator=(const DoubleBuffer& rhs)
	{
		if (this != &rhs)
		{
			this->Release();

			if (rhs.size > 0)
			{
				const size_t size_byte = rhs.SizeAsByte();
				this->pData = DoubleBuffer::AllocateUninitialized(size_byte);
				(void)std::memcpy(this->pData, rhs.pData, size_byte);

				this->size = rhs.size;
			}
		}

		return *this;
	}

	DoubleBuffer& DoubleBuffer::operator=(DoubleBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release();

			this->pData = rhs.pData;
			this->size = rhs.size;

			rhs.pData = nullptr;
			rhs.size = 0;
		}

		return *this;
	}
}