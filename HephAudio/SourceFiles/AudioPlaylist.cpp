#include "AudioPlaylist.h"
#include "HephException.h"
#include "ConsoleLogger.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioPlaylist::AudioPlaylist(Native::NativeAudio* pNativeAudio) : AudioPlaylist(pNativeAudio, TransitionEffect::None, 0) {}

	AudioPlaylist::AudioPlaylist(Audio& audio) : AudioPlaylist(audio.GetNativeAudio()) {}

	AudioPlaylist::AudioPlaylist(Native::NativeAudio* pNativeAudio, const std::vector<std::string>& files) : AudioPlaylist(pNativeAudio, TransitionEffect::None, 0, files) {}

	AudioPlaylist::AudioPlaylist(Audio& audio, const std::vector<std::string>& files) : AudioPlaylist(audio.GetNativeAudio(), files) {}

	AudioPlaylist::AudioPlaylist(Native::NativeAudio* pNativeAudio, TransitionEffect transitionEffect, double transitionDuration_s)
		: stream(pNativeAudio), isPaused(true), applyFadeInOrDelay(false), transitionEffect(transitionEffect), transitionDuration_s(transitionDuration_s) {}

	AudioPlaylist::AudioPlaylist(Audio& audio, TransitionEffect transitionEffect, double transitionDuration_s) : AudioPlaylist(audio.GetNativeAudio()) {}

	AudioPlaylist::AudioPlaylist(Native::NativeAudio* pNativeAudio, TransitionEffect transitionEffect, double transitionDuration_s, const std::vector<std::string>& files)
		: stream(pNativeAudio), files(files), isPaused(true), applyFadeInOrDelay(false)
		, transitionEffect(transitionEffect), transitionDuration_s(transitionDuration_s)
	{
		if (this->files.size() > 0)
		{
			this->ChangeFile();
		}
	}

	AudioPlaylist::AudioPlaylist(Audio& audio, TransitionEffect transitionEffect, double transitionDuration_s, const std::vector<std::string>& files)
		: AudioPlaylist(audio.GetNativeAudio(), transitionEffect, transitionDuration_s, files) {}

	AudioPlaylist::AudioPlaylist(AudioPlaylist&& rhs) noexcept :
		stream(std::move(rhs.stream)), files(std::move(rhs.files))
		, isPaused(rhs.isPaused), applyFadeInOrDelay(rhs.applyFadeInOrDelay)
		, transitionEffect(rhs.transitionEffect), transitionDuration_s(rhs.transitionDuration_s) {}

	AudioPlaylist& AudioPlaylist::operator=(const std::string& rhs)
	{
		this->Clear();
		this->Add(rhs);
		return *this;
	}
	AudioPlaylist& AudioPlaylist::operator=(const std::vector<std::string>& rhs)
	{
		this->Clear();
		this->Add(rhs);
		return *this;
	}
	AudioPlaylist& AudioPlaylist::operator=(AudioPlaylist&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->stream = std::move(rhs.stream);
			this->files = std::move(rhs.files);
			this->isPaused = rhs.isPaused;
			this->applyFadeInOrDelay = rhs.applyFadeInOrDelay;
			this->transitionEffect = rhs.transitionEffect;
			this->transitionDuration_s = rhs.transitionDuration_s;
		}

		return *this;
	}
	size_t AudioPlaylist::Size() const
	{
		return this->files.size();
	}
	Native::NativeAudio* AudioPlaylist::GetNativeAudio() const
	{
		return this->stream.GetNativeAudio();
	}
	AudioPlaylist::TransitionEffect AudioPlaylist::GetTransitionEffect() const
	{
		return this->transitionEffect;
	}
	void AudioPlaylist::SetTransitionEffect(TransitionEffect transitionEffect)
	{
		this->transitionEffect = transitionEffect;
	}
	double AudioPlaylist::GetTransitionDuration() const
	{
		return this->transitionDuration_s;
	}
	void AudioPlaylist::SetTransitionDuration(double transitionDuration_s)
	{
		this->transitionDuration_s = transitionDuration_s;
	}
	void AudioPlaylist::Start()
	{
		this->isPaused = false;
		if (this->files.size() > 0)
		{
			this->stream.GetAudioObject()->isPaused = false;
		}
	}
	void AudioPlaylist::Stop()
	{
		this->isPaused = true;
		if (this->files.size() > 0)
		{
			this->stream.GetAudioObject()->isPaused = true;
		}
	}
	bool AudioPlaylist::IsPaused() const
	{
		return this->isPaused;
	}
	void AudioPlaylist::Add(const std::string& filePath)
	{
		this->files.push_back(filePath);
		if (this->files.size() == 1)
		{
			this->ChangeFile();
		}
	}
	void AudioPlaylist::Add(const std::vector<std::string>& files)
	{
		if (files.size() > 0)
		{
			this->files.insert(this->files.end(), files.begin(), files.end());
			if (this->files.size() == files.size())
			{
				this->ChangeFile();
			}
		}
	}
	void AudioPlaylist::Insert(const std::string& filePath, size_t index)
	{
		if (index > this->files.size())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioPlaylist::Insert", "Index out of range."));
			index = this->files.size();
		}

		this->files.insert(this->files.begin() + index, filePath);
		if (index == 0)
		{
			this->ChangeFile();
		}
	}
	void AudioPlaylist::Insert(const std::vector<std::string>& files, size_t index)
	{
		if (files.size() > 0)
		{
			if (index > this->files.size())
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioPlaylist::Insert", "Index out of range."));
				index = this->files.size();
			}

			this->files.insert(this->files.begin() + index, files.begin(), files.end());
			if (index == 0)
			{
				this->ChangeFile();
			}
		}
	}
	void AudioPlaylist::Remove(size_t index)
	{
		if (index >= this->files.size())
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioPlaylist::Remove", "Index out of range."));
		}

		this->files.erase(this->files.begin() + index);
		if (index == 0)
		{
			this->ChangeFile();
		}
	}
	void AudioPlaylist::Remove(size_t index, size_t count)
	{
		if (count > 0)
		{
			if (index >= this->files.size())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioPlaylist::Remove", "Index out of range."));
			}

			if (index + count >= this->files.size())
			{
				count = index - this->files.size();
			}

			this->files.erase(this->files.begin() + index, this->files.begin() + index + count);
			if (index == 0)
			{
				this->ChangeFile();
			}
		}
	}
	void AudioPlaylist::Remove(const std::string& filePath)
	{
		for (size_t i = 0; i < this->files.size(); i++)
		{
			if (this->files[i] == filePath)
			{
				this->Remove(i);
				return;
			}
		}
	}
	void AudioPlaylist::Skip()
	{
		this->Skip(1);
	}
	void AudioPlaylist::Skip(size_t n)
	{
		if (n > 0)
		{
			if (n >= this->files.size())
			{
				this->Clear();
				return;
			}
			this->files.erase(this->files.begin(), this->files.begin() + n);
			this->ChangeFile();
		}
	}
	void AudioPlaylist::Clear()
	{
		this->files.clear();
		Native::NativeAudio* pNativeAudio = this->stream.GetNativeAudio();
		this->stream.Release();
		this->stream = AudioStream(pNativeAudio, "");
	}
	void AudioPlaylist::ChangeFile()
	{
		this->applyFadeInOrDelay = this->stream.GetAudioObject() != nullptr; // don't apply fade-in and delay effects for the first file
		std::string filePath = this->files.size() > 0 ? this->files[0] : "";

		Native::NativeAudio* pNativeAudio = this->stream.GetNativeAudio();
		this->stream.Release();

	OPEN_NEW_STREAM:
		try
		{
			this->stream = AudioStream(pNativeAudio, filePath);
			if (filePath != "")
			{
				AudioObject* pAudioObject = this->stream.GetAudioObject();
				pAudioObject->OnRender += &AudioPlaylist::ApplyTransitionEffect;
				pAudioObject->OnFinishedPlaying = &AudioPlaylist::OnFinishedPlaying;
				pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY, this);
				pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY, this);
				pAudioObject->isPaused = this->isPaused;
			}
		}
		catch (HephException& ex)
		{
			this->files.erase(this->files.begin());
			if (this->files.size() > 0)
			{
				const std::string oldFilePath = std::move(filePath);
				filePath = this->files[0];
				HEPHAUDIO_LOG("Could not play \"" + oldFilePath + "\", trying next file...", HEPH_CL_ERROR);
			}
			else
			{
				HEPHAUDIO_LOG("Could not play \"" + filePath + "\", playlist finished.", HEPH_CL_ERROR);
				filePath = "";
			}
			goto OPEN_NEW_STREAM;
		}
	}
	void AudioPlaylist::OnFinishedPlaying(const EventParams& eventParams)
	{
		AudioFinishedPlayingEventArgs* pFinishedPlayingEventArgs = (AudioFinishedPlayingEventArgs*)eventParams.pArgs;
		AudioObject* pAudioObject = (AudioObject*)pFinishedPlayingEventArgs->pAudioObject;
		AudioPlaylist* pPlaylist = (AudioPlaylist*)eventParams.userEventArgs[HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY];

		if (pPlaylist != nullptr)
		{
			pPlaylist->Remove(0);
			eventParams.pResult->isHandled = true;
			// event is destroyed since the audio object that owns it is destroyed
			// the event handler count will be random, thus invoke loop will keep going
			// to exit invoke loop set the "isHandled" to true
		}
	}
	void AudioPlaylist::ApplyTransitionEffect(const EventParams& eventParams)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;
		Native::NativeAudio* pNativeAudio = (Native::NativeAudio*)pRenderArgs->pNativeAudio;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		AudioPlaylist* pPlaylist = (AudioPlaylist*)eventParams.userEventArgs[HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY];
		if (pPlaylist != nullptr)
		{
			switch (pPlaylist->transitionEffect)
			{
			case TransitionEffect::Delay:
				AudioPlaylist::Transition_Delay(pPlaylist, pRenderArgs, pRenderResult);
				break;
			case TransitionEffect::Fade:
				AudioPlaylist::Transition_FadeIn(pPlaylist, pRenderArgs, pRenderResult);
				AudioPlaylist::Transition_FadeOut(pPlaylist, pRenderArgs, pRenderResult);
				break;
			case TransitionEffect::FadeIn:
				AudioPlaylist::Transition_FadeIn(pPlaylist, pRenderArgs, pRenderResult);
				break;
			case TransitionEffect::FadeOut:
				AudioPlaylist::Transition_FadeOut(pPlaylist, pRenderArgs, pRenderResult);
				break;
			default:
				break;
			}
		}
	}
	void AudioPlaylist::Transition_Delay(AudioPlaylist* pPlaylist, AudioRenderEventArgs* pRenderArgs, AudioRenderEventResult* pRenderResult)
	{
		Native::NativeAudio* pNativeAudio = (Native::NativeAudio*)pRenderArgs->pNativeAudio;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		if (pPlaylist->applyFadeInOrDelay)
		{
			const double duration_sample = pPlaylist->transitionDuration_s * pNativeAudio->GetRenderFormat().sampleRate;
			if (pAudioObject->frameIndex >= duration_sample)
			{
				pPlaylist->applyFadeInOrDelay = false;
				pAudioObject->frameIndex = 0;
			}
			pRenderResult->renderBuffer.Reset();
			pRenderResult->isFinishedPlaying = false;
		}
	}
	void AudioPlaylist::Transition_FadeIn(AudioPlaylist* pPlaylist, AudioRenderEventArgs* pRenderArgs, AudioRenderEventResult* pRenderResult)
	{
		Native::NativeAudio* pNativeAudio = (Native::NativeAudio*)pRenderArgs->pNativeAudio;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		if (pPlaylist->applyFadeInOrDelay)
		{
			const AudioFormatInfo renderFormatInfo = pNativeAudio->GetRenderFormat();
			const double duration_sample = pPlaylist->transitionDuration_s * renderFormatInfo.sampleRate;
			if (pAudioObject->frameIndex >= duration_sample)
			{
				pPlaylist->applyFadeInOrDelay = false;
			}
			for (size_t i = 0; i < pRenderResult->renderBuffer.FrameCount(); i++)
			{
				const double factor = (i + pAudioObject->frameIndex - pRenderArgs->renderFrameCount) / duration_sample;
				for (size_t j = 0; j < renderFormatInfo.channelLayout.count; j++)
				{
					pRenderResult->renderBuffer[i][j] *= factor;
				}
			}
		}
	}
	void AudioPlaylist::Transition_FadeOut(AudioPlaylist* pPlaylist, AudioRenderEventArgs* pRenderArgs, AudioRenderEventResult* pRenderResult)
	{
		Native::NativeAudio* pNativeAudio = (Native::NativeAudio*)pRenderArgs->pNativeAudio;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		const AudioFormatInfo renderFormatInfo = pNativeAudio->GetRenderFormat();
		const double duration_sample = pPlaylist->transitionDuration_s * renderFormatInfo.sampleRate;
		const size_t fileFrameCount = pPlaylist->stream.GetFrameCount();
		if (pAudioObject->frameIndex + duration_sample >= fileFrameCount + pRenderArgs->renderFrameCount)
		{
			for (size_t i = 0; i < pRenderResult->renderBuffer.FrameCount(); i++)
			{
				const double factor = (fileFrameCount - i - pAudioObject->frameIndex + pRenderArgs->renderFrameCount) / duration_sample;
				for (size_t j = 0; j < renderFormatInfo.channelLayout.count; j++)
				{
					pRenderResult->renderBuffer[i][j] *= factor;
				}
			}
		}
	}
}