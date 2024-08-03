#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "Event.h"
#include "Guid.h"
#include <vector>

#define HEPHAUDIO_INFINITE_LOOP 0
#define HEPHAUDIO_RENDER_HANDLER_DEFAULT &HephAudio::AudioObject::DefaultRenderHandler
#define HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT &HephAudio::AudioObject::MatchFormatRenderHandler

namespace HephAudio
{
	struct AudioObject
	{
		HephCommon::Guid id;
		std::string filePath;
		std::string name;
		bool isPaused;
		uint32_t playCount;
		double volume;
		AudioBuffer buffer;
		size_t frameIndex;
		HephCommon::Event OnRender;
		HephCommon::Event OnFinishedPlaying;
		AudioObject();
		AudioObject(AudioObject&& rhs) noexcept;
		AudioObject& operator=(AudioObject&& rhs) noexcept;
		double GetPosition() const;
		void SetPosition(double position);
		static void DefaultRenderHandler(const HephCommon::EventParams& eventParams);
		static void MatchFormatRenderHandler(const HephCommon::EventParams& eventParams);
	};
}