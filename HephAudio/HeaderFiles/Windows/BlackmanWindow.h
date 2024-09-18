#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Blackman window.
	 * 
	 */
	class BlackmanWindow final : public Window
	{
	private:
		double N;
	public:
		/** @copydoc default_constructor */
		BlackmanWindow();
		
		/** @copydoc Window(size_t) */
		BlackmanWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}