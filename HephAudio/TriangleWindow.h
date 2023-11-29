#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class TriangleWindow : public Window
	{
	private:
		heph_float hN;
		heph_float hL;
	public:
		TriangleWindow();
		TriangleWindow(size_t size);
		heph_float operator[](size_t n) const noexcept override;
		void SetSize(size_t newSize) noexcept override;
	};
}