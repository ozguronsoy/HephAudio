#include "IAudioObject.h"
#include "AudioException.h"
#include "AudioProcessor.h"

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
			constant = false;
			loopCount = 1;
			volume = 1.0;
			categories = std::vector<std::wstring>(0);
			distortionInfo = DistortionInfo();
			buffer = AudioBuffer();
			frameIndex = 0;
			queueName = L"";
			queueIndex = 0;
			queueDelay = 0;
			windowType = AudioWindowType::RectangleWindow;
			GetSubBuffer = OnGetSubBuffer;
			IsFinishedPlaying = OnIsFinishedPlaying;
			OnRender = nullptr;
		}
		bool IAudioObject::IsPlaying() const
		{
			return !paused && !IsInQueue();
		}
		bool IAudioObject::IsInQueue() const
		{
			return queueName != L"" && queueIndex > 0;
		}
		AudioBuffer IAudioObject::OnGetSubBuffer(IAudioObject* sender, size_t nFramesToRender, size_t* outFrameIndex)
		{
			*outFrameIndex = sender->frameIndex;
			return sender->buffer.GetSubBuffer(sender->frameIndex, nFramesToRender);
		}
		bool IAudioObject::OnIsFinishedPlaying(IAudioObject* sender)
		{
			return sender->frameIndex >= sender->buffer.FrameCount();
		}
	}
}