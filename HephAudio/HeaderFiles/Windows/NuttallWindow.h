#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Nuttall window.
	 * 
	 */
	class HEPH_API NuttallWindow final : public Window
	{
	private:
		double N;

	public:
		/** @copydoc default_constructor */
		NuttallWindow();
		
		/** @copydoc Window(size_t) */
		NuttallWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}