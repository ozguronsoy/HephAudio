#include "ComplexBuffer.h"
#include "HephException.h"
#include "HephMath.h"
#include <memory>

namespace HephCommon
{
	ComplexBuffer::ComplexBuffer() : frameCount(0), pComplexData(nullptr)
	{
		this->frameCount = 0;
		this->pComplexData = nullptr;
	}
	ComplexBuffer::ComplexBuffer(size_t frameCount) : frameCount(frameCount), pComplexData(nullptr)
	{
		if (frameCount > 0)
		{
			// allocate memory and initialize it to 0.
			this->pComplexData = (Complex*)malloc(this->Size());
			if (this->pComplexData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			this->Reset();
		}
	}
	ComplexBuffer::ComplexBuffer(const std::initializer_list<Complex>& rhs) : frameCount(rhs.size()), pComplexData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pComplexData = (Complex*)malloc(this->Size());
			if (this->pComplexData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			memcpy(this->pComplexData, rhs.begin(), this->Size());
		}
	}
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs) : frameCount(rhs.frameCount), pComplexData(nullptr)
	{
		if (rhs.frameCount > 0)
		{
			this->pComplexData = (Complex*)malloc(rhs.Size());
			if (this->pComplexData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::ComplexBuffer", "Insufficient memory."));
			}
			memcpy(this->pComplexData, rhs.pComplexData, rhs.Size());
		}
	}
	ComplexBuffer::ComplexBuffer(ComplexBuffer&& rhs) noexcept : frameCount(rhs.frameCount), pComplexData(rhs.pComplexData)
	{
		rhs.frameCount = 0;
		rhs.pComplexData = nullptr;
	}
	ComplexBuffer::~ComplexBuffer()
	{
		this->Empty();
	}
	Complex& ComplexBuffer::operator[](const size_t& frameIndex) const noexcept
	{
		return *(this->pComplexData + frameIndex);
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
	ComplexBuffer& ComplexBuffer::operator=(const std::initializer_list<Complex>& rhs)
	{
		this->Empty();

		this->frameCount = rhs.size();
		if (this->frameCount > 0)
		{
			this->pComplexData = (Complex*)malloc(this->Size());
			if (this->pComplexData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::operator=", "Insufficient memory."));
			}
			memcpy(this->pComplexData, rhs.begin(), this->Size());
		}

		return *this;
	}
	ComplexBuffer& ComplexBuffer::operator=(const ComplexBuffer& rhs)
	{
		if (this->pComplexData != rhs.pComplexData)
		{
			this->Empty(); // destroy the current buffer to avoid memory leaks.

			this->frameCount = rhs.frameCount;

			if (rhs.frameCount > 0)
			{
				this->pComplexData = (Complex*)malloc(rhs.Size());
				if (this->pComplexData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::operator=", "Insufficient memory."));
				}
				memcpy(this->pComplexData, rhs.pComplexData, rhs.Size());
			}
			else
			{
				this->pComplexData = nullptr;
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
			this->pComplexData = rhs.pComplexData;

			rhs.frameCount = 0;
			rhs.pComplexData = nullptr;
		}

		return *this;
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
	ComplexBuffer& ComplexBuffer::operator*=(const Complex& rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
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
	ComplexBuffer& ComplexBuffer::operator/=(const Complex& rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator*(const heph_float& rhs) const noexcept
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const heph_float& rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator/(const heph_float& rhs) const noexcept
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const heph_float& rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator<<(const size_t& rhs) const noexcept
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pComplexData, this->pComplexData + rhs, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator<<=(const size_t& rhs) noexcept
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pComplexData, this->pComplexData + rhs, (this->frameCount - rhs) * sizeof(Complex));
			memset(this->pComplexData + this->frameCount - rhs, 0, rhs * sizeof(Complex));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator>>(const size_t& rhs) const noexcept
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pComplexData + rhs, this->pComplexData, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator>>=(const size_t& rhs) noexcept
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pComplexData + rhs, this->pComplexData, (this->frameCount - rhs) * sizeof(Complex));
			memset(this->pComplexData, 0, rhs * sizeof(Complex));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	bool ComplexBuffer::operator==(const ComplexBuffer& rhs) const noexcept
	{
		return this->pComplexData == rhs.pComplexData || (this->frameCount == rhs.frameCount && this->pComplexData != nullptr && rhs.pComplexData != nullptr && memcmp(this->pComplexData, rhs.pComplexData, this->Size()) == 0);
	}
	bool ComplexBuffer::operator!=(const ComplexBuffer& rhs) const noexcept
	{
		return this->pComplexData != rhs.pComplexData && (this->frameCount != rhs.frameCount || this->pComplexData == nullptr || rhs.pComplexData == nullptr || memcmp(this->pComplexData, rhs.pComplexData, this->Size()) != 0);
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
		if (this->pComplexData == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "ComplexBuffer::At", "Empty buffer."));
		}
		if (frameIndex >= this->frameCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "ComplexBuffer::At", "Index out of bounds."));
		}
		return *(this->pComplexData + frameIndex);
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
			memcpy(subBuffer.pComplexData, this->pComplexData + frameIndex, frameCount * sizeof(Complex));
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

			memcpy(pTemp, this->pComplexData, this->Size());

			memcpy((uint8_t*)pTemp + this->Size(), buffer.pComplexData, buffer.Size());

			free(this->pComplexData);
			this->pComplexData = (Complex*)pTemp;
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
				memcpy(pTemp, this->pComplexData, oldSize > frameIndex_byte ? frameIndex_byte : oldSize);
			}

			memcpy((uint8_t*)pTemp + frameIndex_byte, buffer.pComplexData, buffer.Size()); // insert the buffer.

			// copy the remaining data.
			if (oldSize > frameIndex_byte)
			{
				memcpy((uint8_t*)pTemp + frameIndex_byte + buffer.Size(), (uint8_t*)this->pComplexData + frameIndex_byte, oldSize - frameIndex_byte);
			}

			free(this->pComplexData);
			this->pComplexData = (Complex*)pTemp;
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
				memcpy(pTemp, this->pComplexData, frameIndex_byte);
			}

			if (newSize > frameIndex_byte) // copy the remaining data that we didn't cut.
			{
				memcpy((uint8_t*)pTemp + frameIndex_byte, (uint8_t*)this->pComplexData + frameIndex_byte + frameCount * sizeof(Complex), newSize - frameIndex_byte);
			}

			free(this->pComplexData);
			this->pComplexData = pTemp;
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
				memcpy(pTemp, this->pComplexData, frameIndex_byte > this->Size() ? this->Size() : frameIndex_byte);
			}

			// ensure both buffers have the same format.
			ComplexBuffer tempBuffer = buffer.GetSubBuffer(0, frameCount);
			const size_t tempBufferSize = tempBuffer.Size();

			// copy the replace data.
			const size_t replacedSize = frameIndex_byte + tempBufferSize >= newSize ? newSize - frameIndex_byte : tempBufferSize;
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)pTemp + frameIndex_byte, buffer.pComplexData, replacedSize);
			}

			// copy the remaining data.
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndex_byte + frameCount * sizeof(Complex);
				memcpy((uint8_t*)pTemp + frameIndex_byte + replacedSize, (uint8_t*)this->pComplexData + padding, this->Size() - padding);
			}

			free(this->pComplexData);
			this->pComplexData = (Complex*)pTemp;
			this->frameCount = newFrameCount;
		}
	}
	void ComplexBuffer::Reset() noexcept
	{
		if (this->pComplexData == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "ComplexBuffer::Reset", "Empty buffer."));
			return;
		}

		memset(this->pComplexData, 0, this->Size());
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
				Complex* pTemp = (Complex*)realloc(this->pComplexData, newFrameCount * sizeof(Complex));
				if (pTemp == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ComplexBuffer::Resize", "Insufficient memory."));
				}
				this->pComplexData = pTemp;
				this->frameCount = newFrameCount;
			}
		}
	}
	void ComplexBuffer::Empty() noexcept
	{
		this->frameCount = 0;
		if (this->pComplexData != nullptr)
		{
			free(this->pComplexData);
			this->pComplexData = nullptr;
		}
	}
	Complex* ComplexBuffer::Begin() const noexcept
	{
		return this->pComplexData;
	}
	Complex* ComplexBuffer::End() const noexcept
	{
		return this->pComplexData + this->frameCount;
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