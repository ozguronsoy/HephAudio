#include "Matrix.h"
#include "HephException.h"
#include "HephMath.h"
#include "Simd.h"
#include <cstdlib>
#include <memory>
#include <thread>

#define SIMD_REGISTER_SIZE_BIT (256)
#define SIMD_REGISTER_SIZE_BYTE (SIMD_REGISTER_SIZE_BIT / 8)
#define SIMD_VECTOR_SIZE (SIMD_REGISTER_SIZE_BYTE / sizeof(heph_matrix_element_t))

#define CALC_ALIGNED_SIZE(s) ((((s) % SIMD_REGISTER_SIZE_BYTE) == 0) ? (s) : ((s) + (SIMD_REGISTER_SIZE_BYTE - ((s) % SIMD_REGISTER_SIZE_BYTE))))
#define CALC_PADDED_SIZE(s, b) (((s) + (b-1)) / (b) * (b))

#if defined(_MSVC_LANG) || defined(__INTEL_COMPILER)
#define aligned_malloc(size, alignment) _aligned_malloc(size, alignment)
#define aligned_free _aligned_free
#else
#define aligned_malloc(size, alignment) aligned_alloc(alignment, size)
#define aligned_free free
#endif

namespace HephCommon
{
	Matrix::Matrix() : Matrix(0, 0) {}
	Matrix::Matrix(size_t row, size_t col) : row(row), col(col), pData(nullptr)
	{
		if (this->row != 0 && this->col != 0)
		{
			size_t matrixSize = this->Size();
			matrixSize = CALC_ALIGNED_SIZE(matrixSize);

			this->pData = (heph_matrix_element_t*)aligned_malloc(matrixSize, SIMD_REGISTER_SIZE_BYTE);
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "Matrix::Matrix", "Insufficient memory."));
			}
			(void)memset(this->pData, 0, matrixSize);
		}
	}
	Matrix::Matrix(const Matrix& rhs) : row(rhs.row), col(rhs.col), pData(nullptr)
	{
		if (!rhs.IsEmpty())
		{
			const size_t matrixSize = rhs.Size();
			this->pData = (heph_matrix_element_t*)aligned_malloc(CALC_ALIGNED_SIZE(matrixSize), SIMD_REGISTER_SIZE_BYTE);
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "Matrix::Matrix", "Insufficient memory."));
			}
			(void)memcpy(this->pData, rhs.pData, matrixSize);
		}
	}
	Matrix::Matrix(const std::initializer_list<heph_matrix_element_t>& rhs) : row(1), col(rhs.size()), pData(nullptr)
	{
		if (this->col != 0)
		{
			size_t matrixSize = this->Size();
			matrixSize = CALC_ALIGNED_SIZE(matrixSize);

			this->pData = (heph_matrix_element_t*)aligned_malloc(matrixSize, SIMD_REGISTER_SIZE_BYTE);
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "Matrix::Matrix", "Insufficient memory."));
			}
			(void)memcpy(this->pData, rhs.begin(), matrixSize);
		}
	}
	Matrix::Matrix(const std::initializer_list<std::initializer_list<heph_matrix_element_t>>& rhs) : row(rhs.size()), col(0), pData(nullptr)
	{
		if (this->row != 0)
		{
			this->CopyInitializerList(rhs);
		}
	}
	Matrix::Matrix(Matrix&& rhs) noexcept : row(rhs.row), col(rhs.col), pData(rhs.pData)
	{
		rhs.pData = nullptr;
	}
	Matrix::~Matrix() noexcept
	{
		this->Release();
	}
	heph_matrix_element_t* Matrix::operator[](size_t r) const
	{
		return this->pData + r * this->col;
	}
	Matrix Matrix::operator-() const
	{
		const size_t elementCount = this->ElementCount();
		Matrix result(this->row, this->col);
		for (size_t i = 0; i < elementCount; ++i)
		{
			result.Set(-(this->Get(i)), i);
		}
		return result;
	}
	Matrix& Matrix::operator=(const Matrix& rhs)
	{
		if (this != &rhs)
		{
			this->Release();

			if (!rhs.IsEmpty())
			{
				size_t matrixSize = rhs.Size();
				matrixSize = CALC_ALIGNED_SIZE(matrixSize);

				this->pData = (heph_matrix_element_t*)aligned_malloc(matrixSize, SIMD_REGISTER_SIZE_BYTE);
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "Matrix::operator=", "Insufficient memory."));
				}
				(void)memcpy(this->pData, rhs.pData, matrixSize);
			}
		}

		return *this;
	}
	Matrix& Matrix::operator=(const std::initializer_list<heph_matrix_element_t>& rhs)
	{
		this->Release();
		this->row = 1;
		this->col = rhs.size();

		if (this->col != 0)
		{
			size_t matrixSize = this->Size();
			matrixSize = CALC_ALIGNED_SIZE(matrixSize);

			this->pData = (heph_matrix_element_t*)aligned_malloc(matrixSize, SIMD_REGISTER_SIZE_BYTE);
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "Matrix::operator=", "Insufficient memory."));
			}
			(void)memcpy(this->pData, rhs.begin(), matrixSize);
		}

		return *this;
	}
	Matrix& Matrix::operator=(const std::initializer_list<std::initializer_list<heph_matrix_element_t>>& rhs)
	{
		this->Release();
		this->row = rhs.size();

		if (this->row != 0)
		{
			this->CopyInitializerList(rhs);
		}

		return *this;
	}
	Matrix& Matrix::operator=(Matrix&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release();

			this->row = rhs.row;
			this->col = rhs.col;
			this->pData = rhs.pData;

			rhs.pData = nullptr;
		}

		return *this;
	}
	Matrix Matrix::operator+(heph_matrix_element_t rhs) const
	{
		const size_t elementCount = this->Size();
		Matrix result(this->row, this->col);
		for (size_t i = 0; i < elementCount; ++i)
		{
			result.Set(this->Get(i) + rhs, i);
		}
		return result;
	}
	Matrix Matrix::operator+(const Matrix& rhs) const
	{
		if (this->row != rhs.row || this->col != rhs.col)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_OPERATION, "Matrix::operator+", "Matrices must have the same dimensions."));
		}

		const size_t elementCount = this->ElementCount();
		const size_t i_end_1 = elementCount > SIMD_VECTOR_SIZE ? (elementCount - SIMD_VECTOR_SIZE) : 0;
		size_t i;
		Matrix result(this->row, this->col);

		for (i = 0; i < i_end_1; i += SIMD_VECTOR_SIZE)
		{
			SIMD::Add256(result.pData + i, this->pData + i, rhs.pData + i);
		}

		// not enough elements left for vectorization
		for (; i < elementCount; ++i)
		{
			result.Set(this->Get(i) + rhs.Get(i), i);
		}

		return result;
	}
	Matrix& Matrix::operator+=(heph_matrix_element_t rhs)
	{
		const size_t elementCount = this->Size();
		for (size_t i = 0; i < elementCount; ++i)
		{
			this->Set(this->Get(i) + rhs, i);
		}
		return *this;
	}
	Matrix& Matrix::operator+=(const Matrix& rhs)
	{
		if (this->row != rhs.row || this->col != rhs.col)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_OPERATION, "Matrix::operator+=", "Matrices must have the same dimensions."));
		}

		const size_t elementCount = this->ElementCount();
		const size_t i_end_1 = elementCount > SIMD_VECTOR_SIZE ? (elementCount - SIMD_VECTOR_SIZE) : 0;
		size_t i;

		for (i = 0; i < i_end_1; i += SIMD_VECTOR_SIZE)
		{
			SIMD::Add256(this->pData + i, this->pData + i, rhs.pData + i);
		}

		// not enough elements left for vectorization
		for (; i < elementCount; ++i)
		{
			this->Set(this->Get(i) + rhs.Get(i), i);
		}

		return *this;
	}
	Matrix Matrix::operator-(heph_matrix_element_t rhs) const
	{
		const size_t elementCount = this->Size();
		Matrix result(this->row, this->col);
		for (size_t i = 0; i < elementCount; ++i)
		{
			result.Set(this->Get(i) - rhs, i);
		}
		return result;
	}
	Matrix Matrix::operator-(const Matrix& rhs) const
	{
		if (this->row != rhs.row || this->col != rhs.col)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_OPERATION, "Matrix::operator-", "Matrices must have the same dimensions."));
		}

		const size_t elementCount = this->ElementCount();
		const size_t i_end_1 = elementCount > SIMD_VECTOR_SIZE ? (elementCount - SIMD_VECTOR_SIZE) : 0;
		size_t i;
		Matrix result(this->row, this->col);

		for (i = 0; i < i_end_1; i += SIMD_VECTOR_SIZE)
		{
			SIMD::Sub256(result.pData + i, this->pData + i, rhs.pData + i);
		}

		// not enough elements left for vectorization
		for (; i < elementCount; ++i)
		{
			result.Set(this->Get(i) - rhs.Get(i), i);
		}

		return result;
	}
	Matrix& Matrix::operator-=(heph_matrix_element_t rhs)
	{
		const size_t elementCount = this->Size();
		for (size_t i = 0; i < elementCount; ++i)
		{
			this->Set(this->Get(i) - rhs, i);
		}
		return *this;
	}
	Matrix& Matrix::operator-=(const Matrix& rhs)
	{
		if (this->row != rhs.row || this->col != rhs.col)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_OPERATION, "Matrix::operator-=", "Matrices must have the same dimensions."));
		}

		const size_t elementCount = this->ElementCount();
		const size_t i_end_1 = elementCount > SIMD_VECTOR_SIZE ? (elementCount - SIMD_VECTOR_SIZE) : 0;
		size_t i;

		for (i = 0; i < i_end_1; i += SIMD_VECTOR_SIZE)
		{
			SIMD::Sub256(this->pData + i, this->pData + i, rhs.pData + i);
		}

		// not enough elements left for vectorization
		for (; i < elementCount; ++i)
		{
			this->Set(this->Get(i) - rhs.Get(i), i);
		}

		return *this;
	}
	Matrix Matrix::operator*(heph_matrix_element_t rhs) const
	{
		const size_t elementCount = this->Size();
		Matrix result(this->row, this->col);
		for (size_t i = 0; i < elementCount; ++i)
		{
			result.Set(this->Get(i) * rhs, i);
		}
		return result;
	}
	Matrix Matrix::operator*(const Matrix& rhs) const
	{
		return Matrix::Dot(*this, rhs, 1);
	}
	Matrix Matrix::operator*=(heph_matrix_element_t rhs)
	{
		const size_t elementCount = this->Size();
		for (size_t i = 0; i < elementCount; ++i)
		{
			this->Set(this->Get(i) * rhs, i);
		}
		return *this;
	}
	bool Matrix::operator==(const Matrix& rhs) const
	{
		return this->row == rhs.row && this->col == rhs.col && memcmp(this->pData, rhs.pData, this->Size()) == 0;
	}
	bool Matrix::operator!=(const Matrix& rhs) const
	{
		return this->row != rhs.row || this->col != rhs.col || memcmp(this->pData, rhs.pData, this->Size()) != 0;
	}
	bool Matrix::IsEmpty() const
	{
		return this->row == 0 || this->col == 0 || this->pData == nullptr;
	}
	size_t Matrix::RowCount() const
	{
		return this->row;
	}
	size_t Matrix::ColCount() const
	{
		return this->col;
	}
	size_t Matrix::ElementCount() const
	{
		return this->row * this->col;
	}
	size_t Matrix::Size() const
	{
		return this->ElementCount() * sizeof(heph_matrix_element_t);
	}
	void Matrix::Release() noexcept
	{
		if (this->pData != nullptr)
		{
			aligned_free(this->pData);
			this->pData = nullptr;
		}
		this->row = 0;
		this->col = 0;
	}
	heph_matrix_element_t Matrix::Get(size_t index) const
	{
		return *(this->pData + index);
	}
	heph_matrix_element_t Matrix::Get(size_t r, size_t c) const
	{
		return *(this->pData + r * this->col + c);
	}
	void Matrix::Set(heph_matrix_element_t val, size_t index)
	{
		(*(this->pData + index)) = val;
	}
	void Matrix::Set(heph_matrix_element_t val, size_t r, size_t c)
	{
		(*(this->pData + r * this->col + c)) = val;
	}
	void Matrix::Resize(size_t newRow, size_t newCol)
	{
		if (newRow == this->row && newCol == this->col)
		{
			return;
		}

		if (newRow == 0 || newCol == 0)
		{
			this->Release();
			this->row = newRow;
			this->col = newCol;
			return;
		}

		size_t newSize = newRow * newCol * sizeof(heph_matrix_element_t);
		newSize = CALC_ALIGNED_SIZE(newSize);

		heph_matrix_element_t* pTemp = (heph_matrix_element_t*)aligned_malloc(newSize, SIMD_REGISTER_SIZE_BYTE);
		if (pTemp == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "Matrix::Resize", "Insufficient memory."));
		}
		(void)memset(pTemp, 0, newSize);

		const size_t minRow = HEPH_MATH_MIN(this->row, newRow);
		const size_t minCol = HEPH_MATH_MIN(this->col, newCol);
		for (size_t r = 0; r < minRow; r++)
		{
			for (size_t c = 0; c < minCol; c++)
			{
				(*(pTemp + r * newCol + c)) = (*(this->pData + r * this->col + c));
			}
		}
		this->row = newRow;
		this->col = newCol;

		aligned_free(this->pData);
		this->pData = pTemp;
	}
	Matrix Matrix::Transpose() const
	{
		Matrix result(this->col, this->row);
		for (size_t r = 0; r < this->row; ++r)
		{
			for (size_t c = 0; c < this->col; ++c)
			{
				result[c][r] = (*this)[r][c];
			}
		}
		return result;
	}
	Matrix Matrix::Dot(const Matrix& lhs, const Matrix& rhs, size_t threadCount)
	{
		if (lhs.col != rhs.row)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_OPERATION, "Matrix::Dot", "Invalid dimensions."));
		}

		if (threadCount == 0)
		{
			threadCount = std::thread::hardware_concurrency();
		}

		// increases sequential reads, hence improves performance
		// also helps with the vectorization
		Matrix tempLhs = lhs;
		Matrix tempRhs = rhs.Transpose();

		// add padding
		tempLhs.Resize(CALC_PADDED_SIZE(tempLhs.row, threadCount), CALC_PADDED_SIZE(tempLhs.col, SIMD_VECTOR_SIZE));
		tempRhs.Resize(tempRhs.row, tempLhs.col);

		const size_t operationsPerThread = tempLhs.row / threadCount;
		std::vector<std::thread> threads(threadCount);

		size_t i, r;
		Matrix result(tempLhs.row, tempRhs.row);

		for (i = 0, r = 0; i < threadCount; ++i, r += operationsPerThread)
		{
			threads[i] = std::thread(Matrix::DotInternal, &result, &tempLhs, &tempRhs, r, operationsPerThread);
		}

		for (size_t i = 0; i < threadCount; ++i)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}

		result.Resize(lhs.row, rhs.col);

		return result;
	}
	Matrix Matrix::CreateIdentity(size_t n)
	{
		Matrix result(n, n);
		for (size_t r = 0; r < n; ++r)
		{
			result[r][r] = 1;
		}
		return result;
	}
	void Matrix::CopyInitializerList(const std::initializer_list<std::initializer_list<heph_matrix_element_t>>& rhs)
	{
		for (size_t r = 0; r < this->row; r++)
		{
			this->col = HEPH_MATH_MAX(this->col, rhs.begin()[r].size());
		}

		if (this->col != 0)
		{
			size_t matrixSize = this->Size();
			matrixSize = CALC_ALIGNED_SIZE(matrixSize);

			this->pData = (heph_matrix_element_t*)aligned_malloc(matrixSize, SIMD_REGISTER_SIZE_BYTE);
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "Matrix", "Insufficient memory."));
			}
			(void)memset(this->pData, 0, matrixSize);

			for (size_t r = 0; r < this->row; r++)
			{
				const std::initializer_list<heph_matrix_element_t>& rhsRow = rhs.begin()[r];
				for (size_t c = 0; c < rhsRow.size(); c++)
				{
					(*this)[r][c] = rhsRow.begin()[c];
				}
			}
		}
	}
	void Matrix::DotInternal(Matrix* pResult, const Matrix* pLhs, const Matrix* pRhs, size_t r, size_t operationCount)
	{
		heph_matrix_element_t* pResultData = pResult->pData + r * pResult->col;
		heph_matrix_element_t* pLhsData;
		heph_matrix_element_t* pRhsData;
		heph_matrix_element_t* c1_end;
		heph_matrix_element_t* c2_end;

		const size_t r1_end = r + operationCount;
		size_t r1;

		heph_matrix_element_t resultVector[SIMD_VECTOR_SIZE]{};

		for (r1 = r; r1 < r1_end; ++r1)
		{
			for (c2_end = pResultData + pRhs->row, pRhsData = pRhs->pData; pResultData < c2_end; ++pResultData) // rhs->row due to transpose
			{
				pLhsData = pLhs->pData + r1 * pLhs->col;
				for (c1_end = pLhsData + pLhs->col; pLhsData < c1_end; pLhsData += SIMD_VECTOR_SIZE, pRhsData += SIMD_VECTOR_SIZE)
				{
					SIMD::Mul256(resultVector, pLhsData, pRhsData);
					(*pResultData) += SIMD::SumElems256(resultVector);
				}
			}
		}
	}
}