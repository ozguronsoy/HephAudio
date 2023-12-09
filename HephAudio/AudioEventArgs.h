#pragma once
#include "HephAudioFramework.h"
#include "../HephCommon/HeaderFiles/EventArgs.h"

namespace HephAudio
{
	struct AudioEventArgs : public HephCommon::EventArgs
	{
		void* pNativeAudio;
		void* pAudioObject;
		AudioEventArgs(const void* pNativeAudio, void* pAudioObject);
		virtual ~AudioEventArgs() = default;
	};
}