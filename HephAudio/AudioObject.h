#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
#include "../HephCommon/HeaderFiles/Event.h"
#include "AudioRenderEventArgs.h"
#include "AudioRenderEventResult.h"
#include "AudioFinishedPlayingEventArgs.h"
#include <vector>

namespace HephAudio
{
	struct AudioObject
	{
		HephCommon::StringBuffer filePath;
		HephCommon::StringBuffer name;
		bool isPaused;
		uint32_t loopCount;
		heph_float volume;
		AudioBuffer buffer;
		size_t frameIndex;
		HephCommon::StringBuffer queueName;
		uint32_t queueIndex;
		heph_float queueDelay_ms;
		HephCommon::Event OnRender;
		HephCommon::Event OnFinishedPlaying;
		AudioObject();
		AudioObject(AudioObject&& rhs) noexcept;
		AudioObject& operator=(AudioObject&& rhs) noexcept;
	private:
		static void OnRenderHandler(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
	};
}