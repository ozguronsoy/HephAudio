#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "AudioException.h"
#include <string>

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

namespace HephAudio
{
	AudioBuffer::AudioBuffer()
	{
		wfx = AudioFormatInfo();
	}
	AudioBuffer::AudioBuffer(size_t frameCount, AudioFormatInfo waveFormat)
	{
		wfx = waveFormat;
		buffer = std::vector<uint8_t>(frameCount * wfx.nBlockAlign, 0u);
	}
	size_t AudioBuffer::Size() const noexcept
	{
		return buffer.size();
	}
	size_t AudioBuffer::FrameCount() const noexcept
	{
		return buffer.size() / wfx.nBlockAlign;
	}
	int32_t AudioBuffer::GetAsInt32(uint32_t frameIndex, uint8_t channel) const
	{
		const double result = Get(frameIndex, channel);
		if (result == -1.0)
		{
			return GetMin();
		}
		return result * GetMax();
	}
	double AudioBuffer::Get(uint32_t frameIndex, uint8_t channel) const
	{
		if (channel + 1 > wfx.nChannels)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::Get", L"Channel must be between 0 and " + std::to_wstring(wfx.nChannels - 1) + L".");
		}
		if (frameIndex >= FrameCount())
		{
			throw AudioException(E_FAIL, L"AudioBuffer::Get", L"frameIndex must be lesser than frame count.");
		}
		const uint8_t sampleSize = wfx.wBitsPerSample / 8;
		switch (wfx.wBitsPerSample)
		{
		case 8:
		{
			uint8_t result = 0;
			memcpy(&result, &buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), sampleSize);
			return max(min((double)result / (double)UINT8_MAX, 1.0), -1.0);
		}
		case 16:
		{
			int16_t result = 0;
			memcpy(&result, &buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), sampleSize);
			return max(min((double)result / (double)INT16_MAX, 1.0), -1.0);
		}
		case 24:
		{
			int24 result;
			memcpy(&result, &buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), sampleSize);
			return max(min((double)result.value / (double)INT24_MAX, 1.0), -1.0);
		}
		case 32:
		{
			int32_t result = 0;
			memcpy(&result, &buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), sampleSize);
			return max(min((double)result / (double)INT32_MAX, 1.0), -1.0);
		}
		default:
			break;
		}
		return 0.0f;
	}
	void AudioBuffer::Set(double value, uint32_t frameIndex, uint8_t channel)
	{
		if (channel + 1 > wfx.nChannels)
		{
			throw AudioException(E_FAIL, L"AudioBuffer::Set", L"channel must be between 0 and " + std::to_wstring(wfx.nChannels - 1) + L".");
		}
		if (frameIndex >= FrameCount())
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
		const uint8_t sampleSize = wfx.wBitsPerSample / 8;
		switch (wfx.wBitsPerSample)
		{
		case 8:
		{
			const uint8_t result = max(min(value * (double)UINT8_MAX, UINT8_MAX), 0u);
			memcpy(&buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), &result, sampleSize);
		}
		break;
		case 16:
		{
			const int16_t result = max(min(value * (double)INT16_MAX, INT16_MAX), INT16_MIN);
			memcpy(&buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), &result, sampleSize);
		}
		break;
		case 24:
		{
			int24 result;
			result.value = max(min(value * (double)INT24_MAX, INT24_MAX), INT24_MIN);
			memcpy(&buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), &result, sampleSize);
		}
		break;
		case 32:
		{
			const int32_t result = max(min(value * (double)INT32_MAX, INT32_MAX), INT32_MIN);
			memcpy(&buffer.at(frameIndex * wfx.nBlockAlign + channel * sampleSize), &result, sampleSize);
		}
		break;
		default:
			break;
		}
	}
	AudioBuffer AudioBuffer::GetSubBuffer(uint32_t frameIndex, size_t frameCount) const
	{
		AudioBuffer subBuffer(frameCount, wfx);
		const size_t bufferFrameCount = FrameCount();
		if (frameIndex < bufferFrameCount && frameCount > 0)
		{
			if (frameIndex + frameCount > bufferFrameCount)
			{
				frameCount = bufferFrameCount - frameIndex;
			}
			memcpy(subBuffer.GetInnerBufferAddress(), &buffer.at(frameIndex * wfx.nBlockAlign), frameCount * wfx.nBlockAlign);
		}
		return subBuffer;
	}
	void AudioBuffer::Join(AudioBuffer b)
	{
		if (b.FrameCount() > 0)
		{
			AudioBuffer resultBuffer(this->FrameCount() + b.FrameCount(), this->wfx);
			AudioProcessor audioProcessor(this->wfx);
			audioProcessor.ConvertSampleRate(b);
			audioProcessor.ConvertBPS(b);
			audioProcessor.ConvertChannels(b);
			if (this->FrameCount() > 0)
			{
				memcpy(resultBuffer.GetInnerBufferAddress(), this->GetInnerBufferAddress(), this->Size());
			}
			memcpy((uint8_t*)resultBuffer.GetInnerBufferAddress() + this->Size(), b.GetInnerBufferAddress(), b.Size());
			this->buffer = resultBuffer.buffer;
		}
	}
	void AudioBuffer::Cut(uint32_t frameIndex, size_t frameCount)
	{
		const size_t bufferFrameCount = FrameCount();
		if (frameCount > 0 && frameIndex < bufferFrameCount)
		{
			if (frameIndex + frameCount > bufferFrameCount)
			{
				frameCount = bufferFrameCount - frameIndex;
			}
			AudioBuffer newBuffer(bufferFrameCount - frameCount, wfx);
			if (newBuffer.Size() > 0)
			{
				const size_t frameIndexAsBytes = frameIndex * wfx.nBlockAlign;
				const size_t padding = frameCount * wfx.nBlockAlign;
				if (frameIndexAsBytes > 0)
				{
					memcpy(newBuffer.GetInnerBufferAddress(), &buffer.at(0), frameIndexAsBytes);
				}
				if (newBuffer.Size() - frameIndexAsBytes > 0)
				{
					memcpy((uint8_t*)newBuffer.GetInnerBufferAddress() + frameIndexAsBytes, &buffer.at(frameIndexAsBytes + padding), newBuffer.Size() - frameIndexAsBytes);
				}
			}
			this->buffer = newBuffer.buffer;
		}
	}
	void AudioBuffer::Reset()
	{
		memset(&buffer.at(0), 0, Size());
	}
	double AudioBuffer::CalculateDuration() const noexcept
	{
		return CalculateDuration(FrameCount(), wfx);
	}
	AudioFormatInfo AudioBuffer::GetFormat() const noexcept
	{
		return wfx;
	}
	void* AudioBuffer::GetInnerBufferAddress() const noexcept
	{
		return (void*)&buffer.at(0);
	}
	double AudioBuffer::GetMin() const noexcept
	{
		switch (wfx.wBitsPerSample)
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
		switch (wfx.wBitsPerSample)
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
	AudioBuffer& AudioBuffer::operator+=(const AudioBuffer& rhs)
	{
		this->Join(rhs);
		return *this;
	}
	double AudioBuffer::CalculateDuration(size_t frameCount, AudioFormatInfo waveFormat) noexcept
	{
		return (double)frameCount * (double)waveFormat.nBlockAlign / (double)waveFormat.nAvgBytesPerSec;
	}
}