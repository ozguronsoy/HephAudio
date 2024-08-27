#include "ComplexBuffer.h"
#include "HephException.h"
#include "HephMath.h"
#include <memory>

namespace HephCommon
{
	ComplexBuffer::ComplexBuffer() : frameCount(0), pData(nullptr) {}
	ComplexBuffer::ComplexBuffer(size_t frameCount) : frameCount(frameCount), pData(nullptr)
	{
		if (frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			this->Reset();
		}
	}
	ComplexBuffer::ComplexBuffer(const std::initializer_list<double>& rhs) : frameCount(rhs.size()), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real(rhs.begin()[i]);
				(*this)[i].imag(0);
			}
		}
	}
	ComplexBuffer::ComplexBuffer(const std::initializer_list<Complex>& rhs) : frameCount(rhs.size()), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
		}
	}
	ComplexBuffer::ComplexBuffer(std::nullptr_t rhs) : ComplexBuffer() {}
	ComplexBuffer::ComplexBuffer(const DoubleBuffer& rhs) : frameCount(rhs.Size()), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real(rhs[i]);
				(*this)[i].imag(0);
			}
		}
	}
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs) : frameCount(rhs.frameCount), pData(nullptr)
	{
		if (rhs.frameCount > 0)
		{
			this->pData = (Complex*)malloc(rhs.Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.pData, rhs.Size());
		}
	}
	ComplexBuffer::ComplexBuffer(ComplexBuffer&& rhs) noexcept : frameCount(rhs.frameCount), pData(rhs.pData)
	{
		rhs.frameCount = 0;
		rhs.pData = nullptr;
	}
	ComplexBuffer::~ComplexBuffer()
	{
		this->Release();
	}
	Complex& ComplexBuffer::operator[](size_t frameIndex) const
	{
		return *(this->pData + frameIndex);
	}
	ComplexBuffer ComplexBuffer::operator-() const
	{
		ComplexBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = -(*this)[i];
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator=(const std::initializer_list<double>& rhs)
	{
		this->Release();

		this->frameCount = rhs.size();
		if (this->frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::operator=", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real(rhs.begin()[i]);
				(*this)[i].imag(0);
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const std::initializer_list<Complex>& rhs)
	{
		this->Release();

		this->frameCount = rhs.size();
		if (this->frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::operator=", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(std::nullptr_t rhs)
	{
		this->Release();
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const DoubleBuffer& rhs)
	{
		this->Release();

		this->frameCount = rhs.Size();

		if (this->frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::operator=", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real(rhs[i]);
				(*this)[i].imag(0);
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const ComplexBuffer& rhs)
	{
		if (this->pData != rhs.pData)
		{
			this->Release();

			this->frameCount = rhs.frameCount;

			if (rhs.frameCount > 0)
			{
				this->pData = (Complex*)malloc(rhs.Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::operator=", "Insufficient memory."));
				}
				memcpy(this->pData, rhs.pData, rhs.Size());
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(ComplexBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release();

			this->frameCount = rhs.frameCount;
			this->pData = rhs.pData;

			rhs.frameCount = 0;
			rhs.pData = nullptr;
		}

		return *this;
	}
	ComplexBuffer ComplexBuffer::operator+(double rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] += rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator+(const Complex& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] += rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator+(const ComplexBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] + rhs[i];
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i] = rhs[i];
		}

		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator+(const DoubleBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.Size()));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.Size());
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] + rhs[i];
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i].real(rhs[i]);
			resultBuffer[i].imag(0);
		}

		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator+=(double rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator+=(const Complex& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator+=(const ComplexBuffer& rhs)
	{
		this->Resize(HEPH_MATH_MAX(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs[i];
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator+=(const DoubleBuffer& rhs)
	{
		this->Resize(HEPH_MATH_MAX(this->frameCount, rhs.Size()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs[i];
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator-(double rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] -= rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator-(const Complex& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] -= rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator-(const ComplexBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] - rhs[i];
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i] = -rhs[i];
		}

		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator-(const DoubleBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.Size()));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.Size());
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] - rhs[i];
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i].real(-rhs[i]);
			resultBuffer[i].imag(0);
		}

		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator-=(double rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator-=(const Complex& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator-=(const ComplexBuffer& rhs)
	{
		this->Resize(HEPH_MATH_MAX(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs[i];
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator-=(const DoubleBuffer& rhs)
	{
		this->Resize(HEPH_MATH_MAX(this->frameCount, rhs.Size()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs[i];
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator*(double rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator*(const Complex& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator*(const ComplexBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator*(const DoubleBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.Size()));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.Size());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator*=(double rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const Complex& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const ComplexBuffer& rhs)
	{
		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				(*this)[i] *= rhs[i];
			}
			if (this->frameCount > rhs.frameCount)
			{
				memset(this->pData + rhs.frameCount, 0, (this->frameCount - rhs.frameCount) * sizeof(Complex));
			}
		}
		else
		{
			this->Resize(rhs.frameCount);
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] *= rhs[i];
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const DoubleBuffer& rhs)
	{
		if (this->frameCount >= rhs.Size())
		{
			for (size_t i = 0; i < rhs.Size(); i++)
			{
				(*this)[i] *= rhs[i];
			}
			if (this->frameCount > rhs.Size())
			{
				memset(this->pData + rhs.Size(), 0, (this->frameCount - rhs.Size()) * sizeof(Complex));
			}
		}
		else
		{
			this->Resize(rhs.Size());
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] *= rhs[i];
			}
		}

		return *this;
	}
	ComplexBuffer ComplexBuffer::operator/(double rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator/(const Complex& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator/(const ComplexBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator/(const DoubleBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(HEPH_MATH_MAX(this->frameCount, rhs.Size()));
		const size_t minFrameCount = HEPH_MATH_MIN(this->frameCount, rhs.Size());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator/=(double rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const Complex& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const ComplexBuffer& rhs)
	{
		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				(*this)[i] /= rhs[i];
			}
			if (this->frameCount > rhs.frameCount)
			{
				memset(this->pData + rhs.frameCount, 0, (this->frameCount - rhs.frameCount) * sizeof(Complex));
			}
		}
		else
		{
			this->Resize(rhs.frameCount);
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] /= rhs[i];
			}
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const DoubleBuffer& rhs)
	{
		if (this->frameCount >= rhs.Size())
		{
			for (size_t i = 0; i < rhs.Size(); i++)
			{
				(*this)[i] /= rhs[i];
			}
			if (this->frameCount > rhs.Size())
			{
				memset(this->pData + rhs.Size(), 0, (this->frameCount - rhs.Size()) * sizeof(Complex));
			}
		}
		else
		{
			this->Resize(rhs.Size());
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] /= rhs[i];
			}
		}

		return *this;
	}
	ComplexBuffer ComplexBuffer::operator<<(size_t rhs) const
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator<<=(size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(Complex));
			memset(this->pData + this->frameCount - rhs, 0, rhs * sizeof(Complex));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator>>(size_t rhs) const
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator>>=(size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(Complex));
			memset(this->pData, 0, rhs * sizeof(Complex));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	bool ComplexBuffer::operator==(std::nullptr_t rhs) const
	{
		return this->pData == rhs;
	}
	bool ComplexBuffer::operator==(const ComplexBuffer& rhs) const
	{
		return this->pData == rhs.pData || (this->frameCount == rhs.frameCount && this->pData != nullptr && rhs.pData != nullptr && memcmp(this->pData, rhs.pData, this->Size()) == 0);
	}
	bool ComplexBuffer::operator!=(std::nullptr_t rhs) const
	{
		return this->pData != rhs;
	}
	bool ComplexBuffer::operator!=(const ComplexBuffer& rhs) const
	{
		return this->pData != rhs.pData && (this->frameCount != rhs.frameCount || this->pData == nullptr || rhs.pData == nullptr || memcmp(this->pData, rhs.pData, this->Size()) != 0);
	}
	size_t ComplexBuffer::Size() const
	{
		return this->frameCount * sizeof(Complex);
	}
	size_t ComplexBuffer::FrameCount() const
	{
		return this->frameCount;
	}
	Complex& ComplexBuffer::At(size_t frameIndex) const
	{
		if (this->pData == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "ComplexBuffer::At", "Release buffer."));
		}
		if (frameIndex >= this->frameCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ComplexBuffer::At", "Index out of bounds."));
		}
		return *(this->pData + frameIndex);
	}
	ComplexBuffer ComplexBuffer::GetSubBuffer(size_t frameIndex, size_t frameCount) const
	{
		ComplexBuffer subBuffer(frameCount);
		if (frameIndex < this->frameCount && frameCount > 0)
		{
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			memcpy(subBuffer.pData, this->pData + frameIndex, frameCount * sizeof(Complex));
		}
		return subBuffer;
	}
	void ComplexBuffer::Append(const ComplexBuffer& buffer)
	{
		if (buffer.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = buffer;
				return;
			}

			void* pTemp = malloc(this->Size() + buffer.Size());
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::Append", "Insufficient memory."));
			}

			memcpy(pTemp, this->pData, this->Size());

			memcpy((uint8_t*)pTemp + this->Size(), buffer.pData, buffer.Size());

			free(this->pData);
			this->pData = (Complex*)pTemp;
			this->frameCount += buffer.frameCount;
		}
	}
	void ComplexBuffer::Insert(const ComplexBuffer& buffer, size_t frameIndex)
	{
		if (buffer.frameCount > 0)
		{
			const size_t oldSize = this->Size();
			const size_t newFrameCount = frameIndex > this->frameCount ? (buffer.frameCount + frameIndex) : (this->frameCount + buffer.frameCount);
			const size_t newSize = newFrameCount * sizeof(Complex);

			void* pTemp = malloc(newSize);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::Insert", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize);

			const size_t frameIndex_byte = frameIndex * sizeof(Complex);
			if (frameIndex_byte > 0 && oldSize > 0)
			{
				memcpy(pTemp, this->pData, oldSize > frameIndex_byte ? frameIndex_byte : oldSize);
			}

			memcpy((uint8_t*)pTemp + frameIndex_byte, buffer.pData, buffer.Size());

			if (oldSize > frameIndex_byte)
			{
				memcpy((uint8_t*)pTemp + frameIndex_byte + buffer.Size(), (uint8_t*)this->pData + frameIndex_byte, oldSize - frameIndex_byte);
			}

			free(this->pData);
			this->pData = (Complex*)pTemp;
			this->frameCount = newFrameCount;
		}
	}
	void ComplexBuffer::Cut(size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0 && frameIndex < this->frameCount)
		{

			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			if (frameCount == this->frameCount)
			{
				this->Release();
				return;
			}

			this->frameCount -= frameCount;
			const size_t newSize = this->Size();

			Complex* pTemp = (Complex*)malloc(newSize);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::Cut", "Insufficient memory."));
			}

			const size_t frameIndex_byte = frameIndex * sizeof(Complex);
			if (frameIndex_byte > 0)
			{
				memcpy(pTemp, this->pData, frameIndex_byte);
			}

			if (newSize > frameIndex_byte)
			{
				memcpy((uint8_t*)pTemp + frameIndex_byte, (uint8_t*)this->pData + frameIndex_byte + frameCount * sizeof(Complex), newSize - frameIndex_byte);
			}

			free(this->pData);
			this->pData = pTemp;
		}
	}
	void ComplexBuffer::Replace(const ComplexBuffer& buffer, size_t frameIndex)
	{
		Replace(buffer, frameIndex, buffer.frameCount);
	}
	void ComplexBuffer::Replace(const ComplexBuffer& buffer, size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0)
		{
			const size_t newFrameCount = HEPH_MATH_MAX(frameIndex + frameCount, this->frameCount);
			const size_t newSize = newFrameCount * sizeof(Complex);

			void* pTemp = malloc(newSize);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::Replace", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize);

			const size_t frameIndex_byte = frameIndex * sizeof(Complex);
			if (frameIndex > 0)
			{
				memcpy(pTemp, this->pData, frameIndex_byte > this->Size() ? this->Size() : frameIndex_byte);
			}

			ComplexBuffer tempBuffer = buffer.GetSubBuffer(0, frameCount);
			const size_t tempBufferSize = tempBuffer.Size();

			const size_t replacedSize = frameIndex_byte + tempBufferSize >= newSize ? newSize - frameIndex_byte : tempBufferSize;
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)pTemp + frameIndex_byte, buffer.pData, replacedSize);
			}

			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndex_byte + frameCount * sizeof(Complex);
				memcpy((uint8_t*)pTemp + frameIndex_byte + replacedSize, (uint8_t*)this->pData + padding, this->Size() - padding);
			}

			free(this->pData);
			this->pData = (Complex*)pTemp;
			this->frameCount = newFrameCount;
		}
	}
	void ComplexBuffer::Reset()
	{
		if (this->frameCount > 0)
		{
			memset(this->pData, 0, this->Size());
		}
	}
	void ComplexBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			if (newFrameCount == 0)
			{
				this->Release();
			}
			else
			{
				Complex* pTemp = (Complex*)realloc(this->pData, newFrameCount * sizeof(Complex));
				if (pTemp == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ComplexBuffer::Resize", "Insufficient memory."));
				}
				if (newFrameCount > this->frameCount)
				{
					memset(pTemp + this->frameCount, 0, (newFrameCount - this->frameCount) * sizeof(Complex));
				}
				this->pData = pTemp;
				this->frameCount = newFrameCount;
			}
		}
	}
	void ComplexBuffer::Release()
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	Complex* ComplexBuffer::Begin() const
	{
		return this->pData;
	}
	Complex* ComplexBuffer::End() const
	{
		return this->pData + this->frameCount;
	}
}
HephCommon::ComplexBuffer operator+(double lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator+(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator-(double lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs - rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator-(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs - rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator-(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] - rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator*(double lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator*(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator/(double lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs / rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator/(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs / rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator/(const HephCommon::DoubleBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] / rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator+(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.Size());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] + rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator-(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.Size());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] - rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator-(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.Size());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs - rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator*(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.Size());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] * rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator/(const HephCommon::DoubleBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.Size());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] / rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator/(const HephCommon::Complex& lhs, const HephCommon::DoubleBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.Size());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs / rhs[i];
	}
	return resultBuffer;
}