#include "AudioEffects/ModulationEffect.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	ModulationEffect::ModulationEffect() : AudioEffect(), lfoIndex(0), depth(1.0) {}

	ModulationEffect::ModulationEffect(double depth, Oscillator& lfo) : ModulationEffect()
	{
		this->SetOscillator(lfo);
		this->SetDepth(depth);
	}

	void ModulationEffect::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		if (startIndex + frameCount > buffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "(startIndex + frameCount) exceeds the buffer's frame count."));
		}

		const AudioFormatInfo& formatInfo = buffer.FormatInfo();
		AudioBuffer resultBuffer(buffer.FrameCount(), formatInfo.channelLayout, formatInfo.sampleRate, BufferFlags::AllocUninitialized);

		if (startIndex > 0)
		{
			(void)memcpy(resultBuffer.begin(), buffer.begin(), startIndex * formatInfo.FrameSize());
		}

		if (this->threadCount == 1)
			this->ProcessST(buffer, resultBuffer, startIndex, frameCount);
		else
			this->ProcessMT(buffer, resultBuffer, startIndex, frameCount);

		buffer = std::move(resultBuffer);
		this->lfoIndex += frameCount;
	}

	void ModulationEffect::SetOscillator(Oscillator& lfo)
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