#ifdef __ANDROID__
#include "AndroidAudioBase.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"

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
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, "AndroidAudioBase::AndroidAudioBase", "An error occurred whilst getting the current device's api level."));
			}
			else if (deviceApiLevel >= 23)
			{
				if (jvm == nullptr)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "AndroidAudioBase::AndroidAudioBase", "jvm cannot be nullptr."));
					throw AudioException(E_INVALIDARG, "AndroidAudioBase::AndroidAudioBase", "jvm cannot be nullptr.");
				}

				JNIEnv* env = nullptr;
				GetEnv(&env);
				EnumerateAudioDevices(env);
				deviceThread = std::thread(&AndroidAudioBase::CheckAudioDevices, this);
			}
		}
		AndroidAudioBase::~AndroidAudioBase()
		{
			disposing = true;
			JoinDeviceThread();
		}
		AudioDevice AndroidAudioBase::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			std::vector<AudioDevice> audioDevices = GetAudioDevices(deviceType);
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (audioDevices.at(i).isDefault && audioDevices.at(i).type == deviceType)
				{
					return audioDevices.at(i);
				}
			}
			return AudioDevice();
		}
		std::vector<AudioDevice> AndroidAudioBase::GetAudioDevices(AudioDeviceType deviceType) const
		{
			std::vector<AudioDevice> result;

			if (deviceType == AudioDeviceType::Null)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "AndroidAudioBase::GetAudioDevices", "DeviceType must not be Null."));
				return result;
			}

			mutex.lock();
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if ((audioDevices.at(i).type & deviceType) != AudioDeviceType::Null)
				{
					result.push_back(audioDevices.at(i));
				}
			}
			mutex.unlock();

			return result;
		}
		void AndroidAudioBase::JoinDeviceThread()
		{
			if (deviceThread.joinable())
			{
				deviceThread.join();
			}
		}
		void AndroidAudioBase::EnumerateAudioDevices(JNIEnv* env)
		{
			if (deviceApiLevel >= 23)
			{
				audioDevices.clear();

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
		}
		void AndroidAudioBase::CheckAudioDevices()
		{
			constexpr std::chrono::nanoseconds period_ns = std::chrono::nanoseconds((int64_t)(250.0 * 1e6));
			JNIEnv* env = nullptr;
			GetEnv(&env);
			AudioDeviceEventArgs deviceEventArgs = AudioDeviceEventArgs(this, AudioDevice());

			while (!disposing)
			{
				std::this_thread::sleep_for(period_ns);

				mutex.lock();
				std::vector<AudioDevice> oldDevices = audioDevices;
				EnumerateAudioDevices(env);
				mutex.unlock();

				if (OnAudioDeviceAdded)
				{
					for (size_t i = 0; i < audioDevices.size(); i++)
					{
						for (size_t j = 0; j < oldDevices.size(); j++)
						{
							if (audioDevices.at(i).id == oldDevices.at(j).id)
							{
								goto ADD_BREAK;
							}
						}
						deviceEventArgs.audioDevice = audioDevices.at(i);
						OnAudioDeviceAdded(&deviceEventArgs, nullptr);
					ADD_BREAK:;
					}
				}

				if (OnAudioDeviceRemoved)
				{
					for (size_t i = 0; i < oldDevices.size(); i++)
					{
						for (size_t j = 0; j < audioDevices.size(); j++)
						{
							if (oldDevices.at(i).id == audioDevices.at(j).id)
							{
								goto REMOVE_BREAK;
							}
						}
						deviceEventArgs.audioDevice = oldDevices.at(i);
						OnAudioDeviceRemoved(&deviceEventArgs, nullptr);
					REMOVE_BREAK:;
					}
				}
			}
		}
		void AndroidAudioBase::GetEnv(JNIEnv** pEnv) const
		{
			jint jniResult = jvm->GetEnv((void**)pEnv, JNI_VERSION_1_6);
			if (jniResult == JNI_EDETACHED)
			{
				jniResult = jvm->AttachCurrentThread(pEnv, nullptr);
				if (jniResult != JNI_OK)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(jniResult, "AndroidAudioBase::GetAudioDevices", "Failed to attach to the current thread."));
				}
			}
			else if (jniResult != JNI_OK)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(jniResult, "AndroidAudioBase::GetAudioDevices", "Could not get the current jni environment."));
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