#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "Buffers/DoubleBuffer.h"
#include <../../dependencies/libmysofa/include/mysofa.h>
#include <string>
#include <vector>
#include <filesystem>

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
	 * @brief implements reading the SOFA files and HRTF interpolation for spatialization.
	 * 
	 */
	class HEPH_API Spatializer final
	{
	private:
		MYSOFA_EASY* pEasy;
		size_t frameCount;
		uint32_t sampleRate;

	public:
		Spatializer();
		Spatializer(const std::filesystem::path& sofaFilePath, uint32_t sampleRate);
		Spatializer(const Spatializer&) = delete;
		Spatializer(Spatializer&& rhs) noexcept;
		~Spatializer();
		Spatializer& operator=(const Spatializer&) = delete;
		Spatializer& operator=(Spatializer&& rhs) noexcept;
		
		/**
		 * gets the sampling rate of the read SOFA file.
		 * 
		 */
		uint32_t GetSampleRate() const;

		/**
		 * gets the size of the HRTF filters.
		 * 
		 */
		size_t GetFrameCount() const;

		/**
		 * reads the SOFA file for using the HRTF data.
		 * 
		 * @throws FailException
		 */
		void OpenSofaFile(const std::filesystem::path& sofaFilePath, uint32_t sampleRate);

		/**
		 * closes the SOFA file and releases the resources.
		 * 
		 */
		void CloseSofaFile();

		/**
		 * spatializes the provided audio data.<br>
		 * Uses \link HephAudio::HannWindow HannWindow \endlink for windowing.
		 * 
		 * @param buffer audio data.
		 * @param azimuth_deg desired azimuth angle in degrees.
		 * @param elevation_deg desired elevation angle in degrees.
		 * 
		 * @throws InvalidArgumentException
		 */
		void Process(AudioBuffer& buffer, float azimuth_deg, float elevation_deg);

		/**
		 * spatializes the provided audio data.<br>
		 * 
		 * @param buffer audio data.
		 * @param azimuth_deg desired azimuth angle in degrees.
		 * @param elevation_deg desired elevation angle in degrees.
		 * 
		 * @param windowBuffer buffer that's generated by using one of the \link HephAudio::Window Window \endlink classes.
		 * 
		 * @note size of the <b>windowBuffer</b> must be equal to the HRTF IR size, which can be acquired by the \link HephAudio::Spatializer::GetFrameCount GetFrameCount \endlink method.
		 * 
		 * @throws InvalidArgumentException
		 */
		void Process(AudioBuffer& buffer, float azimuth_deg, float elevation_deg, const HephCommon::DoubleBuffer& windowBuffer);
	
	private:
		static std::string GetErrorString(int errorCode);
	};
}