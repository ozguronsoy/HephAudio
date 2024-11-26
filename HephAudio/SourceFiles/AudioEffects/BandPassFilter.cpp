#include "AudioEffects/BandPassFilter.h"

using namespace Heph;

namespace HephAudio
{
	BandPassFilter::BandPassFilter() : Equalizer() {}

	BandPassFilter::BandPassFilter(double f1, double f2, size_t hopSize, const Window& wnd) : Equalizer(hopSize, wnd)
	{
		if (f1 > f2)
			std::swap(f1, f2);

		this->AddFrequencyRange(0, f1, 0);
		this->AddFrequencyRange(f2, DBL_MAX, 0);
	}

	std::string BandPassFilter::Name() const
	{
		return "Band Pass Filter";
	}

	double BandPassFilter::GetF1() const
	{
		return this->frequencyRanges[0].f2;
	}

	void BandPassFilter::SetF1(double f1)
	{
		double f2 = this->GetF2();
		if (f1 > f2)
			std::swap(f1, f2);

		this->ModifyFrequencyRange(0, Equalizer::FrequencyRange(0, f1, 0));
		this->ModifyFrequencyRange(0, Equalizer::FrequencyRange(f2, DBL_MAX, 0));
	}

	double BandPassFilter::GetF2() const
	{
		return this->frequencyRanges[1].f1;
	}

	void BandPassFilter::SetF2(double f2)
	{
		double f1 = this->GetF1();
		if (f1 > f2)
			std::swap(f2, f1);

		this->ModifyFrequencyRange(0, Equalizer::FrequencyRange(0, f1, 0));
		this->ModifyFrequencyRange(0, Equalizer::FrequencyRange(f2, DBL_MAX, 0));
	}
}