#pragma once
#include "HephCommonShared.h"
#include "Complex.h"
#include "Buffers/DoubleBuffer.h"
#include <initializer_list>

namespace HephCommon
{
	class ComplexBuffer final : 
		public BufferBase<ComplexBuffer, Complex>,
		public BufferArithmeticOperators<ComplexBuffer, Complex>,
		public BufferArithmeticOperators<ComplexBuffer, Complex, double>,
		public BufferArithmeticOperators<ComplexBuffer, Complex, ComplexBuffer, Complex>,
		public BufferArithmeticOperators<ComplexBuffer, Complex, DoubleBuffer, double>,
		public BufferUnaryMinusOperator<ComplexBuffer, Complex>
	{
	public:
		ComplexBuffer();
		explicit ComplexBuffer(size_t size);
		ComplexBuffer(size_t size, BufferFlags flags);
		ComplexBuffer(const std::initializer_list<double>& rhs);
		ComplexBuffer(const std::initializer_list<Complex>& rhs);
		explicit ComplexBuffer(const DoubleBuffer& rhs);
		ComplexBuffer(const ComplexBuffer& rhs);
		ComplexBuffer(ComplexBuffer&& rhs) noexcept;
		ComplexBuffer& operator=(const std::initializer_list<double>& rhs);
		ComplexBuffer& operator=(const std::initializer_list<Complex>& rhs);
		ComplexBuffer& operator=(const DoubleBuffer& rhs);
		ComplexBuffer& operator=(const ComplexBuffer& rhs);
		ComplexBuffer& operator=(ComplexBuffer&& rhs) noexcept;
		void Invert();
	private:
		static void AddEventHandlers();
		static void ResultCreatedEventHandler(const HephCommon::EventParams& params);
		static void ResultCreatedEventHandlerDouble(const HephCommon::EventParams& params);
		static void ResultCreatedEventHandlerComplexBuffer(const HephCommon::EventParams& params);
		static void ResultCreatedEventHandlerDoubleBuffer(const HephCommon::EventParams& params);
	};
};