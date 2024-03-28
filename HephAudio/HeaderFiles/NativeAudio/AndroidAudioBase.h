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
			uint32_t deviceApiLevel;
		public:
			static JavaVM* jvm;
		public:
			AndroidAudioBase();
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