#pragma once
#include "HephAudioShared.h"
#include "Equalizer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief removes the frequencies that are in the provided frequency band.
	 *
	 */
	class BandCutFilter : public Equalizer
	{
	protected:
		using Equalizer::FrequencyRange;
		using Equalizer::GetFrequencyRanges;
		using Equalizer::AddFrequencyRange;
		using Equalizer::ModifyFrequencyRange;
		using Equalizer::RemoveFrequencyRange;

	public:
		/** @copydoc default_constructor */
		BandCutFilter();

		/**
		 * @copydoc constructor
		 *
		 * @param f1 first frequency.
		 * @param f2 second frequency.
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 *
		 */
		BandCutFilter(double f1, double f2, size_t hopSize, const Window& wnd);

		/** @copydoc destructor */
		virtual ~BandCutFilter() = default;

		virtual std::string Name() const override;

		/**
		 * gets the first frequency.
		 *
		 */
		virtual double GetF1() const;

		/**
		 * sets the first frequency.
		 *
		 * @param f first frequency.
		 *
		 */
		virtual void SetF1(double f);

		/**
		 * gets the second frequency.
		 *
		 */
		virtual double GetF2() const;

		/**
		 * sets the second frequency.
		 *
		 * @param f second frequency.
		 *
		 */
		virtual void SetF2(double f);
	};
}