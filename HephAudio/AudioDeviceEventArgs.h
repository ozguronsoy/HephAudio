#pragma once
#include "HephAudioFramework.h"
#include "AudioEventArgs.h"
#include "AudioDevice.h"

namespace HephAudio
{
	struct AudioDeviceEventArgs : public AudioEventArgs
	{
		AudioDevice audioDevice;
		AudioDeviceEventArgs(void* pNativeAudio, AudioDevice audioDevice) : AudioEventArgs(pNativeAudio), audioDevice(audioDevice) {}
	};
}