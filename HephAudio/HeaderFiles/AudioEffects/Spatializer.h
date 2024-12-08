#pragma once
#include "HephAudioShared.h"
#include "FrequencyDomainEffect.h"
#include "Buffers/ComplexBuffer.h"
#include <filesystem>
#include <mysofa.h>
#include <array>

/** @file */

/**
 * the minimum value the azimuth can have in degrees.
 *
 */
#define HEPHAUDIO_SPATIALIZER_AZIMUTH_MIN 0

 /**
  * the maximum value the azimuth can have in degrees.
  *
  */
#define HEPHAUDIO_SPATIALIZER_AZIMUTH_MAX 360

  /**
   * the minimum value the elevation can have in degrees.
   *
   */
#define HEPHAUDIO_SPATIALIZER_ELEVATION_MIN -90

   /**
	* the maximum value the elevation can have in degrees.
	*
	*/
#define HEPHAUDIO_SPATIALIZER_ELEVATION_MAX 90

namespace HephAudio
{
	/**
	 * @brief applies 3D audio spatialization using SOFA files.
	 *
	 */
	class HEPH_API Spatializer : public FrequencyDomainEffect
	{
	public:
		using FrequencyDomainEffect::Process;

	protected:
		/**
		 * in degrees.
		 *
		 */
		double azimuth;

		/**
		 * in degrees.
		 *
		 */
		double elevation;

		/**
		 * path of the SOFA file.
		 * 
		 */
		std::filesystem::path filePath;

		/**
		 * stores the contents of the SOFA file.
		 *
		 */
		MYSOFA_EASY* pEasy;

		/**
		 * size of the HRTF filters.
		 *
		 */
		size_t hrtfSize;

		/**
		 * sampling rate of the HRTF filters.
		 *
		 */
		uint32_t hrtfSampleRate;

		/**
		 * indicates whether there has been any change and the transfer functions needs updating.
		 * 
		 */
		bool updateTransferFunctions;

		/**
		 * an array of transfer functions where the first element is the left channel, and the second element is the right channel.
		 * 
		 */
		std::array<Heph::ComplexBuffer, 2> transferFunctions;

	public:
		/** @copydoc default_constructor */
		Spatializer();

		/** @copydoc FrequencyDomainEffect(size_t, const Window&) */
		Spatializer(size_t hopSize, const Window& wnd);

		/**
		 * @copydoc constructor
		 *
		 * @param azimuth @copydetails azimuth
		 * @param elevation @copydetails elevation
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 *
		 */
		Spatializer(double azimuth, double elevation, size_t hopSize, const Window& wnd);

		/**
		 * @copydoc constructor
		 *
		 * @param filePath @copydetails filePath
		 * @param sampleRate @copydetails hrtfSampleRate
		 * @param azimuth @copydetails azimuth
		 * @param elevation @copydetails elevation
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 *
		 */
		Spatializer(const std::filesystem::path& filePath, uint32_t sampleRate, double azimuth, double elevation, size_t hopSize, const Window& wnd);

		/** @copydoc destructor */
		virtual ~Spatializer();

		virtual std::string Name() const override;
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;
		virtual void SetWindow(const Window& wnd) override;

		/**
		 * reads the SOFA file.
		 *
		 * @param filePath @copydetails filePath
		 * 
		 */
		virtual void OpenSofaFile(const std::filesystem::path& filePath);

		/**
		 * closes the SOFA file and releases the resources.
		 *
		 */
		virtual void CloseSofaFile();

		/**
		 * gets the azimuth angle in degrees.
		 *
		 */
		virtual double GetAzimuth() const;

		/**
		 * sets the azimuth angle.
		 *
		 * @param azimuth @copydetails azimuth
		 *
		 */
		virtual void SetAzimuth(double azimuth);

		/**
		 * gets the elevation angle in degrees.
		 *
		 */
		virtual double GetElevation() const;

		/**
		 * sets the elevation angle.
		 *
		 * @param elevation @copydetails elevation
		 *
		 */
		virtual void SetElevation(double elevation);

		/**
		 * gets the sampling rate of the HRTF filters.
		 *
		 */
		virtual uint32_t GetSampleRate() const;

		/**
		 * sets the sampling rate of the HRTF filters.
		 * 
		 * @param sampleRate @copydetails hrtfSampleRate
		 *
		 */
		virtual void SetSampleRate(uint32_t sampleRate);

		/**
		 * gets the size of the HRTF filters.
		 *
		 */
		virtual size_t GetHrtfSize() const;

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;

		/**
		 * opens the default SOFA file.
		 *
		 */
		virtual void OpenDefaultFile();

		/**
		 * @brief gets the error message that corresponds to the provided code.
		 * 
		 * @param errorCode the error code returned by the libmysofa library.
		 *
		 */
		virtual std::string GetErrorString(int errorCode) const;
	};
}