#include "IAudioObject.h"

namespace HephAudio
{
	namespace Structs
	{
		IAudioObject::IAudioObject()
		{
			filePath = L"";
			name = L"";
			paused = false;
			mute = false;
			reverse = false;
			constant = false;
			loopCount = 1;
			volume = 1.0;
			categories = std::vector<std::wstring>(0);
			distortionInfo = DistortionInfo();
			echoInfo = EchoInfo();
			buffer = AudioBuffer();
			frameIndex = 0;
			queueName = L"";
			queueIndex = 0;
			queueDelay = 0;
		}
		bool IAudioObject::IsPlaying() const
		{
			return !paused && !IsInQueue();
		}
		bool IAudioObject::IsInQueue() const
		{
			return queueName != L"" && queueIndex > 0;
		}
		uint32_t IAudioObject::ReversedFrameIndex() const
		{
			return this->FrameCount() - frameIndex - 1u;
		}
		size_t IAudioObject::FrameCount() const
		{
			if (echoInfo.echo)
			{
				struct EchoKeyPoints
				{
					size_t startFrameIndex;
					size_t endFrameIndex;
				};
				const size_t delayFrameCount = buffer.GetFormat().nSamplesPerSec * echoInfo.reflectionDelay;
				const size_t echoStartFrame = buffer.FrameCount() * echoInfo.echoStartPosition;
				const double echoEndPosition = echoInfo.echoEndPosition > echoInfo.echoStartPosition ? echoInfo.echoEndPosition : 1.0;
				const AudioBuffer echoBuffer = buffer.GetSubBuffer(echoStartFrame, buffer.FrameCount() * echoEndPosition - echoStartFrame);
				size_t resultBufferFrameCount = buffer.FrameCount();
				std::vector<EchoKeyPoints> keyPoints(echoInfo.reflectionCount + 1);
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
			return buffer.FrameCount();
		}
	}
}