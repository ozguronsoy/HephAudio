#include "Matrix.h"
#include "HephException.h"

#define MATRIX_CHECK_ROW_INDEX(i, c, method) if (i >= c) { RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, method, "Row index out of bounds.")); }
#define MATRIX_CHECK_COL_INDEX(i, c, method) if (i >= c) { RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, method, "Col index out of bounds.")); }
#define MATRIX_CHECK_INDICES(ri, ci, rc, cc, method)  MATRIX_CHECK_ROW_INDEX(ri, rc, method) MATRIX_CHECK_COL_INDEX(ci, cc, method)

namespace HephCommon
{
	Matrix::Matrix(size_t row, size_t col) : elements(row* col), rowCount(row), colCount(col) { }
	Matrix::Matrix(heph_float rhs) : elements(1), rowCount(1), colCount(1)
	{
		this->elements[0] = rhs;
	}
	Matrix::Matrix(const Vector2& rhs) : elements(2), rowCount(1), colCount(2)
	{
		this->elements[0] = rhs.x;
		this->elements[1] = rhs.y;
	}
	Matrix::Matrix(const Vector3& rhs) : elements(3), rowCount(1), colCount(3)
	{
		this->elements[0] = rhs.x;
		this->elements[1] = rhs.y;
		this->elements[2] = rhs.z;
	}
	Matrix::Matrix(const std::initializer_list<heph_float>& rhs) : elements(rhs.size()), rowCount(1), colCount(rhs.size())
	{
		if (this->colCount == 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::Matrix", "The matrix must have at least one column."));
		}

		for (size_t i = 0; i < this->colCount; i++)
		{
			this->elements[i] = rhs.begin()[i];
		}
	}
	Matrix::Matrix(const std::initializer_list<std::initializer_list<heph_float>>& rhs)
	{
		this->rowCount = rhs.size();
		if (this->rowCount == 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::Matrix", "The matrix must have at least one row."));
		}

		this->colCount = rhs.begin()[0].size();
		if (this->colCount == 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::Matrix", "The matrix must have at least one column."));
		}

		for (const std::initializer_list<heph_float>& row : rhs)
		{
			if (this->colCount != row.size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::Matrix", "All rows must have the same number of columns."));
			}
		}

		this->elements.Resize(this->ElementCount());

		size_t i = 0;
		for (const std::initializer_list<heph_float>& row : rhs)
		{
			for (heph_float element : row)
			{
				this->elements[i] = element;
				i++;
			}
		}
	}
	Matrix::Matrix(const Matrix& rhs) : elements(rhs.elements), rowCount(rhs.rowCount), colCount(rhs.colCount) {}
	Matrix::Matrix(Matrix&& rhs) noexcept : elements(std::move(rhs.elements)), rowCount(rhs.rowCount), colCount(rhs.colCount)
	{
		rhs.rowCount = 0;
		rhs.colCount = 0;
	}
	heph_float* Matrix::operator[](size_t row) const
	{
		MATRIX_CHECK_ROW_INDEX(row, this->rowCount, "Matrix::operator[]");
		return this->elements.Begin() + this->colCount * row;
	}
	Matrix Matrix::operator+() const
	{
		return *this;
	}
	Matrix Matrix::operator-() const
	{
		Matrix result(this->rowCount, this->colCount);
		const size_t elementCount = this->ElementCount();
		for (size_t i = 0; i < elementCount; i++)
		{
			result.elements[i] = -this->elements[i];
		}
		return result;
	}
	Matrix& Matrix::operator=(heph_float rhs)
	{
		this->rowCount = 1;
		this->colCount = 1;
		this->elements.Resize(1);
		this->elements[0] = rhs;
		return *this;
	}
	Matrix& Matrix::operator=(const Vector2& rhs) 
	{
		this->rowCount = 1;
		this->colCount = 2;
		this->elements.Resize(2);
		this->elements[0] = rhs.x;
		this->elements[1] = rhs.y;
		return *this;
	}
	Matrix& Matrix::operator=(const Vector3& rhs) 
	{
		this->rowCount = 1;
		this->colCount = 3;
		this->elements.Resize(3);
		this->elements[0] = rhs.x;
		this->elements[1] = rhs.y;
		this->elements[2] = rhs.z;
		return *this;
	}
	Matrix& Matrix::operator=(const std::initializer_list<heph_float>& rhs)
	{
		if (rhs.size() == 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator=", "The matrix must have at least one column."));
		}

		this->rowCount = 1;
		this->colCount = rhs.size();
		this->elements.Resize(this->colCount);

		for (size_t i = 0; i < this->colCount; i++)
		{
			this->elements[i] = rhs.begin()[i];
		}

		return *this;
	}
	Matrix& Matrix::operator=(const std::initializer_list<std::initializer_list<heph_float>>& rhs)
	{
		this->rowCount = rhs.size();
		if (this->rowCount == 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator=", "The matrix must have at least one row."));
		}

		this->colCount = rhs.begin()[0].size();
		if (this->colCount == 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator=", "The matrix must have at least one column."));
		}

		for (const std::initializer_list<heph_float>& row : rhs)
		{
			if (this->colCount != row.size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator=", "All rows must have the same number of columns."));
			}
		}

		this->elements.Resize(this->ElementCount());

		size_t i = 0;
		for (const std::initializer_list<heph_float>& row : rhs)
		{
			for (heph_float element : row)
			{
				this->elements[i] = element;
				i++;
			}
		}

		return *this;
	}
	Matrix& Matrix::operator=(const Matrix& rhs)
	{
		this->elements = rhs.elements;
		this->rowCount = rhs.rowCount;
		this->colCount = rhs.colCount;
		return *this;
	}
	Matrix& Matrix::operator=(Matrix&& rhs) noexcept
	{
		this->elements = std::move(rhs.elements);
		this->rowCount = rhs.rowCount;
		this->colCount = rhs.colCount;

		rhs.rowCount = 0;
		rhs.colCount = 0;

		return *this;
	}
	Matrix Matrix::operator+(const Matrix& rhs) const
	{
		if (rhs.rowCount != this->rowCount || rhs.colCount != rhs.colCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator+", "The matrices must have the same number of rows and columns to perform addition."));
		}

		Matrix result(this->rowCount, this->colCount);
		const size_t elementCount = this->ElementCount();

		for (size_t i = 0; i < elementCount; i++)
		{
			result.elements[i] = this->elements[i] + rhs.elements[i];
		}

		return result;
	}
	Matrix& Matrix::operator+=(const Matrix& rhs)
	{
		if (rhs.rowCount != this->rowCount || rhs.colCount != rhs.colCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator+=", "The matrices must have the same number of rows and columns to perform addition."));
		}

		const size_t elementCount = this->ElementCount();
		for (size_t i = 0; i < elementCount; i++)
		{
			this->elements[i] += rhs.elements[i];
		}

		return *this;
	}
	Matrix Matrix::operator-(const Matrix& rhs) const
	{
		if (rhs.rowCount != this->rowCount || rhs.colCount != rhs.colCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator-", "The matrices must have the same number of rows and columns to perform subtraction."));
		}

		Matrix result(this->rowCount, this->colCount);
		const size_t elementCount = this->ElementCount();

		for (size_t i = 0; i < elementCount; i++)
		{
			result.elements[i] = this->elements[i] - rhs.elements[i];
		}

		return result;
	}
	Matrix& Matrix::operator-=(const Matrix& rhs)
	{
		if (rhs.rowCount != this->rowCount || rhs.colCount != rhs.colCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator+=", "The matrices must have the same number of rows and columns to perform subtraction."));
		}

		const size_t elementCount = this->ElementCount();
		for (size_t i = 0; i < elementCount; i++)
		{
			this->elements[i] -= rhs.elements[i];
		}

		return *this;
	}
	Matrix Matrix::operator*(const Matrix& rhs) const
	{
		if (this->colCount != rhs.rowCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator*", "The # of columns in the first matrix does not match the # of rows in the second matrix."));
		}

		Matrix result(this->rowCount, rhs.colCount);

		for (size_t i = 0; i < this->rowCount; i++)
		{
			for (size_t j = 0; j < rhs.colCount; j++)
			{
				for (size_t k = 0; k < rhs.rowCount; k++)
				{
					result.elements[i * result.colCount + j] += this->elements[i * this->colCount + k] * rhs.elements[k * rhs.colCount + j];
				}
			}
		}

		return result;
	}
	Matrix& Matrix::operator*=(const Matrix& rhs)
	{
		if (this->colCount != rhs.rowCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::operator*=", "The # of columns in the first matrix does not match the # of rows in the second matrix."));
		}

		FloatBuffer tempElements(this->rowCount * rhs.colCount);
		for (size_t i = 0; i < this->rowCount; i++)
		{
			for (size_t j = 0; j < rhs.colCount; j++)
			{
				for (size_t k = 0; k < rhs.rowCount; k++)
				{
					tempElements[i * rhs.colCount + j] += this->elements[i * this->colCount + k] * rhs.elements[k * rhs.colCount + j];
				}
			}
		}

		this->elements = std::move(tempElements);
		this->colCount = rhs.colCount;

		return *this;
	}
	Matrix Matrix::operator*(heph_float rhs) const
	{
		Matrix result(this->rowCount, this->colCount);
		const size_t elementCount = this->ElementCount();
		for (size_t i = 0; i < elementCount; i++)
		{
			result.elements[i] = this->elements[i] * rhs;
		}
		return result;
	}
	Matrix& Matrix::operator*=(heph_float rhs)
	{
		this->elements *= rhs;
		return *this;
	}
	Matrix Matrix::operator/(heph_float rhs) const
	{
		Matrix result(this->rowCount, this->colCount);
		const size_t elementCount = this->ElementCount();
		for (size_t i = 0; i < elementCount; i++)
		{
			result.elements[i] = this->elements[i] / rhs;
		}
		return result;
	}
	Matrix& Matrix::operator/=(heph_float rhs)
	{
		this->elements /= rhs;
		return *this;
	}
	size_t Matrix::RowCount() const noexcept
	{
		return this->rowCount;
	}
	size_t Matrix::ColCount() const noexcept
	{
		return this->colCount;
	}
	size_t Matrix::ElementCount() const noexcept
	{
		return this->rowCount * this->colCount;
	}
	bool Matrix::IsEmpty() const noexcept
	{
		return this->rowCount == 0 || this->colCount == 0;
	}
	heph_float& Matrix::At(size_t row, size_t col) const
	{
		MATRIX_CHECK_INDICES(row, col, this->rowCount, this->colCount, "Matrix::At");
		return this->elements[this->colCount * row + col];
	}
	Matrix Matrix::Transpose() const
	{
		Matrix result(this->colCount, this->rowCount);
		for (size_t r = 0; r < this->rowCount; r++)
		{
			for (size_t c = 0; c < this->colCount; c++)
			{
				result.elements[c * result.colCount + r] = this->elements[this->colCount * r + c];
			}
		}
		return result;
	}
	heph_float Matrix::Determinant() const
	{
		if (this->rowCount != this->colCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::Determinant", "The matrix is not quadratic."));
		}

		if (this->rowCount == 1)
		{
			return this->elements[0];
		}
		if (this->rowCount == 2)
		{
			return this->elements[0] * this->elements[3] - this->elements[1] * this->elements[2];
		}

		heph_float result = 0;
		uint8_t negative = 0;
		for (size_t i = 0; i < this->rowCount; i++)
		{
			Matrix subMatrix(this->rowCount - 1, this->colCount - 1);
			for (size_t j = 1, k = 0; j < this->rowCount; j++, k = 0)
			{
				for (size_t l = 0; l < this->rowCount; l++)
				{
					if (l != i)
					{
						subMatrix.elements[(j - 1) * subMatrix.colCount + k] = this->elements[j * this->colCount + l];
						k++;
					}
				}
			}
			result += negative ? (-this->elements[i] * subMatrix.Determinant()) : (this->elements[i] * subMatrix.Determinant());
			negative ^= 1;
		}

		return result;
	}
	Matrix Matrix::Cofactor() const
	{
		if (this->rowCount != this->colCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::Cofactor", "The matrix is not quadratic."));
		}

		Matrix result(this->rowCount, this->colCount);
		Matrix subMatrix(this->rowCount - 1, this->colCount - 1);

		for (size_t i = 0; i < this->rowCount; i++)
		{
			for (size_t j = 0, k = 0; j < this->rowCount; j++, k = 0)
			{
				for (size_t l = 0, m = 0; l < this->rowCount; l++, m = 0)
				{
					if (l != i)
					{
						for (size_t n = 0; n < this->rowCount; n++)
						{
							if (n != j)
							{
								subMatrix.elements[k * subMatrix.colCount + m] = this->elements[l * this->colCount + n];
								m++;
							}
						}
						k++;
					}
				}
				result.elements[i * result.colCount + j] = ((i + j) % 2 == 0) ? subMatrix.Determinant() : -subMatrix.Determinant();
			}
		}

		return result;
	}
	Matrix Matrix::Inverse() const
	{
		if (this->rowCount != this->colCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Matrix::Inverse", "The matrix is not quadratic."));
		}
		const heph_float determinant = this->Determinant();
		return (determinant == 0) ? Matrix(0, 0) : (this->Cofactor().Transpose() / determinant);
	}
}
HephCommon::Matrix operator*(heph_float lhs, const HephCommon::Matrix& rhs)
{
	return rhs * lhs;
}