#include "EchoInfo.h"
#include "HephMath.h"

namespace HephAudio
{
	EchoInfo::EchoInfo() : EchoInfo(0, 0, 0, 0, 1) {}
	EchoInfo::EchoInfo(uint32_t reflectionCount, heph_float reflectionDelay_s, heph_float decayFactor, heph_float echoStartPosition, heph_float echoEndPosition)
		: reflectionCount(reflectionCount), reflectionDelay_s(reflectionDelay_s), decayFactor(decayFactor), echoStartPosition(echoStartPosition), echoEndPosition(echoEndPosition)
	{}
	size_t EchoInfo::CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const
	{
		const size_t bufferFrameCount = buffer.FrameCount();
		const size_t delayFrameCount = buffer.FormatInfo().sampleRate * this->reflectionDelay_s;
		const size_t echoStartFrame = bufferFrameCount * this->echoStartPosition;
		const size_t echoBufferFrameCount = bufferFrameCount * this->echoEndPosition - echoStartFrame;
		const size_t resultBufferFrameCount = echoStartFrame + delayFrameCount * this->reflectionCount + echoBufferFrameCount;
		return HephCommon::HephMath::Max(resultBufferFrameCount, bufferFrameCount);
	}
}