#include "AndroidAudioBase.h"
#ifdef __ANDROID__

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioBase::AndroidAudioBase(JNIEnv* env) : INativeAudio()
		{
			this->env = env;
		}
		AudioDevice AndroidAudioBase::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			std::vector<AudioDevice> audioDevices = GetAudioDevices(deviceType, false);
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (audioDevices.at(i).isDefault)
				{
					return audioDevices.at(i);
				}
			}
			return AudioDevice();
		}
		std::vector<AudioDevice> AndroidAudioBase::GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const
		{
#if __ANDROID_API__ < 23
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudioBase::GetAudioDevices", L"The minimum API level that supports audio device enumeration is 23."));
			return std::vector<AudioDevice>();
#else
			std::vector<AudioDevice> audioDevices;
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
				if ((isSink && (deviceType & AudioDeviceType::Render) == AudioDeviceType::Render) || (!isSink && (deviceType & AudioDeviceType::Capture) == AudioDeviceType::Capture))
				{
					jmethodID getIdMethodId = env->GetMethodID(audioDeviceClass, "getId", "()I");
					jint deviceId = env->CallIntMethod(audioDeviceObject, getIdMethodId);
					jmethodID getNameMethodId = env->GetMethodID(audioDeviceClass, "getProductName", "()Ljava/lang/CharSequence;");
					jobject deviceNameObject = env->CallObjectMethod(audioDeviceObject, getNameMethodId);
					jmethodID toStringMethodId = env->GetMethodID(env->GetObjectClass(deviceNameObject), "toString", "()Ljava/lang/String;");
					jstring deviceName = (jstring)env->CallObjectMethod(deviceNameObject, toStringMethodId);
					AudioDevice audioDevice;
					audioDevice.id = std::to_wstring(deviceId);
					audioDevice.name = JStringToWString(env, deviceName);
					audioDevice.type = isSink ? AudioDeviceType::Render : AudioDeviceType::Capture;
					audioDevice.isDefault = false;
					audioDevices.push_back(audioDevice);
				}
			}
			return audioDevices;
#endif
		}
		std::wstring AndroidAudioBase::JStringToWString(JNIEnv* env, jstring jStr) const
		{
			std::wstring value;
			const jchar* raw = env->GetStringChars(jStr, 0);
			jsize len = env->GetStringLength(jStr);
			value.assign(raw, raw + len);
			env->ReleaseStringChars(jStr, raw);
			return value;
		}
	}
}
#endif