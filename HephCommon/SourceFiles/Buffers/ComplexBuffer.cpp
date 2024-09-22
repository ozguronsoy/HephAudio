#include "Buffers/ComplexBuffer.h"
#include "Exception.h"
#include "HephMath.h"
#include <memory>

namespace Heph
{
	ComplexBuffer::ComplexBuffer() : BufferBase<ComplexBuffer, Complex>() { ComplexBuffer::AddEventHandlers(); }
	ComplexBuffer::ComplexBuffer(size_t size) : BufferBase<ComplexBuffer, Complex>(size) { ComplexBuffer::AddEventHandlers(); }
	ComplexBuffer::ComplexBuffer(size_t size, BufferFlags flags) : BufferBase<ComplexBuffer, Complex>(size, flags) { ComplexBuffer::AddEventHandlers(); }
	
	ComplexBuffer::ComplexBuffer(const std::initializer_list<double>& rhs) : BufferBase<ComplexBuffer, Complex>(rhs.size(), BufferFlags::AllocUninitialized)
	{
		ComplexBuffer::AddEventHandlers();

		for (size_t i = 0; i < this->size; ++i)
		{
			(*this)[i].real = rhs.begin()[i];
			(*this)[i].imag = 0;
		}
	}
	
	ComplexBuffer::ComplexBuffer(const std::initializer_list<Complex>& rhs) : BufferBase<ComplexBuffer, Complex>(rhs) { ComplexBuffer::AddEventHandlers(); }
	
	ComplexBuffer::ComplexBuffer(const DoubleBuffer& rhs) : BufferBase<ComplexBuffer, Complex>(rhs.Size(), BufferFlags::AllocUninitialized)
	{
		ComplexBuffer::AddEventHandlers();

		for (size_t i = 0; i < this->size; ++i)
		{
			(*this)[i].real = rhs[i];
			(*this)[i].imag = 0;
		}
	}
	
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs) : BufferBase<ComplexBuffer, Complex>(rhs) { ComplexBuffer::AddEventHandlers(); }
	
	ComplexBuffer::ComplexBuffer(ComplexBuffer&& rhs) noexcept : BufferBase<ComplexBuffer, Complex>(std::move(rhs)) { ComplexBuffer::AddEventHandlers(); }
	
	ComplexBuffer& ComplexBuffer::operator=(const std::initializer_list<double>& rhs)
	{
		this->Release();

		this->size = rhs.size();
		if (this->size > 0)
		{
			this->pData = ComplexBuffer::AllocateUninitialized(this->SizeAsByte());
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i].real = rhs.begin()[i];
				(*this)[i].imag = 0;
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
				(*this)[i].real = rhs[i];
				(*this)[i].imag = 0;
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

	void ComplexBuffer::AddEventHandlers()
	{
		if (!BufferOperatorEvents<ComplexBuffer, Complex>::OnResultCreated.EventHandlerExists(ComplexBuffer::ResultCreatedEventHandler))
		{
			BufferOperatorEvents<ComplexBuffer, Complex>::OnResultCreated += ComplexBuffer::ResultCreatedEventHandler;
			BufferOperatorEvents<ComplexBuffer, DoubleBuffer>::OnResultCreated += ComplexBuffer::ResultCreatedEventHandlerDouble;
			BufferOperatorEvents<ComplexBuffer, ComplexBuffer>::OnResultCreated += ComplexBuffer::ResultCreatedEventHandlerComplexBuffer;
			BufferOperatorEvents<ComplexBuffer, DoubleBuffer>::OnResultCreated += ComplexBuffer::ResultCreatedEventHandlerDoubleBuffer;
		}
	}

	void ComplexBuffer::ResultCreatedEventHandler(const EventParams& params)
	{
		BufferOperatorResultCreatedEventArgs<ComplexBuffer, Complex>* pArgs = (BufferOperatorResultCreatedEventArgs<ComplexBuffer, Complex>*)params.pArgs;

		pArgs->result.pData = ComplexBuffer::AllocateUninitialized(pArgs->lhs.SizeAsByte());
		pArgs->result.size = pArgs->lhs.size;
	}

	void ComplexBuffer::ResultCreatedEventHandlerDouble(const EventParams& params)
	{
		BufferOperatorResultCreatedEventArgs<ComplexBuffer, double>* pArgs = (BufferOperatorResultCreatedEventArgs<ComplexBuffer, double>*)params.pArgs;

		pArgs->result.pData = ComplexBuffer::AllocateUninitialized(pArgs->lhs.SizeAsByte());
		pArgs->result.size = pArgs->lhs.size;
	}

	void ComplexBuffer::ResultCreatedEventHandlerComplexBuffer(const EventParams& params)
	{
		BufferOperatorResultCreatedEventArgs<ComplexBuffer, ComplexBuffer>* pArgs = (BufferOperatorResultCreatedEventArgs<ComplexBuffer, ComplexBuffer>*)params.pArgs;

		pArgs->result.pData = ComplexBuffer::AllocateUninitialized(pArgs->lhs.SizeAsByte());
		pArgs->result.size = pArgs->lhs.size;
	}

	void ComplexBuffer::ResultCreatedEventHandlerDoubleBuffer(const EventParams& params)
	{
		BufferOperatorResultCreatedEventArgs<ComplexBuffer, DoubleBuffer>* pArgs = (BufferOperatorResultCreatedEventArgs<ComplexBuffer, DoubleBuffer>*)params.pArgs;

		pArgs->result.pData = ComplexBuffer::AllocateUninitialized(pArgs->lhs.SizeAsByte());
		pArgs->result.size = pArgs->lhs.size;
	}
}