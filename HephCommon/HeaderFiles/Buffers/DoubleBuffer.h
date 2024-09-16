#pragma once
#include "HephCommonShared.h"
#include "ArithmeticBuffer.h"

/** @file */

namespace HephCommon
{
	class DoubleBuffer final : public SignedArithmeticBuffer<DoubleBuffer, double>
	{
	public:
		DoubleBuffer();
		explicit DoubleBuffer(size_t size);
		DoubleBuffer(size_t size, BufferFlags flags);
		DoubleBuffer(const std::initializer_list<double>& rhs);
		DoubleBuffer(const DoubleBuffer& rhs);
		DoubleBuffer(DoubleBuffer&& rhs) noexcept;
		DoubleBuffer& operator=(const std::initializer_list<double>& rhs);
		DoubleBuffer& operator=(const DoubleBuffer& rhs);
		DoubleBuffer& operator=(DoubleBuffer&& rhs) noexcept;
	};
}