#pragma once
#include "HephAudioShared.h"
#include "FrequencyDomainEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief removes the frequencies that are greater than the cutoff frequency.
	 * 
	 */
	class LowPassFilter : public FrequencyDomainEffect
	{
	protected:
		/**
		 * cutoff frequency.
		 * 
		 */
		double f;

	public:
		/** @copydoc default_constructor */
		LowPassFilter();
		
		/** 
		 * @copydoc constructor
		 * 
		 * @param f @copydetails f
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 * 
		 */
		LowPassFilter(double f, size_t hopSize, const Window& wnd);

		/** @copydoc destructor */
		virtual ~LowPassFilter() = default;

		virtual std::string Name() const override;

		/**
		 * gets the cutoff frequency.
		 * 
		 */
		virtual double GetCutoffFreq() const;

		/**
		 * sets the cutoff frequency.
		 * 
		 * @param f @copydetails f
		 * 
		 */
		virtual void SetCutoffFreq(double f);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}