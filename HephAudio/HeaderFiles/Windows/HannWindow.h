#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Hann window.
	 * 
	 */
	class HEPH_API HannWindow final : public Window
	{
	private:
		double N;

	public:
		/** @copydoc default_constructor */
		HannWindow();
		
		/** @copydoc Window(size_t) */
		explicit HannWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}