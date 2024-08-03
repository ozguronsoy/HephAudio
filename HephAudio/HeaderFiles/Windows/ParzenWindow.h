#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class ParzenWindow final : public Window
	{
	private:
		double hN;
		double hL;
		double qL;
	public:
		ParzenWindow();
		ParzenWindow(size_t size);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}