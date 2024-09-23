#pragma once
#include "HephShared.h"
#include "Complex.h"
#include "Buffers/DoubleBuffer.h"
#include <initializer_list>

/** @file */

namespace Heph
{
	/**
	 * @brief buffer for storing \link Heph::Complex complex numbers.
	 * 
	 */
	class HEPH_API ComplexBuffer final :
		public BufferBase<ComplexBuffer, Complex>,
		public BufferArithmeticOperators<ComplexBuffer, Complex>,
		public BufferArithmeticOperators<ComplexBuffer, Complex, double>,
		public BufferArithmeticOperators<ComplexBuffer, Complex, ComplexBuffer, Complex>,
		public BufferArithmeticOperators<ComplexBuffer, Complex, DoubleBuffer, double>,
		public BufferUnaryMinusOperator<ComplexBuffer, Complex>
	{
	public:
		/** @copydoc BufferBase() */
		ComplexBuffer();

		/** @copydoc BufferBase(size_t) */
		explicit ComplexBuffer(size_t size);

		/** @copydoc BufferBase(size_t,BufferFlags) */
		ComplexBuffer(size_t size, BufferFlags flags);
		
		/** @copydoc BufferBase(const std::initializer_list<Complex>& rhs) */
		ComplexBuffer(const std::initializer_list<double>& rhs);

		/** @copydoc BufferBase(const std::initializer_list<Complex>& rhs) */
		ComplexBuffer(const std::initializer_list<Complex>& rhs);

		/** @copydoc BufferBase(const BufferBase& rhs) */
		explicit ComplexBuffer(const DoubleBuffer& rhs);
		
		/** @copydoc BufferBase(const BufferBase& rhs) */
		ComplexBuffer(const ComplexBuffer& rhs);

		/** @copydoc BufferBase(BufferBase&& rhs) */
		ComplexBuffer(ComplexBuffer&& rhs) noexcept;

		ComplexBuffer& operator=(const std::initializer_list<double>& rhs);
		ComplexBuffer& operator=(const std::initializer_list<Complex>& rhs);
		ComplexBuffer& operator=(const DoubleBuffer& rhs);
		ComplexBuffer& operator=(const ComplexBuffer& rhs);
		ComplexBuffer& operator=(ComplexBuffer&& rhs) noexcept;

		/** @copydoc SignedArithmeticBuffer::Invert */
		void Invert();

	private:
		static void AddEventHandlers();
		static void ResultCreatedEventHandler(const Heph::EventParams& params);
		static void ResultCreatedEventHandlerDouble(const Heph::EventParams& params);
		static void ResultCreatedEventHandlerComplexBuffer(const Heph::EventParams& params);
		static void ResultCreatedEventHandlerDoubleBuffer(const Heph::EventParams& params);
	};
};