#ifdef __ANDROID__
#include "AndroidAudioBase.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioBase::AndroidAudioBase(JavaVM* jvm) : NativeAudio()
			, jvm(jvm)
		{
			HEPHAUDIO_STOPWATCH_START;

			deviceApiLevel = android_get_device_api_level();
			if (deviceApiLevel == -1)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "AndroidAudioBase::AndroidAudioBase", "An error occurred whilst getting the current device's api level."));
			}
			else if (deviceApiLevel >= 23)
			{
				if (jvm == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "AndroidAudioBase::AndroidAudioBase", "jvm cannot be nullptr."));
				}
			}
		}
		AndroidAudioBase::~AndroidAudioBase()
		{
			disposing = true;
			JoinDeviceThread();
		}
		bool AndroidAudioBase::EnumerateAudioDevices()
		{
			if (deviceApiLevel >= 23)
			{
				JNIEnv* env = nullptr;
				GetEnv(&env);

				jclass audioManagerClass = env->FindClass("android/media/AudioManager");
				jobject audioManagerObject = env->AllocObject(audioManagerClass);

				jmethodID getDevicesMethodId = env->GetMethodID(audioManagerClass, "getDevices", "(I)[Landroid/media/AudioDeviceInfo;");
				jarray audioDeviceArray = (jarray)env->CallObjectMethod(audioManagerObject, getDevicesMethodId, 3);
				jsize audioDeviceCount = env->GetArrayLength(audioDeviceArray);

				for (jsize i = 0; i < audioDeviceCount; i++)
				{
					jobject audioDeviceObject = env->GetObjectArrayElement((jobjectArray)audioDeviceArray, i);
					jclass audioDeviceClass = env->GetObjectClass(audioDeviceObject);

					jmethodID isSinkMethodId = env->GetMethodID(audioDeviceClass, "isSink", "()Z");
					jboolean isSink = env->CallBooleanMethod(audioDeviceObject, isSinkMethodId);

					jmethodID getIdMethodId = env->GetMethodID(audioDeviceClass, "getId", "()I");
					jint deviceId = env->CallIntMethod(audioDeviceObject, getIdMethodId);

					jmethodID getNameMethodId = env->GetMethodID(audioDeviceClass, "getProductName", "()Ljava/lang/CharSequence;");
					jobject deviceNameObject = env->CallObjectMethod(audioDeviceObject, getNameMethodId);
					jclass deviceNameClass = env->GetObjectClass(deviceNameObject);
					jmethodID toStringMethodId = env->GetMethodID(deviceNameClass, "toString", "()Ljava/lang/String;");
					jstring deviceName = (jstring)env->CallObjectMethod(deviceNameObject, toStringMethodId);

					AudioDevice audioDevice;
					audioDevice.id = StringBuffer::ToString(deviceId).c_str();
					audioDevice.name = JStringToString(env, deviceName);
					audioDevice.type = isSink ? AudioDeviceType::Render : AudioDeviceType::Capture;
					audioDevice.isDefault = false;
					audioDevices.push_back(audioDevice);

					env->DeleteLocalRef(audioDeviceObject);
					env->DeleteLocalRef(deviceNameObject);
					env->DeleteLocalRef(audioDeviceClass);
					env->DeleteLocalRef(deviceNameClass);
					env->DeleteLocalRef(deviceName);
				}

				env->DeleteLocalRef(audioDeviceArray);
				env->DeleteLocalRef(audioManagerObject);
				env->DeleteLocalRef(audioManagerClass);
			}

			return NativeAudio::DEVICE_ENUMERATION_SUCCESS;
		}
		void AndroidAudioBase::GetEnv(JNIEnv** pEnv) const
		{
			jint jniResult = jvm->GetEnv((void**)pEnv, JNI_VERSION_1_6);
			if (jniResult == JNI_EDETACHED)
			{
				jniResult = jvm->AttachCurrentThread(pEnv, nullptr);
				if (jniResult != JNI_OK)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(jniResult, "AndroidAudioBase::GetAudioDevices", "Failed to attach to the current thread."));
				}
			}
			else if (jniResult != JNI_OK)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(jniResult, "AndroidAudioBase::GetAudioDevices", "Could not get the current jni environment."));
			}
		}
		StringBuffer AndroidAudioBase::JStringToString(JNIEnv* env, jstring jStr) const
		{
			std::string value;
			const jchar* raw = env->GetStringChars(jStr, 0);
			jsize len = env->GetStringLength(jStr);
			value.assign(raw, raw + len);
			env->ReleaseStringChars(jStr, raw);
			return value.c_str();
		}
	}
}
#endif