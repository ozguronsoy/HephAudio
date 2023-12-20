#pragma once
#include "HephAudioFramework.h"
#include "NativeAudio.h"
#ifdef __ANDROID__
#include <jni.h>
#endif

namespace HephAudio
{
	namespace Native
	{
		class NativeAudio;
	}

	class Audio final
	{
	private:
		Native::NativeAudio* pNativeAudio;
	public:
		void SetOnAudioDeviceAddedHandler(HephCommon::EventHandler handler);
		void AddOnAudioDeviceAddedHandler(HephCommon::EventHandler handler);
		void SetOnAudioDeviceRemovedHandler(HephCommon::EventHandler handler);
		void AddOnAudioDeviceRemovedHandler(HephCommon::EventHandler handler);
		void SetOnCaptureHandler(HephCommon::EventHandler handler);
		void AddOnCaptureHandler(HephCommon::EventHandler handler);
	public:
#ifdef __ANDROID__
		Audio(JavaVM* jvm);
#else
		Audio();
#endif
		Audio(const Audio&) = delete;
		Audio& operator=(const Audio&) = delete;
		~Audio();
		Native::NativeAudio* operator->() const noexcept;
		Native::NativeAudio* GetNativeAudio() const;
		AudioObject* Play(HephCommon::StringBuffer filePath);
		AudioObject* Play(HephCommon::StringBuffer filePath, uint32_t loopCount);
		AudioObject* Play(HephCommon::StringBuffer filePath, uint32_t loopCount, bool isPaused);
		std::vector<AudioObject*> Queue(HephCommon::StringBuffer queueName, heph_float queueDelay_ms, const std::vector<HephCommon::StringBuffer>& filePaths);
		AudioObject* Load(HephCommon::StringBuffer filePath);
		AudioObject* CreateAO(HephCommon::StringBuffer name, size_t bufferFrameCount);
		bool DestroyAO(AudioObject* pAudioObject);
		bool AOExists(AudioObject* pAudioObject) const;
		AudioObject* GetAO(HephCommon::StringBuffer aoName) const;
		AudioObject* GetAO(HephCommon::StringBuffer queueName, size_t index) const;
		void PauseCapture(bool pause);
		bool IsCapturePaused() const noexcept;
		uint64_t GetDeviceEnumerationPeriod() const noexcept;
		void SetDeviceEnumerationPeriod(uint64_t deviceEnumerationPeriod_ns) noexcept;
		void SetMasterVolume(heph_float volume);
		heph_float GetMasterVolume() const;
		void Skip(HephCommon::StringBuffer queueName, bool applyDelay);
		void Skip(size_t skipCount, HephCommon::StringBuffer queueName, bool applyDelay);
		AudioFormatInfo GetRenderFormat() const;
		AudioFormatInfo GetCaptureFormat() const;
		void InitializeRender(AudioDevice* device, AudioFormatInfo format);
		void StopRendering();
		void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
		void StopCapturing();
#if (defined(_WIN32))
		void SetDisplayName(HephCommon::StringBuffer displayName);
		void SetIconPath(HephCommon::StringBuffer iconPath);
#endif
		AudioDevice GetAudioDeviceById(HephCommon::StringBuffer deviceId) const;
		AudioDevice GetRenderDevice() const;
		AudioDevice GetCaptureDevice() const;
		AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
		std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
		bool SaveToFile(HephCommon::StringBuffer filePath, bool overwrite, AudioBuffer& buffer);
	};
}