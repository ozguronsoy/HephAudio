#include "AudioEffects/Tremolo.h"

namespace HephAudio
{
	Tremolo::Tremolo() : ModulationEffect() {}

	Tremolo::Tremolo(double depth, const Oscillator& lfo) : ModulationEffect(depth, lfo) {}

	std::string Tremolo::Name() const
	{
		return "Tremolo";
	}

	void Tremolo::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		AudioEffect::Process(buffer, startIndex, frameCount);
		this->lfoIndex += frameCount;
	}

	void Tremolo::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const size_t channelCount = inputBuffer.FormatInfo().channelLayout.count;
		for (size_t i = startIndex; i < endIndex; ++i)
		{
			const double lfoSample = this->lfoBuffer[(i + this->lfoIndex) % this->lfoBuffer.Size()];
			for (size_t j = 0; j < channelCount; ++j)
			{
				outputBuffer[i][j] *= this->depth * lfoSample + (1.0 - this->depth);
			}
		}
	}
}