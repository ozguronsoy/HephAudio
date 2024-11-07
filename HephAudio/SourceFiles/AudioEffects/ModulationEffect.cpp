#include "AudioEffects/ModulationEffect.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	ModulationEffect::ModulationEffect() : DoubleBufferedAudioEffect(), lfoIndex(0), depth(1.0) {}

	ModulationEffect::ModulationEffect(double depth, const Oscillator& lfo) : ModulationEffect()
	{
		this->SetOscillator(lfo);
		this->SetDepth(depth);
	}

	void ModulationEffect::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		DoubleBufferedAudioEffect::Process(buffer, startIndex, frameCount);
		this->lfoIndex += frameCount;
	}

	void ModulationEffect::SetOscillator(const Oscillator& lfo)
	{
		this->lfoBuffer = lfo.GenerateBuffer();
		for (double& sample : this->lfoBuffer)
			sample = (sample + lfo.peakAmplitude) * 0.5;
	}

	double ModulationEffect::GetDepth() const
	{
		return this->depth;
	}

	void ModulationEffect::SetDepth(double depth)
	{
		if (depth < 0 || depth > 1)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "depth must be between 0 and 1."));
		}

		this->depth = depth;
	}
}