#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class BartlettHannWindow final : public Window
	{
	private:
		heph_float N;
	public:
		BartlettHannWindow();
		BartlettHannWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}