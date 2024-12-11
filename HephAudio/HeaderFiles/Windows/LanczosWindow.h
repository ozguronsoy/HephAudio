#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Lanczos window.
	 * 
	 */
	class HEPH_API LanczosWindow final : public Window
	{
	private:
		double N;

	public:
		/** @copydoc default_constructor */
		LanczosWindow();
		
		/** @copydoc Window(size_t) */
		explicit LanczosWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}