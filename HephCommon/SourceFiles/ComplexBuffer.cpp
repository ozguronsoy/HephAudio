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
	ComplexBuffer::ComplexBuffer(const std::initializer_list<heph_float>& rhs) : frameCount(rhs.size()), pData(nullptr)
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
	ComplexBuffer::ComplexBuffer(const FloatBuffer& rhs) : frameCount(rhs.FrameCount()), pData(nullptr)
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
		this->Empty();
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
	ComplexBuffer& ComplexBuffer::operator=(const std::initializer_list<heph_float>& rhs)
	{
		this->Empty();

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
		this->Empty();

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
		this->Empty();
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const FloatBuffer& rhs)
	{
		this->Empty();

		this->frameCount = rhs.FrameCount();

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
			this->Empty();

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
			this->Empty();

			this->frameCount = rhs.frameCount;
			this->pData = rhs.pData;

			rhs.frameCount = 0;
			rhs.pData = nullptr;
		}

		return *this;
	}
	ComplexBuffer ComplexBuffer::operator+(heph_float rhs) const
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
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
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
	ComplexBuffer ComplexBuffer::operator+(const FloatBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.FrameCount()));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.FrameCount());
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
	ComplexBuffer& ComplexBuffer::operator+=(heph_float rhs)
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
		this->Resize(Math::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs[i];
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator+=(const FloatBuffer& rhs)
	{
		this->Resize(Math::Max(this->frameCount, rhs.FrameCount()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs[i];
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator-(heph_float rhs) const
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
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
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
	ComplexBuffer ComplexBuffer::operator-(const FloatBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.FrameCount()));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.FrameCount());
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
	ComplexBuffer& ComplexBuffer::operator-=(heph_float rhs)
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
		this->Resize(Math::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs[i];
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator-=(const FloatBuffer& rhs)
	{
		this->Resize(Math::Max(this->frameCount, rhs.FrameCount()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs[i];
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator*(heph_float rhs) const
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
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator*(const FloatBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.FrameCount()));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.FrameCount());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator*=(heph_float rhs)
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
	ComplexBuffer& ComplexBuffer::operator*=(const FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.FrameCount())
		{
			for (size_t i = 0; i < rhs.FrameCount(); i++)
			{
				(*this)[i] *= rhs[i];
			}
			if (this->frameCount > rhs.FrameCount())
			{
				memset(this->pData + rhs.FrameCount(), 0, (this->frameCount - rhs.FrameCount()) * sizeof(Complex));
			}
		}
		else
		{
			this->Resize(rhs.FrameCount());
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] *= rhs[i];
			}
		}

		return *this;
	}
	ComplexBuffer ComplexBuffer::operator/(heph_float rhs) const
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
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator/(const FloatBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(Math::Max(this->frameCount, rhs.FrameCount()));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.FrameCount());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs[i];
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator/=(heph_float rhs)
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
	ComplexBuffer& ComplexBuffer::operator/=(const FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.FrameCount())
		{
			for (size_t i = 0; i < rhs.FrameCount(); i++)
			{
				(*this)[i] /= rhs[i];
			}
			if (this->frameCount > rhs.FrameCount())
			{
				memset(this->pData + rhs.FrameCount(), 0, (this->frameCount - rhs.FrameCount()) * sizeof(Complex));
			}
		}
		else
		{
			this->Resize(rhs.FrameCount());
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "ComplexBuffer::At", "Empty buffer."));
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
				this->Empty();
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
			const size_t newFrameCount = Math::Max(frameIndex + frameCount, this->frameCount);
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
				this->Empty();
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
	void ComplexBuffer::Empty()
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	ComplexBuffer ComplexBuffer::Convolve(const ComplexBuffer& h) const
	{
		return this->Convolve(h, ConvolutionMode::Full);
	}
	ComplexBuffer ComplexBuffer::Convolve(const ComplexBuffer& h, ConvolutionMode convolutionMode) const
	{
		if (convolutionMode == ConvolutionMode::ValidPadding && h.frameCount > this->frameCount)
		{
			return FloatBuffer();
		}

		if (this->frameCount == 0 && h.frameCount == 0)
		{
			return FloatBuffer();
		}

		size_t yFrameCount = 0;
		size_t iStart = 0;
		size_t iEnd = 0;

		switch (convolutionMode)
		{
		case ConvolutionMode::Central:
			iStart = h.frameCount / 2;
			yFrameCount = this->frameCount;
			iEnd = yFrameCount + iStart;
			break;
		case ConvolutionMode::ValidPadding:
			iStart = h.frameCount - 1;
			yFrameCount = this->frameCount - h.frameCount + 1;
			iEnd = yFrameCount + iStart;
			break;
		case ConvolutionMode::Full:
		default:
			iStart = 0;
			yFrameCount = this->frameCount + h.frameCount - 1;
			iEnd = yFrameCount;
			break;
		}

		ComplexBuffer y(yFrameCount);
		for (size_t i = iStart; i < iEnd; i++)
		{
			for (int j = (i < this->frameCount ? i : (this->frameCount - 1)); j >= 0 && (i - j) < h.frameCount; j--)
			{
				y.pData[i - iStart] += this->pData[j] * h.pData[i - j];
			}
		}
		return y;
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
HephCommon::ComplexBuffer operator+(heph_float lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator+(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator-(heph_float lhs, const HephCommon::ComplexBuffer& rhs)
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
HephCommon::ComplexBuffer operator-(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] - rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator*(heph_float lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator*(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator/(heph_float lhs, const HephCommon::ComplexBuffer& rhs)
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
HephCommon::ComplexBuffer operator/(const HephCommon::FloatBuffer& lhs, const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] / rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator+(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] + rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator+(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::ComplexBuffer operator-(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] - rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator-(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs - rhs[i];
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator*(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] * rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator*(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::ComplexBuffer operator/(const HephCommon::FloatBuffer& lhs, const HephCommon::Complex& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(lhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs[i] / rhs;
	}
	return resultBuffer;
}
HephCommon::ComplexBuffer operator/(const HephCommon::Complex& lhs, const HephCommon::FloatBuffer& rhs)
{
	HephCommon::ComplexBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs / rhs[i];
	}
	return resultBuffer;
}