#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class TukeyWindow : public Window
	{
	private:
		heph_float alpha;
		heph_float N;
		heph_float hN;
		heph_float aN;
		heph_float haN;
	public:
		TukeyWindow();
		TukeyWindow(size_t size);
		TukeyWindow(size_t size, heph_float alpha);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
		heph_float GetAlpha() const noexcept;
		void SetAlpha(heph_float alpha) noexcept;
	};
}