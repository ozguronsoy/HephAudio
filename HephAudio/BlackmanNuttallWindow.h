#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class BlackmanNuttallWindow : public Window
	{
	private:
		heph_float N;
	public:
		BlackmanNuttallWindow();
		BlackmanNuttallWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}