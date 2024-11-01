#include "AudioEffects/PanningEffect.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	PanningEffect::PanningEffect() : AudioEffect(), leftVolume(1), rightVolume(1) {}

	void PanningEffect::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		if (buffer.FormatInfo().channelLayout != HEPHAUDIO_CH_LAYOUT_STEREO)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "buffer must be stereo."));
		}
		AudioEffect::Process(buffer, startIndex, frameCount);
	}

	void PanningEffect::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		for (size_t i = startIndex; i < endIndex; ++i)
		{
			outputBuffer[i][0] *= this->leftVolume;
			outputBuffer[i][1] *= this->rightVolume;
		}
	}
}