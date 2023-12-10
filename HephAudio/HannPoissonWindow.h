#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class HannPoissonWindow : public Window
	{
	private:
		heph_float N;
	public:
		heph_float alpha;
	public:
		HannPoissonWindow();
		HannPoissonWindow(size_t size);
		HannPoissonWindow(size_t size, heph_float alpha);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
	};
}