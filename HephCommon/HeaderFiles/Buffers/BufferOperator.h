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
		inline size_t& Access_size() const
		{
			return ((Lhs*)this)->size;
		}

		inline size_t Access_SizeAsByte() const
		{
			return ((Lhs*)this)->SizeAsByte();
		}

		inline LhsData*& Access_pData() const
		{
			return ((Lhs*)this)->pData;
		}

		inline LhsData* Access_begin() const
		{
			return ((Lhs*)this)->begin();
		}

		inline LhsData* Access_end() const
		{
			return ((Lhs*)this)->end();
		}

		inline LhsData* Access_Allocate(size_t size_byte) const
		{
			return ((Lhs*)this)->Allocate(size_byte);
		}

		inline LhsData* Access_AllocateUninitialized(size_t size_byte) const
		{
			return ((Lhs*)this)->AllocateUninitialized(size_byte);
		}
	};

	template<typename Lhs, typename LhsData, typename Rhs, typename RhsData>
	class BufferOperator : public BufferOperatorBase<Lhs, LhsData>
	{
	public:
		BufferOperator()
		{
			static_assert((std::is_base_of<BufferBase<Rhs, RhsData>, Rhs>::value && std::is_base_of<BufferOperatorBase<Rhs, RhsData>, Rhs>::value) || std::is_same<Rhs, RhsData>::value, "Rhs must derive from BufferBase and BufferOperatorBase. Or RhsData must be left as default");
		}
	};
#pragma endregion
#pragma region Addition
	template<class Lhs, typename LhsData, typename Rhs = Lhs, typename RhsData = LhsData>
	class BufferAdditionOperator : public BufferOperator<Lhs, LhsData, Rhs, RhsData>
	{
	public:
		BufferAdditionOperator() : BufferOperator<Lhs, LhsData, Rhs, RhsData>()
		{
			static_assert(has_addition_operator<LhsData, RhsData>::value && has_addition_assignment_operator<LhsData, RhsData>::value, "LhsData must have operator+(RhsData) and operator+=(RhsData)");
		}

		template<typename U = Rhs, typename V = RhsData>
		typename std::enable_if<not std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type operator+(const U& rhs) const
		{
			Lhs result{};
			((BufferAdditionOperator*)&result)->Access_pData() = this->Access_AllocateUninitialized(this->Access_SizeAsByte());
			((BufferAdditionOperator*)&result)->Access_size() = this->Access_size();

			LhsData* pLhsData = this->Access_begin();
			LhsData* pLhsDataEnd = this->Access_end();
			LhsData* pResultData = ((BufferAdditionOperator*)&result)->Access_begin();
			for (; pLhsData < pLhsDataEnd; ++pLhsData, ++pResultData)
			{
				(*pResultData) = (*pLhsData) + rhs;
			}

			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		typename std::enable_if<not std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type operator+=(const U& rhs)
		{
			LhsData* pLhsData = this->Access_begin();
			LhsData* pLhsDataEnd = this->Access_end();
			for (; pLhsData < pLhsDataEnd; ++pLhsData)
			{
				(*pLhsData) += rhs;
			}
			return *((Lhs*)this);
		}

		template<typename U = Rhs, typename V = RhsData>
		typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type operator+(const U& rhs) const
		{
			if (this->Access_size() != ((BufferAdditionOperator*)&rhs)->Access_size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferAdditionOperator::operator+", "Both operands must have the same size"));
			}

			Lhs result{};
			((BufferAdditionOperator*)&result)->Access_pData() = this->Access_AllocateUninitialized(this->Access_SizeAsByte());
			((BufferAdditionOperator*)&result)->Access_size() = this->Access_size();

			LhsData* pLhsData = this->Access_begin();
			LhsData* pLhsDataEnd = this->Access_end();
			V* pRhsData = ((BufferAdditionOperator*)&rhs)->Access_begin();
			LhsData* pResultData = ((BufferAdditionOperator*)&result)->Access_begin();
			for (; pLhsData < pLhsDataEnd; ++pLhsData, ++pRhsData, ++pResultData)
			{
				(*pResultData) = (*pLhsData) + (*pRhsData);
			}

			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type operator+=(const U& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(this->Access_size(), ((BufferAdditionOperator*)&rhs)->Access_size());
			LhsData* pLhsData = this->Access_begin();
			V* pRhsData = ((BufferAdditionOperator*)&rhs)->Access_begin();
			for (size_t i = 0; i < minSize; ++i, ++pLhsData, ++pRhsData)
			{
				(*pLhsData) += (*pRhsData);
			}
			return *((Lhs*)this);
		}
	};
#pragma endregion
}