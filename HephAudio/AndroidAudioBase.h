#pragma once
#ifdef __ANDROID__
#include "NativeAudio.h"
#include <jni.h>
#include <android/api-level.h>

namespace HephAudio
{
	namespace Native
	{
		class AndroidAudioBase : public NativeAudio
		{
		protected:
			JavaVM* jvm;
			uint32_t deviceApiLevel;
		public:
			AndroidAudioBase(JavaVM* jvm);
			AndroidAudioBase(const AndroidAudioBase&) = delete;
			AndroidAudioBase& operator=(const AndroidAudioBase&) = delete;
			virtual ~AndroidAudioBase();
		protected:
			bool EnumerateAudioDevices() override;
			void GetEnv(JNIEnv** pEnv) const;
			HephCommon::StringBuffer JStringToString(JNIEnv* env, jstring jStr) const;
		};
	}
}
#endif