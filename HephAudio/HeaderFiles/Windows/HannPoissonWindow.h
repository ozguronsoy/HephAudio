#pragma once
#include "HephAudioShared.h"
#include "Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates Hann-Poisson window.
	 * 
	 */
	class HEPH_API HannPoissonWindow final : public Window
	{
	private:
		double N;

	public:
		/** controls the window shape. */
		double alpha;

	public:
		/** @copydoc default_constructor */
		HannPoissonWindow();
	
		/** @copydoc Window(size_t) */
		HannPoissonWindow(size_t size);
		
		/** 
		 * @copydoc Window(size_t) 
		 * 
		 * @param alpha @copydetails alpha
		 */
		HannPoissonWindow(size_t size, double alpha);

		double operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}