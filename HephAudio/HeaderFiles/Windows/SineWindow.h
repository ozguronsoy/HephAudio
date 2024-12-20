#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates sine window.
	 * 
	 */
	class HEPH_API SineWindow final : public Window
	{
	private:
		double N;

	public:
		/** @copydoc default_constructor */
		SineWindow();
		
		/** @copydoc Window(size_t) */
		explicit SineWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}