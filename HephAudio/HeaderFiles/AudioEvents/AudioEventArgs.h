#pragma once
#include "HephAudioShared.h"
#include "EventArgs.h"

namespace HephAudio
{
	struct AudioEventArgs : public HephCommon::EventArgs
	{
		void* pNativeAudio;
		AudioEventArgs(void* pNativeAudio) : pNativeAudio(pNativeAudio) {}
		virtual ~AudioEventArgs() = default;
	};
}