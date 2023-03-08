#include "FloatBuffer.h"
#include "AudioException.h"

namespace HephAudio
{
	FloatBuffer::FloatBuffer() : FloatBuffer(0) {}
	FloatBuffer::FloatBuffer(size_t frameCount)
	{
		this->frameCount = frameCount;
		if (this->frameCount > 0)
		{
			this->pData = (hephaudio_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "FloatBuffer::FloatBuffer", "Insufficient memory.");
			}
			memset(this->pData, 0, this->Size());
		}
		else
		{
			this->pData = nullptr;
		}
	}
	FloatBuffer::FloatBuffer(const FloatBuffer& rhs)
	{
		this->frameCount = rhs.frameCount;
		if (this->frameCount > 0)
		{
			this->pData = (hephaudio_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "FloatBuffer::FloatBuffer", "Insufficient memory.");
			}
			memcpy(this->pData, rhs.pData, this->Size());
		}
		else
		{
			this->pData = nullptr;
		}
	}
	FloatBuffer::FloatBuffer(FloatBuffer&& rhs) noexcept
	{
		this->frameCount = rhs.frameCount;
		this->pData = rhs.pData;

		rhs.frameCount = 0;
		rhs.pData = nullptr;
	}
	FloatBuffer::~FloatBuffer()
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
		}
	}
	FloatBuffer::operator AudioBuffer() const
	{
		AudioBuffer audioBuffer = AudioBuffer(this->frameCount, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, 1, sizeof(hephaudio_float) * 8, 48000));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			audioBuffer[i][0] = (*this)[i];
		}
		return audioBuffer;
	}
	hephaudio_float& FloatBuffer::operator[](const size_t& frameIndex) const
	{
		return *(this->pData + frameIndex);
	}
	FloatBuffer FloatBuffer::operator-() const
	{
		FloatBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = -(*this)[i];
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator=(const FloatBuffer& rhs)
	{
		this->~FloatBuffer();

		this->frameCount = rhs.frameCount;
		if (this->frameCount > 0)
		{
			this->pData = (hephaudio_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "FloatBuffer::FloatBuffer", "Insufficient memory.");
			}
			memcpy(this->pData, rhs.pData, this->Size());
		}

		return *this;
	}
	FloatBuffer& FloatBuffer::operator=(FloatBuffer&& rhs) noexcept
	{
		this->~FloatBuffer();

		this->frameCount = rhs.frameCount;
		this->pData = rhs.pData;

		rhs.frameCount = 0;
		rhs.pData = nullptr;

		return *this;
	}
	FloatBuffer FloatBuffer::operator*(const hephaudio_float& rhs) const
	{
		FloatBuffer resultBuffer = *this;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] *= rhs;
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator*=(const hephaudio_float& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator/(const hephaudio_float& rhs) const
	{
		FloatBuffer resultBuffer = *this;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] /= rhs;
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator/=(const hephaudio_float& rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator<<(const size_t rhs) const
	{
		FloatBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(hephaudio_float));
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator<<=(const size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(hephaudio_float));
			memset(this->pData + this->frameCount - rhs, 0, rhs * sizeof(hephaudio_float));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator>>(const size_t rhs) const
	{
		FloatBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(hephaudio_float));
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator>>=(const size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(hephaudio_float));
			memset(this->pData, 0, rhs * sizeof(hephaudio_float));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	bool FloatBuffer::operator!=(const FloatBuffer& rhs) const
	{
		return this->frameCount != rhs.frameCount || memcmp(this->pData, rhs.pData, this->Size()) != 0;
	}
	bool FloatBuffer::operator==(const FloatBuffer& rhs) const
	{
		return this->frameCount == rhs.frameCount && memcmp(this->pData, rhs.pData, this->Size()) == 0;
	}
	size_t FloatBuffer::Size() const noexcept
	{
		return this->frameCount * sizeof(hephaudio_float);
	}
	const size_t& FloatBuffer::FrameCount() const noexcept
	{
		return this->frameCount;
	}
	FloatBuffer FloatBuffer::GetSubBuffer(size_t frameIndex, size_t frameCount) const
	{
		FloatBuffer subBuffer(frameCount);
		if (frameIndex < this->frameCount && frameCount > 0)
		{
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			memcpy(subBuffer.pData, this->pData + frameIndex, frameCount * sizeof(hephaudio_float));
		}
		return subBuffer;
	}
	void FloatBuffer::Join(const FloatBuffer& rhs)
	{
		if (rhs.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = rhs;
				return;
			}

			// allocate memory with the combined size and copy the rhs's data to the end of the current buffer's data.
			hephaudio_float* tempPtr = (hephaudio_float*)malloc(this->Size() + rhs.Size());
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "FloatBuffer::Join", "Insufficient memory.");
			}

			memcpy(tempPtr, this->pData, this->Size());
			memcpy(tempPtr + this->frameCount, rhs.pData, rhs.Size());

			free(this->pData);
			this->pData = tempPtr;
			this->frameCount += rhs.frameCount;
		}
	}
	void FloatBuffer::Insert(const FloatBuffer& rhs, size_t frameIndex)
	{
		if (rhs.frameCount > 0)
		{
			const size_t oldSize = this->Size();
			const size_t newFrameCount = frameIndex > this->frameCount ? (rhs.frameCount + frameIndex) : (this->frameCount + rhs.frameCount);
			const size_t newSize = newFrameCount * sizeof(hephaudio_float);

			hephaudio_float* tempPtr = (hephaudio_float*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "FloatBuffer::Insert", "Insufficient memory.");
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to insert start index.
			const size_t frameIndexAsBytes = frameIndex * sizeof(hephaudio_float);
			if (frameIndexAsBytes > 0 && oldSize > 0)
			{
				memcpy(tempPtr, this->pData, oldSize > frameIndexAsBytes ? frameIndexAsBytes : oldSize);
			}

			memcpy(tempPtr + frameIndex, rhs.pData, rhs.Size()); // insert the buffer.

			// copy the remaining data.
			if (oldSize > frameIndexAsBytes)
			{
				memcpy(tempPtr + frameIndex + rhs.frameCount, this->pData + frameIndex, oldSize - frameIndexAsBytes);
			}

			free(this->pData);
			this->pData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void FloatBuffer::Cut(size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0 && frameIndex < this->frameCount)
		{

			if (frameIndex + frameCount > this->frameCount) // to prevent overcutting.
			{
				frameCount = this->frameCount - frameIndex;
			}

			this->frameCount = this->frameCount - frameCount;
			const size_t newSize = this->Size();

			hephaudio_float* tempPtr = (hephaudio_float*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "FloatBuffer::Cut", "Insufficient memory.");
			}

			const size_t frameIndexAsBytes = frameIndex * sizeof(hephaudio_float);

			if (frameIndexAsBytes > 0) // copy from 0 to cut start index.
			{
				memcpy(tempPtr, this->pData, frameIndexAsBytes);
			}

			if (newSize > frameIndexAsBytes) // copy the remaining data that we didn't cut.
			{
				memcpy(tempPtr + frameIndex, this->pData + frameIndex + frameCount, newSize - frameIndexAsBytes);
			}

			free(this->pData);
			this->pData = tempPtr;
		}
	}
	void FloatBuffer::Replace(const FloatBuffer& rhs, size_t frameIndex)
	{
		this->Replace(rhs, frameIndex, rhs.frameCount);
	}
	void FloatBuffer::Replace(const FloatBuffer& rhs, size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0)
		{
			const size_t newFrameCount = frameIndex + frameCount > this->frameCount ? (frameCount + frameIndex) : this->frameCount;
			const size_t newSize = newFrameCount * sizeof(hephaudio_float);

			hephaudio_float* tempPtr = (hephaudio_float*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "AudioBuffer::Replace", "Insufficient memory.");
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to replace start index.
			const size_t frameIndexAsBytes = frameIndex * sizeof(hephaudio_float);
			if (frameIndex > 0)
			{
				memcpy(tempPtr, this->pData, frameIndexAsBytes > this->Size() ? this->Size() : frameIndexAsBytes);
			}

			// copy the replace data.
			const size_t replacedSize = frameIndexAsBytes + frameCount * sizeof(hephaudio_float) >= newSize ? newSize - frameIndexAsBytes : frameCount * sizeof(hephaudio_float);
			if (replacedSize > 0)
			{
				memcpy(tempPtr + frameIndex, rhs.pData, replacedSize);
			}

			// copy the remaining data.
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndexAsBytes + frameCount * sizeof(hephaudio_float);
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + replacedSize, (uint8_t*)this->pData + padding, this->Size() - padding);
			}

			free(this->pData);
			this->pData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void FloatBuffer::Reset()
	{
		memset(this->pData, 0, this->Size());
	}
	void FloatBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			if (newFrameCount == 0)
			{
				this->frameCount = 0;
				if (this->pData != nullptr)
				{
					free(this->pData);
					this->pData = nullptr;
				}
			}
			else
			{
				hephaudio_float* tempPtr = (hephaudio_float*)realloc(this->pData, newFrameCount * sizeof(hephaudio_float));
				if (tempPtr == nullptr)
				{
					throw AudioException(E_OUTOFMEMORY, "FloatBuffer::Resize", "Insufficient memory.");
				}
				this->pData = tempPtr;
				this->frameCount = newFrameCount;
			}
		}
	}
	hephaudio_float FloatBuffer::Min() const noexcept
	{
		hephaudio_float minSample = INT32_MAX;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			const hephaudio_float& currentSample = (*this)[i];
			if (currentSample < minSample)
			{
				minSample = currentSample;
			}
		}
		return minSample;
	}
	hephaudio_float FloatBuffer::Max() const noexcept
	{
		hephaudio_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			const hephaudio_float& currentSample = (*this)[i];
			if (currentSample > maxSample)
			{
				maxSample = currentSample;
			}
		}
		return maxSample;
	}
	hephaudio_float FloatBuffer::AbsMax() const noexcept
	{
		hephaudio_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			const hephaudio_float& currentSample = abs((*this)[i]);
			if (currentSample > maxSample)
			{
				maxSample = currentSample;
			}
		}
		return maxSample;
	}
	hephaudio_float* const& FloatBuffer::Begin() const noexcept
	{
		return this->pData;
	}
	hephaudio_float* FloatBuffer::End() const noexcept
	{
		return this->pData + this->Size();
	}
}