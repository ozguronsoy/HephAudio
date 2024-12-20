#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Hamming window.
	 * 
	 */
	class HEPH_API HammingWindow final : public Window
	{
	private:
		double N;

	public:
		/** @copydoc default_constructor */
		HammingWindow();
		
		/** @copydoc Window(size_t) */
		explicit HammingWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}