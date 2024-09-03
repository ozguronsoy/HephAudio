#pragma once
#include "HephAudioShared.h"
#include "AudioDevice.h"
#include "AudioObject.h"
#include "AudioFormatInfo.h"
#include "IAudioDecoder.h"
#include "IAudioEncoder.h"
#include "AudioEvents/AudioDeviceEventArgs.h"
#include "AudioEvents/AudioCaptureEventArgs.h"
#include "Params/NativeAudioParams.h"
#include "HephException.h"
#include "Event.h"
#include "StringHelpers.h"
#include <memory>
#include <string>
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
			std::shared_ptr<IAudioDecoder> pAudioDecoder;
			std::shared_ptr<IAudioEncoder> pAudioEncoder;
			std::vector<AudioObject> audioObjects;
			std::vector<AudioDevice> audioDevices;
			std::thread::id mainThreadId;
			std::thread renderThread;
			std::thread captureThread;
			std::thread deviceThread;
			std::string renderDeviceId;
			std::string captureDeviceId;
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
			std::shared_ptr<IAudioDecoder> GetAudioDecoder() const;
			void SetAudioDecoder(std::shared_ptr<IAudioDecoder> pNewDecoder);
			std::shared_ptr<IAudioEncoder> GetAudioEncoder() const;
			void SetAudioEncoder(std::shared_ptr<IAudioEncoder> pNewEncoder);
			AudioObject* Play(const std::string& filePath);
			AudioObject* Play(const std::string& filePath, uint32_t playCount);
			AudioObject* Play(const std::string& filePath, uint32_t playCount, bool isPaused);
			AudioObject* Load(const std::string& filePath);
			AudioObject* Load(const std::string& filePath, uint32_t playCount);
			AudioObject* Load(const std::string& filePath, uint32_t playCount, bool isPaused);
			AudioObject* CreateAudioObject(const std::string& name, size_t bufferFrameCount, AudioFormatInfo bufferFormatInfo);
			bool DestroyAudioObject(AudioObject* pAudioObject);
			bool DestroyAudioObject(const HephCommon::Guid& audioObjectId);
			bool AudioObjectExists(AudioObject* pAudioObject) const;
			bool AudioObjectExists(const HephCommon::Guid& audioObjectId) const;
			AudioObject* GetAudioObject(size_t index);
			AudioObject* GetAudioObject(const HephCommon::Guid& audioObjectId);
			AudioObject* GetAudioObject(const std::string& audioObjectName);
			size_t GetAudioObjectCount() const;
			void ResumeCapture();
			void PauseCapture();
			bool IsCapturePaused() const;
			uint32_t GetDeviceEnumerationPeriod() const;
			void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms);
			virtual void SetMasterVolume(double volume) = 0;
			virtual double GetMasterVolume() const = 0;
			const AudioFormatInfo& GetRenderFormat() const;
			const AudioFormatInfo& GetCaptureFormat() const;
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
			virtual void GetNativeParams(NativeAudioParams& nativeParams) const = 0;
			virtual void SetNativeParams(const NativeAudioParams& nativeParams) = 0;
			AudioDevice GetAudioDeviceById(const std::string& deviceId) const;
			AudioDevice GetRenderDevice() const;
			AudioDevice GetCaptureDevice() const;
			AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
			std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
		protected:
			virtual bool EnumerateAudioDevices() = 0;
			virtual void CheckAudioDevices();
			void JoinRenderThread();
			void JoinCaptureThread();
			void JoinDeviceThread();
			void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			size_t GetAOCountToMix() const;
			virtual double GetFinalAOVolume(AudioObject* pAudioObject) const;
		};
	}
}