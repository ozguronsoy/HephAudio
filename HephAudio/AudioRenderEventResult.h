#pragma once
#include "framework.h"
#include "AudioEventResult.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioRenderEventResult : public AudioEventResult
	{
	public:
		AudioBuffer renderBuffer;
		bool isFinishedPlaying;
	public:
		virtual ~AudioRenderEventResult() = default;
	};
}