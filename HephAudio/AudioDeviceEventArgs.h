#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioDevice.h"

namespace HephAudio
{
	struct AudioDeviceEventArgs : public AudioEventArgs
	{
	public:
		AudioDevice audioDevice;
	public:
		AudioDeviceEventArgs(const void* pNativeAudio, AudioDevice audioDevice);
		virtual ~AudioDeviceEventArgs() = default;
	};
}