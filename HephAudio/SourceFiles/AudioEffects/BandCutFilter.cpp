#include "AudioEffects/BandCutFilter.h"

using namespace Heph;

namespace HephAudio
{
	BandCutFilter::BandCutFilter() : Equalizer() {}

	BandCutFilter::BandCutFilter(double f1, double f2, size_t hopSize, const Window& wnd) : Equalizer(hopSize, wnd, { Equalizer::FrequencyRange(f1, f2, 0) }) {}

	std::string BandCutFilter::Name() const
	{
		return "Band Cut Filter";
	}

	double BandCutFilter::GetF1() const
	{
		return this->frequencyRanges[0].f1;
	}

	void BandCutFilter::SetF1(double f)
	{
		this->ModifyFrequencyRange(0, Equalizer::FrequencyRange(f, this->GetF2(), 0));
	}

	double BandCutFilter::GetF2() const
	{
		return this->frequencyRanges[0].f2;
	}

	void BandCutFilter::SetF2(double f)
	{
		this->ModifyFrequencyRange(0, Equalizer::FrequencyRange(this->GetF1(), f, 0));
	}
}