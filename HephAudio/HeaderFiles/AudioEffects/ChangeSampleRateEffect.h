#pragma once
#include "HephAudioShared.h"
#include "DoubleBufferedAudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the sample rate.
	 * 
	 */
	class ChangeSampleRateEffect : public DoubleBufferedAudioEffect
	{
	public:
		using DoubleBufferedAudioEffect::Process;

	protected:
		/**
		 * the sample rate the input will be converted to.
		 * 
		 */
		size_t outputSampleRate;

	public:
		/** @copydoc default_constructor */
		ChangeSampleRateEffect();

		/**
		 * @copydoc constructor
		 *
		 * @param outputSampleRate @copydetails outputSampleRate
		 * 
		 */
		explicit ChangeSampleRateEffect(size_t outputSampleRate);

		/** @copydoc destructor */
		virtual ~ChangeSampleRateEffect() = default;

		virtual std::string Name() const override;
		virtual size_t CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const override;
		virtual size_t CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const override;
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;


		/**
		 * gets the output sample rate.
		 * 
		 */
		virtual size_t GetOutputSampleRate() const;

		/**
		 * sets the output sample rate.
		 * 
		 * @param outputSampleRate @copydetails outputSampleRate
		 * 
		 */
		virtual void SetOutputSampleRate(size_t outputSampleRate);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}