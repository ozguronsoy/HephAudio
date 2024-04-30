#pragma once
#if !defined(__ANDROID__)
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "FloatBuffer.h"
#include "ComplexBuffer.h"
#include <string>
#include <vector>

#define HEPHAUDIO_SPATIALIZER_AZIMUTH_MIN 0
#define HEPHAUDIO_SPATIALIZER_AZIMUTH_MAX 360
#define HEPHAUDIO_SPATIALIZER_ELEVATION_MIN -90
#define HEPHAUDIO_SPATIALIZER_ELEVATION_MAX 90

namespace HephAudio
{
	class Spatializer final
	{
	private:
		static constexpr size_t INDEX_NOT_FOUND = -1;
	private:
		struct SourcePosition
		{
			double azimuth_deg;
			double elevation_deg;
		};
		struct TransferFunctions
		{
			HephCommon::ComplexBuffer left;
			HephCommon::ComplexBuffer right;
		};
	private:
		std::string sofaFilePath;
		size_t measurementCount;
		size_t frameCount;
		uint32_t sampleRate;
		double deltaAzimuth_deg;
		double deltaElevation_deg;
		HephCommon::FloatBuffer windowBuffer;
		std::vector<Spatializer::TransferFunctions> transferFunctions;
		std::vector<Spatializer::SourcePosition> sourcePositions;
	public:
		Spatializer();
		Spatializer(const std::string& sofaFilePath);
		uint32_t GetSampleRate() const;
		void ReadSofaFile(const std::string& sofaFilePath);
		void Process(AudioBuffer& buffer, double azimuth_deg, double elevation_deg);
	private:
		Spatializer::TransferFunctions InterpolateRectangular(double azimuth_deg, double elevation_deg) const;
		size_t FindMeasurementIndex(double azimuth_deg, double elevation_deg) const;
	};
}
#endif