#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "HephException.h"

namespace HephAudio
{
#pragma region Audio Frame
	AudioFrame::AudioFrame(const AudioBuffer* pAudioBuffer, size_t frameIndex)
		: pAudioBuffer(pAudioBuffer), frameIndex(frameIndex) { }
	hephaudio_float& AudioFrame::operator[](const size_t& channel) const
	{
		return *((hephaudio_float*)this->pAudioBuffer->Begin() + this->frameIndex * this->pAudioBuffer->FormatInfo().channelCount + channel);
	}
#pragma endregion
#pragma region Audio Buffer
	AudioBuffer::AudioBuffer()
		: formatInfo(AudioFormatInfo()), frameCount(0), pAudioData(nullptr) { }
	AudioBuffer::AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo)
		: formatInfo(formatInfo), frameCount(frameCount)
	{
		if (frameCount > 0)
		{
			// allocate memory and initialize it to 0.
			this->pAudioData = malloc(this->Size());
			if (this->pAudioData == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::AudioBuffer", "Insufficient memory.");
			}
			this->Reset();
		}
		else
		{
			this->pAudioData = nullptr;
		}
	}
	AudioBuffer::AudioBuffer(const AudioBuffer& rhs)
		: formatInfo(rhs.formatInfo), frameCount(rhs.frameCount)
	{
		if (rhs.frameCount > 0)
		{
			this->pAudioData = malloc(rhs.Size());
			if (this->pAudioData == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::AudioBuffer", "Insufficient memory.");
			}
			memcpy(this->pAudioData, rhs.pAudioData, rhs.Size());
		}
		else
		{
			this->pAudioData = nullptr;
		}
	}
	AudioBuffer::AudioBuffer(AudioBuffer&& rhs) noexcept
		: formatInfo(rhs.formatInfo), frameCount(rhs.frameCount), pAudioData(rhs.pAudioData)
	{
		rhs.frameCount = 0;
		rhs.formatInfo = AudioFormatInfo();
		rhs.pAudioData = nullptr;
	}
	AudioBuffer::~AudioBuffer()
	{
		this->Empty();
		this->formatInfo = AudioFormatInfo();
	}
	AudioFrame AudioBuffer::operator[](const size_t& frameIndex) const
	{
		return AudioFrame(this, frameIndex);
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
					throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::operator=", "Insufficient memory.");
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
	AudioBuffer AudioBuffer::operator<<(const size_t& rhs) const
	{
		AudioBuffer resultBuffer(this->frameCount, this->formatInfo);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pAudioData, (uint8_t*)this->pAudioData + rhs * this->formatInfo.FrameSize(), (this->frameCount - rhs) * this->formatInfo.FrameSize());
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator<<=(const size_t& rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pAudioData, (uint8_t*)this->pAudioData + rhs * this->formatInfo.FrameSize(), (this->frameCount - rhs) * this->formatInfo.FrameSize());
			memset((uint8_t*)this->pAudioData + (this->frameCount - rhs) * this->formatInfo.FrameSize(), 0, rhs * this->formatInfo.FrameSize());
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	AudioBuffer AudioBuffer::operator>>(const size_t& rhs) const
	{
		AudioBuffer resultBuffer(this->frameCount, this->formatInfo);
		if (this->frameCount > rhs)
		{
			memcpy((uint8_t*)resultBuffer.pAudioData + rhs * this->formatInfo.FrameSize(), this->pAudioData, (this->frameCount - rhs) * this->formatInfo.FrameSize());
		}
		return resultBuffer;
	}
	AudioBuffer& AudioBuffer::operator>>=(const size_t& rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy((uint8_t*)this->pAudioData + rhs * this->formatInfo.FrameSize(), this->pAudioData, (this->frameCount - rhs) * this->formatInfo.FrameSize());
			memset(this->pAudioData, 0, rhs * this->formatInfo.FrameSize());
		}
		else
		{
			this->Reset();
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
			void* tempPtr = malloc(this->Size() + buffer.Size());
			if (tempPtr == nullptr)
			{
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Join", "Insufficient memory.");
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
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Insert", "Insufficient memory.");
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
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Cut", "Insufficient memory.");
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
				throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Replace", "Insufficient memory.");
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
				this->Empty();
			}
			else
			{
				void* tempPtr = realloc(this->pAudioData, newFrameCount * this->formatInfo.FrameSize());
				if (tempPtr == nullptr)
				{
					throw HephCommon::HephException(E_OUTOFMEMORY, "AudioBuffer::Resize", "Insufficient memory.");
				}
				this->pAudioData = tempPtr;
				this->frameCount = newFrameCount;
			}
		}
	}
	void AudioBuffer::Empty() noexcept
	{
		this->frameCount = 0;
		if (this->pAudioData != nullptr)
		{
			free(this->pAudioData);
			this->pAudioData = nullptr;
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
	size_t AudioBuffer::CalculateFrameIndex(hephaudio_float t_s) const noexcept
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
				AudioProcessor::ConvertSampleRate(*this, newSampleRate);
			}
			this->formatInfo.sampleRate = newSampleRate;
		}
	}
	void* AudioBuffer::Begin() const noexcept
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
	size_t AudioBuffer::CalculateFrameIndex(hephaudio_float t_s, AudioFormatInfo formatInfo) noexcept
	{
		if (formatInfo.FrameSize() == 0) { return 0.0; }
		return t_s * (hephaudio_float)formatInfo.ByteRate() / (hephaudio_float)formatInfo.FrameSize();
	}
#pragma endregion
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