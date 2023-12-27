#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class SquareWindow final : public Window
	{
	public:
		SquareWindow();
		SquareWindow(size_t size);
		heph_float operator[](size_t n) const override;
	};
}