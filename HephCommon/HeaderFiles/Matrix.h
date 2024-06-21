#pragma once
#include "HephCommonShared.h"
#include <initializer_list>

#if !defined(HEPH_MATRIX_ELEMENT_TYPE)

#if defined(HEPH_MATRIX_ELEMENT_TYPE_DBL)
typedef double heph_matrix_element_t;
#elif defined(HEPH_MATRIX_ELEMENT_TYPE_FLT)
typedef float heph_matrix_element_t;
#elif defined(HEPH_MATRIX_ELEMENT_TYPE_S64)
typedef int64_t heph_matrix_element_t;
#elif defined(HEPH_MATRIX_ELEMENT_TYPE_U64)
typedef uint64_t heph_matrix_element_t;
#elif defined(HEPH_MATRIX_ELEMENT_TYPE_S32)
typedef int32_t heph_matrix_element_t;
#elif defined(HEPH_MATRIX_ELEMENT_TYPE_U32)
typedef uint32_t heph_matrix_element_t;
#elif defined(HEPH_MATRIX_ELEMENT_TYPE_S16)
typedef int16_t heph_matrix_element_t;
#elif defined(HEPH_MATRIX_ELEMENT_TYPE_U16)
typedef uint16_t heph_matrix_element_t;
#else
typedef float heph_matrix_element_t;
#endif

#define HEPH_MATRIX_ELEMENT_TYPE heph_matrix_element_t

#endif

namespace HephCommon
{
	class Matrix final
	{
	private:
		size_t row;
		size_t col;
		heph_matrix_element_t* pData;
	public:
		Matrix();
		Matrix(size_t row, size_t col);
		Matrix(const Matrix& rhs);
		Matrix(const std::initializer_list<heph_matrix_element_t>& rhs);
		Matrix(const std::initializer_list<std::initializer_list<heph_matrix_element_t>>& rhs);
		Matrix(Matrix&& rhs) noexcept;
		~Matrix() noexcept;
		heph_matrix_element_t* operator[](size_t r) const;
		Matrix operator-() const;
		Matrix& operator=(const Matrix& rhs);
		Matrix& operator=(const std::initializer_list<heph_matrix_element_t>& rhs);
		Matrix& operator=(const std::initializer_list<std::initializer_list<heph_matrix_element_t>>& rhs);
		Matrix& operator=(Matrix&& rhs) noexcept;
		Matrix operator+(heph_matrix_element_t rhs) const;
		Matrix operator+(const Matrix& rhs) const;
		Matrix& operator+=(heph_matrix_element_t rhs);
		Matrix& operator+=(const Matrix& rhs);
		Matrix operator-(heph_matrix_element_t rhs) const;
		Matrix operator-(const Matrix& rhs) const;
		Matrix& operator-=(heph_matrix_element_t rhs);
		Matrix& operator-=(const Matrix& rhs);
		Matrix operator*(heph_matrix_element_t rhs) const;
		Matrix operator*(const Matrix& rhs) const;
		Matrix operator*=(heph_matrix_element_t rhs);
		bool operator==(const Matrix& rhs) const;
		bool operator!=(const Matrix& rhs) const;
		bool IsEmpty() const;
		size_t RowCount() const;
		size_t ColCount() const;
		size_t ElementCount() const;
		size_t Size() const;
		void Release() noexcept;
		heph_matrix_element_t Get(size_t index) const;
		heph_matrix_element_t Get(size_t r, size_t c) const;
		void Set(heph_matrix_element_t val, size_t index);
		void Set(heph_matrix_element_t val, size_t r, size_t c);
		void Resize(size_t newRow, size_t newCol);
		Matrix Transpose() const;
		static Matrix Dot(const Matrix& lhs, const Matrix& rhs, size_t threadCount);
		static Matrix CreateIdentity(size_t n);
	private:
		void CopyInitializerList(const std::initializer_list<std::initializer_list<heph_matrix_element_t>>& rhs);
		static void DotInternal(Matrix* pResult, const Matrix* pLhs, const Matrix* pRhs, size_t r, size_t operationCount);
	};
}