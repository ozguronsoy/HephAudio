#include "AudioEffects/HighPassFilter.h"

using namespace Heph;

namespace HephAudio
{
	HighPassFilter::HighPassFilter() : Equalizer() {}

	HighPassFilter::HighPassFilter(double f, size_t hopSize, const Window& wnd) : Equalizer(hopSize, wnd, { Equalizer::FrequencyRange(0, f, 0) }) {}

	std::string HighPassFilter::Name() const
	{
		return "High Pass Filter";
	}

	double HighPassFilter::GetCutoffFreq() const
	{
		return this->frequencyRanges[0].f2;
	}

	void HighPassFilter::SetCutoffFreq(double f)
	{
		this->ModifyFrequencyRange(0, Equalizer::FrequencyRange(0, f, 0));
	}
}