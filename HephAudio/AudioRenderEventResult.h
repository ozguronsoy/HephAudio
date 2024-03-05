#pragma once
#include "HephAudioShared.h"
#include "../HephCommon/HeaderFiles/EventResult.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioRenderEventResult : public HephCommon::EventResult
	{
		AudioBuffer renderBuffer;
		bool isFinishedPlaying;
		AudioRenderEventResult() : isFinishedPlaying(false) {}
	};
}