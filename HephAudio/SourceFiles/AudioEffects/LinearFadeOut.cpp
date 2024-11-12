#include "AudioEffects/LinearFadeOut.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	LinearFadeOut::LinearFadeOut() : LinearFadeOut(0.5) {}

	LinearFadeOut::LinearFadeOut(double duration) : LinearFadeOut(duration, 0) {}

	LinearFadeOut::LinearFadeOut(double duration, size_t startIndex) : AudioEffect(), currentIndex(0)
	{
		this->SetDuration(duration);
		this->SetStartIndex(startIndex);
	}

	std::string LinearFadeOut::Name() const
	{
		return "Linear Fade-out";
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

	size_t LinearFadeOut::GetStartIndex() const
	{
		return this->startIndex;
	}

	void LinearFadeOut::SetStartIndex(size_t startIndex)
	{
		this->startIndex = startIndex;
	}

	void LinearFadeOut::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		startIndex += this->currentIndex;
		size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();
		const double duration_sample = formatInfo.sampleRate * this->duration;

		if (endIndex > this->startIndex && startIndex < (this->startIndex + duration_sample))
		{
			startIndex = HEPH_MATH_MAX(startIndex, this->startIndex) - this->currentIndex;
			endIndex = HEPH_MATH_MIN(endIndex, (duration_sample + this->startIndex)) - this->currentIndex;

			for (size_t i = startIndex; i < endIndex; ++i)
			{
				const double factor = ((this->startIndex + duration_sample) - (i + this->currentIndex)) / duration_sample;
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					outputBuffer[i][j] *= factor;
				}
			}
		}
	}
}