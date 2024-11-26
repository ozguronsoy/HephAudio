#include "AudioEffects/LowPassFilter.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Fourier.h"

using namespace Heph;

namespace HephAudio
{
	LowPassFilter::LowPassFilter() : Equalizer() {}

	LowPassFilter::LowPassFilter(double f, size_t hopSize, const Window& wnd) : Equalizer(hopSize, wnd, { Equalizer::FrequencyRange(f, DBL_MAX, 0) }) {}

	std::string LowPassFilter::Name() const
	{
		return "Low Pass Filter";
	}

	double LowPassFilter::GetCutoffFreq() const
	{
		return this->frequencyRanges[0].f1;
	}

	void LowPassFilter::SetCutoffFreq(double f)
	{
		this->RemoveFrequencyRange(0);
		this->AddFrequencyRange(f, DBL_MAX, 0);
	}
}