#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates exact Blackman window.
	 * 
	 */
	class HEPH_API ExactBlackmanWindow final : public Window
	{
	private:
		double N;
	public:
		/** @copydoc default_constructor */
		ExactBlackmanWindow();
		
		/** @copydoc Window(size_t) */
		explicit ExactBlackmanWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}