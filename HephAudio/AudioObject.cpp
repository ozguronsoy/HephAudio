#include "AudioObject.h"
#include "AudioException.h"
#include "AudioProcessor.h"

namespace HephAudio
{
	AudioObject::AudioObject()
	{
		this->filePath = L"";
		this->name = L"";
		this->pause = false;
		this->mute = false;
		this->constant = false;
		this->loopCount = 1;
		this->volume = 1.0;
		this->categories = std::vector<std::wstring>(0);
		this->buffer = AudioBuffer();
		this->frameIndex = 0;
		this->queueName = L"";
		this->queueIndex = 0;
		this->queueDelay = 0;
		this->windowType = AudioWindowType::RectangleWindow;
		this->GetSubBuffer = OnGetSubBuffer;
		this->IsFinishedPlaying = OnIsFinishedPlaying;
		this->OnRender = nullptr;
	}
	bool AudioObject::IsPlaying() const
	{
		return !this->pause && !this->IsInQueue();
	}
	bool AudioObject::IsInQueue() const
	{
		return this->queueName != L"" && this->queueIndex > 0;
	}
	AudioBuffer AudioObject::OnGetSubBuffer(AudioObject* sender, size_t nFramesToRender, size_t* outFrameIndex)
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
	bool AudioObject::OnIsFinishedPlaying(AudioObject* sender)
	{
		return sender->frameIndex >= sender->buffer.FrameCount();
	}
}