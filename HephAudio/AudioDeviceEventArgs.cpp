#include "AudioDeviceEventArgs.h"

namespace HephAudio
{
	AudioDeviceEventArgs::AudioDeviceEventArgs(const void* pNativeAudio, AudioDevice& audioDevice) : AudioEventArgs(pNativeAudio, nullptr), audioDevice(audioDevice) { }
}