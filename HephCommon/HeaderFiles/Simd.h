#pragma once
#include "HephCommonShared.h"
#include <cinttypes>

namespace HephCommon
{
	class SIMD final
	{
	public:
		SIMD() = delete;
		SIMD(const SIMD&) = delete;
		SIMD& operator=(const SIMD&) = delete;
	public:
#pragma region 256
#pragma region FLT
		static void Add256(float* destination, const float* lhs, const float* rhs) noexcept;
		static void Sub256(float* destination, const float* lhs, const float* rhs) noexcept;
		static void Mul256(float* destination, const float* lhs, const float* rhs) noexcept;
		static void Div256(float* destination, const float* lhs, const float* rhs) noexcept;
		static float SumElems256(const float* v) noexcept;
#pragma endregion
#pragma region DBL
		static void Add256(double* destination, const double* lhs, const double* rhs) noexcept;
		static void Sub256(double* destination, const double* lhs, const double* rhs) noexcept;
		static void Mul256(double* destination, const double* lhs, const double* rhs) noexcept;
		static void Div256(double* destination, const double* lhs, const double* rhs) noexcept;
		static double SumElems256(const double* v) noexcept;
#pragma endregion
#pragma region S16
		static void Add256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept;
		static void Sub256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept;
		static void Mul256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept;
		static void Div256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept;
		static int16_t SumElems256(const int16_t* v) noexcept;
#pragma endregion
#pragma region U16
		static void Add256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept;
		static void Sub256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept;
		static void Mul256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept;
		static void Div256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept;
		static uint16_t SumElems256(const uint16_t* v) noexcept;
#pragma endregion
#pragma region S32
		static void Add256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept;
		static void Sub256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept;
		static void Mul256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept;
		static void Div256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept;
		static int32_t SumElems256(const int32_t* v) noexcept;
#pragma endregion
#pragma region U32
		static void Add256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept;
		static void Sub256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept;
		static void Mul256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept;
		static void Div256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept;
		static uint32_t SumElems256(const uint32_t* v) noexcept;
#pragma endregion
#pragma region S64
		static void Add256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept;
		static void Sub256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept;
		static void Mul256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept;
		static void Div256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept;
		static int64_t SumElems256(const int64_t* v) noexcept;
#pragma endregion
#pragma region U64
		static void Add256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept;
		static void Sub256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept;
		static void Mul256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept;
		static void Div256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept;
		static uint64_t SumElems256(const uint64_t* v) noexcept;
#pragma endregion
#pragma endregion
	};
}