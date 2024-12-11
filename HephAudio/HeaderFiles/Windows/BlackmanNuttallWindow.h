#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Blackman-Nuttall window.
	 * 
	 */
	class HEPH_API BlackmanNuttallWindow final : public Window
	{
	private:
		double N;
	public:
		/** @copydoc default_constructor */
		BlackmanNuttallWindow();
		
		/** @copydoc Window(size_t) */
		explicit BlackmanNuttallWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}