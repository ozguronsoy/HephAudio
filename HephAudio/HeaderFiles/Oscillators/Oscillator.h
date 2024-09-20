#pragma once
#include "HephAudioShared.h"
#include "Buffers/DoubleBuffer.h"
#include <cmath>
#include <cinttypes>

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for oscillators. 
	 * 
	 */
	class HEPH_API Oscillator
	{
	public:
		/**
		 * maximum amplitude of the periodic signal.
		 * 
		 */
		double peakAmplitude;

		/**
		 * frequency of the periodic signal.
		 * 
		 */
		double frequency;

		/**
		 * phase angle, in radians, of the periodic signal.
		 * 
		 */
		double phase_rad;

		/**
		 * sampling rate of the periodic signal.
		 * 
		 */
		uint32_t sampleRate;

	protected:
		/** @copydoc default_constructor */
		Oscillator();
		
		/** 
		 * @copydoc constructor 
		 * 
		 * @param peakAmplitude @copydetails peakAmplitude
		 * @param frequency @copydetails frequency
		 * @param sampleRate @copydetails sampleRate
		 * @param phase_rad @copydetails phase_rad
		 */
		Oscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);

	public:
		/** @copydoc destructor */
		virtual ~Oscillator() = default;

		/**
		 * calculates the sample at index n.
		 * 
		 */
		virtual double operator[](size_t n) const = 0;

		/**
		 * generates a one-period-long buffer.
		 * 
		 */
		HephCommon::DoubleBuffer GenerateBuffer() const;

		/**
		 * generates a buffer with the provided size.
		 * 
		 * @param size size of the buffer in samples.
		 */
		HephCommon::DoubleBuffer GenerateBuffer(size_t size) const;

		/**
		 * generates a buffer with the provided size.
		 * 
		 * @param index index of the first sample.
		 * @param size size of the buffer in samples.
		 */
		HephCommon::DoubleBuffer GenerateBuffer(size_t index, size_t size) const;
	};
}