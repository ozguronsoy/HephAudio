#pragma once
#ifdef __ANDROID__
#include "INativeAudio.h"
#include <mutex>
#include <jni.h>
#include <android/api-level.h>

namespace HephAudio
{
	namespace Native
	{
		class AndroidAudioBase : public INativeAudio
		{
		protected:
			JavaVM* jvm;
			uint32_t deviceApiLevel;
			std::vector<AudioDevice> audioDevices;
			std::thread deviceThread;
			mutable std::mutex mutex;
		public:
			AndroidAudioBase(JavaVM* jvm);
			AndroidAudioBase(const AndroidAudioBase&) = delete;
			AndroidAudioBase& operator=(const AndroidAudioBase&) = delete;
			virtual ~AndroidAudioBase();
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const override;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const override;
		protected:
			virtual void JoinDeviceThread();
			virtual void EnumerateAudioDevices(JNIEnv* env);
			virtual void CheckAudioDevices();
			virtual void GetEnv(JNIEnv** pEnv) const;
			virtual std::wstring JStringToWString(JNIEnv* env, jstring jStr) const;
		};
	}
}
#endif