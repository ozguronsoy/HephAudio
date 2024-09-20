#pragma once
#include "HephCommonShared.h"
#include "BufferBase.h"
#include "HephMath.h"
#include "BufferOperators.h"
#include <initializer_list>
#include <limits>

/** @file */

namespace HephCommon
{
	/**
	 * @brief base class for buffers that store arithmetic types. Provides operations and methods for arithmetic data.
	 * 
	 * @tparam Tself Type of the final buffer that inherits from this class (CRTP).
	 * @tparam Tdata Type of the data the buffer stores, must be arithmetic type.
	 */
	template <class Tself, typename Tdata>
	class HEPH_API ArithmeticBuffer :
		public BufferBase<Tself, Tdata>,
		public BufferArithmeticOperators<Tself, Tdata>,
		public BufferArithmeticOperators<Tself, Tdata, Tself, Tdata>
	{
		static_assert(std::is_arithmetic<Tdata>::value, "Tdata must be an arithmetic type");

	protected:
		/**
		 * element with the minimum value.
		 * 
		 */
		static constexpr Tdata MIN_ELEMENT = std::numeric_limits<Tdata>::lowest();

		/**
		 * element with the maximum value.
		 * 
		 */
		static constexpr Tdata MAX_ELEMENT = std::numeric_limits<Tdata>::max();

	protected:
		/** @copydoc BufferBase() */
		ArithmeticBuffer() : BufferBase<Tself, Tdata>() { ArithmeticBuffer::AddEventHandlers(); }
		
		/** @copydoc BufferBase(size_t) */
		explicit ArithmeticBuffer(size_t size) : BufferBase<Tself, Tdata>(size) { ArithmeticBuffer::AddEventHandlers(); }
		
		/** @copydoc BufferBase(size_t,BufferFlags) */
		ArithmeticBuffer(size_t size, BufferFlags flags) : BufferBase<Tself, Tdata>(size, flags) { ArithmeticBuffer::AddEventHandlers(); }
		
		/** @copydoc BufferBase(const std::initializer_list<Tdata>& rhs) */
		ArithmeticBuffer(const std::initializer_list<Tdata>& rhs) : BufferBase<Tself, Tdata>(rhs) { ArithmeticBuffer::AddEventHandlers(); }
		
		/** @copydoc BufferBase(const BufferBase& rhs) */
		ArithmeticBuffer(const ArithmeticBuffer& rhs) : BufferBase<Tself, Tdata>(rhs) { ArithmeticBuffer::AddEventHandlers(); }
		
		/** @copydoc BufferBase(BufferBase&& rhs) */
		ArithmeticBuffer(ArithmeticBuffer&& rhs) noexcept : BufferBase<Tself, Tdata>(std::move(rhs)) { ArithmeticBuffer::AddEventHandlers(); }

	public:
		/** @copydoc ~BufferBase */
		virtual ~ArithmeticBuffer() = default;

		/**
		 * gets the minimum value stored in the buffer.
		 * 
		 */
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

		/**
		 * gets the maximum value stored in the buffer.
		 * 
		 */
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

		/**
		 * calculates the root mean square.
		 * 
		 */
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

	/**
	 * @brief base class for buffers that store signed arithmetic types. Provides operations and methods for signed arithmetic data.
	 * 
	 * @tparam Tself Type of the final buffer that inherits from this class (CRTP).
	 * @tparam Tdata Type of the data the buffer stores, must be signed arithmetic type.
	 */
	template <class Tself, typename Tdata>
	class HEPH_API SignedArithmeticBuffer :
		public ArithmeticBuffer<Tself, Tdata>,
		public BufferUnaryMinusOperator<Tself, Tdata>
	{
		static_assert(std::is_signed<Tdata>::value, "Tdata must be a signed type");

	protected:
		/** @copydoc BufferBase() */
		SignedArithmeticBuffer() : ArithmeticBuffer<Tself, Tdata>() {}
		
		/** @copydoc BufferBase(size_t) */
		explicit SignedArithmeticBuffer(size_t size) : ArithmeticBuffer<Tself, Tdata>(size) {}
		
		/** @copydoc BufferBase(size_t,BufferFlags) */
		SignedArithmeticBuffer(size_t size, BufferFlags flags) : ArithmeticBuffer<Tself, Tdata>(size, flags) {}
		
		/** @copydoc BufferBase(const std::initializer_list<Tdata>& rhs) */
		SignedArithmeticBuffer(const std::initializer_list<Tdata>& rhs) : ArithmeticBuffer<Tself, Tdata>(rhs) {}
		
		/** @copydoc BufferBase(const BufferBase& rhs) */
		SignedArithmeticBuffer(const SignedArithmeticBuffer& rhs) : ArithmeticBuffer<Tself, Tdata>(rhs) {}
		
		/** @copydoc BufferBase(BufferBase&& rhs) */
		SignedArithmeticBuffer(SignedArithmeticBuffer&& rhs) noexcept : ArithmeticBuffer<Tself, Tdata>(std::move(rhs)) {}

	public:
		/** @copydoc ~BufferBase */
		virtual ~SignedArithmeticBuffer() = default;

		/**
		 * inverts the elements.
		 * 
		 */
		void Invert()
		{
			for (size_t i = 0; i < this->size; ++i)
			{
				this->pData[i] = -this->pData[i];
			}
		}

		/**
		 * gets the absolute maximum value stored in the buffer.
		 * 
		 */
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