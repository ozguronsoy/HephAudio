#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class FlatTopWindow : public Window
	{
	private:
		heph_float N;
	public:
		FlatTopWindow();
		FlatTopWindow(size_t size);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
	};
}