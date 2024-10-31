#include "AudioEffects/SquareLawPanningEffect.h"
#include "Exceptions/InvalidArgumentException.h"
#include <cmath>

using namespace Heph;

namespace HephAudio
{
	SquareLawPanningEffect::SquareLawPanningEffect() : PanningEffect() {}
	
	SquareLawPanningEffect::SquareLawPanningEffect(double factor) : PanningEffect() 
	{
		this->SetFactor(factor);
	}

	std::string SquareLawPanningEffect::Name() const
	{
		return "Square-Law Panning";
	}

	double SquareLawPanningEffect::GetFactor() const
	{
		return (this->rightVolume * this->rightVolume - 0.5) * 2;
	}

	void SquareLawPanningEffect::SetFactor(double factor)
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
		this->rightVolume = sqrt(volume);
		this->leftVolume = sqrt(1.0 - volume);
	}
}