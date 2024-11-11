#include "AudioEffects/ArctanDistortion.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	ArctanDistortion::ArctanDistortion() : ArctanDistortion(1.0) {}

	ArctanDistortion::ArctanDistortion(double factor) : AudioEffect()
	{
		this->SetFactor(factor);
	}

	std::string ArctanDistortion::Name() const
	{
		return "Arctan Distortion";
	}

	double ArctanDistortion::GetFactor() const
	{
		return this->factor - 1;
	}

	void ArctanDistortion::SetFactor(double factor)
	{
		if (factor < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "factor must not be negative."));
		}

		this->factor = factor + 1;
	}

	void ArctanDistortion::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				double fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(outputBuffer[i][j]);
				fltSample = atan(this->factor * fltSample) * (2.0 / HEPH_MATH_PI);

				outputBuffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(HEPH_MATH_MIN(HEPH_MATH_MAX(fltSample, -1.0), 1.0));
			}
		}
	}
}