#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Bartlett-Hann window.
	 * 
	 */
	class HEPH_API BartlettHannWindow final : public Window
	{
	private:
		double N;
	public:
		/** @copydoc default_constructor */
		BartlettHannWindow();
		
		/** @copydoc Window(size_t) */
		BartlettHannWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}