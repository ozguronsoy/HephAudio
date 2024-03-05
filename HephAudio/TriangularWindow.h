#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class TriangularWindow final : public Window
	{
	private:
		heph_float hN;
		heph_float hL;
	public:
		TriangularWindow();
		TriangularWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}