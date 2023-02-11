#pragma once
#include "framework.h"

namespace HephAudio
{
	struct AudioEventArgs
	{
	public:
		void* pNativeAudio;
		void* pAudioObject;
	public:
		AudioEventArgs(const void* pNativeAudio, void* pAudioObject);
		virtual ~AudioEventArgs() = default;
	};
}