#include "ComplexBuffer.h"
#include "HephException.h"
#include "HephMath.h"
#include <memory>

namespace HephCommon
{
	ComplexBuffer::ComplexBuffer() : frameCount(0), pData(nullptr)
	{
		this->frameCount = 0;
		this->pData = nullptr;
	}
	ComplexBuffer::ComplexBuffer(size_t frameCount) : frameCount(frameCount), pData(nullptr)
	{
		if (frameCount > 0)
		{
			// allocate memory and initialize it to 0.
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real = rhs.begin()[i];
				(*this)[i].imaginary = 0;
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
		}
	}
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs) : frameCount(rhs.frameCount), pData(nullptr)
	{
		if (rhs.frameCount > 0)
		{
			this->pData = (Complex*)malloc(rhs.Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.pData, rhs.Size());
		}
	}
	ComplexBuffer::ComplexBuffer(const FloatBuffer& rhs) : frameCount(rhs.FrameCount()), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (Complex*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real = rhs[i];
				(*this)[i].imaginary = 0;
			}
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
	Complex& ComplexBuffer::operator[](size_t frameIndex) const noexcept
	{
		return *(this->pData + frameIndex);
	}
	ComplexBuffer ComplexBuffer::operator-() const noexcept
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::operator=", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real = rhs.begin()[i];
				(*this)[i].imaginary = 0;
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::operator=", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
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
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::operator=", "Insufficient memory."));
				}
				memcpy(this->pData, rhs.pData, rhs.Size());
			}
			else
			{
				this->pData = nullptr;
			}
		}

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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::operator=", "Insufficient memory."));
			}
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i].real = rhs[i];
				(*this)[i].imaginary = 0;
			}
		}
		else
		{
			this->pData = nullptr;
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
	ComplexBuffer ComplexBuffer::operator+(heph_float rhs) const noexcept
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] += rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator+(const Complex& rhs) const noexcept
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
			resultBuffer[i].real = rhs[i];
			resultBuffer[i].imaginary = 0;
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
	ComplexBuffer ComplexBuffer::operator-(heph_float rhs) const noexcept
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] -= rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator-(const Complex& rhs) const noexcept
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
			resultBuffer[i].real = -rhs[i];
			resultBuffer[i].imaginary = 0;
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
	ComplexBuffer ComplexBuffer::operator*(heph_float rhs) const noexcept
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator*(const Complex& rhs) const noexcept
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
	ComplexBuffer& ComplexBuffer::operator*=(heph_float rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const Complex& rhs) noexcept
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
	ComplexBuffer ComplexBuffer::operator/(heph_float rhs) const noexcept
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer ComplexBuffer::operator/(const Complex& rhs) const noexcept
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
	ComplexBuffer& ComplexBuffer::operator/=(heph_float rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const Complex& rhs) noexcept
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
	ComplexBuffer ComplexBuffer::operator<<(size_t rhs) const noexcept
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator<<=(size_t rhs) noexcept
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
	ComplexBuffer ComplexBuffer::operator>>(size_t rhs) const noexcept
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator>>=(size_t rhs) noexcept
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
	bool ComplexBuffer::operator==(std::nullptr_t rhs) const noexcept
	{
		return this->pData == rhs;
	}
	bool ComplexBuffer::operator==(const ComplexBuffer& rhs) const noexcept
	{
		return this->pData == rhs.pData || (this->frameCount == rhs.frameCount && this->pData != nullptr && rhs.pData != nullptr && memcmp(this->pData, rhs.pData, this->Size()) == 0);
	}
	bool ComplexBuffer::operator!=(std::nullptr_t rhs) const noexcept
	{
		return this->pData != rhs;
	}
	bool ComplexBuffer::operator!=(const ComplexBuffer& rhs) const noexcept
	{
		return this->pData != rhs.pData && (this->frameCount != rhs.frameCount || this->pData == nullptr || rhs.pData == nullptr || memcmp(this->pData, rhs.pData, this->Size()) != 0);
	}
	size_t ComplexBuffer::Size() const noexcept
	{
		return this->frameCount * sizeof(Complex);
	}
	size_t ComplexBuffer::FrameCount() const noexcept
	{
		return this->frameCount;
	}
	Complex& ComplexBuffer::At(size_t frameIndex) const
	{
		if (this->pData == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "ComplexBuffer::At", "Empty buffer."));
		}
		if (frameIndex >= this->frameCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "ComplexBuffer::At", "Index out of bounds."));
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

			// allocate memory with the combined size and copy the rhs's data to the end of the current buffer's data.
			void* pTemp = malloc(this->Size() + buffer.Size());
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::Append", "Insufficient memory."));
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::Insert", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to insert start index.
			const size_t frameIndex_byte = frameIndex * sizeof(Complex);
			if (frameIndex_byte > 0 && oldSize > 0)
			{
				memcpy(pTemp, this->pData, oldSize > frameIndex_byte ? frameIndex_byte : oldSize);
			}

			memcpy((uint8_t*)pTemp + frameIndex_byte, buffer.pData, buffer.Size()); // insert the buffer.

			// copy the remaining data.
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

			if (frameIndex + frameCount > this->frameCount) // to prevent overcutting.
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::Cut", "Insufficient memory."));
			}

			const size_t frameIndex_byte = frameIndex * sizeof(Complex);
			if (frameIndex_byte > 0) // copy from 0 to cut start index.
			{
				memcpy(pTemp, this->pData, frameIndex_byte);
			}

			if (newSize > frameIndex_byte) // copy the remaining data that we didn't cut.
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::Replace", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to replace start index.
			const size_t frameIndex_byte = frameIndex * sizeof(Complex);
			if (frameIndex > 0)
			{
				memcpy(pTemp, this->pData, frameIndex_byte > this->Size() ? this->Size() : frameIndex_byte);
			}

			// ensure both buffers have the same format.
			ComplexBuffer tempBuffer = buffer.GetSubBuffer(0, frameCount);
			const size_t tempBufferSize = tempBuffer.Size();

			// copy the replace data.
			const size_t replacedSize = frameIndex_byte + tempBufferSize >= newSize ? newSize - frameIndex_byte : tempBufferSize;
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)pTemp + frameIndex_byte, buffer.pData, replacedSize);
			}

			// copy the remaining data.
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
	void ComplexBuffer::Reset() noexcept
	{
		if (this->pData == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "ComplexBuffer::Reset", "Empty buffer."));
			return;
		}

		memset(this->pData, 0, this->Size());
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
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::Resize", "Insufficient memory."));
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
	void ComplexBuffer::Empty() noexcept
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	ComplexBuffer ComplexBuffer::Convolution(const ComplexBuffer& h) const
	{
		if (this->frameCount > 0 && h.frameCount > 0)
		{
			ComplexBuffer y(this->frameCount + h.frameCount - 1);
			for (size_t i = 0; i < y.frameCount; i++)
			{
				for (int j = (i < this->frameCount ? i : (this->frameCount - 1)); j >= 0 && (i - j) < h.frameCount; j--)
				{
					y.pData[i] += this->pData[j] * h.pData[i - j];
				}
			}
			return y;
		}
		return ComplexBuffer();
	}
	Complex* ComplexBuffer::Begin() const noexcept
	{
		return this->pData;
	}
	Complex* ComplexBuffer::End() const noexcept
	{
		return this->pData + this->frameCount;
	}
}
HephCommon::FloatBuffer abs(const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::FloatBuffer resultBuffer = HephCommon::FloatBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = rhs[i].Magnitude();
	}
	return resultBuffer;
}
HephCommon::FloatBuffer phase(const HephCommon::ComplexBuffer& rhs)
{
	HephCommon::FloatBuffer resultBuffer = HephCommon::FloatBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = rhs[i].Phase();
	}
	return resultBuffer;
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