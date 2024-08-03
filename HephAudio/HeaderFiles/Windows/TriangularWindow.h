#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class TriangularWindow final : public Window
	{
	private:
		double hN;
		double hL;
	public:
		TriangularWindow();
		TriangularWindow(size_t size);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}