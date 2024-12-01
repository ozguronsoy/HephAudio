#include "AudioEffects/LinearFadeIn.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	LinearFadeIn::LinearFadeIn() : LinearFadeIn(0.5) {}

	LinearFadeIn::LinearFadeIn(double duration) : LinearFadeIn(duration, 0) {}

	LinearFadeIn::LinearFadeIn(double duration, double startTime) : AudioEffect(), currentIndex(0)
	{
		this->SetDuration(duration);
		this->SetStartTime(startTime);
	}

	std::string LinearFadeIn::Name() const
	{
		return "Linear Fade-in";
	}

	void LinearFadeIn::ResetInternalState()
	{
		this->currentIndex = 0;
	}

	void LinearFadeIn::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		AudioEffect::Process(buffer, startIndex, frameCount);
		this->currentIndex += frameCount;
	}

	double LinearFadeIn::GetDuration() const
	{
		return this->duration;
	}

	void LinearFadeIn::SetDuration(double duration)
	{
		if (duration < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "duration cannot be negative."));
		}

		this->duration = duration;
	}

	double LinearFadeIn::GetStartTime() const
	{
		return this->startTime;
	}

	void LinearFadeIn::SetStartTime(double startTime)
	{
		if (startTime < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "startTime cannot be negative."));
		}

		this->startTime = startTime;
	}

	void LinearFadeIn::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
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
				const double factor = (i + this->currentIndex - startTime_sample) / ((double)duration_sample);
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					outputBuffer[i][j] *= factor;
				}
			}
		}
	}
}