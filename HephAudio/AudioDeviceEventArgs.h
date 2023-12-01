#pragma once
#include "HephAudioFramework.h"
#include "AudioEventArgs.h"
#include "AudioDevice.h"

namespace HephAudio
{
	struct AudioDeviceEventArgs : public AudioEventArgs
	{
		AudioDevice audioDevice;
		AudioDeviceEventArgs(const void* pNativeAudio, AudioDevice audioDevice);
	};
}