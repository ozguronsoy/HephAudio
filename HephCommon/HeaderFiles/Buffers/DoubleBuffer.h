#pragma once
#include "HephCommonShared.h"
#include "ArithmeticBuffer.h"

namespace HephCommon
{
	class DoubleBuffer final : public SignedArithmeticBuffer<double, DoubleBuffer>
	{
	public:
		DoubleBuffer();
		explicit DoubleBuffer(size_t size);
		DoubleBuffer(size_t size, BufferFlags flags);
		DoubleBuffer(const std::initializer_list<double>& rhs);
		DoubleBuffer(const DoubleBuffer& rhs);
		DoubleBuffer(DoubleBuffer&& rhs) noexcept;
		~DoubleBuffer() = default;
		DoubleBuffer& operator=(const std::initializer_list<double>& rhs);
		DoubleBuffer& operator=(const DoubleBuffer& rhs);
		DoubleBuffer& operator=(DoubleBuffer&& rhs) noexcept;
	};
}

HephCommon::DoubleBuffer operator+(double lhs, const HephCommon::DoubleBuffer& rhs);
HephCommon::DoubleBuffer operator-(double lhs, const HephCommon::DoubleBuffer& rhs);
HephCommon::DoubleBuffer operator*(double lhs, const HephCommon::DoubleBuffer& rhs);
HephCommon::DoubleBuffer operator/(double lhs, const HephCommon::DoubleBuffer& rhs);