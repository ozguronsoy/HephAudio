#include "AudioEffects/SineLawPanningEffect.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	SineLawPanningEffect::SineLawPanningEffect() : PanningEffect() {}

	SineLawPanningEffect::SineLawPanningEffect(double factor) : PanningEffect()
	{
		this->SetFactor(factor);
	}

	std::string SineLawPanningEffect::Name() const
	{
		return "Sine-Law Panning";
	}

	double SineLawPanningEffect::GetFactor() const
	{
		return (asin(this->rightVolume) / (HEPH_MATH_PI * 0.5) - 0.5) * 2;
	}

	void SineLawPanningEffect::SetFactor(double factor)
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