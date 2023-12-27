#pragma once
#include "HephAudioFramework.h"
#include "Window.h"

namespace HephAudio
{
	class ExactBlackmanWindow : public Window
	{
	private:
		heph_float N;
	public:
		ExactBlackmanWindow();
		ExactBlackmanWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}