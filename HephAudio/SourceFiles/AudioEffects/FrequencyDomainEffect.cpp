#include "AudioEffects/FrequencyDomainEffect.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Windows/HannWindow.h"
#include "Fourier.h"

using namespace Heph;

namespace HephAudio
{
	FrequencyDomainEffect::FrequencyDomainEffect() : OlaEffect() {}

	FrequencyDomainEffect::FrequencyDomainEffect(size_t hopSize) : FrequencyDomainEffect(hopSize, HannWindow(Fourier::CalculateFFTSize(hopSize * 4))) {}

	FrequencyDomainEffect::FrequencyDomainEffect(size_t hopSize, const Window& wnd) : OlaEffect(hopSize)
	{
		this->SetWindow(wnd);
	}

	void FrequencyDomainEffect::SetWindow(const Window& wnd)
	{
		const size_t n = wnd.GetSize();
		if (n == 0 || (n & (n - 1)) != 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "window size must be a power of 2."));
		}

		OlaEffect::SetWindow(wnd);
	}
}