#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class BlackmanNuttallWindow final : public Window
	{
	private:
		double N;
	public:
		BlackmanNuttallWindow();
		BlackmanNuttallWindow(size_t size);
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}