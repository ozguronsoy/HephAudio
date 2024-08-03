#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include <cstdint>

namespace HephAudio
{
	struct EchoInfo
	{
		uint32_t reflectionCount;
		double reflectionDelay_s;
		double decayFactor;
		double echoStartPosition;
		double echoEndPosition;
		EchoInfo();
		EchoInfo(uint32_t reflectionCount, double reflectionDelay_s, double decayFactor, double echoStartPosition, double echoEndPosition);
		size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
	};
}