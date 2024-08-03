#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class HannPoissonWindow final : public Window
	{
	private:
		double N;
	public:
		double alpha;
	public:
		HannPoissonWindow();
		HannPoissonWindow(size_t size);
		HannPoissonWindow(size_t size, double alpha);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}