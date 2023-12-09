#pragma once
#include "HephCommonFramework.h"
#include "FloatBuffer.h"
#include "Vector.h"

namespace HephCommon
{
	class Matrix final
	{
	private:
		FloatBuffer elements;
		size_t rowCount;
		size_t colCount;
	public:
		Matrix(size_t row, size_t col);
		explicit Matrix(heph_float rhs);
		explicit Matrix(const Vector2& rhs);
		explicit Matrix(const Vector3& rhs);
		Matrix(const std::initializer_list<heph_float>& rhs);
		Matrix(const std::initializer_list<std::initializer_list<heph_float>>& rhs);
		Matrix(const Matrix& rhs);
		Matrix(Matrix&& rhs) noexcept;
		heph_float* operator[](size_t row) const;
		Matrix operator+() const;
		Matrix operator-() const;
		Matrix& operator=(heph_float rhs);
		Matrix& operator=(const Vector2& rhs);
		Matrix& operator=(const Vector3& rhs);
		Matrix& operator=(const std::initializer_list<heph_float>& rhs);
		Matrix& operator=(const std::initializer_list<std::initializer_list<heph_float>>& rhs);
		Matrix& operator=(const Matrix& rhs);
		Matrix& operator=(Matrix&& rhs) noexcept;
		Matrix operator+(const Matrix& rhs) const;
		Matrix& operator+=(const Matrix& rhs);
		Matrix operator-(const Matrix& rhs) const;
		Matrix& operator-=(const Matrix& rhs);
		Matrix operator*(const Matrix& rhs) const;
		Matrix& operator*=(const Matrix& rhs);
		Matrix operator*(heph_float rhs) const;
		Matrix& operator*=(heph_float rhs);
		Matrix operator/(heph_float rhs) const;
		Matrix& operator/=(heph_float rhs);
		size_t RowCount() const noexcept;
		size_t ColCount() const noexcept;
		size_t ElementCount() const noexcept;
		bool IsEmpty() const noexcept;
		heph_float& At(size_t row, size_t col) const;
		Matrix Transpose() const;
		heph_float Determinant() const;
		Matrix Cofactor() const;
		Matrix Inverse() const;
	};
}
HephCommon::Matrix operator*(heph_float lhs, const HephCommon::Matrix& rhs);