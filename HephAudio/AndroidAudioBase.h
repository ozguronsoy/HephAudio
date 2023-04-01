#pragma once
#ifdef __ANDROID__
#include "NativeAudio.h"
#include <jni.h>
#include <android/api-level.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Handles the audio device enumeration for android.
		/// </summary>
		class AndroidAudioBase : public NativeAudio
		{
		protected:
			/// <summary>
			/// The pointer to java virtual machine to get the JNI environment when needed and attach it to the current thread if not attached.
			/// </summary>
			JavaVM* jvm;
			/// <summary>
			/// The API level of the android device.
			/// </summary>
			uint32_t deviceApiLevel;
		public:
			/// <summary>
			/// Creates and initalizes an AndroidAudioBase instance.
			/// </summary>
			/// <param name="jvm">The pointer to java virtual machine that will be handling native java calls necessary for audio device enumeration.</param>
			AndroidAudioBase(JavaVM* jvm);
			AndroidAudioBase(const AndroidAudioBase&) = delete;
			AndroidAudioBase& operator=(const AndroidAudioBase&) = delete;
			/// <summary>
			/// Frees the JNI resources and joins the audio device thread.
			/// </summary>
			virtual ~AndroidAudioBase();
		protected:
			/// <summary>
			/// Enumerates the audio devices using the JNI environment to handle native java calls.
			/// </summary>
			bool EnumerateAudioDevices() override;
			/// <summary>
			/// Gets the JNI environment.
			/// </summary>
			void GetEnv(JNIEnv** pEnv) const;
			/// <summary>
			/// Creates a C string from the java string.
			/// </summary>
			/// <param name="env">The JNI environment.</param>
			/// <param name="jStr">The java string.</param>
			HephCommon::StringBuffer JStringToString(JNIEnv* env, jstring jStr) const;
		};
	}
}
#endif