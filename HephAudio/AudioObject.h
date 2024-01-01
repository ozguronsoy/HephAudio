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
		void* userEventArgs; // pointer to an object that is supplied by the user to use in OnRender and OnFinishedPlaying events
		HephCommon::Event OnRender;
		HephCommon::Event OnFinishedPlaying;
		AudioObject();
		AudioObject(AudioObject&& rhs) noexcept;
		AudioObject& operator=(AudioObject&& rhs) noexcept;
	private:
		static void OnRenderHandler(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
	};
}