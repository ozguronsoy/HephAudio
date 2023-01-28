#include "EchoInfo.h"

namespace HephAudio
{
	EchoInfo::EchoInfo()
	{
		this->reflectionCount = 0;
		this->reflectionDelay = 0.0;
		this->volumeFactor = 0.0;
		this->echoStartPosition = 0.0;
		this->echoEndPosition = 1.0;
	}
	size_t EchoInfo::CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const
	{
		const size_t delayFrameCount = buffer.FormatInfo().sampleRate * this->reflectionDelay;
		const size_t echoStartFrame = buffer.FrameCount() * this->echoStartPosition;
		const size_t echoBufferFrameCount = buffer.FrameCount() * this->echoEndPosition - echoStartFrame;
		const size_t resultBufferFrameCount = echoStartFrame + delayFrameCount * this->reflectionCount + echoBufferFrameCount;
		return max(resultBufferFrameCount, buffer.FrameCount());
	}
}