#pragma once
#include "HephShared.h"
#include "ArithmeticBuffer.h"

/** @file */

namespace Heph
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
}