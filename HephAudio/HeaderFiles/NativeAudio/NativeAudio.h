#pragma once
#include "HephAudioShared.h"
#include "AudioDevice.h"
#include "AudioObject.h"
#include "AudioFormatInfo.h"
#include "EchoInfo.h"
#include "AudioEvents/AudioDeviceEventArgs.h"
#include "AudioEvents/AudioCaptureEventArgs.h"
#include "HephException.h"
#include "Event.h"
#include <vector>
#include <thread>
#include <mutex>

namespace HephAudio
{
	namespace Native
	{
		class NativeAudio
		{
		protected:
			static constexpr bool DEVICE_ENUMERATION_FAIL = false;
			static constexpr bool DEVICE_ENUMERATION_SUCCESS = true;
		protected:
			std::vector<AudioObject> audioObjects;
			std::vector<AudioDevice> audioDevices;
			std::thread::id mainThreadId;
			std::thread renderThread;
			std::thread captureThread;
			std::thread deviceThread;
			HephCommon::StringBuffer renderDeviceId;
			HephCommon::StringBuffer captureDeviceId;
			AudioFormatInfo renderFormat;
			AudioFormatInfo captureFormat;
			bool disposing;
			bool isRenderInitialized;
			bool isCaptureInitialized;
			bool isCapturePaused;
			uint32_t deviceEnumerationPeriod_ms;
			mutable std::mutex audioDevicesMutex;
			mutable std::recursive_mutex audioObjectsMutex;
		public:
			HephCommon::Event OnAudioDeviceAdded;
			HephCommon::Event OnAudioDeviceRemoved;
			HephCommon::Event OnCapture;
		public:
			NativeAudio();
			NativeAudio(const NativeAudio&) = delete;
			NativeAudio& operator=(const NativeAudio&) = delete;
			virtual ~NativeAudio() = default;
			AudioObject* Play(const HephCommon::StringBuffer& filePath);
			AudioObject* Play(const HephCommon::StringBuffer& filePath, uint32_t playCount);
			AudioObject* Play(const HephCommon::StringBuffer& filePath, uint32_t playCount, bool isPaused);
			AudioObject* Load(const HephCommon::StringBuffer& filePath);
			AudioObject* Load(const HephCommon::StringBuffer& filePath, uint32_t playCount);
			AudioObject* Load(const HephCommon::StringBuffer& filePath, uint32_t playCount, bool isPaused);
			AudioObject* CreateAudioObject(const HephCommon::StringBuffer& name, size_t bufferFrameCount, AudioFormatInfo bufferFormatInfo);
			bool DestroyAudioObject(AudioObject* pAudioObject);
			bool DestroyAudioObject(const HephCommon::Guid& audioObjectId);
			bool AudioObjectExists(AudioObject* pAudioObject) const;
			bool AudioObjectExists(const HephCommon::Guid& audioObjectId) const;
			AudioObject* GetAudioObject(size_t index);
			AudioObject* GetAudioObject(const HephCommon::Guid& audioObjectId);
			AudioObject* GetAudioObject(const HephCommon::StringBuffer& audioObjectName);
			size_t GetAudioObjectCount() const;
			void ResumeCapture();
			void PauseCapture();
			bool IsCapturePaused() const;
			uint32_t GetDeviceEnumerationPeriod() const;
			void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms);
			virtual void SetMasterVolume(heph_float volume) = 0;
			virtual heph_float GetMasterVolume() const = 0;
			AudioFormatInfo GetRenderFormat() const;
			AudioFormatInfo GetCaptureFormat() const;
			void InitializeRender();
			void InitializeRender(AudioChannelLayout channelLayout, uint32_t sampleRate);
			void InitializeRender(AudioFormatInfo format);
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopRendering() = 0;
			void InitializeCapture();
			void InitializeCapture(AudioChannelLayout channelLayout, uint32_t sampleRate);
			void InitializeCapture(AudioFormatInfo format);
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopCapturing() = 0;
			AudioDevice GetAudioDeviceById(const HephCommon::StringBuffer& deviceId) const;
			AudioDevice GetRenderDevice() const;
			AudioDevice GetCaptureDevice() const;
			AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
			std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
			bool SaveToFile(AudioBuffer& buffer, const HephCommon::StringBuffer& filePath, bool overwrite);
		protected:
			virtual bool EnumerateAudioDevices() = 0;
			virtual void CheckAudioDevices();
			void JoinRenderThread();
			void JoinCaptureThread();
			void JoinDeviceThread();
			void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			size_t GetAOCountToMix() const;
			virtual heph_float GetFinalAOVolume(AudioObject* pAudioObject) const;
		};
	}
}