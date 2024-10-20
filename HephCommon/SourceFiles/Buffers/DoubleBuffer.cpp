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
		return BufferBase::operator=(rhs);
	}

	DoubleBuffer& DoubleBuffer::operator=(const DoubleBuffer& rhs)
	{
		return BufferBase::operator=(rhs);
	}

	DoubleBuffer& DoubleBuffer::operator=(DoubleBuffer&& rhs) noexcept
	{
		return BufferBase::operator=(std::move(rhs));
	}

	// explicit instantiate for building shared libraries.
	template class HEPH_API BufferBase<DoubleBuffer, double>;
	template class HEPH_API ArithmeticBuffer<DoubleBuffer, double>;
	template class HEPH_API SignedArithmeticBuffer<DoubleBuffer, double>;

	template class HEPH_API BufferAdditionOperator<DoubleBuffer, double>;
	template class HEPH_API BufferSubtractionOperator<DoubleBuffer, double>;
	template class HEPH_API BufferDivisionOperator<DoubleBuffer, double>;
	template class HEPH_API BufferMultiplicationOperator<DoubleBuffer, double>;
	template class HEPH_API BufferArithmeticOperators<DoubleBuffer, double>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<DoubleBuffer, double>;
	template struct HEPH_API BufferOperatorEvents<DoubleBuffer, double>;

	template class HEPH_API BufferAdditionOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferSubtractionOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferDivisionOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferMultiplicationOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferArithmeticOperators<DoubleBuffer, double, DoubleBuffer, double>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<DoubleBuffer, DoubleBuffer>;
	template struct HEPH_API BufferOperatorEvents<DoubleBuffer, DoubleBuffer>;

	template class HEPH_API BufferUnaryMinusOperator<DoubleBuffer, double>;
}