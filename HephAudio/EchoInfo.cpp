#include "EchoInfo.h"
#include "HephMath.h"

namespace HephAudio
{
	size_t EchoInfo::CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const
	{
		const size_t bufferFrameCount = buffer.FrameCount();
		const size_t delayFrameCount = buffer.FormatInfo().sampleRate * this->reflectionDelay_s;
		const size_t echoStartFrame = bufferFrameCount * this->echoStartPosition;
		const size_t echoBufferFrameCount = bufferFrameCount * this->echoEndPosition - echoStartFrame;
		const size_t resultBufferFrameCount = echoStartFrame + delayFrameCount * this->reflectionCount + echoBufferFrameCount;
		return HephCommon::Math::Max(resultBufferFrameCount, bufferFrameCount);
	}
}