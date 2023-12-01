#pragma once
#include "HephAudioFramework.h"
#include "EventResult.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioRenderEventResult : public HephCommon::EventResult
	{
		AudioBuffer renderBuffer;
		bool isFinishedPlaying{ false };
	};
}