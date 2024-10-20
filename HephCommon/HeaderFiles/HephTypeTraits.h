#pragma once
#include "HephShared.h"
#include <type_traits>

/** @file */

namespace Heph
{
#pragma region Arithmetic Operators
	template<class Lhs, class Rhs = Lhs, class Ret = Lhs, typename = void>
	struct has_addition_operator : std::false_type {};
	template<class Lhs, class Rhs, class Ret>
	struct has_addition_operator<Lhs, Rhs, Ret,
		typename std::enable_if<std::is_convertible<Ret, decltype(std::declval<Lhs>() + std::declval<Rhs>())>::value>::type> : std::true_type {};

	template<class Lhs, class Rhs = Lhs, class Ret = Lhs, typename = void>
	struct has_subtraction_operator : std::false_type {};
	template<class Lhs, class Rhs, class Ret>
	struct has_subtraction_operator<Lhs, Rhs, Ret,
		typename std::enable_if<std::is_convertible<Ret, decltype(std::declval<Lhs>() - std::declval<Rhs>())>::value>::type> : std::true_type {};

	template<class Lhs, class Rhs = Lhs, class Ret = Lhs, typename = void>
	struct has_multiplication_operator : std::false_type {};
	template<class Lhs, class Rhs, class Ret>
	struct has_multiplication_operator<Lhs, Rhs, Ret,
		typename std::enable_if<std::is_convertible<Ret, decltype(std::declval<Lhs>()* std::declval<Rhs>())>::value>::type> : std::true_type {};

	template<class Lhs, class Rhs = Lhs, class Ret = Lhs, typename = void>
	struct has_division_operator : std::false_type {};
	template<class Lhs, class Rhs, class Ret>
	struct has_division_operator<Lhs, Rhs, Ret,
		typename std::enable_if<std::is_convertible<Ret, decltype(std::declval<Lhs>() / std::declval<Rhs>())>::value>::type> : std::true_type {};

	template<class Lhs, class Rhs = Lhs, typename = void>
	struct has_addition_assignment_operator : std::false_type {};
	template<class Lhs, class Rhs>
	struct has_addition_assignment_operator<Lhs, Rhs, typename std::enable_if<std::is_arithmetic<Lhs>::value&& std::is_arithmetic<Rhs>::value>::type>
		: std::true_type {};
	template<class Lhs, class Rhs>
	struct has_addition_assignment_operator<Lhs, Rhs,
		typename std::enable_if<std::is_same<Lhs&, decltype(std::declval<Lhs>() += std::declval<Rhs>())>::value>::type>
		: std::true_type {};

	template<class Lhs, class Rhs = Lhs, typename = void>
	struct has_subtraction_assignment_operator : std::false_type {};
	template<class Lhs, class Rhs>
	struct has_subtraction_assignment_operator<Lhs, Rhs, typename std::enable_if<std::is_arithmetic<Lhs>::value&& std::is_arithmetic<Rhs>::value>::type>
		: std::true_type {};
	template<class Lhs, class Rhs>
	struct has_subtraction_assignment_operator<Lhs, Rhs,
		typename std::enable_if<std::is_same<Lhs&, decltype(std::declval<Lhs>() -= std::declval<Rhs>())>::value>::type>
		: std::true_type {};

	template<class Lhs, class Rhs = Lhs, typename = void>
	struct has_multiplication_assignment_operator : std::false_type {};
	template<class Lhs, class Rhs>
	struct has_multiplication_assignment_operator<Lhs, Rhs, typename std::enable_if<std::is_arithmetic<Lhs>::value&& std::is_arithmetic<Rhs>::value>::type>
		: std::true_type {};
	template<class Lhs, class Rhs>
	struct has_multiplication_assignment_operator<Lhs, Rhs,
		typename std::enable_if<std::is_same<Lhs&, decltype(std::declval<Lhs>() *= std::declval<Rhs>())>::value>::type>
		: std::true_type {};

	template<class Lhs, class Rhs = Lhs, typename = void>
	struct has_division_assignment_operator : std::false_type {};
	template<class Lhs, class Rhs>
	struct has_division_assignment_operator<Lhs, Rhs, typename std::enable_if<std::is_arithmetic<Lhs>::value&& std::is_arithmetic<Rhs>::value>::type>
		: std::true_type {};
	template<class Lhs, class Rhs>
	struct has_division_assignment_operator<Lhs, Rhs,
		typename std::enable_if<std::is_same<Lhs&, decltype(std::declval<Lhs>() /= std::declval<Rhs>())>::value>::type>
		: std::true_type {};

	template<class Lhs, class Rhs = Lhs, class Ret = Lhs, typename = void>
	struct has_arithmetic_operators : std::false_type {};
	template<class Lhs, class Rhs, class Ret>
	struct has_arithmetic_operators<Lhs, Rhs, Ret,
		typename std::enable_if<
		has_addition_operator<Lhs, Rhs, Ret>::value&&
		has_subtraction_operator<Lhs, Rhs, Ret>::value&&
		has_multiplication_operator<Lhs, Rhs, Ret>::value&&
		has_division_operator<Lhs, Rhs, Ret>::value&&
		has_addition_assignment_operator<Lhs, Rhs>::value&&
		has_subtraction_assignment_operator<Lhs, Rhs>::value&&
		has_multiplication_assignment_operator<Lhs, Rhs>::value&&
		has_division_assignment_operator<Lhs, Rhs>::value
		>::type> : std::true_type {};
#pragma endregion
#pragma region Unary Operators
	template<class T, typename = void>
	struct has_post_increment_operator : std::false_type {};
	template<class T>
	struct has_post_increment_operator<T,
		typename std::enable_if<std::is_same<T, decltype(((T&)std::declval<T>())++)>::value>::type> : std::true_type {};

	template<class T, typename = void>
	struct has_post_decrement_operator : std::false_type {};
	template<class T>
	struct has_post_decrement_operator<T,
		typename std::enable_if<std::is_same<T, decltype(((T&)std::declval<T>())--)>::value>::type> : std::true_type {};

	template<class T, typename = void>
	struct has_pre_increment_operator : std::false_type {};
	template<class T>
	struct has_pre_increment_operator<T,
		typename std::enable_if<std::is_same<T, decltype(++((T&)std::declval<T>()))>::value>::type> : std::true_type {};

	template<class T, typename = void>
	struct has_pre_decrement_operator : std::false_type {};
	template<class T>
	struct has_pre_decrement_operator<T,
		typename std::enable_if<std::is_same<T, decltype(--((T&)std::declval<T>()))>::value>::type> : std::true_type {};

	template<class T, typename = void>
	struct has_unary_plus_operator : std::false_type {};
	template<class T>
	struct has_unary_plus_operator<T,
		typename std::enable_if<std::is_same<T, decltype(+std::declval<T>())>::value>::type> : std::true_type {};

	template<class T, typename = void>
	struct has_unary_minus_operator : std::false_type {};
	template<class T>
	struct has_unary_minus_operator<T,
		typename std::enable_if<std::is_same<T, decltype(-std::declval<T>())>::value && !std::is_unsigned<T>::value>::type> : std::true_type {};

	template<class T, typename = void>
	struct has_logical_not_operator : std::false_type {};
	template<class T>
	struct has_logical_not_operator<T,
		typename std::enable_if<std::is_same<T, decltype(!std::declval<T>())>::value>::type> : std::true_type {};

	template<class T, typename = void>
	struct has_bitwise_not_operator : std::false_type {};
	template<class T>
	struct has_bitwise_not_operator<T,
		typename std::enable_if<std::is_same<T, decltype(~std::declval<T>())>::value>::type> : std::true_type {};
#pragma endregion
}