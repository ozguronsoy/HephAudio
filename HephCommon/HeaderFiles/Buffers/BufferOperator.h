#pragma once
#include "HephCommonShared.h"
#include "BufferBase.h"
#include "HephTypeTraits.h"

namespace HephCommon
{
#pragma region Base
	template<typename Lhs, typename LhsData, typename Rhs, typename RhsData>
	class BufferOperator
	{
	protected:
		static constexpr bool DEFINE_RHS_LHS_OPERATOR = !has_addition_operator<Rhs, Lhs, Lhs>::value && !has_addition_operator<Rhs, Lhs, Rhs>::value;

	public:
		BufferOperator()
		{
			static_assert(std::is_base_of<BufferBase<Lhs, LhsData>, Lhs>::value, "Lhs must derive from BufferBase<Lhs, LhsData>");
			static_assert(
				std::is_base_of<BufferBase<Rhs, RhsData>, Rhs>::value || std::is_same<Rhs, RhsData>::value,
				"Rhs must derive from BufferBase<Rhs, RhsData> or RhsData must be the same type as Rhs"
				);
			static_assert(std::is_constructible<Lhs, size_t, BufferFlags>::value, "Lhs must have the constructor Lhs(size_t, BufferFlags)");
		}
	};
#pragma endregion
#pragma region Addition
	template<class Lhs, typename LhsData, typename Rhs = LhsData, typename RhsData = Rhs>
	class BufferAdditionOperator : private BufferOperator<Lhs, LhsData, Rhs, RhsData>
	{
		using BufferOperator = BufferOperator<Lhs, LhsData, Rhs, RhsData>;

	public:
		BufferAdditionOperator() : BufferOperator()
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

		template<typename Ret = Lhs>
		friend static typename std::enable_if<BufferOperator::DEFINE_RHS_LHS_OPERATOR, Ret>::type operator+(const Rhs& rhs, const Lhs& lhs)
		{
			return lhs + rhs;
		}

		template<typename Ret>
		friend static typename std::enable_if<!BufferOperator::DEFINE_RHS_LHS_OPERATOR, Ret>::type operator+(const Rhs& rhs, const Lhs& lhs);

	private:
		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			const size_t size = lhs.Size();
			Lhs result(size, BufferFlags::AllocUninitialized);
			for (size_t i = 0; i < size; ++i)
			{
				result[i] = lhs[i] + rhs;
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				lhs[i] += rhs;
			}
			return lhs;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			if (lhs.Size() != rhs.Size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferAdditionOperator::operator+", "Both operands must have the same size"));
			}

			const size_t size = lhs.Size();
			Lhs result(size, BufferFlags::AllocUninitialized);
			for (size_t i = 0; i < size; ++i)
			{
				result[i] = lhs[i] + rhs[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(lhs.Size(), rhs.Size());
			for (size_t i = 0; i < minSize; ++i)
			{
				lhs[i] += rhs[i];
			}
			return lhs;
		}
	};
#pragma endregion
}