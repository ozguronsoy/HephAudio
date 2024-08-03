#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class GaussianWindow final : public Window
	{
	private:
		double sigma;
		double hN;
		double shN;
	public:
		GaussianWindow();
		GaussianWindow(size_t size);
		GaussianWindow(size_t size, double sigma);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
		double GetSigma() const;
		void SetSigma(double sigma);
	};
}