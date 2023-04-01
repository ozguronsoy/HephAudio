#include "ComplexBuffer.h"
#include "HephException.h"
#include <memory>

namespace HephAudio
{
	ComplexBuffer::ComplexBuffer()
		: frameCount(0), pComplexData(nullptr)
	{
		this->frameCount = 0;
		this->pComplexData = nullptr;
	}
	ComplexBuffer::ComplexBuffer(size_t frameCount)
		: frameCount(frameCount)
	{
		if (frameCount > 0)
		{
			// allocate memory and initialize it to 0.
			this->pComplexData = (Complex*)malloc(this->Size());
			if (this->pComplexData == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory.");
			}
			this->Reset();
		}
		else
		{
			this->pComplexData = nullptr;
		}
	}
	ComplexBuffer::ComplexBuffer(const ComplexBuffer& rhs)
		: frameCount(rhs.frameCount)
	{
		if (rhs.frameCount > 0)
		{
			this->pComplexData = (Complex*)malloc(rhs.Size());
			if (this->pComplexData == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "ComplexBuffer::ComplexBuffer", "Insufficient memory.");
			}
			memcpy(this->pComplexData, rhs.pComplexData, rhs.Size());
		}
		else
		{
			this->pComplexData = nullptr;
		}
	}
	ComplexBuffer::ComplexBuffer(ComplexBuffer&& rhs) noexcept
		: frameCount(rhs.frameCount), pComplexData(rhs.pComplexData)
	{
		rhs.frameCount = 0;
		rhs.pComplexData = nullptr;
	}
	ComplexBuffer::~ComplexBuffer()
	{
		this->Empty();
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
		if (this->pComplexData != rhs.pComplexData)
		{
			this->~ComplexBuffer(); // destroy the current buffer to avoid memory leaks.

			this->frameCount = rhs.frameCount;

			if (rhs.frameCount > 0)
			{
				this->pComplexData = (Complex*)malloc(rhs.Size());
				if (this->pComplexData == nullptr)
				{
					throw HephCommon::HephException(E_OUTOFMEMORY, "ComplexBuffer::operator=", "Insufficient memory.");
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
			this->~ComplexBuffer();

			this->frameCount = rhs.frameCount;
			this->pComplexData = rhs.pComplexData;

			rhs.frameCount = 0;
			rhs.pComplexData = nullptr;
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
	ComplexBuffer ComplexBuffer::operator*(const hephaudio_float& rhs) const
	{
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator*=(const hephaudio_float& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator/(const hephaudio_float& rhs) const
	{
		if (rhs == 0)
		{
			throw HephCommon::HephException(E_FAIL, "ComplexBuffer::operator/", "Divided by zero.");
		}
		ComplexBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs;
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator/=(const hephaudio_float& rhs)
	{
		if (rhs == 0)
		{
			throw HephCommon::HephException(E_FAIL, "ComplexBuffer::operator/=", "Divided by zero.");
		}
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	ComplexBuffer ComplexBuffer::operator<<(const size_t& rhs) const
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pComplexData, this->pComplexData + rhs, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator<<=(const size_t& rhs)
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
	ComplexBuffer ComplexBuffer::operator>>(const size_t& rhs) const
	{
		ComplexBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pComplexData + rhs, this->pComplexData, (this->frameCount - rhs) * sizeof(Complex));
		}
		return resultBuffer;
	}
	ComplexBuffer& ComplexBuffer::operator>>=(const size_t& rhs)
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
			memcpy(subBuffer.pComplexData, this->pComplexData + frameIndex, frameCount * sizeof(Complex));
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

			// allocate memory with the combined size and copy the rhs's data to the end of the current buffer's data.
			void* tempPtr = malloc(this->Size() + buffer.Size());
			if (tempPtr == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Join", "Insufficient memory.");
			}

			memcpy(tempPtr, this->pComplexData, this->Size());

			memcpy((uint8_t*)tempPtr + this->Size(), buffer.pComplexData, buffer.Size());

			free(this->pComplexData);
			this->pComplexData = (Complex*)tempPtr;
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

			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Insert", "Insufficient memory.");
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

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
			this->pComplexData = (Complex*)tempPtr;
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

			this->frameCount = this->frameCount - frameCount;
			const size_t newSize = this->Size();

			Complex* tempPtr = (Complex*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "ComplexBuffer::Cut", "Insufficient memory.");
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
		if (frameCount > 0)
		{
			const size_t newFrameCount = frameIndex > this->frameCount ? (frameCount + frameIndex) : (this->frameCount + frameCount);
			const size_t newSize = newFrameCount * sizeof(Complex);

			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Replace", "Insufficient memory.");
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to replace start index.
			const size_t frameIndexAsBytes = frameIndex * sizeof(Complex);
			if (frameIndex > 0)
			{
				memcpy(tempPtr, this->pComplexData, frameIndexAsBytes > this->Size() ? this->Size() : frameIndexAsBytes);
			}

			// ensure both buffers have the same format.
			ComplexBuffer tempBuffer = buffer.GetSubBuffer(0, frameCount);
			const size_t tempBufferSize = tempBuffer.Size();

			// copy the replace data.
			const size_t replacedSize = frameIndexAsBytes + tempBufferSize >= newSize ? newSize - frameIndexAsBytes : tempBufferSize;
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, buffer.pComplexData, replacedSize);
			}

			// copy the remaining data.
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndexAsBytes + frameCount * sizeof(Complex);
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + replacedSize, (uint8_t*)this->pComplexData + padding, this->Size() - padding);
			}

			free(this->pComplexData);
			this->pComplexData = (Complex*)tempPtr;
			this->frameCount = newFrameCount;
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
			if (newFrameCount == 0)
			{
				this->Empty();
			}
			else
			{
				Complex* tempPtr = (Complex*)realloc(this->pComplexData, newFrameCount * sizeof(Complex));
				if (tempPtr == nullptr)
				{
					throw HephCommon::HephException(E_OUTOFMEMORY, "ComplexBuffer::Resize", "Insufficient memory.");
				}
				this->pComplexData = tempPtr;
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
HephAudio::FloatBuffer abs(const HephAudio::ComplexBuffer& rhs)
{
	HephAudio::FloatBuffer resultBuffer = HephAudio::FloatBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = rhs[i].Magnitude();
	}
	return resultBuffer;
}
HephAudio::FloatBuffer phase(const HephAudio::ComplexBuffer& rhs)
{
	HephAudio::FloatBuffer resultBuffer = HephAudio::FloatBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = rhs[i].Phase();
	}
	return resultBuffer;
}