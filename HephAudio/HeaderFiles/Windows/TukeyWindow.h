#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Tukey window.
	 * 
	 */
	class HEPH_API TukeyWindow final : public Window
	{
	private:
		double alpha;
		double N;
		double hN;
		double aN;
		double haN;

	public:
		/** @copydoc default_constructor */
		TukeyWindow();

		/** @copydoc Window(size_t) */
		TukeyWindow(size_t size);

		/** 
		 * @copydoc Window(size_t) 
		 * 
		 * @param alpha controls the window shape.
		 */
		TukeyWindow(size_t size, double alpha);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;

		/**
		 * gets alpha.
		 * 
		 */
		double GetAlpha() const;

		/**
		 * sets alpha.
		 * 
		 */
		void SetAlpha(double alpha);
	};
}