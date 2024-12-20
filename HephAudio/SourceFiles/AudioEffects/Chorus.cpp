#include "AudioEffects/Chorus.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	Chorus::Chorus() : Flanger(), extent(1.0) {}

	Chorus::Chorus(double depth, double constantDelay, double variableDelay, double extent, const Oscillator& lfo)
		: Flanger(depth, constantDelay, variableDelay, lfo), extent(1.0)
	{
		this->SetExtent(extent);
	}

	std::string Chorus::Name() const
	{
		return "Chorus";
	}

	size_t Chorus::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		if (this->extent < 0)
			return outputFrameCount;

		const double peakAmplitude = this->lfoBuffer.AbsMax();
		const double extent_sample = fabs(formatInfo.sampleRate * (pow(2, HephAudio::SemitoneToOctave(this->extent)) - 1.0));
		return outputFrameCount + peakAmplitude * extent_sample + 1;
	}

	double Chorus::GetExtent() const
	{
		return this->extent;
	}

	void Chorus::SetExtent(double extent)
	{
		this->extent = extent;
	}

	void Chorus::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const double extent_sample = formatInfo.sampleRate * (pow(2, HephAudio::SemitoneToOctave(this->extent)) - 1.0);
		const double constantDelay_sample = this->constantDelay * 1e-3 * formatInfo.sampleRate;
		const double variableDelay_sample = this->variableDelay * 1e-3 * formatInfo.sampleRate;

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			const double lfoSample = this->lfoBuffer[(i + this->lfoIndex) % this->lfoBuffer.Size()];
			const double delaySampleIndex = i - round(lfoSample * variableDelay_sample + constantDelay_sample);
			double resampleIndex = delaySampleIndex + lfoSample * extent_sample;
			const double resampleFactor = fabs(resampleIndex - floor(resampleIndex));

			resampleIndex = floor(resampleIndex);

			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				heph_audio_sample_t wetSample = 0;
				if (resampleIndex >= 0)
				{
					if (resampleIndex + 1 < inputBuffer.FrameCount())
						wetSample = inputBuffer[resampleIndex][j] * (1.0 - resampleFactor) + inputBuffer[resampleIndex + 1.0][j] * resampleFactor;
				}
				else
				{
					wetSample = this->pastSamples[resampleIndex + this->pastSamples.FrameCount()][j] * (1.0 - resampleFactor);
					wetSample += (resampleIndex == -1)
						? (inputBuffer[0][j] * resampleFactor)
						: (this->pastSamples[resampleIndex + this->pastSamples.FrameCount() + 1.0][j] * resampleFactor);
				}

				outputBuffer[i][j] = wetSample * this->depth + inputBuffer[i][j] * (1.0 - this->depth);
			}
		}
	}
	
	size_t Chorus::CalculatePastSamplesSize(const AudioBuffer& inputBuffer) const
	{
		const size_t maxDelay_sample = Flanger::CalculatePastSamplesSize(inputBuffer);
		const size_t extent_sample = fabs(ceil(inputBuffer.FormatInfo().sampleRate * (pow(2, HephAudio::SemitoneToOctave(this->extent)) - 1.0)));

		return maxDelay_sample + extent_sample + 1;
	}
}