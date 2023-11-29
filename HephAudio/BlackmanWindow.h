#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class BlackmanWindow : public Window
	{
	private:
		heph_float N;
	public:
		BlackmanWindow();
		BlackmanWindow(size_t size);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
	};
}