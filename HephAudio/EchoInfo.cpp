#include "EchoInfo.h"

namespace HephAudio
{
	namespace Structs
	{
		EchoInfo::EchoInfo()
		{
			reflectionCount = 0;
			reflectionDelay = 0.0;
			volumeFactor = 0.0;
			echoStartPosition = 0.0;
			echoEndPosition = 1.0;
		}
		size_t EchoInfo::CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const
		{
			struct EchoKeyPoints
			{
				size_t startFrameIndex;
				size_t endFrameIndex;
			};
			const size_t delayFrameCount = buffer.GetFormat().nSamplesPerSec * this->reflectionDelay;
			const size_t echoStartFrame = buffer.FrameCount() * this->echoStartPosition;
			const double echoEndPosition = this->echoEndPosition > this->echoStartPosition ? this->echoEndPosition : 1.0;
			const AudioBuffer echoBuffer = buffer.GetSubBuffer(echoStartFrame, buffer.FrameCount() * echoEndPosition - echoStartFrame);
			size_t resultBufferFrameCount = buffer.FrameCount();
			std::vector<EchoKeyPoints> keyPoints(this->reflectionCount + 1);
			keyPoints.at(0).startFrameIndex = 0; // Original data key points.
			keyPoints.at(0).endFrameIndex = buffer.FrameCount() - 1;
			for (size_t i = 1; i < keyPoints.size(); i++) // Find echo key points.
			{
				keyPoints.at(i).startFrameIndex = echoStartFrame + delayFrameCount * i;
				keyPoints.at(i).endFrameIndex = keyPoints.at(i).startFrameIndex + echoBuffer.FrameCount() - 1;
				if (keyPoints.at(i).endFrameIndex >= resultBufferFrameCount)
				{
					resultBufferFrameCount = keyPoints.at(i).endFrameIndex + 1;
				}
			}
			return resultBufferFrameCount;
		}
	}
}