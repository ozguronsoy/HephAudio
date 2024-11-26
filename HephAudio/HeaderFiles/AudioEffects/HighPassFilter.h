#pragma once
#include "HephAudioShared.h"
#include "Equalizer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief removes the frequencies that are lower than the cutoff frequency.
	 *
	 */
	class HighPassFilter : public Equalizer
	{
	protected:
		using Equalizer::FrequencyRange;
		using Equalizer::GetFrequencyRanges;
		using Equalizer::AddFrequencyRange;
		using Equalizer::RemoveFrequencyRange;

	public:
		/** @copydoc default_constructor */
		HighPassFilter();

		/**
		 * @copydoc constructor
		 *
		 * @param f cutoff frequency.
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 *
		 */
		HighPassFilter(double f, size_t hopSize, const Window& wnd);

		/** @copydoc destructor */
		virtual ~HighPassFilter() = default;

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