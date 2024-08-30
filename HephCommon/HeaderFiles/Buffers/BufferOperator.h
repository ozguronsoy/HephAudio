#pragma once
#include "HephCommonShared.h"
#include "BufferBase.h"
#include "HephTypeTraits.h"

namespace HephCommon
{
#pragma region Base
	template<typename Lhs, typename LhsData>
	class BufferOperatorBase
	{
	public:
		BufferOperatorBase()
		{
			static_assert(std::is_base_of<BufferOperatorBase, Lhs>::value, "Lhs must derive from BufferOperatorBase");
			static_assert(std::is_base_of<BufferBase<Lhs, LhsData>, Lhs>::value, "Lhs must derive from BufferBase");
		}

	protected:
		inline size_t& Access_size()
		{
			return ((BufferBase<Lhs, LhsData>*)this)->size;
		}

		inline size_t Access_Size() const
		{
			return ((BufferBase<Lhs, LhsData>*)this)->size;
		}

		inline size_t Access_SizeAsByte() const
		{
			return ((BufferBase<Lhs, LhsData>*)this)->SizeAsByte();
		}

		inline LhsData*& Access_pData()
		{
			return ((BufferBase<Lhs, LhsData>*)this)->pData;
		}

		inline LhsData* Access_begin() const
		{
			return ((BufferBase<Lhs, LhsData>*)this)->begin();
		}

		inline LhsData* Access_end() const
		{
			return ((BufferBase<Lhs, LhsData>*)this)->end();
		}

		inline LhsData* Access_Allocate(size_t size_byte) const
		{
			return ((BufferBase<Lhs, LhsData>*)this)->Allocate(size_byte);
		}

		inline LhsData* Access_AllocateUninitialized(size_t size_byte) const
		{
			return ((BufferBase<Lhs, LhsData>*)this)->AllocateUninitialized(size_byte);
		}
	};

	template<typename Lhs, typename LhsData, typename Rhs, typename RhsData>
	class BufferOperator : public BufferOperatorBase<Lhs, LhsData>
	{
	public:
		BufferOperator()
		{
			static_assert(
				(std::is_base_of<BufferBase<Rhs, RhsData>, Rhs>::value && std::is_base_of<BufferOperatorBase<Rhs, RhsData>, Rhs>::value) 
				|| std::is_same<Rhs, RhsData>::value,
				"Rhs must derive from BufferBase and BufferOperatorBase. Or RhsData must be left as default"
				);
		}
	};
#pragma endregion
#pragma region Addition
	template<class Lhs, typename LhsData, typename Rhs = LhsData, typename RhsData = Rhs>
	class BufferAdditionOperator : public BufferOperator<Lhs, LhsData, Rhs, RhsData>
	{
	public:
		BufferAdditionOperator() : BufferOperator<Lhs, LhsData, Rhs, RhsData>()
		{
			static_assert(has_addition_operator<LhsData, RhsData>::value && has_addition_assignment_operator<LhsData, RhsData>::value, "LhsData must have operator+(RhsData) and operator+=(RhsData)");
		}

	public:
		// friend static so no need to do using::operator in derived class 
		// when inheriting BufferXOperator with multiple overloads
		friend static Lhs operator+(const Lhs& lhs, const Rhs& rhs)
		{
			return Impl<Rhs, RhsData>(lhs, rhs);
		}

		friend static Lhs& operator+=(Lhs& lhs, const Rhs& rhs)
		{
			return ImplAssign<Rhs, RhsData>(lhs, rhs);
		}

	private:
		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			Lhs result{};
			reinterpret_cast<BufferAdditionOperator*>(&result)->Access_pData() = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_AllocateUninitialized(reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_SizeAsByte());
			reinterpret_cast<BufferAdditionOperator*>(&result)->Access_size() = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_Size();

			LhsData* pLhsData = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_begin();
			LhsData* pLhsDataEnd = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_end();
			LhsData* pResultData = reinterpret_cast<BufferAdditionOperator*>(&result)->Access_begin();
			for (; pLhsData < pLhsDataEnd; ++pLhsData, ++pResultData)
			{
				(*pResultData) = (*pLhsData) + rhs;
			}

			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			LhsData* pLhsData = reinterpret_cast<BufferAdditionOperator*>(&lhs)->Access_begin();
			LhsData* pLhsDataEnd = reinterpret_cast<BufferAdditionOperator*>(&lhs)->Access_end();
			for (; pLhsData < pLhsDataEnd; ++pLhsData)
			{
				(*pLhsData) += rhs;
			}
			return lhs;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			if (reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_Size() != ((BufferAdditionOperator*)(void*)&rhs)->Access_size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferAdditionOperator::operator+", "Both operands must have the same size"));
			}

			Lhs result{};
			reinterpret_cast<BufferAdditionOperator*>(&result)->Access_pData() = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_AllocateUninitialized(reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_SizeAsByte());
			reinterpret_cast<BufferAdditionOperator*>(&result)->Access_size() = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_Size();

			LhsData* pLhsData = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_begin();
			LhsData* pLhsDataEnd = reinterpret_cast<const BufferAdditionOperator*>(&lhs)->Access_end();
			V* pRhsData = (V*)((BufferAdditionOperator*)(void*)&rhs)->Access_begin();
			LhsData* pResultData = reinterpret_cast<BufferAdditionOperator*>(&result)->Access_begin();
			for (; pLhsData < pLhsDataEnd; ++pLhsData, ++pRhsData, ++pResultData)
			{
				(*pResultData) = (*pLhsData) + (*pRhsData);
			}

			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t lhsSize = reinterpret_cast<BufferAdditionOperator*>(&lhs)->Access_size();
			const size_t rhsSize = ((BufferAdditionOperator*)(void*)&rhs)->Access_size();
			const size_t minSize = HEPH_MATH_MIN(lhsSize, rhsSize);

			LhsData* pLhsData = reinterpret_cast<BufferAdditionOperator*>(&lhs)->Access_begin();
			V* pRhsData = (V*)((BufferAdditionOperator*)(void*)&rhs)->Access_begin();
			for (size_t i = 0; i < minSize; ++i, ++pLhsData, ++pRhsData)
			{
				(*pLhsData) += (*pRhsData);
			}
			return lhs;
		}
	};
#pragma endregion
}