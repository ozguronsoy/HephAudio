#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include <cstdint>

namespace HephAudio
{
	struct EchoInfo
	{
		uint32_t reflectionCount;
		heph_float reflectionDelay_s;
		heph_float decayFactor;
		heph_float echoStartPosition;
		heph_float echoEndPosition;
		EchoInfo();
		EchoInfo(uint32_t reflectionCount, heph_float reflectionDelay_s, heph_float decayFactor, heph_float echoStartPosition, heph_float echoEndPosition);
		size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
	};
}