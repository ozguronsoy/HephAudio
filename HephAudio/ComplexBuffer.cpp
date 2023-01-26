#include "ComplexBuffer.h"
#include "AudioException.h"

namespace HephAudio
{
	ComplexBuffer::ComplexBuffer()
	{
		this->frameCount = 0;
		this->pComplexData = nullptr;
	}
	ComplexBuffer::ComplexBuffer(size_t frameCount)
	{
		this->frameCount = frameCount;

		if (frameCount > 0)
		{
			// allocate memory and initialize it to 0.
			this->pComplexData = (Complex*)malloc(Size());
			if (this->pComplexData != nullptr)
			{
				memset(this->pComplexData, 0, this->Size());
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::ComplexBuffer", L"Insufficient memory.");
			}
		}
		else
		{
			this->pComplexData = nullptr;
		}
	}
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs)
	{
		this->frameCount = rhs.frameCount;

		if (rhs.frameCount > 0)
		{
			this->pComplexData = (Complex*)malloc(rhs.Size());
			if (this->pComplexData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::ComplexBuffer", L"Insufficient memory.");
			}
			memcpy(this->pComplexData, rhs.pComplexData, rhs.Size());
		}
		else
		{
			this->pComplexData = nullptr;
		}
	}
	ComplexBuffer::~ComplexBuffer()
	{
		this->frameCount = 0;
		if (this->pComplexData != nullptr)
		{
			free(this->pComplexData);
			this->pComplexData = nullptr;
		}
	}
	Complex& ComplexBuffer::operator[](const size_t& index) const
	{
		return *(this->pComplexData + index);
	}
	ComplexBuffer ComplexBuffer::operator-() const
	{
		ComplexBuffer resultBuffer = ComplexBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = -(*this)[i];
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator=(const ComplexBuffer& rhs)
	{
		this->~ComplexBuffer(); // destroy the current buffer to avoid memory leaks.

		this->frameCount = rhs.frameCount;

		if (rhs.frameCount > 0)
		{
			this->pComplexData = (Complex*)malloc(rhs.Size());
			if (this->pComplexData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::operator=", L"Insufficient memory.");
			}
			memcpy(this->pComplexData, rhs.pComplexData, rhs.Size());
		}
		else
		{
			this->pComplexData = nullptr;
		}

		return *this;
	}
	ComplexBuffer ComplexBuffer::operator+(const ComplexBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(this->frameCount + rhs.frameCount);

		if (this->pComplexData != nullptr && this->frameCount > 0)
		{
			memcpy(resultBuffer.pComplexData, this->pComplexData, this->Size());
		}

		if (rhs.pComplexData != nullptr && rhs.frameCount > 0)
		{
			memcpy((uint8_t*)resultBuffer.pComplexData + this->Size(), rhs.pComplexData, rhs.Size());
		}

		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator+=(const ComplexBuffer& rhs)
	{
		this->Join(rhs);
		return *this;
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
	ComplexBuffer& ComplexBuffer::operator*=(const Complex& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
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
	ComplexBuffer& ComplexBuffer::operator/=(const Complex& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator*(const double& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const double& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator/(const double& rhs) const
	{
		if (rhs == 0)
		{
			throw AudioException(E_FAIL, L"ComplexBuffer::operator/", L"Divided by zero.");
		}
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const double& rhs)
	{
		if (rhs == 0)
		{
			throw AudioException(E_FAIL, L"ComplexBuffer::operator/=", L"Divided by zero.");
		}
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	bool ComplexBuffer::operator==(const ComplexBuffer& rhs) const
	{
		return this == &rhs || (this->frameCount == rhs.frameCount && memcmp(this->pComplexData, rhs.pComplexData, this->Size()) == 0);
	}
	bool ComplexBuffer::operator!=(const ComplexBuffer& rhs) const
	{
		return this != &rhs && (this->frameCount != rhs.frameCount || memcmp(this->pComplexData, rhs.pComplexData, this->Size()) != 0);
	}
	size_t ComplexBuffer::Size() const noexcept
	{
		return this->frameCount * sizeof(Complex);
	}
	size_t ComplexBuffer::FrameCount() const noexcept
	{
		return this->frameCount;
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
			memcpy(subBuffer.pComplexData, this->pComplexData + frameIndex, subBuffer.Size());
		}
		return subBuffer;
	}
	void ComplexBuffer::Join(const ComplexBuffer& buffer)
	{
		if (buffer.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = buffer;
				return;
			}

			// reallocate memory with the combined size and copy the rhs's data to the end of the current buffer's data.
			const size_t oldSize = this->Size();
			this->frameCount += buffer.frameCount;

			Complex* tempPtr = (Complex*)realloc(this->pComplexData, this->Size());
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Join", L"Insufficient memory.");
			}

			memcpy((uint8_t*)tempPtr + oldSize, buffer.pComplexData, buffer.Size());
			this->pComplexData = tempPtr;
		}
	}
	void ComplexBuffer::Insert(const ComplexBuffer& buffer, size_t frameIndex)
	{
		if (buffer.frameCount > 0)
		{
			const size_t oldSize = this->Size();
			this->frameCount = frameIndex > this->frameCount ? (buffer.frameCount + frameIndex) : (this->frameCount + buffer.frameCount);
			const size_t newSize = this->Size();

			Complex* tempPtr = (Complex*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Insert", L"Insufficient memory.");
			}
			memset(tempPtr, 0, newSize); // if the oldSize is greater than the frameIndexAsBytes, make sure the padded data is set to 0.

			// copy from 0 to insert start index.
			const size_t frameIndexAsBytes = frameIndex * sizeof(Complex);
			if (frameIndexAsBytes > 0 && oldSize > 0)
			{
				memcpy(tempPtr, this->pComplexData, oldSize > frameIndexAsBytes ? frameIndexAsBytes : oldSize);
			}

			memcpy((uint8_t*)tempPtr + frameIndexAsBytes, buffer.pComplexData, buffer.Size()); // insert the buffer.

			// copy the remaining data.
			if (oldSize > frameIndexAsBytes)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + buffer.Size(), (uint8_t*)this->pComplexData + frameIndexAsBytes, oldSize - frameIndexAsBytes);
			}

			free(this->pComplexData);
			this->pComplexData = tempPtr;
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

			this->frameCount = this->frameCount - frameCount;
			const size_t newSize = this->Size();

			Complex* tempPtr = (Complex*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Cut", L"Insufficient memory.");
			}

			const size_t frameIndexAsBytes = frameIndex * sizeof(Complex);

			if (frameIndexAsBytes > 0) // copy from 0 to cut start index.
			{
				memcpy(tempPtr, this->pComplexData, frameIndexAsBytes);
			}

			if (newSize > frameIndexAsBytes) // copy the remaining data that we didn't cut.
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, (uint8_t*)this->pComplexData + frameIndexAsBytes + frameCount * sizeof(Complex), newSize - frameIndexAsBytes);
			}

			free(this->pComplexData);
			this->pComplexData = tempPtr;
		}
	}
	void ComplexBuffer::Replace(const ComplexBuffer& buffer, size_t frameIndex)
	{
		Replace(buffer, frameIndex, buffer.frameCount);
	}
	void ComplexBuffer::Replace(const ComplexBuffer& buffer, size_t frameIndex, size_t frameCount)
	{
		if (buffer.frameCount > 0 && frameCount > 0 && frameIndex < this->frameCount)
		{
			const size_t oldSize = this->Size();
			this->frameCount += buffer.frameCount - frameCount;
			const size_t newSize = this->Size();

			Complex* tempPtr = (Complex*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Replace", L"Insufficient memory.");
			}

			// copy from 0 to replace start index.
			const size_t frameIndexAsBytes = frameIndex * sizeof(Complex);
			if (frameIndex > 0)
			{
				memcpy(tempPtr, this->pComplexData, frameIndexAsBytes);
			}

			// copy the replace data.
			const size_t replacedSize = frameIndexAsBytes + buffer.Size() >= newSize ? newSize - frameIndexAsBytes : buffer.Size();
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, buffer.pComplexData, replacedSize);
			}

			// copy the remaining data.
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndexAsBytes + frameCount * sizeof(Complex);
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + replacedSize, (uint8_t*)this->pComplexData + padding, oldSize - padding);
			}

			free(this->pComplexData);
			this->pComplexData = tempPtr;
		}
		else
		{
			this->Insert(buffer, frameIndex);
		}
	}
	void ComplexBuffer::Reset()
	{
		memset(this->pComplexData, 0, this->Size());
	}
	void ComplexBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			Complex* tempPtr = (Complex*)realloc(this->pComplexData, newFrameCount * sizeof(Complex));
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Resize", L"Insufficient memory.");
			}
			this->pComplexData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	Complex* const& ComplexBuffer::Begin() const noexcept
	{
		return this->pComplexData;
	}
	Complex* ComplexBuffer::End() const noexcept
	{
		return this->pComplexData + this->frameCount;
	}
}