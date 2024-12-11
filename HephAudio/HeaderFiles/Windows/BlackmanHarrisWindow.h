#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Blackman-Harris window.
	 * 
	 */
	class HEPH_API BlackmanHarrisWindow final : public Window
	{
	private:
		double N;
	public:
		/** @copydoc default_constructor */
		BlackmanHarrisWindow();
		
		/** @copydoc Window(size_t) */
		explicit BlackmanHarrisWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}