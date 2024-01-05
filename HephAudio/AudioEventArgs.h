#pragma once
#include "HephAudioFramework.h"
#include "../HephCommon/HeaderFiles/EventArgs.h"

namespace HephAudio
{
	struct AudioEventArgs : public HephCommon::EventArgs
	{
		void* pNativeAudio;
		AudioEventArgs(void* pNativeAudio) : pNativeAudio(pNativeAudio) {}
		virtual ~AudioEventArgs() = default;
	};
}