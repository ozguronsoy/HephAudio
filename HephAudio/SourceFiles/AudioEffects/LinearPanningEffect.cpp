#include "AudioEffects/LinearPanningEffect.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	LinearPanningEffect::LinearPanningEffect() : PanningEffect() {}

	LinearPanningEffect::LinearPanningEffect(double factor) : PanningEffect() 
	{
		this->SetFactor(factor);
	}

	std::string LinearPanningEffect::Name() const
	{
		return "Linear Panning";
	}

	double LinearPanningEffect::GetFactor() const
	{
		return (this->rightVolume - 0.5) * 2;
	}

	void LinearPanningEffect::SetFactor(double factor)
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

		this->rightVolume = factor * 0.5 + 0.5;
		this->leftVolume = 1.0 - this->rightVolume;
	}
}