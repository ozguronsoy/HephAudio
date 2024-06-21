#include "Simd.h"
#include <immintrin.h>
#include <intrin.h>

namespace HephCommon
{
#pragma region 256
#pragma region FLT
	void SIMD::Add256(float* destination, const float* lhs, const float* rhs) noexcept
	{
		const __m256 m_lhs = _mm256_load_ps(lhs);
		const __m256 m_rhs = _mm256_load_ps(rhs);
		_mm256_store_ps(destination, _mm256_add_ps(m_lhs, m_rhs));
	}
	void SIMD::Sub256(float* destination, const float* lhs, const float* rhs) noexcept
	{
		const __m256 m_lhs = _mm256_load_ps(lhs);
		const __m256 m_rhs = _mm256_load_ps(rhs);
		_mm256_store_ps(destination, _mm256_sub_ps(m_lhs, m_rhs));
	}
	void SIMD::Mul256(float* destination, const float* lhs, const float* rhs) noexcept
	{
		const __m256 m_lhs = _mm256_load_ps(lhs);
		const __m256 m_rhs = _mm256_load_ps(rhs);
		_mm256_store_ps(destination, _mm256_mul_ps(m_lhs, m_rhs));
	}
	void SIMD::Div256(float* destination, const float* lhs, const float* rhs) noexcept
	{
		const __m256 m_lhs = _mm256_load_ps(lhs);
		const __m256 m_rhs = _mm256_load_ps(rhs);
		_mm256_store_ps(destination, _mm256_div_ps(m_lhs, m_rhs));
	}
	void SIMD::Fmadd256(float* destination, const float* a, const float* b, const float* c) noexcept
	{
		const __m256 m_a = _mm256_load_ps(a);
		const __m256 m_b = _mm256_load_ps(b);
		const __m256 m_c = _mm256_load_ps(c);
		_mm256_store_ps(destination, _mm256_fmadd_ps(m_a, m_b, m_c));
	}
	float SIMD::SumElems256(const float* v) noexcept
	{
		const __m256 m256 = _mm256_load_ps(v);
		__m128 m128 = _mm_add_ps(_mm256_castps256_ps128(m256), _mm256_extractf128_ps(m256, 1));
		m128 = _mm_add_ps(m128, _mm_movehl_ps(m128, m128));
		return _mm_cvtss_f32(_mm_add_ss(m128, _mm_shuffle_ps(m128, m128, 1)));
	}
#pragma endregion
#pragma region DBL
	void SIMD::Add256(double* destination, const double* lhs, const double* rhs) noexcept
	{
		const __m256d m_lhs = _mm256_load_pd(lhs);
		const __m256d m_rhs = _mm256_load_pd(rhs);
		_mm256_store_pd(destination, _mm256_add_pd(m_lhs, m_rhs));
	}
	void SIMD::Sub256(double* destination, const double* lhs, const double* rhs) noexcept
	{
		const __m256d m_lhs = _mm256_load_pd(lhs);
		const __m256d m_rhs = _mm256_load_pd(rhs);
		_mm256_store_pd(destination, _mm256_sub_pd(m_lhs, m_rhs));
	}
	void SIMD::Mul256(double* destination, const double* lhs, const double* rhs) noexcept
	{
		const __m256d m_lhs = _mm256_load_pd(lhs);
		const __m256d m_rhs = _mm256_load_pd(rhs);
		_mm256_store_pd(destination, _mm256_mul_pd(m_lhs, m_rhs));
	}
	void SIMD::Div256(double* destination, const double* lhs, const double* rhs) noexcept
	{
		const __m256d m_lhs = _mm256_load_pd(lhs);
		const __m256d m_rhs = _mm256_load_pd(rhs);
		_mm256_store_pd(destination, _mm256_div_pd(m_lhs, m_rhs));
	}
	void SIMD::Fmadd256(double* destination, const double* a, const double* b, const double* c) noexcept
	{
		const __m256d m_a = _mm256_load_pd(a);
		const __m256d m_b = _mm256_load_pd(b);
		const __m256d m_c = _mm256_load_pd(c);
		_mm256_store_pd(destination, _mm256_fmadd_pd(m_a, m_b, m_c));
	}
	double SIMD::SumElems256(const double* v) noexcept
	{
		const __m256d m256 = _mm256_load_pd(v);
		const __m128d m128 = _mm_add_pd(_mm256_castpd256_pd128(m256), _mm256_extractf128_pd(m256, 1));
		return _mm_cvtsd_f64(_mm_add_pd(m128, _mm_shuffle_pd(m128, m128, 1)));
	}
#pragma endregion
#pragma region S16
	void SIMD::Add256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi16((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi16((const void*)rhs);
		_mm256_storeu_epi16((void*)destination, _mm256_add_epi16(m_lhs, m_rhs));
	}
	void SIMD::Sub256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi16((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi16((const void*)rhs);
		_mm256_storeu_epi16((void*)destination, _mm256_sub_epi16(m_lhs, m_rhs));
	}
	void SIMD::Mul256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi16((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi16((const void*)rhs);
		_mm256_storeu_epi16((void*)destination, _mm256_mullo_epi16(m_lhs, m_rhs));
	}
	void SIMD::Div256(int16_t* destination, const int16_t* lhs, const int16_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi16((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi16((const void*)rhs);
		_mm256_storeu_epi16((void*)destination, _mm256_div_epi16(m_lhs, m_rhs));
	}
	void SIMD::Fmadd256(int16_t* destination, const int16_t* a, const int16_t* b, const int16_t* c) noexcept
	{
		SIMD::Mul256(destination, a, b);
		SIMD::Add256(destination, destination, c);
	}
	int16_t SIMD::SumElems256(const int16_t* v) noexcept
	{
		const __m256i m256 = _mm256_loadu_epi16((const void*)v);
		__m128i m128 = _mm_add_epi16(_mm256_castsi256_si128(m256), _mm256_extracti128_si256(m256, 1));
		m128 = _mm_add_epi16(m128, _mm_set_epi16(0, 0, 0, 0, _mm_extract_epi16(m128, 7), _mm_extract_epi16(m128, 6), _mm_extract_epi16(m128, 5), _mm_extract_epi16(m128, 4)));
		m128 = _mm_add_epi16(m128, _mm_set_epi16(0, 0, 0, 0, 0, 0, _mm_extract_epi16(m128, 3), _mm_extract_epi16(m128, 2)));
		return _mm_extract_epi16(m128, 0) + _mm_extract_epi16(m128, 1);
	}
#pragma endregion
#pragma region U16
	void SIMD::Add256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept
	{
		SIMD::Add256((int16_t*)destination, (int16_t*)lhs, (int16_t*)rhs);
	}
	void SIMD::Sub256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept
	{
		SIMD::Sub256((int16_t*)destination, (int16_t*)lhs, (int16_t*)rhs);
	}
	void SIMD::Mul256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept
	{
		SIMD::Mul256((int16_t*)destination, (int16_t*)lhs, (int16_t*)rhs);
	}
	void SIMD::Div256(uint16_t* destination, const uint16_t* lhs, const uint16_t* rhs) noexcept
	{
		SIMD::Div256((int16_t*)destination, (int16_t*)lhs, (int16_t*)rhs);
	}
	void SIMD::Fmadd256(uint16_t* destination, const uint16_t* a, const uint16_t* b, const uint16_t* c) noexcept
	{
		SIMD::Fmadd256((int16_t*)destination, (int16_t*)a, (int16_t*)b, (int16_t*)c);
	}
	uint16_t SIMD::SumElems256(const uint16_t* v) noexcept
	{
		return SIMD::SumElems256((int16_t*)v);
	}
#pragma endregion
#pragma region S32
	void SIMD::Add256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi32((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi32((const void*)rhs);
		_mm256_storeu_epi32((void*)destination, _mm256_add_epi32(m_lhs, m_rhs));
	}
	void SIMD::Sub256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi32((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi32((const void*)rhs);
		_mm256_storeu_epi32((void*)destination, _mm256_sub_epi32(m_lhs, m_rhs));
	}
	void SIMD::Mul256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi32((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi32((const void*)rhs);
		_mm256_storeu_epi32((void*)destination, _mm256_mullo_epi32(m_lhs, m_rhs));
	}
	void SIMD::Div256(int32_t* destination, const int32_t* lhs, const int32_t* rhs) noexcept
	{
		const __m256i m_lhs = _mm256_loadu_epi32((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi32((const void*)rhs);
		_mm256_storeu_epi32((void*)destination, _mm256_div_epi32(m_lhs, m_rhs));
	}
	void SIMD::Fmadd256(int32_t* destination, const int32_t* a, const int32_t* b, const int32_t* c) noexcept
	{
		SIMD::Mul256(destination, a, b);
		SIMD::Add256(destination, destination, c);
	}
	int32_t SIMD::SumElems256(const int32_t* v) noexcept
	{
		const __m256i m256 = _mm256_loadu_epi32((const void*)v);
		__m128i m128 = _mm_add_epi32(_mm256_castsi256_si128(m256), _mm256_extracti128_si256(m256, 1));
		m128 = _mm_add_epi32(m128, _mm_set_epi32(0, 0, _mm_extract_epi32(m128, 3), _mm_extract_epi32(m128, 2)));
		return _mm_extract_epi32(m128, 0) + _mm_extract_epi32(m128, 1);
	}
#pragma endregion
#pragma region U32
	void SIMD::Add256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept
	{
		SIMD::Add256((int32_t*)destination, (int32_t*)lhs, (int32_t*)rhs);
	}
	void SIMD::Sub256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept
	{
		SIMD::Sub256((int32_t*)destination, (int32_t*)lhs, (int32_t*)rhs);
	}
	void SIMD::Mul256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept
	{
		SIMD::Mul256((int32_t*)destination, (int32_t*)lhs, (int32_t*)rhs);
	}
	void SIMD::Div256(uint32_t* destination, const uint32_t* lhs, const uint32_t* rhs) noexcept
	{
		SIMD::Div256((int32_t*)destination, (int32_t*)lhs, (int32_t*)rhs);
	}
	void SIMD::Fmadd256(uint32_t* destination, const uint32_t* a, const uint32_t* b, const uint32_t* c) noexcept
	{
		SIMD::Fmadd256((int32_t*)destination, (int32_t*)a, (int32_t*)b, (int32_t*)c);
	}
	uint32_t SIMD::SumElems256(const uint32_t* v) noexcept
	{
		return SIMD::SumElems256((int32_t*)v);
	}
#pragma endregion
#pragma region S64
	void SIMD::Add256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept 
	{
		const __m256i m_lhs = _mm256_loadu_epi64((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi64((const void*)rhs);
		_mm256_storeu_epi64((void*)destination, _mm256_add_epi64(m_lhs, m_rhs));
	}
	void SIMD::Sub256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept 
	{
		const __m256i m_lhs = _mm256_loadu_epi64((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi64((const void*)rhs);
		_mm256_storeu_epi64((void*)destination, _mm256_sub_epi64(m_lhs, m_rhs));
	}
	void SIMD::Mul256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept 
	{
		const __m256i m_lhs = _mm256_loadu_epi64((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi64((const void*)rhs);
		_mm256_storeu_epi64((void*)destination, _mm256_mul_epi32(m_lhs, m_rhs));
	}
	void SIMD::Div256(int64_t* destination, const int64_t* lhs, const int64_t* rhs) noexcept 
	{
		const __m256i m_lhs = _mm256_loadu_epi64((const void*)lhs);
		const __m256i m_rhs = _mm256_loadu_epi64((const void*)rhs);
		_mm256_storeu_epi64((void*)destination, _mm256_div_epi64(m_lhs, m_rhs));
	}
	void SIMD::Fmadd256(int64_t* destination, const int64_t* a, const int64_t* b, const int64_t* c) noexcept
	{
		SIMD::Mul256(destination, a, b);
		SIMD::Add256(destination, destination, c);
	}
	int64_t SIMD::SumElems256(const int64_t* v) noexcept 
	{
		const __m256i m256 = _mm256_loadu_epi64((const void*)v);
		__m128i m128 = _mm_add_epi64(_mm256_castsi256_si128(m256), _mm256_extracti128_si256(m256, 1));
		return _mm_extract_epi64(m128, 0) + _mm_extract_epi64(m128, 1);
	}
#pragma endregion
#pragma region U64
	void SIMD::Add256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept
	{
		SIMD::Add256((int64_t*)destination, (int64_t*)lhs, (int64_t*)rhs);
	}
	void SIMD::Sub256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept
	{
		SIMD::Sub256((int64_t*)destination, (int64_t*)lhs, (int64_t*)rhs);
	}
	void SIMD::Mul256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept
	{
		SIMD::Mul256((int64_t*)destination, (int64_t*)lhs, (int64_t*)rhs);
	}
	void SIMD::Div256(uint64_t* destination, const uint64_t* lhs, const uint64_t* rhs) noexcept
	{
		SIMD::Div256((int64_t*)destination, (int64_t*)lhs, (int64_t*)rhs);
	}
	void SIMD::Fmadd256(uint64_t* destination, const uint64_t* a, const uint64_t* b, const uint64_t* c) noexcept
	{
		SIMD::Fmadd256((int64_t*)destination, (int64_t*)a, (int64_t*)b, (int64_t*)c);
	}
	uint64_t SIMD::SumElems256(const uint64_t* v) noexcept
	{
		return SIMD::SumElems256((int64_t*)v);
	}
#pragma endregion
#pragma endregion
}