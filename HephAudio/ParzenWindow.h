#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class ParzenWindow : public Window
	{
	private:
		heph_float hN;
		heph_float hL;
		heph_float qL;
	public:
		ParzenWindow();
		ParzenWindow(size_t size);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
	};
}