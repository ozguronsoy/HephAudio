#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "AudioException.h"
#include <string>
#include <algorithm>

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
	AudioBuffer::~AudioBuffer()
	{
		if (pAudioData != nullptr)
		{
			free(pAudioData);
			pAudioData = nullptr;
		}
	}
	AudioBuffer AudioBuffer::operator-() const
	{
		AudioBuffer resultBuffer(frameCount, formatInfo);
		switch (formatInfo.bitsPerSample)
		{
		case 8:
			std::transform((uint8_t*)pAudioData, (uint8_t*)pAudioData + Size(), (uint8_t*)resultBuffer.pAudioData, [](uint8_t& sample) {return -sample; });
			break;
		case 16:
			std::transform((int16_t*)pAudioData, (int16_t*)((uint8_t*)pAudioData + Size()), (int16_t*)resultBuffer.pAudioData, [](int16_t& sample) {return -sample; });
			break;
		case 24:
			std::transform((int24*)pAudioData, (int24*)((uint8_t*)pAudioData + Size()), (int24*)resultBuffer.pAudioData, [](int24& sample) {return -sample; });
			break;
		case 32:
			std::transform((int32_t*)pAudioData, (int32_t*)((uint8_t*)pAudioData + Size()), (int32_t*)resultBuffer.pAudioData, [](int32_t& sample) {return -sample; });
			break;
		default:
			memcpy(resultBuffer.pAudioData, pAudioData, Size());
			break;
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator=(const AudioBuffer& rhs)
	{
		if (rhs.pAudioData != nullptr)
		{
			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;
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
	bool AudioBuffer::operator==(AudioBuffer& rhs) const
	{
		return this == &rhs || (this->formatInfo == rhs.formatInfo && this->frameCount == rhs.frameCount && memcmp(this->pAudioData, rhs.pAudioData, this->Size()) == 0);
	}
	bool AudioBuffer::operator!=(AudioBuffer& rhs) const
	{
		return this != &rhs && (this->formatInfo != rhs.formatInfo || this->frameCount != rhs.frameCount || memcmp(this->pAudioData, rhs.pAudioData, this->Size()) != 0);
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
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::AudioBuffer", L"Insufficient memory.");
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
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::AudioBuffer", L"Insufficient memory.");
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
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::AudioBuffer", L"Insufficient memory.");
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
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::AudioBuffer", L"Insufficient memory.");
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
				throw AudioException(E_OUTOFMEMORY, L"AudioBuffer::AudioBuffer", L"Insufficient memory.");
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
				AudioProcessor::ConvertSampleRate(*this, newFormat);
				AudioProcessor::ConvertBPS(*this, newFormat);
				AudioProcessor::ConvertChannels(*this, newFormat);
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
	size_t AudioBuffer::CalculateFrameIndex(double ts, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.FrameSize() == 0) { return 0.0; }
		return ts * (double)formatInfo.ByteRate() / (double)formatInfo.FrameSize();
	}
}
#pragma region Exports
using namespace HephAudio;
#if defined(_WIN32)
AudioBuffer* _stdcall CreateAudioBuffer(size_t frameCount, AudioFormatInfo* pFormatInfo)
{
	return new AudioBuffer(frameCount, *pFormatInfo);
}
size_t _stdcall AudioBufferGetSize(AudioBuffer* pAudioBuffer)
{
	return pAudioBuffer->Size();
}
size_t _stdcall AudioBufferGetFrameCount(AudioBuffer* pAudioBuffer)
{
	return pAudioBuffer->FrameCount();
}
double _stdcall AudioBufferGetSample(AudioBuffer* pAudioBuffer, size_t frameIndex, uint8_t channel)
{
	return pAudioBuffer->Get(frameIndex, channel);
}
void _stdcall AudioBufferSetSample(AudioBuffer* pAudioBuffer, double value, size_t frameIndex, uint8_t channel)
{
	pAudioBuffer->Set(value, frameIndex, channel);
}
AudioBuffer* _stdcall AudioBufferGetSubBuffer(AudioBuffer* pAudioBuffer, size_t frameIndex, size_t frameCount)
{
	AudioBuffer* subBuffer = new AudioBuffer();
	*subBuffer = pAudioBuffer->GetSubBuffer(frameIndex, frameCount);
	return subBuffer;
}
void _stdcall AudioBufferJoin(AudioBuffer* pB1, AudioBuffer* pB2)
{
	pB1->Join(*pB2);
}
void _stdcall AudioBufferInsert(AudioBuffer* pB1, size_t frameIndex, AudioBuffer* pB2)
{
	pB1->Insert(*pB2, frameIndex);
}
void _stdcall AudioBufferCut(AudioBuffer* pAudioBuffer, size_t frameIndex, size_t frameCount)
{
	pAudioBuffer->Cut(frameIndex, frameCount);
}
void _stdcall AudioBufferReplace(AudioBuffer* pB1, AudioBuffer* pB2, size_t frameIndex, size_t frameCount)
{
	pB1->Replace(*pB2, frameIndex, frameCount);
}
void _stdcall AudioBufferReset(AudioBuffer* pAudioBuffer)
{
	pAudioBuffer->Reset();
}
void _stdcall AudioBufferResize(AudioBuffer* pAudioBuffer, size_t newFrameCount)
{
	pAudioBuffer->Resize(newFrameCount);
}
double _stdcall AudioBufferCalculateDuration(AudioBuffer* pAudioBuffer)
{
	return pAudioBuffer->CalculateDuration();
}
AudioFormatInfo* _stdcall AudioBufferGetFormat(AudioBuffer* pAudioBuffer)
{
	return &pAudioBuffer->formatInfo;
}
void _stdcall AudioBufferSetFormat(AudioBuffer* pAudioBuffer, AudioFormatInfo* newFormat)
{
	pAudioBuffer->SetFormat(*newFormat);
}
void _stdcall DestroyAudioBuffer(AudioBuffer* pAudioBuffer)
{
	delete pAudioBuffer;
}
#endif
#pragma endregion