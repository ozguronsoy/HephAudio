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
	class ArithmeticBuffer : public BufferBase<Tdata>
	{
		static_assert(std::is_arithmetic<Tdata>::value, "Tdata must be an arithmetic type");

	public:
		static constexpr Tdata MIN_ELEMENT = std::numeric_limits<Tdata>::lowest();
		static constexpr Tdata MAX_ELEMENT = std::numeric_limits<Tdata>::max();

	protected:
		size_t size;

	protected:
		ArithmeticBuffer() : BufferBase<Tdata>(), size(0) {}

		explicit ArithmeticBuffer(size_t size) : BufferBase<Tdata>(), size(size)
		{
			if (this->size > 0)
			{
				this->pData = BufferBase<Tdata>::Allocate(this->SizeAsByte());
			}
		}

		ArithmeticBuffer(const std::initializer_list<Tdata>& rhs) : BufferBase<Tdata>(), size(rhs.size())
		{
			if (this->size > 0)
			{
				const size_t size_byte = this->SizeAsByte();
				this->pData = BufferBase<Tdata>::AllocateUninitialized(size_byte);
				(void)std::memcpy(this->pData, rhs.begin(), size_byte);
			}
		}

		ArithmeticBuffer(const ArithmeticBuffer& rhs) : BufferBase<Tdata>(), size(rhs.size)
		{
			if (!rhs.IsEmpty())
			{
				const size_t size_byte = rhs.SizeAsByte();
				this->pData = BufferBase<Tdata>::AllocateUninitialized(size_byte);
				(void)std::memcpy(this->pData, rhs.pData, size_byte);
			}
		}

		ArithmeticBuffer(ArithmeticBuffer&& rhs) noexcept : BufferBase<Tdata>(std::move(rhs)), size(rhs.size)
		{
			rhs.size = 0;
		}

	public:
		virtual ~ArithmeticBuffer() = default;

		Tself operator+(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] + rhs;
			}

			return result;
		}

		Tself operator+(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator+", "Both operands must have the same size"));
			}

			size_t i;
			Tself result{};

			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] + rhs[i];
			}

			return result;
		}

		ArithmeticBuffer& operator+=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] += rhs;
			}
			return *this;
		}

		ArithmeticBuffer& operator+=(const Tself& rhs)
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator+=", "Both operands must have the same size"));
			}

			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] += rhs[i];
			}

			return *this;
		}

		Tself operator-(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] - rhs;
			}

			return result;
		}

		Tself operator-(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator-", "Both operands must have the same size"));
			}

			size_t i;
			Tself result{};

			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] - rhs[i];
			}

			return result;
		}

		ArithmeticBuffer& operator-=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] -= rhs;
			}
			return *this;
		}

		ArithmeticBuffer& operator-=(const Tself& rhs)
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator-=", "Both operands must have the same size"));
			}

			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] -= rhs[i];
			}

			return *this;
		}

		Tself operator*(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] * rhs;
			}

			return result;
		}

		Tself operator*(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator*", "Both operands must have the same size"));
			}

			size_t i;
			Tself result{};

			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] * rhs[i];
			}

			return result;
		}

		ArithmeticBuffer& operator*=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] *= rhs;
			}
			return *this;
		}

		ArithmeticBuffer& operator*=(const Tself& rhs)
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator*=", "Both operands must have the same size"));
			}

			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] *= rhs[i];
			}

			return *this;
		}

		Tself operator/(Tdata rhs) const
		{
			Tself result{};
			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] / rhs;
			}

			return result;
		}

		Tself operator/(const Tself& rhs) const
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator/", "Both operands must have the same size"));
			}

			size_t i;
			Tself result{};

			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (i = 0; i < result.size; ++i)
			{
				result[i] = (*this)[i] / rhs[i];
			}

			return result;
		}

		ArithmeticBuffer& operator/=(Tdata rhs)
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] /= rhs;
			}
			return *this;
		}

		ArithmeticBuffer& operator/=(const Tself& rhs)
		{
			if (this->size != rhs.size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ArithmeticBuffer::operator/=", "Both operands must have the same size"));
			}

			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] /= rhs[i];
			}

			return *this;
		}

		Tself operator<<(size_t rhs) const
		{
			const size_t thisSize_byte = this->SizeAsByte();
			const size_t rhsSize_byte = ArithmeticBuffer::SizeAsByte(rhs);

			Tself result{};
			result.pData = BufferBase<Tdata>::AllocateUninitialized(thisSize_byte);
			result.size = this->size;

			(void)std::memcpy(result.pData, this->pData + rhs, thisSize_byte - rhsSize_byte);
			if (rhs > 0)
			{
				(void)std::memset(result.pData + this->size - rhs, 0, rhsSize_byte);
			}

			return result;
		}

		ArithmeticBuffer& operator<<=(size_t rhs)
		{
			if (rhs > 0)
			{
				const size_t thisSize_byte = this->SizeAsByte();
				const size_t rhsSize_byte = ArithmeticBuffer::SizeAsByte(rhs);

				(void)std::memcpy(this->pData, this->pData + rhs, thisSize_byte - rhsSize_byte);
				(void)std::memset(this->pData + this->size - rhs, 0, rhsSize_byte);
			}
			return *this;
		}

		Tself operator>>(size_t rhs) const
		{
			const size_t thisSize_byte = this->SizeAsByte();
			const size_t rhsSize_byte = ArithmeticBuffer::SizeAsByte(rhs);

			Tself result{};
			result.pData = BufferBase<Tdata>::AllocateUninitialized(thisSize_byte);
			result.size = this->size;

			(void)std::memcpy(result.pData + rhs, this->pData, thisSize_byte - rhsSize_byte);
			if (rhs > 0)
			{
				(void)std::memset(result.pData, 0, rhsSize_byte);
			}

			return result;
		}

		ArithmeticBuffer& operator>>=(size_t rhs)
		{
			if (rhs > 0)
			{
				const size_t thisSize_byte = this->SizeAsByte();
				const size_t rhsSize_byte = ArithmeticBuffer::SizeAsByte(rhs);

				(void)std::memcpy(this->pData + rhs, this->pData, thisSize_byte - rhsSize_byte);
				(void)std::memset(this->pData, 0, rhsSize_byte);
			}
			return *this;
		}

		bool operator==(const Tself& rhs) const
		{
			return (this->IsEmpty() && rhs.IsEmpty()) || (this->size == rhs.size && std::memcmp(this->pData, rhs.pData, this->SizeAsByte()) == 0);
		}

		bool operator!=(const Tself& rhs) const
		{
			return !((*this) == rhs);
		}

		Tdata& operator[](size_t index) const
		{
			return this->pData[index];
		}

		size_t Size() const override
		{
			return this->size;
		}

		size_t SizeAsByte() const override
		{
			return this->size * sizeof(Tdata);
		}

		bool IsEmpty() const override
		{
			return this->pData == nullptr || this->size == 0;
		}

		void Release() override
		{
			BufferBase<Tdata>::Release();
			this->size = 0;
		}

		Tself SubBuffer(size_t index, size_t size) const
		{
			Tself result{};

			result.pData = BufferBase<Tdata>::SubBuffer(this->pData, this->SizeAsByte(), ArithmeticBuffer::SizeAsByte(index), ArithmeticBuffer::SizeAsByte(size));
			result.size = size;

			return result;
		}

		void Append(const Tself& rhs)
		{
			this->pData = BufferBase<Tdata>::Append(this->pData, this->SizeAsByte(), rhs.pData, rhs.SizeAsByte());
			this->size += rhs.size;
		}

		void Insert(const Tself& rhs, size_t index)
		{
			this->pData = BufferBase<Tdata>::Insert(this->pData, this->SizeAsByte(), rhs.pData, rhs.SizeAsByte(), ArithmeticBuffer::SizeAsByte(index));
			this->size += rhs.size;
		}

		void Cut(size_t index, size_t size)
		{
			size_t cutSize_byte = ArithmeticBuffer::SizeAsByte(size);
			this->pData = BufferBase<Tdata>::Cut(this->pData, this->SizeAsByte(), ArithmeticBuffer::SizeAsByte(index), cutSize_byte);
			this->size -= cutSize_byte / sizeof(Tdata);
		}

		void Replace(const Tself& rhs, size_t index)
		{
			this->Replace(rhs, index, rhs.size);
		}

		void Replace(const Tself& rhs, size_t index, size_t size)
		{
			BufferBase<Tdata>::Replace(this->pData, this->SizeAsByte(), rhs.pData, rhs.SizeAsByte(), ArithmeticBuffer::SizeAsByte(index));
		}

		void Resize(size_t newSize)
		{
			if (this->size != newSize)
			{
				if (newSize == 0)
				{
					this->Release();
				}
				else
				{
					Tdata* pTemp = (Tdata*)std::realloc(this->pData, ArithmeticBuffer::SizeAsByte(newSize));
					if (pTemp == nullptr)
					{
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ArithmeticBuffer::Resize", "Insufficient memory"));
					}
					if (newSize > this->size)
					{
						(void)std::memset(pTemp + this->size, 0, ArithmeticBuffer::SizeAsByte(newSize - this->size));
					}

					this->pData = pTemp;
					this->size = newSize;
				}
			}
		}

		void Reverse()
		{
			const size_t halfSize = this->size / 2;
			for (size_t i = 0; i < halfSize; ++i)
			{
				std::swap(this->operator[](i), this->operator[](this->size - i - 1));
			}
		}

		Tdata Min() const
		{
			Tdata result = ArithmeticBuffer::MAX_ELEMENT;
			for (size_t i = 0; i < this->size; ++i)
			{
				if ((*this)[i] < result)
				{
					result = (*this)[i];
				}
			}
			return result;
		}

		Tdata Max() const
		{
			Tdata result = ArithmeticBuffer::MIN_ELEMENT;
			for (size_t i = 0; i < this->size; ++i)
			{
				if ((*this)[i] > result)
				{
					result = (*this)[i];
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
					sumSquared += (*this)[i] * (*this)[i];
				}
				return std::sqrt(sumSquared / this->size);
			}
			return 0;
		}

	protected:
		static size_t SizeAsByte(size_t size)
		{
			return size * sizeof(Tdata);
		}
	};

	template <typename Tdata, class Tself>
	class SignedArithmeticBuffer : public ArithmeticBuffer<Tdata, Tself>
	{
		static_assert(std::is_signed<Tdata>::value, "Tdata must be a signed type");
	public:
		SignedArithmeticBuffer() : ArithmeticBuffer<Tdata, Tself>() {}
		SignedArithmeticBuffer(size_t size) : ArithmeticBuffer<Tdata, Tself>(size) {}
		SignedArithmeticBuffer(const std::initializer_list<double>& rhs) : ArithmeticBuffer<Tdata, Tself>(rhs) {}
		SignedArithmeticBuffer(const SignedArithmeticBuffer& rhs) : ArithmeticBuffer<Tdata, Tself>(rhs) {}
		SignedArithmeticBuffer(SignedArithmeticBuffer&& rhs) noexcept : ArithmeticBuffer<Tdata, Tself>(std::move(rhs)) {}

		Tself operator-() const
		{
			Tself result{};
			result.pData = BufferBase<Tdata>::AllocateUninitialized(this->SizeAsByte());
			result.size = this->size;

			for (size_t i = 0; i < this->size; ++i)
			{
				result[i] = -(*this)[i];
			}
			
			return result;
		}

		void Invert()
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				(*this)[i] = -(*this)[i];
			}
		}

		Tdata AbsMax() const
		{
			Tdata result = 0;
			for (size_t i = 0; i < this->size; ++i)
			{
				const Tdata abs_current = std::abs((*this)[i]);
				if (abs_current > result)
				{
					result = abs_current;
				}
			}
			return result;
		}
	};
}