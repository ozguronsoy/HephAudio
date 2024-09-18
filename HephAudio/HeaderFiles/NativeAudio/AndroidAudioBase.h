#pragma once
#ifdef __ANDROID__
#include "NativeAudio.h"
#include "StringHelpers.h"
#include <jni.h>
#include <android/api-level.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief base class for classes that interact with the Android audio APIs.
		 * 
		 */
		class AndroidAudioBase : public NativeAudio
		{
		protected:
			/**
			 * API level of the current device.
			 * 
			 */
			uint32_t deviceApiLevel;

		public:
			/**
			 * pointer to the Java virtual machine.
			 * 
			 */
			static JavaVM* jvm;

		public:
			/**
			 * creates a new instance and initializes it with default values.
			 * 
			 */
			AndroidAudioBase();

			AndroidAudioBase(const AndroidAudioBase&) = delete;
			AndroidAudioBase& operator=(const AndroidAudioBase&) = delete;

			/**
			 * releases the resources and destroys the instance.
			 * 
			 */
			virtual ~AndroidAudioBase();

		protected:
			bool EnumerateAudioDevices() override;
			void GetEnv(JNIEnv** pEnv) const;
			std::string JStringToString(JNIEnv* env, jstring jStr) const;
		};
	}
}
#endif