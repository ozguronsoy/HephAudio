#include "IAudioObject.h"
#include "AudioException.h"
#include "AudioProcessor.h"

namespace HephAudio
{
	IAudioObject::IAudioObject()
	{
		filePath = L"";
		name = L"";
		pause = false;
		mute = false;
		constant = false;
		loopCount = 1;
		volume = 1.0;
		categories = std::vector<std::wstring>(0);
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
		return !pause && !IsInQueue();
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
			AudioProcessor::ApplyTriangleWindow(subBuffer);
			break;
		case AudioWindowType::ParzenWindow:
			AudioProcessor::ApplyParzenWindow(subBuffer);
			break;
		case AudioWindowType::WelchWindow:
			AudioProcessor::ApplyWelchWindow(subBuffer);
			break;
		case AudioWindowType::SineWindow:
			AudioProcessor::ApplySineWindow(subBuffer);
			break;
		case AudioWindowType::HannWindow:
			AudioProcessor::ApplyHannWindow(subBuffer);
			break;
		case AudioWindowType::HammingWindow:
			AudioProcessor::ApplyHammingWindow(subBuffer);
			break;
		case AudioWindowType::BlackmanWindow:
			AudioProcessor::ApplyBlackmanWindow(subBuffer);
			break;
		case AudioWindowType::ExactBlackmanWindow:
			AudioProcessor::ApplyExactBlackmanWindow(subBuffer);
			break;
		case AudioWindowType::NuttallWindow:
			AudioProcessor::ApplyNuttallWindow(subBuffer);
			break;
		case AudioWindowType::BlackmanNuttallWindow:
			AudioProcessor::ApplyBlackmanNuttallWindow(subBuffer);
			break;
		case AudioWindowType::BlackmanHarrisWindow:
			AudioProcessor::ApplyBlackmanHarrisWindow(subBuffer);
			break;
		case AudioWindowType::FlatTopWindow:
			AudioProcessor::ApplyFlatTopWindow(subBuffer);
			break;
		case AudioWindowType::GaussianWindow:
			AudioProcessor::ApplyGaussianWindow(subBuffer, 0.4);
			break;
		case AudioWindowType::TukeyWindow:
			AudioProcessor::ApplyTukeyWindow(subBuffer, 0.5);
			break;
		case AudioWindowType::BartlettHannWindow:
			AudioProcessor::ApplyBartlettHannWindow(subBuffer);
			break;
		case AudioWindowType::HannPoissonWindow:
			AudioProcessor::ApplyHannPoissonWindow(subBuffer, 2.0);
			break;
		case AudioWindowType::LanczosWindow:
			AudioProcessor::ApplyLanczosWindow(subBuffer);
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