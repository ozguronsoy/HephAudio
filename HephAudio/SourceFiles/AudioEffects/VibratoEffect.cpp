#include "AudioEffects/VibratoEffect.h"
#include <cmath>

using namespace Heph;

namespace HephAudio
{
	VibratoEffect::VibratoEffect() : ModulationEffect(), extent(1.0) {}

	VibratoEffect::VibratoEffect(double depth, double extent, Oscillator& lfo) : ModulationEffect(depth, lfo), extent(extent) {}

	std::string VibratoEffect::Name() const
	{
		return "Vibrato";
	}

	size_t VibratoEffect::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		const double peakAmplitude = this->lfoBuffer.AbsMax();
		const double extent_sample = formatInfo.sampleRate * (pow(2, this->extent / 12.0) - 1.0);
		return outputFrameCount + peakAmplitude * extent_sample + 1;
	}

	void VibratoEffect::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const double extent_sample = formatInfo.sampleRate * (pow(2, HephAudio::SemitoneToOctave(this->extent)) - 1.0);

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			const double resampleIndex = i + this->lfoBuffer[(i + this->lfoIndex) % this->lfoBuffer.Size()] * extent_sample;
			const double resampleFactor = resampleIndex - floor(resampleIndex);

			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				heph_audio_sample_t wetSample = 0;
				if (resampleIndex + 1 < inputBuffer.FrameCount())
				{
					wetSample = inputBuffer[resampleIndex][j] * (1.0 - resampleFactor) + inputBuffer[resampleIndex + 1.0][j] * resampleFactor;
				}

				outputBuffer[i][j] = wetSample * this->depth + inputBuffer[i][j] * (1.0 - this->depth);
			}
		}
	}
}