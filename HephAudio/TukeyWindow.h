#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class TukeyWindow final : public Window
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
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
		heph_float GetAlpha() const;
		void SetAlpha(heph_float alpha);
	};
}