#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class SineWindow final : public Window
	{
	private:
		double N;
	public:
		SineWindow();
		SineWindow(size_t size);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}