#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class GaussianWindow : public Window
	{
	private:
		heph_float sigma;
		heph_float hN;
		heph_float shN;
	public:
		GaussianWindow();
		GaussianWindow(size_t size);
		GaussianWindow(size_t size, heph_float sigma);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
		heph_float GetSigma() const noexcept;
		void SetSigma(heph_float sigma) noexcept;
	};
}