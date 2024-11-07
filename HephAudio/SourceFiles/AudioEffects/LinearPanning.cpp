#include "AudioEffects/LinearPanning.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	LinearPanning::LinearPanning() : PanningEffect() {}

	LinearPanning::LinearPanning(double factor) : PanningEffect() 
	{
		this->SetFactor(factor);
	}

	std::string LinearPanning::Name() const
	{
		return "Linear Panning";
	}

	double LinearPanning::GetFactor() const
	{
		return (this->rightVolume - 0.5) * 2;
	}

	void LinearPanning::SetFactor(double factor)
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