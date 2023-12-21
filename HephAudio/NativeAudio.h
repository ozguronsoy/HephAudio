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

namespace HephAudio
{
	namespace Native
	{
		class NativeAudio
		{
		protected:
			static HEPH_CONSTEXPR bool DEVICE_ENUMERATION_FAIL = false;
			static HEPH_CONSTEXPR bool DEVICE_ENUMERATION_SUCCESS = true;
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
			HephCommon::StringBuffer displayName;
			HephCommon::StringBuffer iconPath;
			uint32_t deviceEnumerationPeriod_ms;
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
			AudioObject* Load(HephCommon::StringBuffer filePath);
			AudioObject* CreateAO(HephCommon::StringBuffer name, size_t bufferFrameCount);
			bool DestroyAO(AudioObject* pAudioObject);
			bool AOExists(AudioObject* pAudioObject) const;
			AudioObject* GetAO(size_t index);
			AudioObject* GetAO(HephCommon::StringBuffer aoName);
			void PauseCapture(bool pause);
			bool IsCapturePaused() const noexcept;
			uint32_t GetDeviceEnumerationPeriod() const noexcept;
			void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms) noexcept;
			virtual void SetMasterVolume(heph_float volume) = 0;
			virtual heph_float GetMasterVolume() const = 0;
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
			void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			size_t GetAOCountToMix() const;
			virtual heph_float GetFinalAOVolume(AudioObject* pAudioObject) const;
		};
	}
}