#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
#include "../HephCommon/HeaderFiles/Event.h"
#include "../HephCommon/HeaderFiles/Guid.h"
#include <vector>

#define HEPHAUDIO_INFINITE_LOOP 0
#define HEPHAUDIO_RENDER_HANDLER_DEFAULT &HephAudio::AudioObject::DefaultRenderHandler
#define HEPHAUDIO_RENDER_HANDLER_ENSURE_FORMAT &HephAudio::AudioObject::EnsureFormatRenderHandler

namespace HephAudio
{
	struct AudioObject
	{
		HephCommon::Guid id;
		HephCommon::StringBuffer filePath;
		HephCommon::StringBuffer name;
		bool isPaused;
		uint32_t playCount;
		heph_float volume;
		AudioBuffer buffer;
		size_t frameIndex;
		HephCommon::Event OnRender;
		HephCommon::Event OnFinishedPlaying;
		AudioObject();
		AudioObject(AudioObject&& rhs) noexcept;
		AudioObject& operator=(AudioObject&& rhs) noexcept;
		heph_float GetPosition() const;
		void SetPosition(heph_float position);
		static void DefaultRenderHandler(const HephCommon::EventParams& eventParams);
		static void EnsureFormatRenderHandler(const HephCommon::EventParams& eventParams);
	};
}