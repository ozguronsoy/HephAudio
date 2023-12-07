#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "HephException.h"
#include "HephMath.h"

namespace HephAudio
{
	AudioBuffer::AudioBuffer()
		: formatInfo(AudioFormatInfo()), frameCount(0), pData(nullptr) { }
	AudioBuffer::AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo)
		: formatInfo(formatInfo), frameCount(frameCount)
	{
		if (frameCount > 0)
		{
			this->pData = malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::AudioBuffer", "Insufficient memory."));
			}
			this->Reset();
		}
		else
		{
			this->pData = nullptr;
		}
	}
	AudioBuffer::AudioBuffer(const AudioBuffer& rhs)
		: formatInfo(rhs.formatInfo), frameCount(rhs.frameCount)
	{
		if (rhs.frameCount > 0)
		{
			this->pData = malloc(rhs.Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::AudioBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.pData, rhs.Size());
		}
		else
		{
			this->pData = nullptr;
		}
	}
	AudioBuffer::AudioBuffer(AudioBuffer&& rhs) noexcept
		: formatInfo(rhs.formatInfo), frameCount(rhs.frameCount), pData(rhs.pData)
	{
		rhs.frameCount = 0;
		rhs.formatInfo = AudioFormatInfo();
		rhs.pData = nullptr;
	}
	AudioBuffer::~AudioBuffer()
	{
		this->Empty();
		this->formatInfo = AudioFormatInfo();
	}
	heph_float* AudioBuffer::operator[](size_t frameIndex) const
	{
		return (heph_float*)(((uint8_t*)this->pData) + this->formatInfo.FrameSize() * frameIndex);
	}
	AudioBuffer AudioBuffer::operator-() const
	{
		AudioBuffer resultBuffer(this->frameCount, this->formatInfo);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = -(*this)[i][j];
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator=(const AudioBuffer& rhs)
	{
		if (this->pData != rhs.pData)
		{
			this->~AudioBuffer(); // destroy the current buffer to avoid memory leaks.

			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;

			if (rhs.frameCount > 0)
			{
				this->pData = malloc(rhs.Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::operator=", "Insufficient memory."));
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
	AudioBuffer& AudioBuffer::operator=(AudioBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->~AudioBuffer();

			this->frameCount = rhs.frameCount;
			this->formatInfo = rhs.formatInfo;
			this->pData = rhs.pData;

			rhs.frameCount = 0;
			rhs.formatInfo = AudioFormatInfo();
			rhs.pData = nullptr;
		}

		return *this;
	}
	AudioBuffer AudioBuffer::operator+(heph_float rhs) const noexcept
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] += rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator+(const HephCommon::FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.FrameCount());
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] + rhs[i];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = rhs[i];
			}
		}

		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator+(const AudioBuffer& rhs) const
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator+", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] + rhs[i][j];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = rhs[i][j];
			}
		}

		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator+=(heph_float rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] += rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator+=(const HephCommon::FloatBuffer& rhs)
	{
		this->Resize(HephCommon::Math::Max(this->frameCount, rhs.FrameCount()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] += rhs[i];
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator+=(const AudioBuffer& rhs)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator+=", "Buffers must have the same format."));
		}

		this->Resize(HephCommon::Math::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] += rhs[i][j];
			}
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator-(heph_float rhs) const noexcept
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] -= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator-(const HephCommon::FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.FrameCount());
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] - rhs[i];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = -rhs[i];
			}
		}

		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator-(const AudioBuffer& rhs) const
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator-", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] - rhs[i][j];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = -rhs[i][j];
			}
		}

		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator-=(heph_float rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] -= rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator-=(const HephCommon::FloatBuffer& rhs)
	{
		this->Resize(HephCommon::Math::Max(this->frameCount, rhs.FrameCount()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] -= rhs[i];
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator-=(const AudioBuffer& rhs)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator-=", "Buffers must have the same format."));
		}

		this->Resize(HephCommon::Math::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] -= rhs[i][j];
			}
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator*(heph_float rhs) const
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] *= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator*(const HephCommon::FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.FrameCount());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] * rhs[i];
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator*(const AudioBuffer& rhs) const
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator*", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] * rhs[i][j];
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator*=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] *= rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator*=(const HephCommon::FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.FrameCount())
		{
			for (size_t i = 0; i < rhs.FrameCount(); i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] *= rhs[i];
				}
			}
			if (this->frameCount > rhs.FrameCount())
			{
				memset((uint8_t*)this->pData + rhs.FrameCount() * this->formatInfo.FrameSize(), 0, (this->frameCount - rhs.FrameCount()) * this->formatInfo.FrameSize());
			}
		}
		else
		{
			this->Resize(rhs.FrameCount());
			for (size_t i = 0; i < this->frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] *= rhs[i];
				}
			}
		}

		return *this;
	}
	AudioBuffer& AudioBuffer::operator*=(const AudioBuffer& rhs)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator*=", "Buffers must have the same format."));
		}

		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] *= rhs[i][j];
				}
			}
			if (this->frameCount > rhs.frameCount)
			{
				memset((uint8_t*)this->pData + rhs.frameCount * this->formatInfo.FrameSize(), 0, (this->frameCount - rhs.frameCount) * this->formatInfo.FrameSize());
			}
		}
		else
		{
			this->Resize(rhs.frameCount);
			for (size_t i = 0; i < this->frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] *= rhs[i][j];
				}
			}
		}

		return *this;
	}
	AudioBuffer AudioBuffer::operator/(heph_float rhs) const
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] /= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator/(const HephCommon::FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.FrameCount());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] / rhs[i];
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator/(const AudioBuffer& rhs) const
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator/", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephCommon::Math::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephCommon::Math::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] / rhs[i][j];
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator/=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				(*this)[i][j] /= rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator/=(const HephCommon::FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.FrameCount())
		{
			for (size_t i = 0; i < rhs.FrameCount(); i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] /= rhs[i];
				}
			}
			if (this->frameCount > rhs.FrameCount())
			{
				memset((uint8_t*)this->pData + rhs.FrameCount() * this->formatInfo.FrameSize(), 0, (this->frameCount - rhs.FrameCount()) * this->formatInfo.FrameSize());
			}
		}
		else
		{
			this->Resize(rhs.FrameCount());
			for (size_t i = 0; i < this->frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] /= rhs[i];
				}
			}
		}

		return *this;
	}
	AudioBuffer& AudioBuffer::operator/=(const AudioBuffer& rhs)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioBuffer::operator/=", "Buffers must have the same format."));
		}

		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] /= rhs[i][j];
				}
			}
			if (this->frameCount > rhs.frameCount)
			{
				memset((uint8_t*)this->pData + rhs.frameCount * this->formatInfo.FrameSize(), 0, (this->frameCount - rhs.frameCount) * this->formatInfo.FrameSize());
			}
		}
		else
		{
			this->Resize(rhs.frameCount);
			for (size_t i = 0; i < this->frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelCount; j++)
				{
					(*this)[i][j] /= rhs[i][j];
				}
			}
		}

		return *this;
	}
	AudioBuffer AudioBuffer::operator<<(size_t rhs) const
	{
		AudioBuffer resultBuffer(this->frameCount, this->formatInfo);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData, (uint8_t*)this->pData + rhs * this->formatInfo.FrameSize(), (this->frameCount - rhs) * this->formatInfo.FrameSize());
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator<<=(size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pData, (uint8_t*)this->pData + rhs * this->formatInfo.FrameSize(), (this->frameCount - rhs) * this->formatInfo.FrameSize());
			memset((uint8_t*)this->pData + (this->frameCount - rhs) * this->formatInfo.FrameSize(), 0, rhs * this->formatInfo.FrameSize());
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator>>(size_t rhs) const
	{
		AudioBuffer resultBuffer(this->frameCount, this->formatInfo);
		if (this->frameCount > rhs)
		{
			memcpy((uint8_t*)resultBuffer.pData + rhs * this->formatInfo.FrameSize(), this->pData, (this->frameCount - rhs) * this->formatInfo.FrameSize());
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator>>=(size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy((uint8_t*)this->pData + rhs * this->formatInfo.FrameSize(), this->pData, (this->frameCount - rhs) * this->formatInfo.FrameSize());
			memset(this->pData, 0, rhs * this->formatInfo.FrameSize());
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	bool AudioBuffer::operator==(std::nullptr_t rhs) const noexcept
	{
		return this->pData == rhs;
	}
	bool AudioBuffer::operator==(const AudioBuffer& rhs) const
	{
		return this == &rhs || (this->formatInfo == rhs.formatInfo && this->frameCount == rhs.frameCount && memcmp(this->pData, rhs.pData, this->Size()) == 0);
	}
	bool AudioBuffer::operator!=(std::nullptr_t rhs) const noexcept
	{
		return this->pData == rhs;
	}
	bool AudioBuffer::operator!=(const AudioBuffer& rhs) const
	{
		return this != &rhs && (this->formatInfo != rhs.formatInfo || this->frameCount != rhs.frameCount || memcmp(this->pData, rhs.pData, this->Size()) != 0);
	}
	size_t AudioBuffer::Size() const noexcept
	{
		return this->frameCount * this->formatInfo.FrameSize();
	}
	size_t AudioBuffer::FrameCount() const noexcept
	{
		return this->frameCount;
	}
	AudioBuffer AudioBuffer::GetSubBuffer(size_t frameIndex, size_t frameCount) const
	{
		AudioBuffer subBuffer(frameCount, this->formatInfo);
		if (frameIndex < this->frameCount && frameCount > 0)
		{
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			memcpy(subBuffer.pData, (uint8_t*)this->pData + frameIndex * this->formatInfo.FrameSize(), frameCount * this->formatInfo.FrameSize());
		}
		return subBuffer;
	}
	void AudioBuffer::Append(const AudioBuffer& buffer)
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::Join", "Insufficient memory."));
			}

			memcpy(tempPtr, this->pData, this->Size());

			// ensure both buffers have the same format.
			AudioBuffer tempBuffer = buffer;
			tempBuffer.SetFormat(this->formatInfo);

			memcpy((uint8_t*)tempPtr + this->Size(), tempBuffer.pData, tempBuffer.Size());

			free(this->pData);
			this->pData = tempPtr;
			this->frameCount += buffer.frameCount;
		}
	}
	void AudioBuffer::Insert(const AudioBuffer& buffer, size_t frameIndex)
	{
		if (buffer.frameCount > 0)
		{
			const size_t oldSize = this->Size();
			const size_t newFrameCount = frameIndex > this->frameCount ? (buffer.frameCount + frameIndex) : (this->frameCount + buffer.frameCount);
			const size_t newSize = newFrameCount * this->formatInfo.FrameSize();

			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::Insert", "Insufficient memory."));
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to insert start index.
			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();
			if (frameIndexAsBytes > 0 && oldSize > 0)
			{
				memcpy(tempPtr, this->pData, oldSize > frameIndexAsBytes ? frameIndexAsBytes : oldSize);
			}

			// ensure both buffers have the same format.
			AudioBuffer tempBuffer = buffer;
			tempBuffer.SetFormat(this->formatInfo);
			const size_t tempBufferSize = tempBuffer.Size();

			memcpy((uint8_t*)tempPtr + frameIndexAsBytes, tempBuffer.pData, tempBufferSize); // insert the buffer.

			// copy the remaining data.
			if (oldSize > frameIndexAsBytes)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + tempBufferSize, (uint8_t*)this->pData + frameIndexAsBytes, oldSize - frameIndexAsBytes);
			}

			free(this->pData);
			this->pData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void AudioBuffer::Cut(size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0 && frameIndex < this->frameCount)
		{

			if (frameIndex + frameCount > this->frameCount) // to prevent overcutting.
			{
				frameCount = this->frameCount - frameIndex;
			}

			this->frameCount = this->frameCount - frameCount;
			const size_t newSize = this->Size();

			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::Cut", "Insufficient memory."));
			}

			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();

			if (frameIndexAsBytes > 0) // copy from 0 to cut start index.
			{
				memcpy(tempPtr, this->pData, frameIndexAsBytes);
			}

			if (newSize > frameIndexAsBytes) // copy the remaining data that we didn't cut.
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, (uint8_t*)this->pData + frameIndexAsBytes + frameCount * this->formatInfo.FrameSize(), newSize - frameIndexAsBytes);
			}

			free(this->pData);
			this->pData = tempPtr;
		}
	}
	void AudioBuffer::Replace(const AudioBuffer& buffer, size_t frameIndex)
	{
		Replace(buffer, frameIndex, buffer.frameCount);
	}
	void AudioBuffer::Replace(const AudioBuffer& buffer, size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0)
		{
			const size_t newFrameCount = frameIndex > this->frameCount ? (frameCount + frameIndex) : (this->frameCount + frameCount);
			const size_t newSize = newFrameCount * this->formatInfo.FrameSize();

			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::Replace", "Insufficient memory."));
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to replace start index.
			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();
			if (frameIndex > 0)
			{
				memcpy(tempPtr, this->pData, frameIndexAsBytes > this->Size() ? this->Size() : frameIndexAsBytes);
			}

			// ensure both buffers have the same format.
			AudioBuffer tempBuffer = buffer.GetSubBuffer(0, frameCount);
			tempBuffer.SetFormat(this->formatInfo);
			const size_t tempBufferSize = tempBuffer.Size();

			// copy the replace data.
			const size_t replacedSize = frameIndexAsBytes + tempBufferSize >= newSize ? newSize - frameIndexAsBytes : tempBufferSize;
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, tempBuffer.pData, replacedSize);
			}

			// copy the remaining data.
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndexAsBytes + frameCount * this->formatInfo.FrameSize();
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + replacedSize, (uint8_t*)this->pData + padding, this->Size() - padding);
			}

			free(this->pData);
			this->pData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void AudioBuffer::Reset()
	{
		memset(this->pData, 0, this->Size());
	}
	void AudioBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			if (newFrameCount == 0)
			{
				this->Empty();
			}
			else
			{
				void* tempPtr = realloc(this->pData, newFrameCount * this->formatInfo.FrameSize());
				if (tempPtr == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AudioBuffer::Resize", "Insufficient memory."));
				}
				if (newFrameCount > this->frameCount)
				{
					memset((uint8_t*)tempPtr + this->frameCount * this->formatInfo.FrameSize(), 0, (newFrameCount - this->frameCount) * this->formatInfo.FrameSize());
				}
				this->pData = tempPtr;
				this->frameCount = newFrameCount;
			}
		}
	}
	void AudioBuffer::Empty() noexcept
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	heph_float AudioBuffer::Min() const noexcept
	{
		heph_float minSample = INT32_MAX;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				const heph_float& currentSample = (*this)[i][j];
				if (currentSample < minSample)
				{
					minSample = currentSample;
				}
			}
		}
		return minSample;
	}
	heph_float AudioBuffer::Max() const noexcept
	{
		heph_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				const heph_float& currentSample = (*this)[i][j];
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
				}
			}
		}
		return maxSample;
	}
	heph_float AudioBuffer::AbsMax() const noexcept
	{
		heph_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				heph_float currentSample = abs((*this)[i][j]);
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
				}
			}
		}
		return maxSample;
	}
	heph_float AudioBuffer::Rms() const noexcept
	{
		heph_float sumOfSamplesSquared = 0.0;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				sumOfSamplesSquared += (*this)[i][j] * (*this)[i][j];
			}
		}
		return sqrt(sumOfSamplesSquared / this->frameCount / this->formatInfo.channelCount);
	}
	AudioBuffer AudioBuffer::Convolution(const HephCommon::FloatBuffer& h) const
	{
		if (this->frameCount > 0 && h.FrameCount() > 0)
		{
			AudioBuffer y(this->frameCount + h.FrameCount() - 1, this->formatInfo);
			for (size_t ch = 0; ch < this->formatInfo.channelCount; ch++)
			{
				for (size_t i = 0; i < y.frameCount; i++)
				{
					for (int j = (i < this->frameCount ? i : (this->frameCount - 1)); j >= 0 && (i - j) < h.FrameCount(); j--)
					{
						y[i][ch] += (*this)[j][ch] * h[i - j];
					}
				}
			}
			return y;
		}
		return AudioBuffer();
	}
	AudioBuffer AudioBuffer::Convolution(const AudioBuffer& h) const
	{
		if (this->frameCount > 0 && h.FrameCount() > 0)
		{
			AudioBuffer y(this->frameCount + h.FrameCount() - 1, this->formatInfo);
			for (size_t ch = 0; ch < this->formatInfo.channelCount; ch++)
			{
				for (size_t i = 0; i < y.frameCount; i++)
				{
					for (int j = (i < this->frameCount ? i : (this->frameCount - 1)); j >= 0 && (i - j) < h.FrameCount(); j--)
					{
						y[i][ch] += (*this)[j][ch] * h[i - j][ch];
					}
				}
			}
			return y;
		}
		return AudioBuffer();
	}
	heph_float AudioBuffer::CalculateDuration() const noexcept
	{
		return CalculateDuration(this->frameCount, this->formatInfo);
	}
	size_t AudioBuffer::CalculateFrameIndex(heph_float t_s) const noexcept
	{
		return CalculateFrameIndex(t_s, this->formatInfo);
	}
	const AudioFormatInfo& AudioBuffer::FormatInfo() const noexcept
	{
		return this->formatInfo;
	}
	void AudioBuffer::SetFormat(AudioFormatInfo newFormat)
	{
		if (newFormat != this->formatInfo)
		{
			if (this->frameCount > 0)
			{
				AudioProcessor::ConvertToTargetFormat(*this, newFormat);
				AudioProcessor::ConvertSampleRate(*this, newFormat.sampleRate);
				AudioProcessor::ConvertChannels(*this, newFormat.channelCount);
			}
			this->formatInfo = newFormat;
		}
	}
	void* AudioBuffer::Begin() const noexcept
	{
		return this->pData;
	}
	void* AudioBuffer::End() const noexcept
	{
		return (uint8_t*)this->pData + this->Size();
	}
	heph_float AudioBuffer::CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.ByteRate() == 0) { return 0.0; }
		return (heph_float)frameCount * (heph_float)formatInfo.FrameSize() / (heph_float)formatInfo.ByteRate();
	}
	size_t AudioBuffer::CalculateFrameIndex(heph_float t_s, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.FrameSize() == 0) { return 0.0; }
		return t_s * (heph_float)formatInfo.ByteRate() / (heph_float)formatInfo.FrameSize();
	}
}
HephAudio::AudioBuffer abs(const HephAudio::AudioBuffer& rhs)
{
	const HephAudio::AudioFormatInfo& audioFormatInfo = rhs.FormatInfo();
	HephAudio::AudioBuffer resultBuffer = HephAudio::AudioBuffer(rhs.FrameCount(), audioFormatInfo);
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		for (size_t j = 0; j < audioFormatInfo.channelCount; j++)
		{
			resultBuffer[i][j] = abs(rhs[i][j]);
		}
	}
	return resultBuffer;
}
HephAudio::AudioBuffer operator+(heph_float lhs, const HephAudio::AudioBuffer& rhs)
{
	return rhs + lhs;
}
HephAudio::AudioBuffer operator+(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	return rhs + lhs;
}
HephAudio::AudioBuffer operator-(heph_float lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(rhs);
	for (size_t i = 0; i < rhs.FrameCount(); i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelCount; j++)
		{
			resultBuffer[i][j] = lhs - rhs[i][j];
		}
	}
	return resultBuffer;
}
HephAudio::AudioBuffer operator-(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(HephCommon::Math::Max(lhs.FrameCount(), rhs.FrameCount()), rhs.FormatInfo());
	const size_t minFrameCount = HephCommon::Math::Min(lhs.FrameCount(), rhs.FrameCount());
	size_t i;

	for (i = 0; i < minFrameCount; i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelCount; j++)
		{
			resultBuffer[i][j] = lhs[i] - rhs[i][j];
		}
	}

	for (; i < resultBuffer.FrameCount(); i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelCount; j++)
		{
			resultBuffer[i][j] = -rhs[i][j];
		}
	}

	return resultBuffer;
}
HephAudio::AudioBuffer operator*(heph_float lhs, const HephAudio::AudioBuffer& rhs)
{
	return rhs * lhs;
}
HephAudio::AudioBuffer operator*(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	return rhs * lhs;
}
HephAudio::AudioBuffer operator/(heph_float lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(rhs);
	for (size_t i = 0; i < rhs.FrameCount(); i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelCount; j++)
		{
			resultBuffer[i][j] = lhs / rhs[i][j];
		}
	}
	return resultBuffer;
}
HephAudio::AudioBuffer operator/(const HephCommon::FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(HephCommon::Math::Max(lhs.FrameCount(), rhs.FrameCount()), rhs.FormatInfo());
	const size_t minFrameCount = HephCommon::Math::Min(lhs.FrameCount(), rhs.FrameCount());
	for (size_t i = 0; i < minFrameCount; i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelCount; j++)
		{
			resultBuffer[i][j] = lhs[i] / rhs[i][j];
		}
	}
	return resultBuffer;
}