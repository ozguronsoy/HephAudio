#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include <cstdint>

namespace HephAudio
{
	struct EchoInfo
	{
		uint32_t reflectionCount{ 0 };
		heph_float reflectionDelay_s{ 0.0 };
		heph_float decayFactor{ 0.0 };
		heph_float echoStartPosition{ 0.0 };
		heph_float echoEndPosition{ 1.0 };
		size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
	};
}