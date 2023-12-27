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
		Native::NativeAudio* GetNativeAudio() const;
		AudioObject* Play(HephCommon::StringBuffer filePath);
		AudioObject* Play(HephCommon::StringBuffer filePath, uint32_t loopCount);
		AudioObject* Play(HephCommon::StringBuffer filePath, uint32_t loopCount, bool isPaused);
		AudioObject* Load(HephCommon::StringBuffer filePath);
		AudioObject* CreateAudioObject(HephCommon::StringBuffer name, size_t bufferFrameCount);
		bool DestroyAudioObject(AudioObject* pAudioObject);
		bool AudioObjectExists(AudioObject* pAudioObject) const;
		AudioObject* GetAudioObject(size_t index) const;
		AudioObject* GetAudioObject(HephCommon::StringBuffer audioObjectName) const;
		void PauseCapture(bool pause);
		bool IsCapturePaused() const;
		uint32_t GetDeviceEnumerationPeriod() const;
		void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms);
		void SetMasterVolume(heph_float volume);
		heph_float GetMasterVolume() const;
		AudioFormatInfo GetRenderFormat() const;
		AudioFormatInfo GetCaptureFormat() const;
		void InitializeRender(AudioDevice* device, AudioFormatInfo format);
		void StopRendering();
		void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
		void StopCapturing();
		AudioDevice GetAudioDeviceById(HephCommon::StringBuffer deviceId) const;
		AudioDevice GetRenderDevice() const;
		AudioDevice GetCaptureDevice() const;
		AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
		std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
		bool SaveToFile(HephCommon::StringBuffer filePath, bool overwrite, AudioBuffer& buffer);
	};
}