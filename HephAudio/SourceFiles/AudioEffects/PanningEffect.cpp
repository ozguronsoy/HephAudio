#include "AudioEffects/PanningEffect.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	PanningEffect::PanningEffect() : AudioEffect(), leftVolume(1), rightVolume(1) {}

	void PanningEffect::ProcessST(AudioBuffer& buffer, size_t startIndex, size_t endIndex)
	{
		for (size_t i = startIndex; i < endIndex; ++i)
		{
			buffer[i][0] *= this->leftVolume;
			buffer[i][1] *= this->rightVolume;
		}
	}
}