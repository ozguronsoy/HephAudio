#include "AudioEffects/LinearFadeOut.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	LinearFadeOut::LinearFadeOut() : LinearFadeOut(0.5) {}

	LinearFadeOut::LinearFadeOut(double duration) : LinearFadeOut(duration, 0) {}

	LinearFadeOut::LinearFadeOut(double duration, double startTime) : AudioEffect(), currentIndex(0)
	{
		this->SetDuration(duration);
		this->SetStartTime(startTime);
	}

	std::string LinearFadeOut::Name() const
	{
		return "Linear Fade-out";
	}

	void LinearFadeOut::ResetInternalState()
	{
		this->currentIndex = 0;
	}

	void LinearFadeOut::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		AudioEffect::Process(buffer, startIndex, frameCount);
		this->currentIndex += frameCount;
	}

	double LinearFadeOut::GetDuration() const
	{
		return this->duration;
	}

	void LinearFadeOut::SetDuration(double duration)
	{
		if (duration < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "duration cannot be negative."));
		}

		this->duration = duration;
	}

	double LinearFadeOut::GetStartTime() const
	{
		return this->startTime;
	}

	void LinearFadeOut::SetStartTime(double startTime)
	{
		if (startTime < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "startTime cannot be negative."));
		}

		this->startTime = startTime;
	}

	void LinearFadeOut::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		startIndex += this->currentIndex;
		size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();
		const size_t startTime_sample = formatInfo.sampleRate * this->startTime;
		const size_t duration_sample = formatInfo.sampleRate * this->duration;

		if (endIndex > startTime_sample && startIndex < (startTime_sample + duration_sample))
		{
			startIndex = HEPH_MATH_MAX(startIndex, startTime_sample) - this->currentIndex;
			endIndex = HEPH_MATH_MIN(endIndex, (duration_sample + startTime_sample)) - this->currentIndex;

			for (size_t i = startIndex; i < endIndex; ++i)
			{
				const double factor = ((startTime_sample + duration_sample) - (i + this->currentIndex)) / ((double)duration_sample);
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					outputBuffer[i][j] *= factor;
				}
			}
		}
	}
}