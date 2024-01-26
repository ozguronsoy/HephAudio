#pragma once
#include "HephAudioFramework.h"
#include "NativeAudio.h"
#ifdef __ANDROID__
#include <jni.h>
#endif

namespace HephAudio
{
	enum AudioAPI
	{
		Default,
#if defined(_WIN32)
		WASAPI,
		DirectSound,
		MMEAPI,
#elif defined(__APPLE__)
		CoreAudio,
#elif defined(__ANDROID__)
		AAudio,
		OpenSLES,
#elif defined(__linux__)
		ALSA,
#endif
	};

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
		Audio(JavaVM* jvm, AudioAPI api);
#else
		Audio();
		Audio(AudioAPI api);
#endif
		Audio(const Audio&) = delete;
		Audio& operator=(const Audio&) = delete;
		~Audio();
		Native::NativeAudio* GetNativeAudio() const;
		AudioObject* Play(const HephCommon::StringBuffer& filePath);
		AudioObject* Play(const HephCommon::StringBuffer& filePath, uint32_t playCount);
		AudioObject* Play(const HephCommon::StringBuffer& filePath, uint32_t playCount, bool isPaused);
		AudioObject* Load(const HephCommon::StringBuffer& filePath);
		AudioObject* Load(const HephCommon::StringBuffer& filePath, uint32_t playCount);
		AudioObject* Load(const HephCommon::StringBuffer& filePath, uint32_t playCount, bool isPaused);
		AudioObject* CreateAudioObject(const HephCommon::StringBuffer& name, size_t bufferFrameCount);
		bool DestroyAudioObject(AudioObject* pAudioObject);
		bool AudioObjectExists(AudioObject* pAudioObject) const;
		AudioObject* GetAudioObject(size_t index) const;
		AudioObject* GetAudioObject(const HephCommon::StringBuffer& audioObjectName) const;
		void PauseCapture(bool pause);
		bool IsCapturePaused() const;
		uint32_t GetDeviceEnumerationPeriod() const;
		void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms);
		void SetMasterVolume(heph_float volume);
		heph_float GetMasterVolume() const;
		AudioFormatInfo GetRenderFormat() const;
		AudioFormatInfo GetCaptureFormat() const;
		void InitializeRender();
		void InitializeRender(uint16_t channelCount, uint32_t sampleRate);
		void InitializeRender(AudioFormatInfo format);
		void InitializeRender(AudioDevice* device, AudioFormatInfo format);
		void StopRendering();
		void InitializeCapture();
		void InitializeCapture(uint16_t channelCount, uint32_t sampleRate);
		void InitializeCapture(AudioFormatInfo format);
		void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
		void StopCapturing();
		AudioDevice GetAudioDeviceById(const HephCommon::StringBuffer& deviceId) const;
		AudioDevice GetRenderDevice() const;
		AudioDevice GetCaptureDevice() const;
		AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
		std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
		bool SaveToFile(const HephCommon::StringBuffer& filePath, bool overwrite, AudioBuffer& buffer);
	};
}