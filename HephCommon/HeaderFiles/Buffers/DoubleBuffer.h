#pragma once
#include "HephCommonShared.h"
#include "ArithmeticBuffer.h"

/** @file */

namespace HephCommon
{
	/**
	 * @brief buffer for storing doubles.
	 * 
	 */
	class HEPH_API DoubleBuffer final : public SignedArithmeticBuffer<DoubleBuffer, double>
	{
	public:
		/** @copydoc BufferBase() */
		DoubleBuffer();

		/** @copydoc BufferBase(size_t) */
		explicit DoubleBuffer(size_t size);
		
		/** @copydoc BufferBase(size_t,BufferFlags) */
		DoubleBuffer(size_t size, BufferFlags flags);
		
		/** @copydoc BufferBase(const std::initializer_list<double>& rhs) */
		DoubleBuffer(const std::initializer_list<double>& rhs);
		
		/** @copydoc BufferBase(const BufferBase& rhs) */
		DoubleBuffer(const DoubleBuffer& rhs);
		
		/** @copydoc BufferBase(BufferBase&& rhs) */
		DoubleBuffer(DoubleBuffer&& rhs) noexcept;
		
		DoubleBuffer& operator=(const std::initializer_list<double>& rhs);
		DoubleBuffer& operator=(const DoubleBuffer& rhs);
		DoubleBuffer& operator=(DoubleBuffer&& rhs) noexcept;
	};

	// explicit instantiate for DLLs.
	template class HEPH_API BufferBase<DoubleBuffer, double>;
	template class HEPH_API ArithmeticBuffer<DoubleBuffer, double>;
	template class HEPH_API SignedArithmeticBuffer<DoubleBuffer, double>;

	template class HEPH_API BufferAdditionOperator<DoubleBuffer, double>;
	template class HEPH_API BufferSubtractionOperator<DoubleBuffer, double>;
	template class HEPH_API BufferDivisionOperator<DoubleBuffer, double>;
	template class HEPH_API BufferMultiplicationOperator<DoubleBuffer, double>;
	template class HEPH_API BufferArithmeticOperators<DoubleBuffer, double>;

	template class HEPH_API BufferAdditionOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferSubtractionOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferDivisionOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferMultiplicationOperator<DoubleBuffer, double, DoubleBuffer, double>;
	template class HEPH_API BufferArithmeticOperators<DoubleBuffer, double, DoubleBuffer, double>;
	
	template class HEPH_API BufferUnaryMinusOperator<DoubleBuffer, double>;
}