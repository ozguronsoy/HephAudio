#include "AudioEffects/HardClipDistortion.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	HardClipDistortion::HardClipDistortion() : HardClipDistortion(0) {}

	HardClipDistortion::HardClipDistortion(double clippingLevel) : AudioEffect()
	{
		this->SetClippingLevel(clippingLevel);
	}

	std::string HardClipDistortion::Name() const
	{
		return "Hard Clipping Distortion";
	}

	double HardClipDistortion::GetClippingLevel() const
	{
		return HephAudio::GainToDecibel(this->clippingLevel / (double)HEPH_AUDIO_SAMPLE_MAX);
	}

	void HardClipDistortion::SetClippingLevel(double clippingLevel)
	{
		if (clippingLevel > 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "clippingLevel must be negative or zero."));
		}

		this->clippingLevel = HephAudio::DecibelToGain(clippingLevel) * HEPH_AUDIO_SAMPLE_MAX;
	}

	void HardClipDistortion::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				heph_audio_sample_t& sample = outputBuffer[i][j];
				if (sample > this->clippingLevel)
				{
					sample = this->clippingLevel;
				}
				else if (sample < -this->clippingLevel)
				{
					sample = -this->clippingLevel;
				}
			}
		}
	}
}