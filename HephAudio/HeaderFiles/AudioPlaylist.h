#pragma once
#include "HephAudioShared.h"
#include "AudioStream.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "EventArgs.h"
#include "EventResult.h"
#include <string>
#include <vector>

#define HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY std::string("audio_playlist")

namespace HephAudio
{
	class AudioPlaylist final
	{
	public:
		enum TransitionEffect : uint8_t
		{
			None,
			Delay,
			Fade,
			FadeIn,
			FadeOut
		};
	private:
		AudioStream stream;
		std::vector<std::string> files;
		bool isPaused;
		bool applyFadeInOrDelay;
		TransitionEffect transitionEffect;
		heph_float transitionDuration_s;
	public:
		AudioPlaylist(Native::NativeAudio* pNativeAudio);
		AudioPlaylist(Audio& audio);
		AudioPlaylist(Native::NativeAudio* pNativeAudio, const std::vector<std::string>& files);
		AudioPlaylist(Audio& audio, const std::vector<std::string>& files);
		AudioPlaylist(Native::NativeAudio* pNativeAudio, TransitionEffect transitionEffect, heph_float transitionDuration_s);
		AudioPlaylist(Audio& audio, TransitionEffect transitionEffect, heph_float transitionDuration_s);
		AudioPlaylist(Native::NativeAudio* pNativeAudio, TransitionEffect transitionEffect, heph_float transitionDuration_s, const std::vector<std::string>& files);
		AudioPlaylist(Audio& audio, TransitionEffect transitionEffect, heph_float transitionDuration_s, const std::vector<std::string>& files);
		AudioPlaylist(const AudioPlaylist&) = delete;
		AudioPlaylist(AudioPlaylist&& rhs) noexcept;
		AudioPlaylist& operator=(const AudioPlaylist&) = delete;
		AudioPlaylist& operator=(const std::string& rhs);
		AudioPlaylist& operator=(const std::vector<std::string>& rhs);
		AudioPlaylist& operator=(AudioPlaylist&& rhs) noexcept;
		size_t Size() const;
		Native::NativeAudio* GetNativeAudio() const;
		TransitionEffect GetTransitionEffect() const;
		void SetTransitionEffect(TransitionEffect transitionEffect);
		heph_float GetTransitionDuration() const;
		void SetTransitionDuration(heph_float transitionDuration_s);
		void Start();
		void Stop();
		bool IsPaused() const;
		void Add(const std::string& filePath);
		void Add(const std::vector<std::string>& files);
		void Insert(const std::string& filePath, size_t index);
		void Insert(const std::vector<std::string>& files, size_t index);
		void Remove(size_t index);
		void Remove(size_t index, size_t count);
		void Remove(const std::string& filePath);
		void Skip();
		void Skip(size_t n);
		void Clear();
	private:
		void ChangeFile();
		static void OnFinishedPlaying(const HephCommon::EventParams& eventParams);
		static void ApplyTransitionEffect(const HephCommon::EventParams& eventParams);
		static void Transition_Delay(AudioPlaylist* pPlaylist, AudioRenderEventArgs* pRenderArgs, AudioRenderEventResult* pRenderResult);
		static void Transition_FadeIn(AudioPlaylist* pPlaylist, AudioRenderEventArgs* pRenderArgs, AudioRenderEventResult* pRenderResult);
		static void Transition_FadeOut(AudioPlaylist* pPlaylist, AudioRenderEventArgs* pRenderArgs, AudioRenderEventResult* pRenderResult);
	};
}