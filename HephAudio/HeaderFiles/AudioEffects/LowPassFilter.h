#pragma once
#include "HephAudioShared.h"
#include "Equalizer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief removes the frequencies that are greater than the cutoff frequency.
	 * 
	 */
	class LowPassFilter : public Equalizer
	{
	protected:
		using Equalizer::FrequencyRange;
		using Equalizer::GetFrequencyRanges;
		using Equalizer::AddFrequencyRange;
		using Equalizer::ModifyFrequencyRange;
		using Equalizer::RemoveFrequencyRange;

	public:
		/** @copydoc default_constructor */
		LowPassFilter();
		
		/** 
		 * @copydoc constructor
		 * 
		 * @param f cutoff frequency.
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
		 * @param f cutoff frequency.
		 * 
		 */
		virtual void SetCutoffFreq(double f);
	};
}