#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates flat top window.
	 * 
	 */
	class HEPH_API FlatTopWindow final : public Window
	{
	private:
		double N;

	public:
		/** @copydoc default_constructor */
		FlatTopWindow();
		
		/** @copydoc Window(size_t) */
		FlatTopWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}