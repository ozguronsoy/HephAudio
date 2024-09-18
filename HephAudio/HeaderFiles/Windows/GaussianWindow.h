#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Gaussian window.
	 * 
	 */
	class GaussianWindow final : public Window
	{
	private:
		double sigma;
		double hN;
		double shN;

	public:
		/** @copydoc default_constructor */
		GaussianWindow();
	
		/** @copydoc Window(size_t) */
		GaussianWindow(size_t size);
		
		/** 
		 * @copydoc Window(size_t) 
		 * 
		 * @param sigma controls the window shape.
		 */
		GaussianWindow(size_t size, double sigma);
		
		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;

		/**
		 * gets sigma.
		 * 
		 */
		double GetSigma() const;

		/**
		 * sets sigma.
		 * 
		 */
		void SetSigma(double sigma);
	};
}