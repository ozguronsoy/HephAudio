#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates rectangular window
	 * 
	 */
	class HEPH_API RectangularWindow final : public Window
	{
	public:
		/** @copydoc default_constructor */
		RectangularWindow();
		
		/** @copydoc Window(size_t) */
		explicit RectangularWindow(size_t size);

		double operator[](size_t n) const override;
	};
}