#include "AudioEffects/Echo.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	Echo::Echo() : Echo(0, 0, 0, 0, 0) {}

	Echo::Echo(size_t reflectionCount, double reflectionDelay, double decayFactor, double echoStart, double echoDuration)
		: AudioEffect(), currentIndex(0)
	{
		this->SetReflectionCount(reflectionCount);
		this->SetReflectionDelay(reflectionDelay);
		this->SetDecayFactor(decayFactor);
		this->SetEchoStart(echoStart);
		this->SetEchoDuration(echoDuration);
	}

	std::string Echo::Name() const
	{
		return "Echo";
	}

	void Echo::ResetInternalState()
	{
		this->pastSamples.Release();
		this->currentIndex = 0;
	}

	void Echo::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		const AudioFormatInfo& formatInfo = buffer.FormatInfo();
		const size_t pastSamplesSize = this->echoDuration * formatInfo.sampleRate;

		if (pastSamplesSize != this->pastSamples.FrameCount())
		{
			this->pastSamples = AudioBuffer(pastSamplesSize, formatInfo.channelLayout, formatInfo.sampleRate);
		}

		const size_t echoStartIndex = this->echoStart * formatInfo.sampleRate;
		const size_t echoEndIndex = echoStartIndex + pastSamplesSize;
		size_t i1 = this->currentIndex + startIndex;
		const size_t i2 = HEPH_MATH_MIN(i1 + frameCount, echoEndIndex);

		if (i1 < echoEndIndex && i2 > echoStartIndex)
		{
			i1 = HEPH_MATH_MAX(i1, echoStartIndex);
			for (size_t i = i1; i < i2; ++i)
			{
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					this->pastSamples[i - echoStartIndex][j] = buffer[i - i1][j];
				}
			}
		}

		AudioEffect::Process(buffer, startIndex, frameCount);
		this->currentIndex += frameCount;
	}

	size_t Echo::GetReflectionCount() const
	{
		return this->reflectionCount;
	}

	void Echo::SetReflectionCount(size_t reflectionCount)
	{
		this->reflectionCount = reflectionCount;
	}

	double Echo::GetReflectionDelay() const
	{
		return this->reflectionDelay;
	}

	void Echo::SetReflectionDelay(double reflectionDelay)
	{
		if (reflectionDelay < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "reflectionDelay cannot be negative."));
		}

		this->reflectionDelay = reflectionDelay;
	}

	double Echo::GetDecayFactor() const
	{
		return this->decayFactor;
	}

	void Echo::SetDecayFactor(double decayFactor)
	{
		this->decayFactor = decayFactor;
	}

	double Echo::GetEchoStart() const
	{
		return this->echoStart;
	}

	void Echo::SetEchoStart(double echoStart)
	{
		if (echoStart < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "echoStart must not be negative."));
		}

		this->echoStart = echoStart;
	}

	double Echo::GetEchoDuration() const
	{
		return this->echoDuration;
	}

	void Echo::SetEchoDuration(double echoDuration)
	{
		if (echoDuration < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "echoDuration must not be negative."));
		}

		this->echoDuration = echoDuration;
	}

	void Echo::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				outputBuffer[i][j] /= this->reflectionCount;
			}
		}

		const size_t echoStartIndex = this->echoStart * formatInfo.sampleRate;
		const size_t echoDuration_sample = this->echoDuration * formatInfo.sampleRate;
		const size_t reflectionDelay_sample = this->reflectionDelay * formatInfo.sampleRate;
		double factor = this->decayFactor / this->reflectionCount;

		for (size_t i = 0; i < this->reflectionCount; ++i, factor *= this->decayFactor)
		{
			const size_t reflectionStartIndex = echoStartIndex + (i + 1) * reflectionDelay_sample;
			const size_t reflectionEndIndex = reflectionStartIndex + echoDuration_sample;
			const size_t j1 = this->currentIndex + startIndex;
			const size_t j2 = HEPH_MATH_MIN(j1 + frameCount, reflectionEndIndex);

			if (j1 < reflectionEndIndex && j2 > reflectionStartIndex)
			{
				for (size_t j = HEPH_MATH_MAX(j1, reflectionStartIndex); j < j2; ++j)
				{
					for (size_t k = 0; k < formatInfo.channelLayout.count; ++k)
					{
						outputBuffer[j - this->currentIndex][k] += this->pastSamples[j - reflectionStartIndex][k] * factor;
					}
				}
			}
		}
	}
}