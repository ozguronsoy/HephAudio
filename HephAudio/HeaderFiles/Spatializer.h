#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "FloatBuffer.h"
#include <../../dependencies/libmysofa/include/mysofa.h>
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
		MYSOFA_EASY* pEasy;
		size_t frameCount;
		uint32_t sampleRate;
	public:
		Spatializer();
		Spatializer(const std::string& sofaFilePath, uint32_t sampleRate);
		Spatializer(const Spatializer&) = delete;
		Spatializer(Spatializer&& rhs) noexcept;
		~Spatializer();
		Spatializer& operator=(const Spatializer&) = delete;
		Spatializer& operator=(Spatializer&& rhs) noexcept;
		uint32_t GetSampleRate() const;
		size_t GetFrameCount() const;
		void OpenSofaFile(const std::string& sofaFilePath, uint32_t sampleRate);
		void CloseSofaFile();
		void Process(AudioBuffer& buffer, float azimuth_deg, float elevation_deg);
		void Process(AudioBuffer& buffer, float azimuth_deg, float elevation_deg, const HephCommon::FloatBuffer& windowBuffer);
	private:
		static std::string GetErrorString(int errorCode);
	};
}