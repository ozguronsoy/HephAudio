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
		const double echoEndPosition = this->echoEndPosition > this->echoStartPosition ? this->echoEndPosition : 1.0;
		const size_t echoBufferFrameCount = buffer.FrameCount() * echoEndPosition - echoStartFrame;
		const size_t resultBufferFrameCount = echoStartFrame + delayFrameCount * this->reflectionCount + echoBufferFrameCount;
		return resultBufferFrameCount > buffer.FrameCount() ? resultBufferFrameCount : buffer.FrameCount();
	}
}