#include "AudioEffects/ChangeSampleRateEffect.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	ChangeSampleRateEffect::ChangeSampleRateEffect() : ChangeSampleRateEffect(48000) {}

	ChangeSampleRateEffect::ChangeSampleRateEffect(size_t outputSampleRate) : DoubleBufferedAudioEffect(), outputSampleRate(outputSampleRate) {}

	std::string ChangeSampleRateEffect::Name() const
	{
		return "Change Sample Rate";
	}

	size_t ChangeSampleRateEffect::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return ceil(outputFrameCount * ((double)formatInfo.sampleRate / this->outputSampleRate));
	}

	size_t ChangeSampleRateEffect::CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		if (formatInfo.sampleRate == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "input sample rate cannot be 0."));
		}

		return inputFrameCount * ((double)this->outputSampleRate / formatInfo.sampleRate);
	}

	void ChangeSampleRateEffect::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		DoubleBufferedAudioEffect::Process(buffer, startIndex, frameCount);
		buffer.SetSampleRate(this->outputSampleRate);
	}

	size_t ChangeSampleRateEffect::GetOutputSampleRate() const
	{
		return this->outputSampleRate;
	}

	void ChangeSampleRateEffect::SetOutputSampleRate(size_t outputSampleRate)
	{
		if (outputSampleRate == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "output sample rate cannot be 0."));
		}

		this->outputSampleRate = outputSampleRate;
	}

	void ChangeSampleRateEffect::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const AudioFormatInfo& inputFormatInfo = inputBuffer.FormatInfo();
		if (inputFormatInfo.sampleRate == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "input sample rate cannot be 0."));
		}

		const double srRatio = (double)this->outputSampleRate / (double)inputFormatInfo.sampleRate;

		startIndex = startIndex * srRatio;
		frameCount = frameCount * srRatio;
		const size_t endIndex = startIndex + frameCount;

		if (srRatio != 1)
		{
			for (size_t i = startIndex; i < endIndex; ++i)
			{
				const double resampleIndex = i / srRatio;
				const double resampleFactor = resampleIndex - floor(resampleIndex);

				for (size_t j = 0; j < inputFormatInfo.channelLayout.count; ++j)
				{
					outputBuffer[i][j] = inputBuffer[resampleIndex][j] * (1.0 - resampleFactor);
					if ((resampleIndex + 1) < inputBuffer.FrameCount())
					{
						outputBuffer[i][j] += inputBuffer[resampleIndex + 1][j] * resampleFactor;
					}
				}
			}
		}
	}
}