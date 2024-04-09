#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "HephException.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioBuffer::AudioBuffer() : frameCount(0), pData(nullptr) { }
	AudioBuffer::AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo) : formatInfo(formatInfo), frameCount(frameCount), pData(nullptr)
	{
		if (frameCount > 0)
		{
			this->pData = (heph_audio_sample*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::AudioBuffer", "Insufficient memory."));
			}
			this->Reset();
		}
	}
	AudioBuffer::AudioBuffer(size_t frameCount, AudioChannelLayout channelLayout, uint32_t sampleRate) : AudioBuffer(frameCount, HEPHAUDIO_INTERNAL_FORMAT(channelLayout, sampleRate)) {}
	AudioBuffer::AudioBuffer(std::nullptr_t rhs) : AudioBuffer() {}
	AudioBuffer::AudioBuffer(const AudioBuffer& rhs) : formatInfo(rhs.formatInfo), frameCount(rhs.frameCount), pData(nullptr)
	{
		if (rhs.frameCount > 0)
		{
			this->pData = (heph_audio_sample*)malloc(rhs.Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::AudioBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.pData, rhs.Size());
		}
	}
	AudioBuffer::AudioBuffer(AudioBuffer&& rhs) noexcept : formatInfo(rhs.formatInfo), frameCount(rhs.frameCount), pData(rhs.pData)
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
	heph_audio_sample* AudioBuffer::operator[](size_t frameIndex) const
	{
		return (heph_audio_sample*)(((uint8_t*)this->pData) + this->formatInfo.FrameSize() * frameIndex);
	}
	AudioBuffer AudioBuffer::operator-() const
	{
		AudioBuffer resultBuffer(this->frameCount, this->formatInfo);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] = -(*this)[i][j];
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator=(std::nullptr_t rhs)
	{
		this->Empty();
		this->formatInfo = AudioFormatInfo();
		return *this;
	}
	AudioBuffer& AudioBuffer::operator=(const AudioBuffer& rhs)
	{
		if (this != &rhs)
		{
			this->Empty();

			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;

			if (rhs.frameCount > 0)
			{
				this->pData = (heph_audio_sample*)malloc(rhs.Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::operator=", "Insufficient memory."));
				}
				memcpy(this->pData, rhs.pData, rhs.Size());
			}
		}

		return *this;
	}
	AudioBuffer& AudioBuffer::operator=(AudioBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Empty();

			this->frameCount = rhs.frameCount;
			this->formatInfo = rhs.formatInfo;
			this->pData = rhs.pData;

			rhs.frameCount = 0;
			rhs.formatInfo = AudioFormatInfo();
			rhs.pData = nullptr;
		}

		return *this;
	}
	AudioBuffer AudioBuffer::operator+(heph_float rhs) const
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] += rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator+(const FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.FrameCount());
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] + rhs[i];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator+", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] + rhs[i][j];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] = rhs[i][j];
			}
		}

		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator+=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				(*this)[i][j] += rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator+=(const FloatBuffer& rhs)
	{
		this->Resize(HephMath::Max(this->frameCount, rhs.FrameCount()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator+=", "Buffers must have the same format."));
		}

		this->Resize(HephMath::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				(*this)[i][j] += rhs[i][j];
			}
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator-(heph_float rhs) const
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] -= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator-(const FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.FrameCount());
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] - rhs[i];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator-", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] = (*this)[i][j] - rhs[i][j];
			}
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] = -rhs[i][j];
			}
		}

		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator-=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				(*this)[i][j] -= rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator-=(const FloatBuffer& rhs)
	{
		this->Resize(HephMath::Max(this->frameCount, rhs.FrameCount()));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator-=", "Buffers must have the same format."));
		}

		this->Resize(HephMath::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] *= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator*(const FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.FrameCount());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator*", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				(*this)[i][j] *= rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator*=(const FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.FrameCount())
		{
			for (size_t i = 0; i < rhs.FrameCount(); i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator*=", "Buffers must have the same format."));
		}

		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				resultBuffer[i][j] /= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer AudioBuffer::operator/(const FloatBuffer& rhs) const
	{
		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.FrameCount()), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.FrameCount());
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator/", "Buffers must have the same format."));
		}

		AudioBuffer resultBuffer(HephMath::Max(this->frameCount, rhs.frameCount), this->formatInfo);
		const size_t minFrameCount = HephMath::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				(*this)[i][j] /= rhs;
			}
		}
		return *this;
	}
	AudioBuffer& AudioBuffer::operator/=(const FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.FrameCount())
		{
			for (size_t i = 0; i < rhs.FrameCount(); i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::operator/=", "Buffers must have the same format."));
		}

		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
				for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
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
	bool AudioBuffer::operator==(std::nullptr_t rhs) const
	{
		return this->pData == rhs;
	}
	bool AudioBuffer::operator==(const AudioBuffer& rhs) const
	{
		return this == &rhs || (this->formatInfo == rhs.formatInfo && this->frameCount == rhs.frameCount && memcmp(this->pData, rhs.pData, this->Size()) == 0);
	}
	bool AudioBuffer::operator!=(std::nullptr_t rhs) const
	{
		return this->pData != rhs;
	}
	bool AudioBuffer::operator!=(const AudioBuffer& rhs) const
	{
		return this != &rhs && (this->formatInfo != rhs.formatInfo || this->frameCount != rhs.frameCount || memcmp(this->pData, rhs.pData, this->Size()) != 0);
	}
	size_t AudioBuffer::Size() const
	{
		return this->frameCount * this->formatInfo.FrameSize();
	}
	size_t AudioBuffer::FrameCount() const
	{
		return this->frameCount;
	}
	AudioFormatInfo AudioBuffer::FormatInfo() const
	{
		return this->formatInfo;
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
		if (this->formatInfo != buffer.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::Append", "Both buffers must have the same format."));
		}

		if (buffer.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = buffer;
				return;
			}

			heph_audio_sample* tempPtr = (heph_audio_sample*)malloc(this->Size() + buffer.Size());
			if (tempPtr == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::Join", "Insufficient memory."));
			}
			memcpy(tempPtr, this->pData, this->Size());

			memcpy((uint8_t*)tempPtr + this->Size(), buffer.pData, buffer.Size());

			free(this->pData);
			this->pData = tempPtr;
			this->frameCount += buffer.frameCount;
		}
	}
	void AudioBuffer::Insert(const AudioBuffer& buffer, size_t frameIndex)
	{
		if (this->formatInfo != buffer.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::Insert", "Both buffers must have the same format."));
		}

		if (buffer.frameCount > 0)
		{
			const size_t oldSize = this->Size();
			const size_t newFrameCount = frameIndex > this->frameCount ? (buffer.frameCount + frameIndex) : (this->frameCount + buffer.frameCount);
			const size_t newSize = newFrameCount * this->formatInfo.FrameSize();

			heph_audio_sample* tempPtr = (heph_audio_sample*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::Insert", "Insufficient memory."));
			}
			memset(tempPtr, 0, newSize);

			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();
			if (frameIndexAsBytes > 0 && oldSize > 0)
			{
				memcpy(tempPtr, this->pData, oldSize > frameIndexAsBytes ? frameIndexAsBytes : oldSize);
			}

			const size_t bufferSize = buffer.Size();

			memcpy((uint8_t*)tempPtr + frameIndexAsBytes, buffer.pData, bufferSize);

			if (oldSize > frameIndexAsBytes)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + bufferSize, (uint8_t*)this->pData + frameIndexAsBytes, oldSize - frameIndexAsBytes);
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
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}

			this->frameCount = this->frameCount - frameCount;
			const size_t newSize = this->Size();

			heph_audio_sample* tempPtr = (heph_audio_sample*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::Cut", "Insufficient memory."));
			}

			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();

			if (frameIndexAsBytes > 0)
			{
				memcpy(tempPtr, this->pData, frameIndexAsBytes);
			}

			if (newSize > frameIndexAsBytes)
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
		if (this->formatInfo != buffer.formatInfo)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioBuffer::Replace", "Both buffers must have the same format."));
		}

		if (frameCount > 0)
		{
			const size_t newFrameCount = frameIndex > this->frameCount ? (frameCount + frameIndex) : (this->frameCount + frameCount);
			const size_t newSize = newFrameCount * this->formatInfo.FrameSize();

			heph_audio_sample* tempPtr = (heph_audio_sample*)malloc(newSize);
			if (tempPtr == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::Replace", "Insufficient memory."));
			}
			memset(tempPtr, 0, newSize);

			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();
			if (frameIndex > 0)
			{
				memcpy(tempPtr, this->pData, frameIndexAsBytes > this->Size() ? this->Size() : frameIndexAsBytes);
			}

			const AudioBuffer tempBuffer = buffer.GetSubBuffer(0, frameCount);
			const size_t tempBufferSize = tempBuffer.Size();

			const size_t replacedSize = frameIndexAsBytes + tempBufferSize >= newSize ? newSize - frameIndexAsBytes : tempBufferSize;
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, tempBuffer.pData, replacedSize);
			}

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
		if (this->frameCount > 0)
		{
			memset(this->pData, 0, this->Size());
		}
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
				heph_audio_sample* tempPtr = (heph_audio_sample*)realloc(this->pData, newFrameCount * this->formatInfo.FrameSize());
				if (tempPtr == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AudioBuffer::Resize", "Insufficient memory."));
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
	void AudioBuffer::Empty()
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	heph_audio_sample AudioBuffer::Min() const
	{
		heph_audio_sample minSample = HEPH_AUDIO_SAMPLE_MAX;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				const heph_audio_sample& currentSample = (*this)[i][j];
				if (currentSample < minSample)
				{
					minSample = currentSample;
				}
			}
		}
		return minSample;
	}
	heph_audio_sample AudioBuffer::Max() const
	{
		heph_audio_sample maxSample = HEPH_AUDIO_SAMPLE_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				const heph_audio_sample& currentSample = (*this)[i][j];
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
				}
			}
		}
		return maxSample;
	}
	heph_audio_sample AudioBuffer::AbsMax() const
	{
		heph_audio_sample maxSample = HEPH_AUDIO_SAMPLE_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				heph_audio_sample currentSample = HephMath::Abs((*this)[i][j]);
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
				}
			}
		}
		return maxSample;
	}
	heph_float AudioBuffer::Rms() const
	{
		heph_float sumOfSamplesSquared = 0.0;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; j++)
			{
				sumOfSamplesSquared += (*this)[i][j] * (*this)[i][j];
			}
		}
		return sqrt(sumOfSamplesSquared / this->frameCount / this->formatInfo.channelLayout.count);
	}
	AudioBuffer AudioBuffer::Convolve(const FloatBuffer& h) const
	{
		return this->Convolve(h, ConvolutionMode::Full);
	}
	AudioBuffer AudioBuffer::Convolve(const FloatBuffer& h, ConvolutionMode convolutionMode) const
	{
		if (convolutionMode == ConvolutionMode::ValidPadding && h.FrameCount() > this->frameCount)
		{
			return AudioBuffer();
		}

		if (this->frameCount == 0 && h.FrameCount() == 0)
		{
			return AudioBuffer();
		}

		size_t yFrameCount = 0;
		size_t iStart = 0;
		size_t iEnd = 0;

		switch (convolutionMode)
		{
		case ConvolutionMode::Central:
			iStart = h.FrameCount() / 2;
			yFrameCount = this->frameCount;
			iEnd = yFrameCount + iStart;
			break;
		case ConvolutionMode::ValidPadding:
			iStart = h.FrameCount() - 1;
			yFrameCount = this->frameCount - h.FrameCount() + 1;
			iEnd = yFrameCount + iStart;
			break;
		case ConvolutionMode::Full:
		default:
			iStart = 0;
			yFrameCount = this->frameCount + h.FrameCount() - 1;
			iEnd = yFrameCount;
			break;
		}

		AudioBuffer y(yFrameCount, this->formatInfo);
		for (size_t ch = 0; ch < this->formatInfo.channelLayout.count; ch++)
		{
			for (size_t i = iStart; i < iEnd; i++)
			{
				for (int j = (i < this->frameCount ? i : (this->frameCount - 1)); j >= 0 && (i - j) < h.FrameCount(); j--)
				{
					y[i - iStart][ch] += (*this)[j][ch] * h[i - j];
				}
			}
		}
		return y;
	}
	AudioBuffer AudioBuffer::Convolve(const AudioBuffer& h) const
	{
		return this->Convolve(h, ConvolutionMode::Full);
	}
	AudioBuffer AudioBuffer::Convolve(const AudioBuffer& h, ConvolutionMode convolutionMode) const
	{
		if (convolutionMode == ConvolutionMode::ValidPadding && h.frameCount > this->frameCount)
		{
			return AudioBuffer();
		}

		if (this->frameCount == 0 && h.frameCount == 0)
		{
			return AudioBuffer();
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

		AudioBuffer y(yFrameCount, this->formatInfo);
		const uint16_t minChannelCount = HephMath::Min(this->formatInfo.channelLayout.count, h.formatInfo.channelLayout.count);
		for (size_t ch = 0; ch < minChannelCount; ch++)
		{
			for (size_t i = iStart; i < iEnd; i++)
			{
				for (int j = (i < iEnd ? i : (iEnd - 1)); j >= 0 && (i - j) < h.frameCount; j--)
				{
					y[i - iStart][ch] += (*this)[j][ch] * h[i - j][ch];
				}
			}
		}
		return y;
	}
	heph_float AudioBuffer::CalculateDuration() const
	{
		return CalculateDuration(this->frameCount, this->formatInfo);
	}
	size_t AudioBuffer::CalculateFrameIndex(heph_float t_s) const
	{
		return CalculateFrameIndex(t_s, this->formatInfo);
	}
	void* AudioBuffer::Begin() const
	{
		return this->pData;
	}
	void* AudioBuffer::End() const
	{
		return (uint8_t*)this->pData + this->Size();
	}
	heph_float AudioBuffer::CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo)
	{
		if (formatInfo.ByteRate() == 0) { return 0.0; }
		return (heph_float)frameCount * (heph_float)formatInfo.FrameSize() / (heph_float)formatInfo.ByteRate();
	}
	size_t AudioBuffer::CalculateFrameIndex(heph_float t_s, AudioFormatInfo formatInfo)
	{
		if (formatInfo.FrameSize() == 0) { return 0.0; }
		return t_s * (heph_float)formatInfo.ByteRate() / (heph_float)formatInfo.FrameSize();
	}
}
HephAudio::AudioBuffer operator+(heph_float lhs, const HephAudio::AudioBuffer& rhs)
{
	return rhs + lhs;
}
HephAudio::AudioBuffer operator+(const FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	return rhs + lhs;
}
HephAudio::AudioBuffer operator-(heph_float lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(rhs);
	for (size_t i = 0; i < rhs.FrameCount(); i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelLayout.count; j++)
		{
			resultBuffer[i][j] = lhs - rhs[i][j];
		}
	}
	return resultBuffer;
}
HephAudio::AudioBuffer operator-(const FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(HephMath::Max(lhs.FrameCount(), rhs.FrameCount()), rhs.FormatInfo());
	const size_t minFrameCount = HephMath::Min(lhs.FrameCount(), rhs.FrameCount());
	size_t i;

	for (i = 0; i < minFrameCount; i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelLayout.count; j++)
		{
			resultBuffer[i][j] = lhs[i] - rhs[i][j];
		}
	}

	for (; i < resultBuffer.FrameCount(); i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelLayout.count; j++)
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
HephAudio::AudioBuffer operator*(const FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	return rhs * lhs;
}
HephAudio::AudioBuffer operator/(heph_float lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(rhs);
	for (size_t i = 0; i < rhs.FrameCount(); i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelLayout.count; j++)
		{
			resultBuffer[i][j] = lhs / rhs[i][j];
		}
	}
	return resultBuffer;
}
HephAudio::AudioBuffer operator/(const FloatBuffer& lhs, const HephAudio::AudioBuffer& rhs)
{
	HephAudio::AudioBuffer resultBuffer(HephMath::Max(lhs.FrameCount(), rhs.FrameCount()), rhs.FormatInfo());
	const size_t minFrameCount = HephMath::Min(lhs.FrameCount(), rhs.FrameCount());
	for (size_t i = 0; i < minFrameCount; i++)
	{
		for (size_t j = 0; j < rhs.FormatInfo().channelLayout.count; j++)
		{
			resultBuffer[i][j] = lhs[i] / rhs[i][j];
		}
	}
	return resultBuffer;
}