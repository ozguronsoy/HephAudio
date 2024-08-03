#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class WelchWindow final : public Window
	{
	private:
		double hN;
	public:
		WelchWindow();
		WelchWindow(size_t size);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}