#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class LanczosWindow final : public Window
	{
	private:
		double N;
	public:
		LanczosWindow();
		LanczosWindow(size_t size);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}