#include "AudioProcessor.h"
#include "AudioException.h"

using namespace HephAudio::Structs;

namespace HephAudio
{
	AudioProcessor::AudioProcessor(WAVEFORMATEX targetFormat)
	{
		this->targetFormat = targetFormat;
	}
	void AudioProcessor::ConvertBPS(AudioBuffer& buffer) const
	{
		if (buffer.wfx.wBitsPerSample == targetFormat.wBitsPerSample) { return; }
		const size_t frameCount = buffer.FrameCount();
		WAVEFORMATEX resultFormat = AudioBuffer::CreateWaveFormat(buffer.wfx.wFormatTag, buffer.wfx.nChannels, targetFormat.wBitsPerSample, buffer.wfx.nSamplesPerSec);
		AudioBuffer resultBuffer(frameCount, resultFormat);
		for (size_t i = 0; i < frameCount; i++)
		{
			for (uint8_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				double sample = buffer.Get(i, j);
				if (targetFormat.wBitsPerSample == 8)
				{
					sample += 1.0;
					sample /= 2.0;
				}
				else if (buffer.wfx.wBitsPerSample == 8)
				{
					sample *= 2.0;
					sample -= 1.0;
				}
				resultBuffer.Set(sample, i, j); // Get the normailzed data from current buffer and set it in the result buffer. (AudioBuffer will do the rest)
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::ConvertChannels(AudioBuffer& buffer) const
	{
		if (buffer.wfx.nChannels == targetFormat.nChannels) { return; }
		const size_t frameCount = buffer.FrameCount();
		WAVEFORMATEX resultFormat = AudioBuffer::CreateWaveFormat(buffer.wfx.wFormatTag, targetFormat.nChannels, buffer.wfx.wBitsPerSample, buffer.wfx.nSamplesPerSec);
		AudioBuffer resultBuffer(frameCount, resultFormat);
		for (size_t i = 0; i < frameCount; i++) // For each frame, find the average value and then set all the result channels to it.
		{
			double averageValue = 0.0f;
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				averageValue += buffer.Get(i, j);
			}
			averageValue /= buffer.wfx.nChannels;
			for (size_t j = 0; j < targetFormat.nChannels; j++)
			{
				resultBuffer.Set(averageValue, i, j);
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, size_t outFrameCount) const
	{
		if (buffer.wfx.nSamplesPerSec == targetFormat.nSamplesPerSec) { return; }
		const double srRatio = (double)targetFormat.nSamplesPerSec / (double)buffer.wfx.nSamplesPerSec;
		const size_t currentFrameCount = buffer.FrameCount();
		size_t targetFrameCount = outFrameCount;
		if (targetFrameCount == 0)
		{
			targetFrameCount = ceil((double)currentFrameCount * srRatio);
		}
		WAVEFORMATEX resultFormat = AudioBuffer::CreateWaveFormat(buffer.wfx.wFormatTag, buffer.wfx.nChannels, buffer.wfx.wBitsPerSample, targetFormat.nSamplesPerSec);
		AudioBuffer resultBuffer(targetFrameCount, resultFormat);
		const double cursorRatio = (1.0 / (targetFrameCount - 1)) * (currentFrameCount - 1);
		double cursor = 0.0;
		double A = 0.0;
		double B = 0.0;
		for (size_t i = 0; i < targetFrameCount; i++)
		{
			const double fc = floor(cursor);
			const double cursorFactor = cursor - fc;
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				A = buffer.Get(fc, j);
				if (fc + 1 < currentFrameCount)
				{
					B = buffer.Get(fc + 1, j);
				}
				else
				{
					B = buffer.Get(currentFrameCount - 1, j);
				}
				resultBuffer.Set(A * (1.0 - cursorFactor) + B * cursorFactor, i, j);
			}
			cursor += cursorRatio;
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::Reverse(AudioBuffer& buffer)
	{
		const size_t frameCount = buffer.FrameCount();
		AudioBuffer resultBuffer(frameCount, buffer.wfx);
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				resultBuffer.Set(buffer.Get(frameCount - 1 - i, j), i, j);
			}
		}
		buffer = resultBuffer;
	}
	std::vector<AudioBuffer> AudioProcessor::SplitChannels(AudioBuffer& buffer)
	{
		const size_t frameCount = buffer.FrameCount();
		WAVEFORMATEX resultFormat = AudioBuffer::CreateWaveFormat(buffer.wfx.wFormatTag, 1, buffer.wfx.wBitsPerSample, buffer.wfx.nSamplesPerSec);
		std::vector<AudioBuffer> channels(buffer.wfx.nChannels, AudioBuffer(frameCount, resultFormat));
		if (buffer.wfx.nChannels == 1)
		{
			channels.at(0) = buffer;
			return channels;
		}
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				channels.at(j).Set(buffer.Get(i, j), i, 0);
			}
		}
		return channels;
	}
	AudioBuffer AudioProcessor::MergeChannels(std::vector<AudioBuffer>& channels)
	{
		if (channels.size() == 0) { return AudioBuffer(0, WAVEFORMATEX()); }
		for (size_t i = 0; i < channels.size() - 1; i++)
		{
			if (!CompareWFX(channels.at(i).wfx, channels.at(i + 1).wfx))
			{
				throw AudioException(E_FAIL, L"AudioProcessor::MergeChannels", L"All channels must have the same wave format.");
			}
		}
		AudioBuffer resultBuffer(channels.at(0).FrameCount(), AudioBuffer::CreateWaveFormat(channels.at(0).wfx.wFormatTag, channels.size(), channels.at(0).wfx.wBitsPerSample, channels.at(0).wfx.nSamplesPerSec));
		for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < resultBuffer.wfx.nChannels; j++)
			{
				resultBuffer.Set(channels.at(j).Get(i, 0), i, j);
			}
		}
		return resultBuffer;
	}
	bool AudioProcessor::CompareWFX(const WAVEFORMATEX lhs, const WAVEFORMATEX rhs) noexcept
	{
		return lhs.wFormatTag == rhs.wFormatTag && lhs.wBitsPerSample == rhs.wBitsPerSample && lhs.nChannels == rhs.nChannels && lhs.nSamplesPerSec == rhs.nSamplesPerSec
			&& lhs.nBlockAlign == rhs.nBlockAlign && lhs.nAvgBytesPerSec == rhs.nAvgBytesPerSec;
	}
}