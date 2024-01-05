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
	private:
		static void OnRenderHandler(const HephCommon::EventParams& eventParams);
	};
}