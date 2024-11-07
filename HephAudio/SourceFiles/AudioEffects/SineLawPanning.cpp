#include "AudioEffects/SineLawPanning.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	SineLawPanning::SineLawPanning() : PanningEffect() {}

	SineLawPanning::SineLawPanning(double factor) : PanningEffect()
	{
		this->SetFactor(factor);
	}

	std::string SineLawPanning::Name() const
	{
		return "Sine-Law Panning";
	}

	double SineLawPanning::GetFactor() const
	{
		return (asin(this->rightVolume) / (HEPH_MATH_PI * 0.5) - 0.5) * 2;
	}

	void SineLawPanning::SetFactor(double factor)
	{
		if (factor < -1)
		{
			HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "factor must be between -1 and 1, using -1 instead."));
			factor = -1;
		}

		if (factor > 1)
		{
			HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "factor must be between -1 and 1, using 1 instead."));
			factor = 1;
		}

		const double volume = factor * 0.5 + 0.5;
		this->rightVolume = sin(volume * (HEPH_MATH_PI * 0.5));
		this->leftVolume = sin((1.0 - volume) * (HEPH_MATH_PI * 0.5));
	}
}