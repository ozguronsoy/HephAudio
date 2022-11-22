#include "ComplexBuffer.h"
#include "AudioException.h"
#include <algorithm>

namespace HephAudio
{
	ComplexBuffer::ComplexBuffer()
	{
		frameCount = 0;
		pComplexData = nullptr;
	}
	ComplexBuffer::ComplexBuffer(size_t frameCount)
	{
		this->frameCount = frameCount;
		pComplexData = (Complex*)malloc(Size());
		if (pComplexData != nullptr)
		{
			memset(pComplexData, 0, Size());
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::ComplexBuffer", L"Insufficient memory.");
		}
	}
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs)
	{
		(*this) = rhs;
	}
	ComplexBuffer::~ComplexBuffer()
	{
		if (pComplexData != nullptr)
		{
			free(pComplexData);
			pComplexData = nullptr;
		}
	}
	Complex& ComplexBuffer::operator[](size_t index) const
	{
		return *(pComplexData + index);
	}
	ComplexBuffer ComplexBuffer::operator-() const
	{
		ComplexBuffer resultBuffer = ComplexBuffer(frameCount);
		std::transform((Complex*)pComplexData, (Complex*)((uint8_t*)pComplexData + Size()), (Complex*)resultBuffer.pComplexData, [](Complex& sample) { return -sample; });
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator=(const ComplexBuffer& rhs)
	{
		if (rhs.pComplexData != nullptr)
		{
			this->frameCount = rhs.frameCount;
			if (this->pComplexData != nullptr)
			{
				free(this->pComplexData);
			}
			this->pComplexData = (Complex*)malloc(rhs.Size());
			if (this->pComplexData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::operator=", L"Insufficient memory.");
			}
			memcpy(this->pComplexData, rhs.pComplexData, rhs.Size());
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator+(const ComplexBuffer& rhs) const
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->pComplexData != nullptr && this->frameCount > 0)
		{
			memcpy(resultBuffer.pComplexData, this->pComplexData, this->Size());
		}
		if (rhs.pComplexData != nullptr && rhs.frameCount > 0)
		{
			resultBuffer.Join(rhs);
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
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const Complex& rhs)
	{
		for (size_t i = 0; i < frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator/(const Complex& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const Complex& rhs)
	{
		for (size_t i = 0; i < frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator*(const double& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const double& rhs)
	{
		for (size_t i = 0; i < frameCount; i++)
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
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
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
		for (size_t i = 0; i < frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	bool ComplexBuffer::operator==(ComplexBuffer& rhs) const
	{
		return this == &rhs || (this->frameCount == rhs.frameCount && memcmp(this->pComplexData, rhs.pComplexData, this->Size()) == 0);
	}
	bool ComplexBuffer::operator!=(ComplexBuffer& rhs) const
	{
		return this != &rhs && (this->frameCount != rhs.frameCount || memcmp(this->pComplexData, rhs.pComplexData, this->Size()) != 0);
	}
	size_t ComplexBuffer::Size() const noexcept
	{
		return frameCount * sizeof(Complex);
	}
	size_t ComplexBuffer::FrameCount() const noexcept
	{
		return frameCount;
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
			memcpy(subBuffer.pComplexData, pComplexData + frameIndex, frameCount * sizeof(Complex));
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
			const size_t newFrameCount = this->frameCount + buffer.frameCount;
			Complex* tempPtr = (Complex*)malloc(newFrameCount * sizeof(Complex));
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Join", L"Insufficient memory.");
			}
			const size_t oldSize = this->Size();
			if (oldSize > 0)
			{
				memcpy(tempPtr, this->pComplexData, oldSize);
			}
			memcpy((uint8_t*)tempPtr + oldSize, buffer.pComplexData, buffer.Size());
			free(pComplexData);
			pComplexData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void ComplexBuffer::Insert(const ComplexBuffer& buffer, size_t frameIndex)
	{
		if (buffer.frameCount > 0)
		{
			const size_t newFrameCount = frameIndex > this->frameCount ? buffer.frameCount + frameIndex : this->frameCount + buffer.frameCount;
			const size_t newSize = newFrameCount * sizeof(Complex);
			Complex* tempPtr = (Complex*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Insert", L"Insufficient memory.");
			}
			memset(tempPtr, 0, newSize);
			size_t cursor = 0;
			if (frameIndex > 0 && this->frameCount > 0)
			{
				cursor = frameIndex * sizeof(Complex);
				memcpy(tempPtr, this->pComplexData, frameIndex > this->frameCount ? this->Size() : cursor);
			}
			memcpy((uint8_t*)tempPtr + cursor, buffer.pComplexData, buffer.Size());
			if (frameIndex < this->frameCount)
			{
				memcpy((uint8_t*)tempPtr + cursor + buffer.Size(), (uint8_t*)this->pComplexData + cursor, this->Size() - cursor);
			}
			free(pComplexData);
			pComplexData = tempPtr;
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
			const size_t newFrameCount = this->frameCount - frameCount;
			const size_t newSize = newFrameCount * sizeof(Complex);
			Complex* tempPtr = (Complex*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Cut", L"Insufficient memory.");
			}
			const size_t frameIndexAsBytes = frameIndex * sizeof(Complex);
			if (frameIndexAsBytes > 0)
			{
				memcpy(tempPtr, pComplexData, frameIndexAsBytes);
			}
			if (newSize > frameIndexAsBytes)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, (uint8_t*)pComplexData + frameIndexAsBytes + frameCount * sizeof(Complex), newSize - frameIndexAsBytes);
			}
			free(pComplexData);
			pComplexData = tempPtr;
			this->frameCount = newFrameCount;
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
			const size_t newFrameCount = this->frameCount + buffer.frameCount - frameCount;
			const size_t newSize = newFrameCount * sizeof(Complex);
			Complex* tempPtr = (Complex*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Replace", L"Insufficient memory.");
			}
			size_t cursor = 0;
			if (frameIndex > 0)
			{
				cursor = frameIndex * sizeof(Complex);
				memcpy(tempPtr, this->pComplexData, cursor);
			}
			const size_t replacedSize = cursor + buffer.Size() >= newSize ? newSize - cursor : buffer.Size();
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)tempPtr + cursor, buffer.pComplexData, replacedSize);
			}
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = cursor + frameCount * sizeof(Complex);
				memcpy((uint8_t*)tempPtr + cursor + replacedSize, (uint8_t*)this->pComplexData + padding, this->Size() - padding);
			}
			free(pComplexData);
			pComplexData = tempPtr;
			this->frameCount = newFrameCount;
		}
		else
		{
			this->Insert(buffer, frameIndex);
		}
	}
	void ComplexBuffer::Reset()
	{
		memset(pComplexData, 0, Size());
	}
	void ComplexBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			Complex* tempPtr = (Complex*)realloc(pComplexData, newFrameCount * sizeof(Complex));
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"ComplexBuffer::Resize", L"Insufficient memory.");
			}
			pComplexData = tempPtr;
			frameCount = newFrameCount;
		}
	}
	Complex* ComplexBuffer::GetComplexDataAddress() const noexcept
	{
		return pComplexData;
	}
}