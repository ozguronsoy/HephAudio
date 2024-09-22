#ifdef __ANDROID__
#include "NativeAudio/AndroidAudioBase.h"
#include "Stopwatch.h"
#include "ConsoleLogger.h"

using namespace Heph;

namespace HephAudio
{
	namespace Native
	{
		JavaVM* AndroidAudioBase::jvm = nullptr;

		AndroidAudioBase::AndroidAudioBase() : NativeAudio()
		{
			deviceApiLevel = android_get_device_api_level();
			if (deviceApiLevel == -1)
			{
				HEPH_RAISE_EXCEPTION(this, Exception(HEPH_EC_FAIL, HEPH_FUNC, "An error occurred while getting the current device's API level."));
			}
			else if (deviceApiLevel >= 23)
			{
				if (AndroidAudioBase::jvm == nullptr)
				{
					HEPH_RAISE_AND_THROW_EXCEPTION(this, Exception(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "jvm cannot be nullptr. Set the AndroidAudioBase::jvm static field first."));
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
					audioDevice.id = StringHelpers::ToString(deviceId).c_str();
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

				return NativeAudio::DEVICE_ENUMERATION_SUCCESS;
			}

			return NativeAudio::DEVICE_ENUMERATION_FAIL;
		}
		void AndroidAudioBase::GetEnv(JNIEnv** pEnv) const
		{
			jint jniResult = AndroidAudioBase::jvm->GetEnv((void**)pEnv, JNI_VERSION_1_6);
			if (jniResult == JNI_EDETACHED)
			{
				jniResult = AndroidAudioBase::jvm->AttachCurrentThread(pEnv, nullptr);
				if (jniResult != JNI_OK)
				{
					HEPH_RAISE_EXCEPTION(this, Exception(jniResult, HEPH_FUNC, "Failed to attach to the current thread."));
				}
			}
			else if (jniResult != JNI_OK)
			{
				HEPH_RAISE_EXCEPTION(this, Exception(jniResult, HEPH_FUNC, "Could not get the current jni environment."));
			}
		}
		std::string AndroidAudioBase::JStringToString(JNIEnv* env, jstring jStr) const
		{
			std::string value;
			const jchar* raw = env->GetStringChars(jStr, 0);
			jsize len = env->GetStringLength(jStr);
			value.assign(raw, raw + len);
			env->ReleaseStringChars(jStr, raw);
			return value;
		}
	}
}

extern "C" __attribute__((weak)) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	HephAudio::Native::AndroidAudioBase::jvm = vm;
	return JNI_VERSION_1_6;
}

#endif