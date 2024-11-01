#include "AudioEffects/TremoloEffect.h"

namespace HephAudio
{
	TremoloEffect::TremoloEffect() : ModulationEffect() {}

	TremoloEffect::TremoloEffect(double depth, Oscillator& lfo) : ModulationEffect(depth, lfo) {}

	std::string TremoloEffect::Name() const
	{
		return "Tremolo";
	}

	void TremoloEffect::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const size_t channelCount = inputBuffer.FormatInfo().channelLayout.count;
		for (size_t i = startIndex; i < endIndex; ++i)
		{
			const double lfoSample = this->lfoBuffer[(i + this->lfoIndex) % this->lfoBuffer.Size()];
			for (size_t j = 0; j < channelCount; ++j)
			{
				outputBuffer[i][j] = inputBuffer[i][j] * this->depth * lfoSample + inputBuffer[i][j] * (1.0 - this->depth);
			}
		}
	}
}