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
			AudioBuffer subBuffer = sender->buffer.GetSubBuffer(sender->frameIndex, nFramesToRender);
			switch (sender->windowType)
			{
			case AudioWindowType::RectangleWindow:
				break;
			case AudioWindowType::TriangleWindow:
				AudioProcessor::TriangleWindow(subBuffer);
				break;
			case AudioWindowType::ParzenWindow:
				AudioProcessor::ParzenWindow(subBuffer);
				break;
			case AudioWindowType::WelchWindow:
				AudioProcessor::WelchWindow(subBuffer);
				break;
			case AudioWindowType::SineWindow:
				AudioProcessor::SineWindow(subBuffer);
				break;
			case AudioWindowType::HannWindow:
				AudioProcessor::HannWindow(subBuffer);
				break;
			case AudioWindowType::HammingWindow:
				AudioProcessor::HammingWindow(subBuffer);
				break;
			case AudioWindowType::BlackmanWindow:
				AudioProcessor::BlackmanWindow(subBuffer);
				break;
			case AudioWindowType::ExactBlackmanWindow:
				AudioProcessor::ExactBlackmanWindow(subBuffer);
				break;
			case AudioWindowType::NuttallWindow:
				AudioProcessor::NuttallWindow(subBuffer);
				break;
			case AudioWindowType::BlackmanNuttallWindow:
				AudioProcessor::BlackmanNuttallWindow(subBuffer);
				break;
			case AudioWindowType::BlackmanHarrisWindow:
				AudioProcessor::BlackmanHarrisWindow(subBuffer);
				break;
			case AudioWindowType::FlatTopWindow:
				AudioProcessor::FlatTopWindow(subBuffer);
				break;
			case AudioWindowType::GaussianWindow:
				AudioProcessor::GaussianWindow(subBuffer, 0.4);
				break;
			case AudioWindowType::TukeyWindow:
				AudioProcessor::TukeyWindow(subBuffer, 0.5);
				break;
			case AudioWindowType::BartlettHannWindow:
				AudioProcessor::BartlettHannWindow(subBuffer);
				break;
			case AudioWindowType::HannPoissonWindow:
				AudioProcessor::HannPoissonWindow(subBuffer, 2.0);
				break;
			case AudioWindowType::LanczosWindow:
				AudioProcessor::LanczosWindow(subBuffer);
				break;
			default:
				break;
			}
			return subBuffer;
		}
		bool IAudioObject::OnIsFinishedPlaying(IAudioObject* sender)
		{
			return sender->frameIndex >= sender->buffer.FrameCount();
		}
	}
}