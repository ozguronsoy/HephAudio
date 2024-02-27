#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class BlackmanHarrisWindow final : public Window
	{
	private:
		heph_float N;
	public:
		BlackmanHarrisWindow();
		BlackmanHarrisWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}