#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class RectangularWindow final : public Window
	{
	public:
		RectangularWindow();
		RectangularWindow(size_t size);
		heph_float operator[](size_t n) const override;
	};
}