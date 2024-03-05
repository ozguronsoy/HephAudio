#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class LanczosWindow final : public Window
	{
	private:
		heph_float N;
	public:
		LanczosWindow();
		LanczosWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}