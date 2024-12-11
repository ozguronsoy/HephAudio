#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates triangular window.
	 * 
	 */
	class HEPH_API TriangularWindow final : public Window
	{
	private:
		double hN;
		double hL;

	public:
		/** @copydoc default_constructor */
		TriangularWindow();
		
		/** @copydoc Window(size_t) */
		explicit TriangularWindow(size_t size);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}