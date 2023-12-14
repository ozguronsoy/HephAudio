#pragma once
#include "HephAudioFramework.h"
#include "AudioDevice.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "AudioObject.h"
#include "AudioFormatInfo.h"
#include "EchoInfo.h"
#include "../HephCommon/HeaderFiles/Event.h"
#include "AudioDeviceEventArgs.h"
#include "AudioCaptureEventArgs.h"
#include <vector>
#include <thread>
#include <mutex>

#define HEPHAUDIO_PAUSE true
#define HEPHAUDIO_RESUME false

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
			std::vector<std::thread> queueThreads;
			HephCommon::StringBuffer renderDeviceId;
			HephCommon::StringBuffer captureDeviceId;
			AudioFormatInfo renderFormat;
			AudioFormatInfo captureFormat;
			bool disposing;
			bool isRenderInitialized;
			bool isCaptureInitialized;
			bool isCapturePaused;
			HephCommon::StringBuffer displayName;
			HephCommon::StringBuffer iconPath;
			uint64_t deviceEnumerationPeriod_ns;
			mutable std::mutex audioObjectsMutex;
			mutable std::mutex audioDevicesMutex;
		public:
			HephCommon::Event OnAudioDeviceAdded;
			HephCommon::Event OnAudioDeviceRemoved;
			HephCommon::Event OnCapture;
		public:
			NativeAudio();
			NativeAudio(const NativeAudio&) = delete;
			NativeAudio& operator=(const NativeAudio&) = delete;
			virtual ~NativeAudio() = default;
			AudioObject* Play(HephCommon::StringBuffer filePath);
			AudioObject* Play(HephCommon::StringBuffer filePath, uint32_t loopCount);
			AudioObject* Play(HephCommon::StringBuffer filePath, uint32_t loopCount, bool isPaused);
			std::vector<AudioObject*> Queue(HephCommon::StringBuffer queueName, heph_float queueDelay_ms, const std::vector<HephCommon::StringBuffer>& filePaths);
			AudioObject* Load(HephCommon::StringBuffer filePath);
			AudioObject* CreateAO(HephCommon::StringBuffer name, size_t bufferFrameCount);
			bool DestroyAO(AudioObject* pAudioObject);
			bool AOExists(AudioObject* pAudioObject) const;
			AudioObject* GetAO(HephCommon::StringBuffer aoName);
			AudioObject* GetAO(HephCommon::StringBuffer queueName, size_t index);
			void PauseCapture(bool pause);
			bool IsCapturePaused() const noexcept;
			uint64_t GetDeviceEnumerationPeriod() const noexcept;
			void SetDeviceEnumerationPeriod(uint64_t deviceEnumerationPeriod_ns) noexcept;
			virtual void SetMasterVolume(heph_float volume) = 0;
			virtual heph_float GetMasterVolume() const = 0;
			void Skip(HephCommon::StringBuffer queueName, bool applyDelay);
			void Skip(size_t skipCount, HephCommon::StringBuffer queueName, bool applyDelay);
			AudioFormatInfo GetRenderFormat() const;
			AudioFormatInfo GetCaptureFormat() const;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopRendering() = 0;
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopCapturing() = 0;
			virtual void SetDisplayName(HephCommon::StringBuffer displayName) = 0;
			virtual void SetIconPath(HephCommon::StringBuffer iconPath) = 0;
			AudioDevice GetAudioDeviceById(HephCommon::StringBuffer deviceId) const;
			AudioDevice GetRenderDevice() const;
			AudioDevice GetCaptureDevice() const;
			AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
			std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
			bool SaveToFile(HephCommon::StringBuffer filePath, bool overwrite, AudioBuffer& buffer);
		protected:
			virtual bool EnumerateAudioDevices() = 0;
			void CheckAudioDevices();
			void JoinRenderThread();
			void JoinCaptureThread();
			void JoinDeviceThread();
			void JoinQueueThreads();
			std::vector<AudioObject*> GetQueue(HephCommon::StringBuffer queueName);
			void PlayNextInQueue(HephCommon::StringBuffer queueName, heph_float queueDelay, uint32_t decreaseQueueIndex);
			void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			size_t GetAOCountToMix() const;
			virtual heph_float GetFinalAOVolume(AudioObject* pAudioObject) const;
		};
	}
}