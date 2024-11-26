#pragma once
#include "HephAudioShared.h"
#include "FrequencyDomainEffect.h"
#include <vector>

/** @file */

namespace HephAudio
{
	/**
	 * @brief adjusts the volumes of multiple frequency ranges.
	 * 
	 */
	class Equalizer : public FrequencyDomainEffect
	{
	public:
		/**
		 * @brief represents a range of frequencies with volume adjustment.
		 * 
		 */
		struct FrequencyRange
		{
			/**
			 * first frequency.
			 * 
			 */
			double f1;

			/**
			 * second frequency.
			 * 
			 */
			double f2;

			/**
			 * volume adjustment for the frequency range.
			 * 
			 */
			double volume;

			/**
			 * @copydoc constructor
			 * 
			 * @param f1 @copydetails f1
			 * @param f2 @copydetails f2
			 * @param volume @copydetails volume
			 * 
			 */
			FrequencyRange(double f1, double f2, double volume);
		};

	protected:
		/**
		 * stores the frequency ranges which will be adjusted.
		 * 
		 */
		std::vector<Equalizer::FrequencyRange> frequencyRanges;

	public:
		/** @copydoc default_constructor */
		Equalizer();

		/** @copydoc FrequencyDomainEffect(size_t, const Window&) */
		Equalizer(size_t hopSize, const Window& wnd);

		/**
		 * @copydoc FrequencyDomainEffect(size_t, const Window&)
		 * 
		 * @param frequencyRanges @copydetails frequencyRanges
		 * 
		 */
		Equalizer(size_t hopSize, const Window& wnd, const std::initializer_list<Equalizer::FrequencyRange>& frequencyRanges);
		
		/**
		 * @copydoc FrequencyDomainEffect(size_t, const Window&)
		 *
		 * @param frequencyRanges @copydetails frequencyRanges
		 *
		 */
		Equalizer(size_t hopSize, const Window& wnd, const std::vector<Equalizer::FrequencyRange>& frequencyRanges);

		/** @copydoc destructor */
		virtual ~Equalizer() = default;

		virtual std::string Name() const override;

		/**
		 * gets a const reference to the internal frequency range vector.
		 * 
		 */
		virtual const std::vector<Equalizer::FrequencyRange>& GetFrequencyRanges() const;

		/**
		 * adds frequency range.
		 * 
		 * @param f1 @copydetails Equalizer::FrequencyRange::f1
		 * @param f2 @copydetails Equalizer::FrequencyRange::f2
		 * @param volume @copydetails Equalizer::FrequencyRange::volume
		 * 
		 */
		virtual void AddFrequencyRange(double f1, double f2, double volume);
		
		/**
		 * adds frequency range.
		 * 
		 * @param range @copydoc Equalizer::FrequencyRange
		 * 
		 */
		virtual void AddFrequencyRange(const Equalizer::FrequencyRange& range);

		/**
		 * modifies the frequency range at the provided index.
		 *
		 * @param index index of the frequency range that will be modified.
		 * @param range @copydoc Equalizer::FrequencyRange
		 *
		 */
		virtual void ModifyFrequencyRange(size_t index, const Equalizer::FrequencyRange& range);

		/**
		 * removes the frequency range at the provided index.
		 * 
		 * @param index index of the frequency range that will be removed.
		 * 
		 */
		virtual void RemoveFrequencyRange(size_t index);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}