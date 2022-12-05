#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "AudioException.h"

namespace HephAudio
{
#pragma region Audio Frame
	AudioFrame::AudioFrame(void* pAudioData, size_t frameIndex, size_t channelCount)
	{
		this->pAudioData = (double*)pAudioData;
		this->frameIndex = frameIndex;
		this->channelCount = channelCount;
	}
	double& AudioFrame::operator[](const size_t& channel) const
	{
		return *(pAudioData + frameIndex * channelCount + channel);
	}
#pragma endregion
#pragma region Audio Buffer
	AudioBuffer::AudioBuffer()
	{
		formatInfo = AudioFormatInfo();
		frameCount = 0;
		pAudioData = nullptr;
	}
	AudioBuffer::AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo)
	{
		this->formatInfo = formatInfo;
		this->frameCount = frameCount;
		pAudioData = malloc(Size());
		if (pAudioData != nullptr)
		{
			memset(pAudioData, 0, Size());
		}
		else
		{
			throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::AudioBuffer", L"Insufficient memory.");
		}
	}
	AudioBuffer::AudioBuffer(const AudioBuffer& rhs)
	{
		if (rhs.pAudioData != nullptr)
		{
			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;
			this->pAudioData = malloc(rhs.Size());
			if (this->pAudioData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::opeartor=", L"Insufficient memory.");
			}
			memcpy(this->pAudioData, rhs.pAudioData, rhs.Size());
		}
		else
		{
			formatInfo = AudioFormatInfo();
			frameCount = 0;
			pAudioData = nullptr;
		}
	}
	AudioBuffer::~AudioBuffer()
	{
		if (pAudioData != nullptr)
		{
			free(pAudioData);
			frameCount = 0;
			formatInfo = AudioFormatInfo();
			pAudioData = nullptr;
		}
	}
	AudioFrame AudioBuffer::operator[](const size_t& frameIndex) const
	{
		return AudioFrame(pAudioData, frameIndex, formatInfo.channelCount);
	}
	AudioBuffer AudioBuffer::operator-() const
	{
		AudioBuffer resultBuffer(frameCount, formatInfo);
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = -(*this)[i][j];
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator=(const AudioBuffer& rhs)
	{
		if (rhs.pAudioData != nullptr)
		{
			this->~AudioBuffer();
			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;
			this->pAudioData = malloc(rhs.Size());
			if (this->pAudioData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::opeartor=", L"Insufficient memory.");
			}
			memcpy(this->pAudioData, rhs.pAudioData, rhs.Size());
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator+(const AudioBuffer& rhs) const
	{
		AudioBuffer resultBuffer(this->frameCount, this->formatInfo);
		if (this->pAudioData != nullptr && this->frameCount > 0)
		{
			memcpy(resultBuffer.pAudioData, this->pAudioData, this->Size());
		}
		if (rhs.pAudioData != nullptr && rhs.frameCount > 0)
		{
			resultBuffer.Join(rhs);
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator+=(const AudioBuffer& rhs)
	{
		this->Join(rhs);
		return *this;
	}
	AudioBuffer AudioBuffer::operator*(const double& rhs) const
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] *= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator*=(const double& rhs)
	{
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < formatInfo.channelCount; j++)
			{
				(*this)[i][j] *= rhs;
			}
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator/(const double& rhs) const
	{
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] /= rhs;
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator/=(const double& rhs)
	{
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < formatInfo.channelCount; j++)
			{
				(*this)[i][j] /= rhs;
			}
		}
		return *this;
	}
	bool AudioBuffer::operator==(const AudioBuffer& rhs) const
	{
		return this == &rhs || (this->formatInfo == rhs.formatInfo && this->frameCount == rhs.frameCount && memcmp(this->pAudioData, rhs.pAudioData, this->Size()) == 0);
	}
	bool AudioBuffer::operator!=(const AudioBuffer& rhs) const
	{
		return this != &rhs && (this->formatInfo != rhs.formatInfo || this->frameCount != rhs.frameCount || memcmp(this->pAudioData, rhs.pAudioData, this->Size()) != 0);
	}
	size_t AudioBuffer::Size() const noexcept
	{
		return frameCount * formatInfo.FrameSize();
	}
	const size_t& AudioBuffer::FrameCount() const noexcept
	{
		return frameCount;
	}
	double AudioBuffer::Get(size_t frameIndex, uint8_t channel) const
	{
		const uint8_t sampleSize = formatInfo.bitsPerSample / 8;
		switch (formatInfo.bitsPerSample)
		{
		case 8:
		{
			uint8_t result = 0;
			memcpy(&result, (uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((double)result / (double)UINT8_MAX, 1.0), -1.0);
		}
		case 16:
		{
			int16_t result = 0;
			memcpy(&result, (uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((double)result / (double)INT16_MAX, 1.0), -1.0);
		}
		case 24:
		{
			int24 result;
			memcpy(&result, (uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((double)result / (double)INT24_MAX, 1.0), -1.0);
		}
		case 32:
		{
			int32_t result = 0;
			memcpy(&result, (uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((double)result / (double)INT32_MAX, 1.0), -1.0);
		}
		case 64:
		{
			return (*this)[frameIndex][channel];
		}
		default:
			break;
		}
		return 0.0f;
	}
	void AudioBuffer::Set(double value, size_t frameIndex, uint8_t channel)
	{
		const uint8_t sampleSize = formatInfo.bitsPerSample / 8;
		switch (formatInfo.bitsPerSample)
		{
		case 8:
		{
			const uint8_t result = max(min(value * (double)UINT8_MAX, UINT8_MAX), 0u);
			memcpy((uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 16:
		{
			const int16_t result = max(min(value * (double)INT16_MAX, INT16_MAX), INT16_MIN);
			memcpy((uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 24:
		{
			int24 result = max(min(value * (double)INT24_MAX, INT24_MAX), INT24_MIN);
			memcpy((uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 32:
		{
			const int32_t result = max(min(value * (double)INT32_MAX, INT32_MAX), INT32_MIN);
			memcpy((uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 64:
		{
			(*this)[frameIndex][channel] = value;
		}
		break;
		default:
			break;
		}
	}
	AudioBuffer AudioBuffer::GetSubBuffer(size_t frameIndex, size_t frameCount) const
	{
		AudioBuffer subBuffer(frameCount, formatInfo);
		if (frameIndex < this->frameCount && frameCount > 0)
		{
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			memcpy(subBuffer.pAudioData, (uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize(), frameCount * formatInfo.FrameSize());
		}
		return subBuffer;
	}
	void AudioBuffer::Join(AudioBuffer buffer)
	{
		if (buffer.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = buffer;
				return;
			}
			const size_t newFrameCount = this->frameCount + buffer.frameCount;
			void* tempPtr = malloc(newFrameCount * formatInfo.FrameSize());
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::Join", L"Insufficient memory.");
			}
			const size_t oldSize = this->Size();
			if (oldSize > 0)
			{
				memcpy(tempPtr, this->pAudioData, oldSize);
			}
			buffer.SetFormat(this->formatInfo);
			memcpy((uint8_t*)tempPtr + oldSize, buffer.pAudioData, buffer.Size());
			free(pAudioData);
			pAudioData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void AudioBuffer::Insert(AudioBuffer buffer, size_t frameIndex)
	{
		if (buffer.frameCount > 0)
		{
			const size_t newFrameCount = frameIndex > this->frameCount ? buffer.frameCount + frameIndex : this->frameCount + buffer.frameCount;
			const size_t newSize = newFrameCount * formatInfo.FrameSize();
			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::Insert", L"Insufficient memory.");
			}
			memset(tempPtr, 0, newSize);
			size_t cursor = 0;
			if (frameIndex > 0 && this->frameCount > 0)
			{
				cursor = frameIndex * formatInfo.FrameSize();
				memcpy(tempPtr, this->pAudioData, frameIndex > this->frameCount ? this->Size() : cursor);
			}
			buffer.SetFormat(this->formatInfo);
			memcpy((uint8_t*)tempPtr + cursor, buffer.pAudioData, buffer.Size());
			if (frameIndex < this->frameCount)
			{
				memcpy((uint8_t*)tempPtr + cursor + buffer.Size(), (uint8_t*)this->pAudioData + cursor, this->Size() - cursor);
			}
			free(pAudioData);
			pAudioData = tempPtr;
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
			const size_t newFrameCount = this->frameCount - frameCount;
			const size_t newSize = newFrameCount * formatInfo.FrameSize();
			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::Cut", L"Insufficient memory.");
			}
			const size_t frameIndexAsBytes = frameIndex * formatInfo.FrameSize();
			if (frameIndexAsBytes > 0)
			{
				memcpy(tempPtr, pAudioData, frameIndexAsBytes);
			}
			if (newSize > frameIndexAsBytes)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, (uint8_t*)pAudioData + frameIndexAsBytes + frameCount * formatInfo.FrameSize(), newSize - frameIndexAsBytes);
			}
			free(pAudioData);
			pAudioData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void AudioBuffer::Replace(AudioBuffer buffer, size_t frameIndex)
	{
		Replace(buffer, frameIndex, buffer.frameCount);
	}
	void AudioBuffer::Replace(AudioBuffer buffer, size_t frameIndex, size_t frameCount)
	{
		if (buffer.frameCount > 0 && frameCount > 0 && frameIndex < this->frameCount)
		{
			const size_t newFrameCount = this->frameCount + buffer.frameCount - frameCount;
			const size_t newSize = newFrameCount * formatInfo.FrameSize();
			void* tempPtr = malloc(newSize);
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::Replace", L"Insufficient memory.");
			}
			size_t cursor = 0;
			if (frameIndex > 0)
			{
				cursor = frameIndex * formatInfo.FrameSize();
				memcpy(tempPtr, this->pAudioData, cursor);
			}
			buffer.SetFormat(formatInfo);
			const size_t replacedSize = cursor + buffer.Size() >= newSize ? newSize - cursor : buffer.Size();
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)tempPtr + cursor, buffer.pAudioData, replacedSize);
			}
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = cursor + frameCount * formatInfo.FrameSize();
				memcpy((uint8_t*)tempPtr + cursor + replacedSize, (uint8_t*)this->pAudioData + padding, this->Size() - padding);
			}
			free(pAudioData);
			pAudioData = tempPtr;
			this->frameCount = newFrameCount;
		}
		else
		{
			this->Insert(buffer, frameIndex);
		}
	}
	void AudioBuffer::Reset()
	{
		memset(pAudioData, 0, Size());
	}
	void AudioBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			void* tempPtr = realloc(pAudioData, newFrameCount * formatInfo.FrameSize());
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::Resize", L"Insufficient memory.");
			}
			pAudioData = tempPtr;
			frameCount = newFrameCount;
		}
	}
	double AudioBuffer::CalculateDuration() const noexcept
	{
		return CalculateDuration(frameCount, formatInfo);
	}
	size_t AudioBuffer::CalculateFrameIndex(double ts) const noexcept
	{
		return CalculateFrameIndex(ts, formatInfo);
	}
	const AudioFormatInfo& AudioBuffer::FormatInfo() const noexcept
	{
		return formatInfo;
	}
	void AudioBuffer::SetFormat(AudioFormatInfo newFormat)
	{
		if (newFormat != formatInfo)
		{
			if (frameCount > 0)
			{
				AudioProcessor::ConvertSampleRate(*this, newFormat.sampleRate);
				AudioProcessor::ConvertChannels(*this, newFormat.channelCount);
			}
			formatInfo = newFormat;
		}
	}
	void* const& AudioBuffer::Begin() const noexcept
	{
		return pAudioData;
	}
	void* AudioBuffer::End() const noexcept
	{
		return (uint8_t*)pAudioData + Size();
	}
	double AudioBuffer::CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.ByteRate() == 0) { return 0.0; }
		return (double)frameCount * (double)formatInfo.FrameSize() / (double)formatInfo.ByteRate();
	}
	size_t AudioBuffer::CalculateFrameIndex(double ts, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.FrameSize() == 0) { return 0.0; }
		return ts * (double)formatInfo.ByteRate() / (double)formatInfo.FrameSize();
	}
#pragma endregion
}