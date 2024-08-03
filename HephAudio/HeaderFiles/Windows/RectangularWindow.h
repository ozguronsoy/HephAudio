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
		double operator[](size_t n) const override;
	};
}