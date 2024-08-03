#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class TukeyWindow final : public Window
	{
	private:
		double alpha;
		double N;
		double hN;
		double aN;
		double haN;
	public:
		TukeyWindow();
		TukeyWindow(size_t size);
		TukeyWindow(size_t size, double alpha);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
		double GetAlpha() const;
		void SetAlpha(double alpha);
	};
}