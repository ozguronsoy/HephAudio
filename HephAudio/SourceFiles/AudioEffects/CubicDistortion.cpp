#include "AudioEffects/CubicDistortion.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	CubicDistortion::CubicDistortion() : CubicDistortion(1.0) {}

	CubicDistortion::CubicDistortion(double factor) : AudioEffect()
	{
		this->SetFactor(factor);
	}

	std::string CubicDistortion::Name() const
	{
		return "Cubic Distortion";
	}

	double CubicDistortion::GetFactor() const
	{
		return this->factor - 1;
	}

	void CubicDistortion::SetFactor(double factor)
	{
		if (factor < 0 || factor > 10)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "factor must be in the range of [0, 10]."));
		}

		this->factor = factor + 1;
	}

	void CubicDistortion::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				double fltSample = this->factor * HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(outputBuffer[i][j]);
				fltSample -= fltSample * fltSample * fltSample / 3.0;

				outputBuffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(HEPH_MATH_MIN(HEPH_MATH_MAX(fltSample, -1.0), 1.0));
			}
		}
	}
}