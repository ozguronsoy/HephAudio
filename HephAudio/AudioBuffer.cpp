#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "AudioException.h"

namespace HephAudio
{
#pragma region Audio Frame
	AudioFrame::AudioFrame(void* pAudioData, size_t frameIndex, size_t channelCount)
	{
		this->pAudioData = (hephaudio_float*)pAudioData;
		this->frameIndex = frameIndex;
		this->channelCount = channelCount;
	}
	hephaudio_float& AudioFrame::operator[](const size_t& channel) const
	{
		return *(this->pAudioData + this->frameIndex * this->channelCount + channel);
	}
#pragma endregion
#pragma region Audio Buffer
	AudioBuffer::AudioBuffer()
	{
		this->formatInfo = AudioFormatInfo();
		this->frameCount = 0;
		this->pAudioData = nullptr;
	}
	AudioBuffer::AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo)
	{
		this->formatInfo = formatInfo;
		this->frameCount = frameCount;

		if (frameCount > 0)
		{
			// allocate memory and initialize it to 0.
			this->pAudioData = malloc(Size());
			if (this->pAudioData != nullptr)
			{
				memset(this->pAudioData, 0, this->Size());
			}
			else
			{
				throw AudioException(E_OUTOFMEMORY, "AudioBuffer::AudioBuffer", "Insufficient memory.");
			}
		}
		else
		{
			this->pAudioData = nullptr;
		}
	}
	AudioBuffer::AudioBuffer(const AudioBuffer& rhs)
	{
		this->formatInfo = rhs.formatInfo;
		this->frameCount = rhs.frameCount;

		if (rhs.frameCount > 0)
		{
			this->pAudioData = malloc(rhs.Size());
			if (this->pAudioData == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "AudioBuffer::AudioBuffer", "Insufficient memory.");
			}
			memcpy(this->pAudioData, rhs.pAudioData, rhs.Size());
		}
		else
		{
			this->pAudioData = nullptr;
		}
	}
	AudioBuffer::AudioBuffer(AudioBuffer&& rhs) noexcept
	{
		this->frameCount = rhs.frameCount;
		this->formatInfo = rhs.formatInfo;
		this->pAudioData = rhs.pAudioData;

		rhs.frameCount = 0;
		rhs.formatInfo = AudioFormatInfo();
		rhs.pAudioData = nullptr;
	}
	AudioBuffer::~AudioBuffer()
	{
		this->frameCount = 0;
		this->formatInfo = AudioFormatInfo();
		if (this->pAudioData != nullptr)
		{
			free(this->pAudioData);
			this->pAudioData = nullptr;
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
		if (this->pAudioData != rhs.pAudioData)
		{
			this->~AudioBuffer(); // destroy the current buffer to avoid memory leaks.

			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;

			if (rhs.frameCount > 0)
			{
				this->pAudioData = malloc(rhs.Size());
				if (this->pAudioData == nullptr)
				{
					throw AudioException(E_OUTOFMEMORY, "AudioBuffer::operator=", "Insufficient memory.");
				}
				memcpy(this->pAudioData, rhs.pAudioData, rhs.Size());
			}
			else
			{
				this->pAudioData = nullptr;
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
			this->pAudioData = rhs.pAudioData;

			rhs.frameCount = 0;
			rhs.formatInfo = AudioFormatInfo();
			rhs.pAudioData = nullptr;
		}

		return *this;
	}
	AudioBuffer AudioBuffer::operator+(const AudioBuffer& rhs) const
	{
		AudioBuffer resultBuffer(this->frameCount + rhs.frameCount, this->formatInfo);

		if (this->pAudioData != nullptr && this->frameCount > 0)
		{
			memcpy(resultBuffer.pAudioData, this->pAudioData, this->Size());
		}

		if (rhs.pAudioData != nullptr && rhs.frameCount > 0)
		{
			// ensure both buffers have the same format.
			AudioBuffer tempRhs = rhs;
			tempRhs.SetFormat(this->formatInfo);

			memcpy((uint8_t*)resultBuffer.pAudioData + this->Size(), rhs.pAudioData, rhs.Size());
		}

		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator+=(const AudioBuffer& rhs)
	{
		this->Join(rhs);
		return *this;
	}
	AudioBuffer AudioBuffer::operator*(const hephaudio_float& rhs) const
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
	AudioBuffer& AudioBuffer::operator*=(const hephaudio_float& rhs)
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
	AudioBuffer AudioBuffer::operator/(const hephaudio_float& rhs) const
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
	AudioBuffer& AudioBuffer::operator/=(const hephaudio_float& rhs)
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
		return this->frameCount * this->formatInfo.FrameSize();
	}
	const size_t& AudioBuffer::FrameCount() const noexcept
	{
		return this->frameCount;
	}
	hephaudio_float AudioBuffer::Get(size_t frameIndex, uint8_t channel) const
	{
		const uint8_t sampleSize = this->formatInfo.bitsPerSample / 8;
		switch (this->formatInfo.bitsPerSample)
		{
		case 8:
		{
			uint8_t result = 0;
			memcpy(&result, (uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((hephaudio_float)result / (hephaudio_float)UINT8_MAX, 1.0), -1.0);
		}
		case 16:
		{
			int16_t result = 0;
			memcpy(&result, (uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((hephaudio_float)result / (hephaudio_float)INT16_MAX, 1.0), -1.0);
		}
		case 24:
		{
			int24 result;
			memcpy(&result, (uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((hephaudio_float)result / (hephaudio_float)INT24_MAX, 1.0), -1.0);
		}
		case 32:
		{
			if (this->formatInfo.formatTag == WAVE_FORMAT_HEPHAUDIO)
			{
				return (*this)[frameIndex][channel];
			}
			int32_t result = 0;
			memcpy(&result, (uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, sampleSize);
			return max(min((hephaudio_float)result / (hephaudio_float)INT32_MAX, 1.0), -1.0);
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
	void AudioBuffer::Set(hephaudio_float value, size_t frameIndex, uint8_t channel)
	{
		const uint8_t sampleSize = this->formatInfo.bitsPerSample / 8;
		switch (this->formatInfo.bitsPerSample)
		{
		case 8:
		{
			const uint8_t result = max(min(value * (hephaudio_float)UINT8_MAX, UINT8_MAX), 0u);
			memcpy((uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 16:
		{
			const int16_t result = max(min(value * (hephaudio_float)INT16_MAX, INT16_MAX), INT16_MIN);
			memcpy((uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 24:
		{
			int24 result = max(min(value * (hephaudio_float)INT24_MAX, INT24_MAX), INT24_MIN);
			memcpy((uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
		}
		break;
		case 32:
		{
			if (this->formatInfo.formatTag == WAVE_FORMAT_HEPHAUDIO)
			{
				(*this)[frameIndex][channel] = value;
			}
			else
			{
				const int32_t result = max(min(value * (hephaudio_float)INT32_MAX, INT32_MAX), INT32_MIN);
				memcpy((uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize() + channel * sampleSize, &result, sampleSize);
			}
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
		AudioBuffer subBuffer(frameCount, this->formatInfo);
		if (frameIndex < this->frameCount && frameCount > 0)
		{
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			memcpy(subBuffer.pAudioData, (uint8_t*)this->pAudioData + frameIndex * this->formatInfo.FrameSize(), frameCount * this->formatInfo.FrameSize());
		}
		return subBuffer;
	}
	void AudioBuffer::Join(const AudioBuffer& buffer)
	{
		if (buffer.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = buffer;
				return;
			}

			// allocate memory with the combined size and copy the rhs's data to the end of the current buffer's data.
			void* tempPtr = malloc(this->Size() + buffer.frameCount * this->formatInfo.FrameSize());
			if (tempPtr == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "AudioBuffer::Join", "Insufficient memory.");
			}

			memcpy(tempPtr, this->pAudioData, this->Size());

			// ensure both buffers have the same format.
			AudioBuffer tempBuffer = buffer;
			tempBuffer.SetFormat(this->formatInfo);

			memcpy((uint8_t*)tempPtr + this->Size(), tempBuffer.pAudioData, tempBuffer.Size());

			free(this->pAudioData);
			this->pAudioData = tempPtr;
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
				throw AudioException(E_OUTOFMEMORY, "AudioBuffer::Insert", "Insufficient memory.");
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to insert start index.
			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();
			if (frameIndexAsBytes > 0 && oldSize > 0)
			{
				memcpy(tempPtr, this->pAudioData, oldSize > frameIndexAsBytes ? frameIndexAsBytes : oldSize);
			}

			// ensure both buffers have the same format.
			AudioBuffer tempBuffer = buffer;
			tempBuffer.SetFormat(this->formatInfo);
			const size_t tempBufferSize = tempBuffer.Size();

			memcpy((uint8_t*)tempPtr + frameIndexAsBytes, tempBuffer.pAudioData, tempBufferSize); // insert the buffer.

			// copy the remaining data.
			if (oldSize > frameIndexAsBytes)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + tempBufferSize, (uint8_t*)this->pAudioData + frameIndexAsBytes, oldSize - frameIndexAsBytes);
			}

			free(this->pAudioData);
			this->pAudioData = tempPtr;
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
				throw AudioException(E_OUTOFMEMORY, "AudioBuffer::Cut", "Insufficient memory.");
			}

			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();

			if (frameIndexAsBytes > 0) // copy from 0 to cut start index.
			{
				memcpy(tempPtr, this->pAudioData, frameIndexAsBytes);
			}

			if (newSize > frameIndexAsBytes) // copy the remaining data that we didn't cut.
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, (uint8_t*)this->pAudioData + frameIndexAsBytes + frameCount * this->formatInfo.FrameSize(), newSize - frameIndexAsBytes);
			}

			free(this->pAudioData);
			this->pAudioData = tempPtr;
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
				throw AudioException(E_OUTOFMEMORY, "AudioBuffer::Replace", "Insufficient memory.");
			}
			memset(tempPtr, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to replace start index.
			const size_t frameIndexAsBytes = frameIndex * this->formatInfo.FrameSize();
			if (frameIndex > 0)
			{
				memcpy(tempPtr, this->pAudioData, frameIndexAsBytes > this->Size() ? this->Size() : frameIndexAsBytes);
			}

			// ensure both buffers have the same format.
			AudioBuffer tempBuffer = buffer.GetSubBuffer(0, frameCount);
			tempBuffer.SetFormat(this->formatInfo);
			const size_t tempBufferSize = tempBuffer.Size();

			// copy the replace data.
			const size_t replacedSize = frameIndexAsBytes + tempBufferSize >= newSize ? newSize - frameIndexAsBytes : tempBufferSize;
			if (replacedSize > 0)
			{
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes, tempBuffer.pAudioData, replacedSize);
			}

			// copy the remaining data.
			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndexAsBytes + frameCount * this->formatInfo.FrameSize();
				memcpy((uint8_t*)tempPtr + frameIndexAsBytes + replacedSize, (uint8_t*)this->pAudioData + padding, this->Size() - padding);
			}

			free(this->pAudioData);
			this->pAudioData = tempPtr;
			this->frameCount = newFrameCount;
		}
	}
	void AudioBuffer::Reset()
	{
		memset(this->pAudioData, 0, this->Size());
	}
	void AudioBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			if (newFrameCount == 0)
			{
				this->frameCount = 0;
				if (this->pAudioData != nullptr)
				{
					free(this->pAudioData);
					this->pAudioData = nullptr;
				}
			}
			else
			{
				void* tempPtr = realloc(this->pAudioData, newFrameCount * this->formatInfo.FrameSize());
				if (tempPtr == nullptr)
				{
					throw AudioException(E_OUTOFMEMORY, "AudioBuffer::Resize", "Insufficient memory.");
				}
				this->pAudioData = tempPtr;
				this->frameCount = newFrameCount;
			}
		}
	}
	hephaudio_float AudioBuffer::Min() const noexcept
	{
		hephaudio_float minSample = INT32_MAX;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				const hephaudio_float& currentSample = (*this)[i][j];
				if (currentSample < minSample)
				{
					minSample = currentSample;
				}
			}
		}
		return minSample;
	}
	hephaudio_float AudioBuffer::Max() const noexcept
	{
		hephaudio_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				const hephaudio_float& currentSample = (*this)[i][j];
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
				}
			}
		}
		return maxSample;
	}
	hephaudio_float AudioBuffer::AbsMax() const noexcept
	{
		hephaudio_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				hephaudio_float currentSample = abs((*this)[i][j]);
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
				}
			}
		}
		return maxSample;
	}
	hephaudio_float AudioBuffer::Rms() const noexcept
	{
		hephaudio_float sumOfSamplesSquared = 0.0;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			for (size_t j = 0; j < this->formatInfo.channelCount; j++)
			{
				sumOfSamplesSquared += (*this)[i][j] * (*this)[i][j];
			}
		}
		return sqrt(sumOfSamplesSquared / this->frameCount / this->formatInfo.channelCount);
	}
	hephaudio_float AudioBuffer::CalculateDuration() const noexcept
	{
		return CalculateDuration(this->frameCount, this->formatInfo);
	}
	size_t AudioBuffer::CalculateFrameIndex(hephaudio_float ts) const noexcept
	{
		return CalculateFrameIndex(ts, this->formatInfo);
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
				AudioProcessor::ConvertSampleRate(*this, newFormat.sampleRate);
				AudioProcessor::ConvertChannels(*this, newFormat.channelCount);
			}
			this->formatInfo = newFormat;
		}
	}
	void AudioBuffer::SetChannelCount(uint16_t newChannelCount)
	{
		if (newChannelCount != this->formatInfo.channelCount)
		{
			if (this->frameCount > 0)
			{
				AudioProcessor::ConvertChannels(*this, newChannelCount);
			}
			this->formatInfo.channelCount = newChannelCount;
		}
	}
	void AudioBuffer::SetSampleRate(uint32_t newSampleRate)
	{
		if (newSampleRate != this->formatInfo.sampleRate)
		{
			if (this->frameCount > 0)
			{
				AudioProcessor::ConvertChannels(*this, newSampleRate);
			}
			this->formatInfo.sampleRate = newSampleRate;
		}
	}
	void* const& AudioBuffer::Begin() const noexcept
	{
		return this->pAudioData;
	}
	void* AudioBuffer::End() const noexcept
	{
		return (uint8_t*)this->pAudioData + this->Size();
	}
	hephaudio_float AudioBuffer::CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.ByteRate() == 0) { return 0.0; }
		return (hephaudio_float)frameCount * (hephaudio_float)formatInfo.FrameSize() / (hephaudio_float)formatInfo.ByteRate();
	}
	size_t AudioBuffer::CalculateFrameIndex(hephaudio_float ts, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.FrameSize() == 0) { return 0.0; }
		return ts * (hephaudio_float)formatInfo.ByteRate() / (hephaudio_float)formatInfo.FrameSize();
	}
#pragma endregion
}