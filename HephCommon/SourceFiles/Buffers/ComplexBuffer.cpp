#include "Buffers/ComplexBuffer.h"
#include "HephException.h"
#include "HephMath.h"
#include <memory>

namespace HephCommon
{
	ComplexBuffer::ComplexBuffer() : BufferBase<ComplexBuffer, Complex>() {}
	ComplexBuffer::ComplexBuffer(size_t size) : BufferBase<ComplexBuffer, Complex>(size) {}
	ComplexBuffer::ComplexBuffer(size_t size, BufferFlags flags) : BufferBase<ComplexBuffer, Complex>(size, flags) {}
	ComplexBuffer::ComplexBuffer(const std::initializer_list<double>& rhs) : BufferBase<ComplexBuffer, Complex>(rhs.size())
	{
		if (this->size > 0)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i].real(rhs.begin()[i]);
				(*this)[i].imag(0);
			}
		}
	}
	ComplexBuffer::ComplexBuffer(const std::initializer_list<Complex>& rhs) : BufferBase<ComplexBuffer, Complex>(rhs) {}
	ComplexBuffer::ComplexBuffer(const DoubleBuffer& rhs) : BufferBase<ComplexBuffer, Complex>()
	{
		//if (this->frameCount > 0)
		//{
		//	this->pData = (Complex*)malloc(this->Size());
		//	if (this->pData == nullptr)
		//	{
		//		RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
		//	}
		//	for (size_t i = 0; i < this->frameCount; i++)
		//	{
		//		(*this)[i].real(rhs[i]);
		//		(*this)[i].imag(0);
		//	}
		//}
	}
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs) : BufferBase<ComplexBuffer, Complex>(rhs) {}
	ComplexBuffer::ComplexBuffer(ComplexBuffer&& rhs) noexcept : BufferBase<ComplexBuffer, Complex>(std::move(rhs)) {}
	ComplexBuffer& ComplexBuffer::operator=(const std::initializer_list<double>& rhs)
	{
		this->Release();

		this->size = rhs.size();
		if (this->size > 0)
		{
			this->pData = ComplexBuffer::AllocateUninitialized(this->SizeAsByte());
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i].real(rhs.begin()[i]);
				(*this)[i].imag(0);
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const std::initializer_list<Complex>& rhs)
	{
		this->Release();

		this->size = rhs.size();
		if (this->size > 0)
		{
			const size_t size_byte = this->SizeAsByte();
			this->pData = ComplexBuffer::AllocateUninitialized(size_byte);
			memcpy(this->pData, rhs.begin(), size_byte);
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const DoubleBuffer& rhs)
	{
		this->Release();

		this->size = rhs.Size();
		if (this->size > 0)
		{
			const size_t size_byte = this->SizeAsByte();
			this->pData = ComplexBuffer::AllocateUninitialized(size_byte);
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i].real(rhs[i]);
				(*this)[i].imag(0);
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const ComplexBuffer& rhs)
	{
		if (this != &rhs)
		{
			this->Release();

			if (rhs.size > 0)
			{
				const size_t size_byte = rhs.SizeAsByte();
				this->pData = ComplexBuffer::AllocateUninitialized(size_byte);
				(void)std::memcpy(this->pData, rhs.pData, size_byte);
				this->size = rhs.size;
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(ComplexBuffer&& rhs) noexcept
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
	void ComplexBuffer::Invert()
	{
		for (size_t i = 0; i < this->size; ++i)
		{
			this->pData[i] = -this->pData[i];
		}
	}
}