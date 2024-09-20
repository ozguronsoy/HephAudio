#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Parzen window.
	 * 
	 */
	class HEPH_API ParzenWindow final : public Window
	{
	private:
		double hN;
		double hL;
		double qL;

	public:
		/** @copydoc default_constructor */
		ParzenWindow();
		
		/** @copydoc Window(size_t) */
		ParzenWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}