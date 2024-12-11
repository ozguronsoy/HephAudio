#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates welch window.
	 * 
	 */
	class HEPH_API WelchWindow final : public Window
	{
	private:
		double hN;
	public:
		/** @copydoc default_constructor */
		WelchWindow();
		
		/** @copydoc Window(size_t) */
		explicit WelchWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}