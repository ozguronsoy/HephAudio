#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "AudioException.h"
#include <string>

namespace HephAudio
{
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
	AudioBuffer::AudioBuffer(const AudioBuffer& rhs) : AudioBuffer()
	{
		(*this) = rhs;
	}
	AudioBuffer& AudioBuffer::operator=(const AudioBuffer& rhs)
	{
		this->formatInfo = rhs.formatInfo;
		this->frameCount = rhs.frameCount;
		if (rhs.pAudioData != nullptr)
		{
			if (this->pAudioData != nullptr)
			{
				free(this->pAudioData);
			}
			this->pAudioData = malloc(rhs.Size());
			if (this->pAudioData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::AudioBuffer", L"Insufficient memory.");
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
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				resultBuffer.Set(resultBuffer.Get(i, j) * rhs, i, j);
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
				Set(Get(i, j) * rhs, i, j);
			}
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator/(const double& rhs) const
	{
		if (rhs == 0)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::operator/", L"Divided by zero.");
		}
		AudioBuffer resultBuffer(*this);
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				resultBuffer.Set(resultBuffer.Get(i, j) / rhs, i, j);
			}
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator/=(const double& rhs)
	{
		if (rhs == 0)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::operator/=", L"Divided by zero.");
		}
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < formatInfo.channelCount; j++)
			{
				Set(Get(i, j) / rhs, i, j);
			}
		}
		return *this;
	}
	AudioBuffer::~AudioBuffer()
	{
		if (pAudioData != nullptr)
		{
			free(pAudioData);
			pAudioData = nullptr;
		}
	}
	size_t AudioBuffer::Size() const noexcept
	{
		return frameCount * formatInfo.FrameSize();
	}
	size_t AudioBuffer::FrameCount() const noexcept
	{
		return frameCount;
	}
	int32_t AudioBuffer::GetAsInt32(size_t frameIndex, uint8_t channel) const
	{
		const double result = Get(frameIndex, channel);
		if (result == -1.0)
		{
			return GetMin();
		}
		return result * GetMax();
	}
	double AudioBuffer::Get(size_t frameIndex, uint8_t channel) const
	{
		if (channel + 1 > formatInfo.channelCount)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::Get", L"Channel must be between 0 and " + std::to_wstring(formatInfo.channelCount - 1) + L".");
		}
		if (frameIndex >= frameCount)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::Get", L"frameIndex must be lesser than frame count.");
		}
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
			return max(min((double)result.value / (double)INT24_MAX, 1.0), -1.0);
		}
		case 32:
		{
			int32_t result = 0;
			memcpy(&result, (uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((double)result / (double)INT32_MAX, 1.0), -1.0);
		}
		default:
			break;
		}
		return 0.0f;
	}
	void AudioBuffer::Set(double value, size_t frameIndex, uint8_t channel)
	{
		if (channel + 1 > formatInfo.channelCount)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::Set", L"channel must be between 0 and " + std::to_wstring(formatInfo.channelCount - 1) + L".");
		}
		if (frameIndex >= frameCount)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::Set", L"frameIndex must be lesser than frame count.");
		}
		if (value < -1.0)
		{
			value = -1.0;
		}
		if (value > 1.0)
		{
			value = 1.0;
		}
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
			int24 result;
			result.value = max(min(value * (double)INT24_MAX, INT24_MAX), INT24_MIN);
			memcpy((uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 32:
		{
			const int32_t result = max(min(value * (double)INT32_MAX, INT32_MAX), INT32_MIN);
			memcpy((uint8_t*)pAudioData + frameIndex * formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
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
			buffer.SetFormat(this->formatInfo);
			if (this->frameCount == 0)
			{
				*this = buffer;
				return;
			}
			AudioBuffer resultBuffer(this->frameCount + buffer.frameCount, this->formatInfo);
			memcpy(resultBuffer.pAudioData, this->pAudioData, this->Size());
			memcpy((uint8_t*)resultBuffer.pAudioData + this->Size(), buffer.pAudioData, buffer.Size());
			*this = resultBuffer;
		}
	}
	void AudioBuffer::Insert(size_t frameIndex, AudioBuffer buffer)
	{
		if (buffer.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = buffer;
				return;
			}
			if (frameIndex > this->frameCount)
			{
				frameIndex = this->frameCount;
			}
			buffer.SetFormat(this->formatInfo);
			AudioBuffer resultBuffer(this->frameCount + buffer.frameCount, this->formatInfo);
			const size_t byteIndex = frameIndex * this->formatInfo.FrameSize();
			if (byteIndex > 0)
			{
				memcpy(resultBuffer.pAudioData, this->pAudioData, byteIndex);
			}
			if (buffer.Size() > 0)
			{
				memcpy((uint8_t*)resultBuffer.pAudioData + byteIndex, buffer.pAudioData, buffer.Size());
			}
			if (this->Size() - byteIndex > 0)
			{
				memcpy((uint8_t*)resultBuffer.pAudioData + byteIndex + buffer.Size(), (uint8_t*)this->pAudioData + byteIndex, this->Size() - byteIndex);
			}
			*this = resultBuffer;
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
			AudioBuffer resultBuffer(this->frameCount - frameCount, formatInfo);
			if (resultBuffer.Size() > 0)
			{
				const size_t frameIndexAsBytes = frameIndex * formatInfo.FrameSize();
				const size_t padding = frameCount * formatInfo.FrameSize();
				if (frameIndexAsBytes > 0)
				{
					memcpy(resultBuffer.pAudioData, this->pAudioData, frameIndexAsBytes);
				}
				if (resultBuffer.Size() - frameIndexAsBytes > 0)
				{
					memcpy((uint8_t*)resultBuffer.pAudioData + frameIndexAsBytes, (uint8_t*)this->pAudioData + frameIndexAsBytes + padding, resultBuffer.Size() - frameIndexAsBytes);
				}
			}
			*this = resultBuffer;
		}
	}
	void AudioBuffer::Replace(AudioBuffer buffer, size_t frameIndex)
	{
		Replace(buffer, frameIndex, buffer.frameCount);
	}
	void AudioBuffer::Replace(AudioBuffer buffer, size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0 && frameIndex < this->frameCount)
		{
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			AudioBuffer resultBuffer(this->frameCount + buffer.frameCount - frameCount, this->formatInfo);
			const size_t frameIndexAsBytes = frameIndex * formatInfo.FrameSize();
			const size_t padding = frameCount * formatInfo.FrameSize();
			if (frameIndexAsBytes > 0)
			{
				memcpy(resultBuffer.pAudioData, this->pAudioData, frameIndexAsBytes);
			}
			if (buffer.frameCount > 0)
			{
				buffer.SetFormat(this->formatInfo);
				memcpy((uint8_t*)resultBuffer.pAudioData + frameIndexAsBytes, buffer.pAudioData, buffer.Size());
			}
			if (this->Size() - padding - frameIndexAsBytes > 0)
			{
				memcpy((uint8_t*)resultBuffer.pAudioData + frameIndexAsBytes + buffer.Size(), (uint8_t*)this->pAudioData + frameIndexAsBytes + padding, this->Size() - padding - frameIndexAsBytes);
			}
		}
		else
		{
			this->Insert(frameIndex, buffer);
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
			if (newFrameCount > this->frameCount)
			{
				this->Join(AudioBuffer(newFrameCount - this->frameCount, formatInfo));
			}
			else
			{
				this->Cut(newFrameCount, this->frameCount - newFrameCount);
			}
		}
	}
	double AudioBuffer::CalculateDuration() const noexcept
	{
		return CalculateDuration(frameCount, formatInfo);
	}
	AudioFormatInfo AudioBuffer::GetFormat() const noexcept
	{
		return formatInfo;
	}
	void AudioBuffer::SetFormat(AudioFormatInfo newFormat)
	{
		if (newFormat != formatInfo)
		{
			if (frameCount > 0)
			{
				AudioProcessor audioProcessor(newFormat);
				audioProcessor.ConvertSampleRate(*this);
				audioProcessor.ConvertBPS(*this);
				audioProcessor.ConvertChannels(*this);
			}
			formatInfo = newFormat;
		}
	}
	void* AudioBuffer::GetAudioDataAddress() const noexcept
	{
		return pAudioData;
	}
	double AudioBuffer::GetMin() const noexcept
	{
		switch (formatInfo.bitsPerSample)
		{
		case 8:
			return 0.0f;
		case 16:
			return INT16_MIN;
		case 24:
			return INT24_MIN;
		case 32:
			return INT32_MIN;
		default:
			return 1.0;
		}
	}
	double AudioBuffer::GetMax() const noexcept
	{
		switch (formatInfo.bitsPerSample)
		{
		case 8:
			return UINT8_MAX;
		case 16:
			return INT16_MAX;
		case 24:
			return INT24_MAX;
		case 32:
			return INT32_MAX;
		default:
			return 1.0;
		}
	}
	double AudioBuffer::CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.ByteRate() == 0) { return 0.0; }
		return (double)frameCount * (double)formatInfo.FrameSize() / (double)formatInfo.ByteRate();
	}
}