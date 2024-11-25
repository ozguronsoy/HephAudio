#include "AudioEffects/OlaEffect.h"
#include "Windows/HannWindow.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	OlaEffect::OlaEffect() : OlaEffect(0) {}

	OlaEffect::OlaEffect(size_t hopSize) : OlaEffect(hopSize, HannWindow(hopSize * 4)) {}

	OlaEffect::OlaEffect(size_t hopSize, const Window& wnd) : DoubleBufferedAudioEffect()
	{
		this->SetHopSize(hopSize);
		this->SetWindow(wnd);
	}

	size_t OlaEffect::GetHopSize() const
	{
		return this->hopSize;
	}

	void OlaEffect::SetHopSize(size_t hopSize)
	{
		if (hopSize == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "hopSize must be greater than zero."));
		}

		this->hopSize = hopSize;
	}

	size_t OlaEffect::GetWindowSize() const
	{
		return this->wnd.Size();
	}

	void OlaEffect::SetWindow(const Window& wnd)
	{
		if (wnd.GetSize() == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "window size must be greater than zero."));
		}

		this->wnd = wnd.GenerateBuffer();
	}

	size_t OlaEffect::CalculateMaxNumberOfOverlaps() const
	{
		return ceil(((double)this->GetWindowSize()) / ((double)this->hopSize));
	}
}