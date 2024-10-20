#include "Buffers/ComplexBuffer.h"
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
		return BufferBase::operator=(rhs);
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
		return BufferBase::operator=(rhs);
	}

	ComplexBuffer& ComplexBuffer::operator=(ComplexBuffer&& rhs) noexcept
	{
		return BufferBase::operator=(std::move(rhs));
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

	// explicit instantiate for building shared libraries.
	template class HEPH_API BufferBase<ComplexBuffer, Complex>;

	template class HEPH_API BufferAdditionOperator<ComplexBuffer, Complex>;
	template class HEPH_API BufferSubtractionOperator<ComplexBuffer, Complex>;
	template class HEPH_API BufferDivisionOperator<ComplexBuffer, Complex>;
	template class HEPH_API BufferMultiplicationOperator<ComplexBuffer, Complex>;
	template class HEPH_API BufferArithmeticOperators<ComplexBuffer, Complex>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<ComplexBuffer, Complex>;
	template struct HEPH_API BufferOperatorEvents<ComplexBuffer, Complex>;

	template class HEPH_API BufferAdditionOperator<ComplexBuffer, Complex, double>;
	template class HEPH_API BufferSubtractionOperator<ComplexBuffer, Complex, double>;
	template class HEPH_API BufferDivisionOperator<ComplexBuffer, Complex, double>;
	template class HEPH_API BufferMultiplicationOperator<ComplexBuffer, Complex, double>;
	template class HEPH_API BufferArithmeticOperators<ComplexBuffer, Complex, double>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<ComplexBuffer, double>;
	template struct HEPH_API BufferOperatorEvents<ComplexBuffer, double>;

	template class HEPH_API BufferAdditionOperator<ComplexBuffer, Complex, ComplexBuffer, Complex>;
	template class HEPH_API BufferSubtractionOperator<ComplexBuffer, Complex, ComplexBuffer, Complex>;
	template class HEPH_API BufferDivisionOperator<ComplexBuffer, Complex, ComplexBuffer, Complex>;
	template class HEPH_API BufferMultiplicationOperator<ComplexBuffer, Complex, ComplexBuffer, Complex>;
	template class HEPH_API BufferArithmeticOperators<ComplexBuffer, Complex, ComplexBuffer, Complex>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<ComplexBuffer, ComplexBuffer>;
	template struct HEPH_API BufferOperatorEvents<ComplexBuffer, ComplexBuffer>;

	template class HEPH_API BufferAdditionOperator<ComplexBuffer, Complex, DoubleBuffer, double>;
	template class HEPH_API BufferSubtractionOperator<ComplexBuffer, Complex, DoubleBuffer, double>;
	template class HEPH_API BufferDivisionOperator<ComplexBuffer, Complex, DoubleBuffer, double>;
	template class HEPH_API BufferMultiplicationOperator<ComplexBuffer, Complex, DoubleBuffer, double>;
	template class HEPH_API BufferArithmeticOperators<ComplexBuffer, Complex, DoubleBuffer, double>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<ComplexBuffer, DoubleBuffer>;
	template struct HEPH_API BufferOperatorEvents<ComplexBuffer, DoubleBuffer>;

	template class HEPH_API BufferUnaryMinusOperator<ComplexBuffer, Complex>;
}