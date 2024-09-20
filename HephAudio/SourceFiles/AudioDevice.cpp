#include "AudioDevice.h"

namespace HephAudio
{
	AudioDevice::AudioDevice() : id(""), name(""), type(AudioDeviceType::Render), isDefault(false) {}
}