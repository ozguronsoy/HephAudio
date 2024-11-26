#pragma once
#include "HephAudioShared.h"
#include "Equalizer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief removes the frequencies that are not in the provided frequency band.
	 *
	 */
	class BandPassFilter : public Equalizer
	{
	protected:
		using Equalizer::FrequencyRange;
		using Equalizer::GetFrequencyRanges;
		using Equalizer::AddFrequencyRange;
		using Equalizer::ModifyFrequencyRange;
		using Equalizer::RemoveFrequencyRange;

	public:
		/** @copydoc default_constructor */
		BandPassFilter();

		/**
		 * @copydoc constructor
		 *
		 * @param f1 first frequency.
		 * @param f2 second frequency.
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 *
		 */
		BandPassFilter(double f1, double f2, size_t hopSize, const Window& wnd);

		/** @copydoc destructor */
		virtual ~BandPassFilter() = default;

		virtual std::string Name() const override;

		/**
		 * gets the first frequency.
		 *
		 */
		virtual double GetF1() const;

		/**
		 * sets the first frequency.
		 *
		 * @param f1 first frequency.
		 *
		 */
		virtual void SetF1(double f1);

		/**
		 * gets the second frequency.
		 *
		 */
		virtual double GetF2() const;

		/**
		 * sets the second frequency.
		 *
		 * @param f2 second frequency.
		 *
		 */
		virtual void SetF2(double f2);
	};
}