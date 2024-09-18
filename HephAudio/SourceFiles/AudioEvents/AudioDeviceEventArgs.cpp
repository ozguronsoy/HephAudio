#include "AudioEvents/AudioDeviceEventArgs.h"

namespace HephAudio
{
	AudioDeviceEventArgs::AudioDeviceEventArgs(Native::NativeAudio* pNativeAudio, AudioDevice audioDevice) 
		: AudioEventArgs(pNativeAudio), audioDevice(audioDevice) {}
}