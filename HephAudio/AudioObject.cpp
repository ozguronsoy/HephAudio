#include "AudioObject.h"
#include "AudioException.h"
#include "AudioProcessor.h"

namespace HephAudio
{
	AudioObject::AudioObject()
	{
		this->filePath = "";
		this->name = "";
		this->pause = false;
		this->mute = false;
		this->constant = false;
		this->loopCount = 1;
		this->volume = 1.0;
		this->categories = std::vector<StringBuffer>(0);
		this->buffer = AudioBuffer();
		this->frameIndex = 0;
		this->queueName = "";
		this->queueIndex = 0;
		this->queueDelay = 0.0;
		this->windowType = AudioWindowType::RectangleWindow;
		this->OnRender += OnRenderHandler;
	}
	bool AudioObject::IsPlaying() const
	{
		return !this->pause && !this->IsInQueue();
	}
	bool AudioObject::IsInQueue() const
	{
		return !this->queueName.CompareContent("") && this->queueIndex > 0;
	}
	void AudioObject::OnRenderHandler(AudioEventArgs* pArgs, AudioEventResult* pResult)
	{
		AudioObject* pAudioObject = (AudioObject*)pArgs->pAudioObject;
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

		pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, pRenderArgs->renderFrameCount);

		switch (pAudioObject->windowType)
		{
		case AudioWindowType::RectangleWindow:
			break;
		case AudioWindowType::TriangleWindow:
			AudioProcessor::ApplyTriangleWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::ParzenWindow:
			AudioProcessor::ApplyParzenWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::WelchWindow:
			AudioProcessor::ApplyWelchWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::SineWindow:
			AudioProcessor::ApplySineWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::HannWindow:
			AudioProcessor::ApplyHannWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::HammingWindow:
			AudioProcessor::ApplyHammingWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::BlackmanWindow:
			AudioProcessor::ApplyBlackmanWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::ExactBlackmanWindow:
			AudioProcessor::ApplyExactBlackmanWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::NuttallWindow:
			AudioProcessor::ApplyNuttallWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::BlackmanNuttallWindow:
			AudioProcessor::ApplyBlackmanNuttallWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::BlackmanHarrisWindow:
			AudioProcessor::ApplyBlackmanHarrisWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::FlatTopWindow:
			AudioProcessor::ApplyFlatTopWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::GaussianWindow:
			AudioProcessor::ApplyGaussianWindow(pRenderResult->renderBuffer, 0.4);
			break;
		case AudioWindowType::TukeyWindow:
			AudioProcessor::ApplyTukeyWindow(pRenderResult->renderBuffer, 0.5);
			break;
		case AudioWindowType::BartlettHannWindow:
			AudioProcessor::ApplyBartlettHannWindow(pRenderResult->renderBuffer);
			break;
		case AudioWindowType::HannPoissonWindow:
			AudioProcessor::ApplyHannPoissonWindow(pRenderResult->renderBuffer, 2.0);
			break;
		case AudioWindowType::LanczosWindow:
			AudioProcessor::ApplyLanczosWindow(pRenderResult->renderBuffer);
			break;
		default:
			break;
		}
		
		pAudioObject->frameIndex += pRenderArgs->renderFrameCount;
		pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
	}
}