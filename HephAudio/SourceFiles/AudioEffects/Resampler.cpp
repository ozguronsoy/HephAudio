#include "AudioEffects/Resampler.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	Resampler::Resampler() : Resampler(48000) {}

	Resampler::Resampler(size_t outputSampleRate) : DoubleBufferedAudioEffect(), outputSampleRate(outputSampleRate) {}

	std::string Resampler::Name() const
	{
		return "Resampler";
	}

	size_t Resampler::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return ceil(outputFrameCount * ((double)formatInfo.sampleRate / this->outputSampleRate)) + 1;
	}

	size_t Resampler::CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		if (formatInfo.sampleRate == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "input sample rate cannot be 0."));
		}

		return inputFrameCount * ((double)this->outputSampleRate / formatInfo.sampleRate);
	}

	size_t Resampler::GetOutputSampleRate() const
	{
		return this->outputSampleRate;
	}

	void Resampler::SetOutputSampleRate(size_t outputSampleRate)
	{
		if (outputSampleRate == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "output sample rate cannot be 0."));
		}

		this->outputSampleRate = outputSampleRate;
	}

	void Resampler::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
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

	AudioBuffer Resampler::CreateOutputBuffer(const AudioBuffer& inputBuffer, size_t startIndex, size_t frameCount) const
	{
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		return AudioBuffer(
			(inputBuffer.FrameCount() - frameCount) + this->CalculateOutputFrameCount(frameCount, formatInfo),
			formatInfo.channelLayout,
			this->outputSampleRate, BufferFlags::AllocUninitialized);
	}

	void Resampler::InitializeOutputBuffer(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) const
	{
		if (startIndex != 0 || frameCount != inputBuffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "this effect must be applied to the entire buffer."));
		}

		outputBuffer.Reset();
	}
}