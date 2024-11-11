#include "AudioEffects/Overdrive.h"
#include "Exceptions/InvalidArgumentException.h"
#include <cmath>

using namespace Heph;

namespace HephAudio
{
	Overdrive::Overdrive() : Overdrive(0) {}

	Overdrive::Overdrive(double drive) : AudioEffect()
	{
		this->SetDrive(drive);
	}

	std::string Overdrive::Name() const
	{
		return "Overdrive";
	}

	double Overdrive::GetDrive() const
	{
		return this->drive - 1;
	}

	void Overdrive::SetDrive(double drive)
	{
		if (drive < 0 || drive > 100)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "drive must be in the range of [0, 100]."));
		}

		this->drive = drive + 1;
	}

	void Overdrive::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				const double fltSample = tanh(this->drive * sin(HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(outputBuffer[i][j])));
				outputBuffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(HEPH_MATH_MIN(HEPH_MATH_MAX(fltSample, -1.0), 1.0));
			}
		}
	}
}