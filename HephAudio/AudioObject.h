#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
#include "../HephCommon/HeaderFiles/Event.h"
#include "../HephCommon/HeaderFiles/Guid.h"
#include "AudioRenderEventArgs.h"
#include "AudioRenderEventResult.h"
#include "AudioFinishedPlayingEventArgs.h"
#include <vector>

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
		uint32_t loopCount;
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