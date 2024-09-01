#pragma once
#include "HephCommonShared.h"
#include "BufferBase.h"
#include "HephTypeTraits.h"
#include "Event.h"

namespace HephCommon
{
#pragma region Event
	template<typename Lhs, typename Rhs>
	struct BufferOperatorResultCreatedEventArgs : public EventArgs
	{
		const Lhs& lhs;
		const Rhs& rhs;
		Lhs& result;
		BufferOperatorResultCreatedEventArgs(const Lhs& lhs, const Rhs& rhs, Lhs& result) : lhs(lhs), rhs(rhs), result(result) {}
	};

	template<typename Lhs, typename Rhs>
	struct BufferOperatorEvents
	{
		// invoked after the result object is created in non-assignment operators.
		// the result object is created via default constructor, hence the buffer must handle the allocation with this event
		static inline Event OnResultCreated = Event();
	};
#pragma endregion
#pragma region Addition
	template<class Lhs, typename LhsData, typename Rhs = LhsData, typename RhsData = Rhs>
	class BufferAdditionOperator
	{
		static constexpr bool DEFINE_RHS_LHS_OPERATOR = !has_addition_operator<Rhs, Lhs, Lhs>::value && !has_addition_operator<Rhs, Lhs, Rhs>::value;

	public:
		BufferAdditionOperator()
		{
			static_assert(std::is_default_constructible<Lhs>::value, "Lhs must have a default constructor");
			static_assert(std::is_base_of<BufferBase<Lhs, LhsData>, Lhs>::value, "Lhs must derive from BufferBase<Lhs, LhsData>");
			static_assert(
				std::is_base_of<BufferBase<Rhs, RhsData>, Rhs>::value || std::is_same<Rhs, RhsData>::value,
				"Rhs must derive from BufferBase<Rhs, RhsData> or RhsData must be the same type as Rhs"
				);
			static_assert(
				has_addition_operator<LhsData, RhsData>::value && has_addition_assignment_operator<LhsData, RhsData>::value,
				"LhsData must have operator+(RhsData) and operator+=(RhsData)"
				);
		}
		BufferAdditionOperator(const BufferAdditionOperator&) = delete;
		BufferAdditionOperator& operator=(const BufferAdditionOperator&) = delete;

	public:
		friend inline Lhs operator+(const Lhs& lhs, const Rhs& rhs)
		{
			return Impl<Rhs, RhsData>(lhs, rhs);
		}

		friend inline Lhs& operator+=(Lhs& lhs, const Rhs& rhs)
		{
			return ImplAssign<Rhs, RhsData>(lhs, rhs);
		}

		template<typename Ret = Lhs>
		friend inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR, Ret>::type operator+(const Rhs& rhs, const Lhs& lhs)
		{
			return lhs + rhs;
		}

		template<typename Ret>
		friend inline typename std::enable_if<!DEFINE_RHS_LHS_OPERATOR, Ret>::type operator+(const Rhs& rhs, const Lhs& lhs);

	private:
		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferAdditionOperator::operator+", "BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] + rhs;
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				lhs.begin()[i] += rhs;
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

			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferAdditionOperator::operator+",
					"BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] + rhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(lhs.Size(), rhs.Size());
			for (size_t i = 0; i < minSize; ++i)
			{
				lhs.begin()[i] += rhs.begin()[i];
			}
			return lhs;
		}
	};
#pragma endregion
#pragma region Subtraction
	template<class Lhs, typename LhsData, typename Rhs = LhsData, typename RhsData = Rhs>
	class BufferSubtractionOperator
	{
		static constexpr bool DEFINE_RHS_LHS_OPERATOR = !has_subtraction_operator<Rhs, Lhs, Lhs>::value && !has_subtraction_operator<Rhs, Lhs, Rhs>::value;

	public:
		BufferSubtractionOperator()
		{
			static_assert(std::is_default_constructible<Lhs>::value, "Lhs must have a default constructor");
			static_assert(std::is_base_of<BufferBase<Lhs, LhsData>, Lhs>::value, "Lhs must derive from BufferBase<Lhs, LhsData>");
			static_assert(
				std::is_base_of<BufferBase<Rhs, RhsData>, Rhs>::value || std::is_same<Rhs, RhsData>::value,
				"Rhs must derive from BufferBase<Rhs, RhsData> or RhsData must be the same type as Rhs"
				);
			static_assert(
				has_subtraction_operator<LhsData, RhsData>::value && has_subtraction_assignment_operator<LhsData, RhsData>::value,
				"LhsData must have operator-(RhsData) and operator-=(RhsData)"
				);
		}
		BufferSubtractionOperator(const BufferSubtractionOperator&) = delete;
		BufferSubtractionOperator& operator=(const BufferSubtractionOperator&) = delete;

	public:
		friend inline Lhs operator-(const Lhs& lhs, const Rhs& rhs)
		{
			return Impl<Rhs, RhsData>(lhs, rhs);
		}

		friend inline Lhs& operator-=(Lhs& lhs, const Rhs& rhs)
		{
			return ImplAssign<Rhs, RhsData>(lhs, rhs);
		}

		template<typename Ret = Lhs>
		friend inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR, Ret>::type operator-(const Rhs& rhs, const Lhs& lhs)
		{
			return ImplRhs<Rhs, RhsData>(rhs, lhs);
		}

		template<typename Ret>
		friend inline typename std::enable_if<!DEFINE_RHS_LHS_OPERATOR, Ret>::type operator-(const Rhs& rhs, const Lhs& lhs);

	private:
		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferSubtractionOperator::operator-", "BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] - rhs;
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR&& std::is_same<U, V>::value, Lhs>::type ImplRhs(const U& rhs, const Lhs& lhs)
		{
			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferSubtractionOperator::operator-", "BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = rhs - lhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				lhs.begin()[i] -= rhs;
			}
			return lhs;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			if (lhs.Size() != rhs.Size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferSubtractionOperator::operator-", "Both operands must have the same size"));
			}

			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferSubtractionOperator::operator-",
					"BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] - rhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR&& std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type ImplRhs(const U& rhs, const Lhs& lhs)
		{
			if (lhs.Size() != rhs.Size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferSubtractionOperator::operator-", "Both operands must have the same size"));
			}

			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferSubtractionOperator::operator-",
					"BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = rhs.begin()[i] - lhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(lhs.Size(), rhs.Size());
			for (size_t i = 0; i < minSize; ++i)
			{
				lhs.begin()[i] -= rhs.begin()[i];
			}
			return lhs;
		}
	};
#pragma endregion
#pragma region Multiplication
	template<class Lhs, typename LhsData, typename Rhs = LhsData, typename RhsData = Rhs>
	class BufferMultiplicationOperator
	{
		static constexpr bool DEFINE_RHS_LHS_OPERATOR = !has_multiplication_operator<Rhs, Lhs, Lhs>::value && !has_multiplication_operator<Rhs, Lhs, Rhs>::value;

	public:
		BufferMultiplicationOperator()
		{
			static_assert(std::is_default_constructible<Lhs>::value, "Lhs must have a default constructor");
			static_assert(std::is_base_of<BufferBase<Lhs, LhsData>, Lhs>::value, "Lhs must derive from BufferBase<Lhs, LhsData>");
			static_assert(
				std::is_base_of<BufferBase<Rhs, RhsData>, Rhs>::value || std::is_same<Rhs, RhsData>::value,
				"Rhs must derive from BufferBase<Rhs, RhsData> or RhsData must be the same type as Rhs"
				);
			static_assert(
				has_multiplication_operator<LhsData, RhsData>::value && has_multiplication_operator<LhsData, RhsData>::value,
				"LhsData must have operator*(RhsData) and operator*=(RhsData)"
				);
		}
		BufferMultiplicationOperator(const BufferMultiplicationOperator&) = delete;
		BufferMultiplicationOperator& operator=(const BufferMultiplicationOperator&) = delete;

	public:
		friend inline Lhs operator*(const Lhs& lhs, const Rhs& rhs)
		{
			return Impl<Rhs, RhsData>(lhs, rhs);
		}

		friend inline Lhs& operator*=(Lhs& lhs, const Rhs& rhs)
		{
			return ImplAssign<Rhs, RhsData>(lhs, rhs);
		}

		template<typename Ret = Lhs>
		friend inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR, Ret>::type operator*(const Rhs& rhs, const Lhs& lhs)
		{
			return lhs * rhs;
		}

		template<typename Ret>
		friend inline typename std::enable_if<!DEFINE_RHS_LHS_OPERATOR, Ret>::type operator*(const Rhs& rhs, const Lhs& lhs);

	private:
		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferMultiplicationOperator::operator*", "BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] * rhs;
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				lhs.begin()[i] *= rhs;
			}
			return lhs;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			if (lhs.Size() != rhs.Size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferMultiplicationOperator::operator*", "Both operands must have the same size"));
			}

			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferMultiplicationOperator::operator*",
					"BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] * rhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(lhs.Size(), rhs.Size());
			for (size_t i = 0; i < minSize; ++i)
			{
				lhs.begin()[i] *= rhs.begin()[i];
			}
			return lhs;
		}
	};
#pragma endregion
#pragma region Division
	template<class Lhs, typename LhsData, typename Rhs = LhsData, typename RhsData = Rhs>
	class BufferDivisionOperator
	{
		static constexpr bool DEFINE_RHS_LHS_OPERATOR = !has_division_operator<Rhs, Lhs, Lhs>::value && !has_division_operator<Rhs, Lhs, Rhs>::value;

	public:
		BufferDivisionOperator()
		{
			static_assert(std::is_default_constructible<Lhs>::value, "Lhs must have a default constructor");
			static_assert(std::is_base_of<BufferBase<Lhs, LhsData>, Lhs>::value, "Lhs must derive from BufferBase<Lhs, LhsData>");
			static_assert(
				std::is_base_of<BufferBase<Rhs, RhsData>, Rhs>::value || std::is_same<Rhs, RhsData>::value,
				"Rhs must derive from BufferBase<Rhs, RhsData> or RhsData must be the same type as Rhs"
				);
			static_assert(
				has_division_operator<LhsData, RhsData>::value && has_division_assignment_operator<LhsData, RhsData>::value,
				"LhsData must have operator/(RhsData) and operator/=(RhsData)"
				);
		}
		BufferDivisionOperator(const BufferDivisionOperator&) = delete;
		BufferDivisionOperator& operator=(const BufferDivisionOperator&) = delete;

	public:
		friend inline Lhs operator/(const Lhs& lhs, const Rhs& rhs)
		{
			return Impl<Rhs, RhsData>(lhs, rhs);
		}

		friend inline Lhs& operator/=(Lhs& lhs, const Rhs& rhs)
		{
			return ImplAssign<Rhs, RhsData>(lhs, rhs);
		}

		template<typename Ret = Lhs>
		friend inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR, Ret>::type operator/(const Rhs& rhs, const Lhs& lhs)
		{
			return ImplRhs<Rhs, RhsData>(rhs, lhs);
		}

		template<typename Ret>
		friend inline typename std::enable_if<!DEFINE_RHS_LHS_OPERATOR, Ret>::type operator/(const Rhs& rhs, const Lhs& lhs);

	private:
		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferDivisionOperator::operator/", "BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] / rhs;
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR and std::is_same<U, V>::value, Lhs>::type ImplRhs(const U& rhs, const Lhs& lhs)
		{
			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferDivisionOperator::operator/", "BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = rhs / lhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_same<U, V>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				lhs.begin()[i] /= rhs;
			}
			return lhs;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type Impl(const Lhs& lhs, const U& rhs)
		{
			if (lhs.Size() != rhs.Size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferDivisionOperator::operator/", "Both operands must have the same size"));
			}

			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferDivisionOperator::operator/",
					"BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = lhs.begin()[i] / rhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<DEFINE_RHS_LHS_OPERATOR and std::is_base_of<BufferBase<U, V>, U>::value, Lhs>::type ImplRhs(const U& rhs, const Lhs& lhs)
		{
			if (lhs.Size() != rhs.Size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferDivisionOperator::operator/", "Both operands must have the same size"));
			}

			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Rhs> args(lhs, rhs, result);
			if (!BufferOperatorEvents<Lhs, Rhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferDivisionOperator::operator/",
					"BufferOperatorEvents::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Rhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = rhs.begin()[i] / lhs.begin()[i];
			}
			return result;
		}

		template<typename U = Rhs, typename V = RhsData>
		static inline typename std::enable_if<std::is_base_of<BufferBase<U, V>, U>::value, Lhs&>::type ImplAssign(Lhs& lhs, const U& rhs)
		{
			const size_t minSize = HEPH_MATH_MIN(lhs.Size(), rhs.Size());
			for (size_t i = 0; i < minSize; ++i)
			{
				lhs.begin()[i] /= rhs.begin()[i];
			}
			return lhs;
		}
	};
#pragma endregion
#pragma region Arithmetic
	template<class Lhs, typename LhsData, typename Rhs = LhsData, typename RhsData = Rhs>
	class BufferArithmeticOperators :
		public BufferAdditionOperator<Lhs, LhsData, Rhs, RhsData>,
		public BufferSubtractionOperator<Lhs, LhsData, Rhs, RhsData>,
		public BufferMultiplicationOperator<Lhs, LhsData, Rhs, RhsData>,
		public BufferDivisionOperator<Lhs, LhsData, Rhs, RhsData>
	{
	public:
		BufferArithmeticOperators() = default;
		BufferArithmeticOperators(const BufferArithmeticOperators&) = delete;
		BufferArithmeticOperators& operator=(const BufferArithmeticOperators&) = delete;
	};
#pragma endregion
#pragma region Unary Minus
	template<class Lhs, typename LhsData>
	class BufferUnaryMinusOperator
	{
	public:
		BufferUnaryMinusOperator()
		{
			static_assert(std::is_default_constructible<Lhs>::value, "Lhs must have a default constructor");
			static_assert(std::is_base_of<BufferBase<Lhs, LhsData>, Lhs>::value, "Lhs must derive from BufferBase<Lhs, LhsData>");
			static_assert(has_unary_minus_operator<LhsData>::value, "LhsData must have operator-()");
		}
		BufferUnaryMinusOperator(const BufferUnaryMinusOperator&) = delete;
		BufferUnaryMinusOperator& operator=(const BufferUnaryMinusOperator&) = delete;

	public:
		friend inline Lhs operator-(const Lhs& lhs)
		{
			Lhs result{};
			BufferOperatorResultCreatedEventArgs<Lhs, Lhs> args(lhs, lhs, result);
			if (!BufferOperatorEvents<Lhs, Lhs>::OnResultCreated)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(&lhs, HephException(HEPH_EC_INVALID_OPERATION, "BufferUnaryMinusOperator::operator-", "BufferUnaryMinusOperator::OnResultCreated event must be handled"));
			}
			BufferOperatorEvents<Lhs, Lhs>::OnResultCreated(&args, nullptr);

			const size_t size = lhs.Size();
			for (size_t i = 0; i < size; ++i)
			{
				result.begin()[i] = -lhs.begin()[i];
			}

			return result;
		}
	};
#pragma endregion
}