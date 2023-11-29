#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class SineWindow : public Window
	{
	private:
		heph_float N;
	public:
		SineWindow();
		SineWindow(size_t size);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
	};
}