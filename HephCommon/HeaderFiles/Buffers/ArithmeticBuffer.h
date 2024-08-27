#pragma once
#include "HephCommonShared.h"
#include "BufferBase.h"
#include "HephMath.h"
#include <initializer_list>
#include <type_traits>
#include <cstdlib>

namespace HephCommon
{
	template <typename Tdata, class Tself>
	class ArithmeticBuffer : public BufferBase<Tdata, Tself>
	{
		static_assert(std::is_arithmetic<Tdata>::value, "Tdata must be an arithmetic type");

	public:
		static constexpr Tdata MIN_ELEMENT = std::numeric_limits<Tdata>::lowest();
		static constexpr Tdata MAX_ELEMENT = std::numeric_limits<Tdata>::max();

	protected:
		ArithmeticBuffer() : BufferBase<Tdata, Tself>() {}
		explicit ArithmeticBuffer(size_t size) : BufferBase<Tdata, Tself>(size) {}
		ArithmeticBuffer(size_t size, BufferFlags flags) : BufferBase<Tdata, Tself>(size, flags) {}
		ArithmeticBuffer(const std::initializer_list<Tdata>& rhs) : BufferBase<Tdata, Tself>(rhs) {}
		ArithmeticBuffer(const ArithmeticBuffer& rhs) : BufferBase<Tdata, Tself>(rhs) {}
		ArithmeticBuffer(ArithmeticBuffer&& rhs) noexcept : BufferBase<Tdata, Tself>(std::move(rhs)) {}

	public:
		virtual ~ArithmeticBuffer() = default;

		virtual Tself operator+(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] + rhs;
			}

			return result;
		}

		virtual Tself operator+(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator+", "Both operands must have the same size"));
			}

			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] + rhs.pData[i];
			}

			return result;
		}

		virtual ArithmeticBuffer& operator+=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				this->pData[i] += rhs;
			}
			return *this;
		}

		virtual ArithmeticBuffer& operator+=(const Tself& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(this->size, rhs.size);
			for (size_t i = 0; i < minSize; ++i)
			{
				this->pData[i] += rhs.pData[i];
			}
			return *this;
		}

		virtual Tself operator-(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] - rhs;
			}

			return result;
		}

		virtual Tself operator-(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator-", "Both operands must have the same size"));
			}

			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] - rhs.pData[i];
			}

			return result;
		}

		virtual ArithmeticBuffer& operator-=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				this->pData[i] -= rhs;
			}
			return *this;
		}

		virtual ArithmeticBuffer& operator-=(const Tself& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(this->size, rhs.size);
			for (size_t i = 0; i < minSize; ++i)
			{
				this->pData[i] -= rhs.pData[i];
			}
			return *this;
		}

		virtual Tself operator*(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] * rhs;
			}

			return result;
		}

		virtual Tself operator*(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator*", "Both operands must have the same size"));
			}

			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] * rhs.pData[i];
			}

			return result;
		}

		virtual ArithmeticBuffer& operator*=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				this->pData[i] *= rhs;
			}
			return *this;
		}

		virtual ArithmeticBuffer& operator*=(const Tself& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(this->size, rhs.size);
			for (size_t i = 0; i < minSize; ++i)
			{
				this->pData[i] *= rhs.pData[i];
			}
			return *this;
		}

		virtual Tself operator/(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] / rhs;
			}

			return result;
		}

		virtual Tself operator/(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator/", "Both operands must have the same size"));
			}

			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result.pData[i] = this->pData[i] / rhs.pData[i];
			}

			return result;
		}

		virtual ArithmeticBuffer& operator/=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				this->pData[i] /= rhs;
			}
			return *this;
		}

		virtual ArithmeticBuffer& operator/=(const Tself& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(this->size, rhs.size);
			for (size_t i = 0; i < minSize; ++i)
			{
				this->pData[i] /= rhs.pData[i];
			}
			return *this;
		}

		Tdata Min() const
		{
			Tdata result = ArithmeticBuffer::MAX_ELEMENT;
			for (size_t i = 0; i < this->size; ++i)
			{
				if (this->pData[i] < result)
				{
					result = this->pData[i];
				}
			}
			return result;
		}

		Tdata Max() const
		{
			Tdata result = ArithmeticBuffer::MIN_ELEMENT;
			for (size_t i = 0; i < this->size; ++i)
			{
				if (this->pData[i] > result)
				{
					result = this->pData[i];
				}
			}
			return result;
		}

		double Rms() const
		{
			if (this->size > 0)
			{
				double sumSquared = 0;
				for (size_t i = 0; i < this->size; ++i)
				{
					sumSquared += this->pData[i] * this->pData[i];
				}
				return std::sqrt(sumSquared / this->size);
			}
			return 0;
		}
	};

	template <typename Tdata, class Tself>
	class SignedArithmeticBuffer : public ArithmeticBuffer<Tdata, Tself>
	{
		static_assert(std::is_signed<Tdata>::value, "Tdata must be a signed type");
	public:
		using ArithmeticBuffer<Tdata, Tself>::operator-;

	protected:
		SignedArithmeticBuffer() : ArithmeticBuffer<Tdata, Tself>() {}
		explicit SignedArithmeticBuffer(size_t size) : ArithmeticBuffer<Tdata, Tself>(size) {}
		SignedArithmeticBuffer(size_t size, BufferFlags flags) : ArithmeticBuffer<Tdata, Tself>(size, flags) {}
		SignedArithmeticBuffer(const std::initializer_list<double>& rhs) : ArithmeticBuffer<Tdata, Tself>(rhs) {}
		SignedArithmeticBuffer(const SignedArithmeticBuffer& rhs) : ArithmeticBuffer<Tdata, Tself>(rhs) {}
		SignedArithmeticBuffer(SignedArithmeticBuffer&& rhs) noexcept : ArithmeticBuffer<Tdata, Tself>(std::move(rhs)) {}
		
	public:
		virtual ~SignedArithmeticBuffer() = default;

		virtual Tself operator-() const
		{
			Tself result{};
			result.pData = BufferBase<Tdata, Tself>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < this->size; ++i)
			{
				result.pData[i] = -this->pData[i];
			}

			return result;
		}

		void Invert()
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				this->pData[i] = -this->pData[i];
			}
		}

		Tdata AbsMax() const
		{
			Tdata result = 0;
			for (size_t i = 0; i < this->size; ++i)
			{
				const Tdata abs_current = std::abs(this->pData[i]);
				if (abs_current > result)
				{
					result = abs_current;
				}
			}
			return result;
		}
	};
}