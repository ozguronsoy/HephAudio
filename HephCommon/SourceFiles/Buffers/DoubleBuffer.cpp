#include "Buffers/DoubleBuffer.h"

namespace HephCommon
{
	DoubleBuffer::DoubleBuffer() : SignedArithmeticBuffer<double, DoubleBuffer>() {}
	DoubleBuffer::DoubleBuffer(size_t size) : SignedArithmeticBuffer<double, DoubleBuffer>(size) {}
	DoubleBuffer::DoubleBuffer(size_t size, BufferFlags flags) : SignedArithmeticBuffer<double, DoubleBuffer>(size, flags) {}
	DoubleBuffer::DoubleBuffer(const std::initializer_list<double>& rhs) : SignedArithmeticBuffer<double, DoubleBuffer>(rhs) {}
	DoubleBuffer::DoubleBuffer(const DoubleBuffer& rhs) : SignedArithmeticBuffer<double, DoubleBuffer>(rhs) {}
	DoubleBuffer::DoubleBuffer(DoubleBuffer&& rhs) noexcept : SignedArithmeticBuffer<double, DoubleBuffer>(std::move(rhs)) {}

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

HephCommon::DoubleBuffer operator+(double lhs, const HephCommon::DoubleBuffer& rhs)
{
	return rhs + lhs;
}

HephCommon::DoubleBuffer operator-(double lhs, const HephCommon::DoubleBuffer& rhs)
{
	HephCommon::DoubleBuffer result(rhs.Size(), HephCommon::BufferFlags::AllocUninitialized);
	for (size_t i = 0; i < result.Size(); ++i)
	{
		result[i] = lhs - rhs[i];
	}
	return result;
}

HephCommon::DoubleBuffer operator*(double lhs, const HephCommon::DoubleBuffer& rhs)
{
	return rhs * lhs;
}

HephCommon::DoubleBuffer operator/(double lhs, const HephCommon::DoubleBuffer& rhs)
{
	HephCommon::DoubleBuffer result(rhs.Size(), HephCommon::BufferFlags::AllocUninitialized);
	for (size_t i = 0; i < result.Size(); ++i)
	{
		result[i] = lhs / rhs[i];
	}
	return result;
}