#pragma once
#ifdef __ANDROID__
#include "NativeAudio.h"
#include <mutex>
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
			/// <summary>
			/// The available audio devices.
			/// </summary>
			std::vector<AudioDevice> audioDevices;
			/// <summary>
			/// Enumerates the audio devices periodically to detect any change in available devices.
			/// </summary>
			std::thread deviceThread;
			/// <summary>
			/// Locks to prevent race condition while accessing and enumerating audio devices.
			/// </summary>
			mutable std::mutex deviceMutex;
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
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const override;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const override;
		protected:
			/// <summary>
			/// Waits for the device thread to join.
			/// </summary>
			virtual void JoinDeviceThread();
			/// <summary>
			/// Enumerates the audio devices using the JNI environment to handle native java calls.
			/// </summary>
			/// <param name="env">The JNI environment handles the native java calls for audio device enumeration.</param>
			virtual void EnumerateAudioDevices(JNIEnv* env);
			/// <summary>
			/// Enumerates the audio devices periodically to detect any change in available devices.
			/// </summary>
			virtual void CheckAudioDevices();
			/// <summary>
			/// Gets the JNI environment.
			/// </summary>
			virtual void GetEnv(JNIEnv** pEnv) const;
			/// <summary>
			/// Creates a C string from the java string.
			/// </summary>
			/// <param name="env">The JNI environment.</param>
			/// <param name="jStr">The java string.</param>
			/// <returns></returns>
			virtual StringBuffer JStringToString(JNIEnv* env, jstring jStr) const;
		};
	}
}
#endif