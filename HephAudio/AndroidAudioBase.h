#pragma once
#ifdef __ANDROID__
#include "INativeAudio.h"
#include <jni.h>

namespace HephAudio
{
	namespace Native
	{
		class AndroidAudioBase : public INativeAudio
		{
		protected:
			JNIEnv* env;
		public:
			AndroidAudioBase(JNIEnv* env);
			AndroidAudioBase(const AndroidAudioBase&) = delete;
			AndroidAudioBase& operator=(const AndroidAudioBase&) = delete;
			virtual ~AndroidAudioBase() = default;
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const override;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const override;
		protected:
			virtual std::wstring JStringToWString(JNIEnv* env, jstring jStr) const;
		};
	}
}
#endif