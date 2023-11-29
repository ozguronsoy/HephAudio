#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class WelchWindow : public Window
	{
	private:
		heph_float hN;
	public:
		WelchWindow();
		WelchWindow(size_t size);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
	};
}