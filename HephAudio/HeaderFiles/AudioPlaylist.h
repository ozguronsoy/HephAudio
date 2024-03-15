#pragma once
#include "HephAudioShared.h"
#include "AudioStream.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
#include "../HephCommon/HeaderFiles/EventArgs.h"
#include "../HephCommon/HeaderFiles/EventResult.h"
#include <vector>

#define HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY "audio_playlist"_sb

namespace HephAudio
{
	class AudioPlaylist final
	{
	public:
		enum TransitionEffect
		{
			None,
			Delay,
			Fade,
			FadeIn,
			FadeOut
		};
	private:
		AudioStream stream;
		std::vector<HephCommon::StringBuffer> files;
		bool isPaused;
		bool applyFadeInOrDelay;
		TransitionEffect transitionEffect;
		heph_float transitionDuration_s;
	public:
		AudioPlaylist(Native::NativeAudio* pNativeAudio);
		AudioPlaylist(Audio& audio);
		AudioPlaylist(Native::NativeAudio* pNativeAudio, const std::vector<HephCommon::StringBuffer>& files);
		AudioPlaylist(Audio& audio, const std::vector<HephCommon::StringBuffer>& files);
		AudioPlaylist(Native::NativeAudio* pNativeAudio, TransitionEffect transitionEffect, heph_float transitionDuration_s);
		AudioPlaylist(Audio& audio, TransitionEffect transitionEffect, heph_float transitionDuration_s);
		AudioPlaylist(Native::NativeAudio* pNativeAudio, TransitionEffect transitionEffect, heph_float transitionDuration_s, const std::vector<HephCommon::StringBuffer>& files);
		AudioPlaylist(Audio& audio, TransitionEffect transitionEffect, heph_float transitionDuration_s, const std::vector<HephCommon::StringBuffer>& files);
		AudioPlaylist(const AudioPlaylist&) = delete;
		AudioPlaylist(AudioPlaylist&& rhs) noexcept;
		AudioPlaylist& operator=(const AudioPlaylist&) = delete;
		AudioPlaylist& operator=(const HephCommon::StringBuffer& rhs);
		AudioPlaylist& operator=(const std::vector<HephCommon::StringBuffer>& rhs);
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
		void Add(const HephCommon::StringBuffer& filePath);
		void Add(const std::vector<HephCommon::StringBuffer>& files);
		void Insert(const HephCommon::StringBuffer& filePath, size_t index);
		void Insert(const std::vector<HephCommon::StringBuffer>& files, size_t index);
		void Remove(size_t index);
		void Remove(size_t index, size_t count);
		void Remove(const HephCommon::StringBuffer& filePath);
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