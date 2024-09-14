#pragma once
#include "HephCommonShared.h"
#include "BufferBase.h"
#include "HephMath.h"
#include "BufferOperators.h"
#include <initializer_list>
#include <limits>

namespace HephCommon
{
	template <class Tself, typename Tdata>
	class ArithmeticBuffer : 
		public BufferBase<Tself, Tdata>,
		public BufferArithmeticOperators<Tself, Tdata>,
		public BufferArithmeticOperators<Tself, Tdata, Tself, Tdata>
	{
		static_assert(std::is_arithmetic<Tdata>::value, "Tdata must be an arithmetic type");

	protected:
		static constexpr Tdata MIN_ELEMENT = std::numeric_limits<Tdata>::lowest();
		static constexpr Tdata MAX_ELEMENT = std::numeric_limits<Tdata>::max();

	protected:
		ArithmeticBuffer() : BufferBase<Tself, Tdata>() { ArithmeticBuffer::AddEventHandlers(); }
		explicit ArithmeticBuffer(size_t size) : BufferBase<Tself, Tdata>(size) { ArithmeticBuffer::AddEventHandlers(); }
		ArithmeticBuffer(size_t size, BufferFlags flags) : BufferBase<Tself, Tdata>(size, flags) { ArithmeticBuffer::AddEventHandlers(); }
		ArithmeticBuffer(const std::initializer_list<Tdata>& rhs) : BufferBase<Tself, Tdata>(rhs) { ArithmeticBuffer::AddEventHandlers(); }
		ArithmeticBuffer(const ArithmeticBuffer& rhs) : BufferBase<Tself, Tdata>(rhs) { ArithmeticBuffer::AddEventHandlers(); }
		ArithmeticBuffer(ArithmeticBuffer&& rhs) noexcept : BufferBase<Tself, Tdata>(std::move(rhs)) { ArithmeticBuffer::AddEventHandlers(); }

	public:
		virtual ~ArithmeticBuffer() = default;

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

	private:
		static inline bool ADD_EVENT_HANDLERS = false;
		static void AddEventHandlers()
		{
			if (!ArithmeticBuffer::ADD_EVENT_HANDLERS)
			{
				BufferOperatorEvents<Tself, Tdata>::OnResultCreated = ArithmeticBuffer::ResultCreatedEventHandler<Tdata>;
				BufferOperatorEvents<Tself, Tself>::OnResultCreated = ArithmeticBuffer::ResultCreatedEventHandler<Tself>;
				ArithmeticBuffer::ADD_EVENT_HANDLERS = true;
			}
		}

		template<typename Rhs>
		static void ResultCreatedEventHandler(const EventParams& params)
		{
			BufferOperatorResultCreatedEventArgs<Tself, Rhs>* pArgs = (BufferOperatorResultCreatedEventArgs<Tself, Rhs>*)params.pArgs;

			pArgs->result.pData = ArithmeticBuffer::AllocateUninitialized(pArgs->lhs.SizeAsByte());
			pArgs->result.size = pArgs->lhs.size;
		}
	};

	template <class Tself, typename Tdata>
	class SignedArithmeticBuffer :
		public ArithmeticBuffer<Tself, Tdata>,
		public BufferUnaryMinusOperator<Tself, Tdata>
	{
		static_assert(std::is_signed<Tdata>::value, "Tdata must be a signed type");

	protected:
		SignedArithmeticBuffer() : ArithmeticBuffer<Tself, Tdata>() {}
		explicit SignedArithmeticBuffer(size_t size) : ArithmeticBuffer<Tself, Tdata>(size) {}
		SignedArithmeticBuffer(size_t size, BufferFlags flags) : ArithmeticBuffer<Tself, Tdata>(size, flags) {}
		SignedArithmeticBuffer(const std::initializer_list<Tdata>& rhs) : ArithmeticBuffer<Tself, Tdata>(rhs) {}
		SignedArithmeticBuffer(const SignedArithmeticBuffer& rhs) : ArithmeticBuffer<Tself, Tdata>(rhs) {}
		SignedArithmeticBuffer(SignedArithmeticBuffer&& rhs) noexcept : ArithmeticBuffer<Tself, Tdata>(std::move(rhs)) {}

	public:
		virtual ~SignedArithmeticBuffer() = default;

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