#pragma once
#include "framework.h"
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
		HephAudio::Native::NativeAudio* pNativeAudio;
	public:
		Formats::AudioFormats* GetAudioFormats() const;
		void SetOnExceptionHandler(AudioEventHandler handler);
		void SetOnAudioDeviceAddedHandler(AudioEventHandler handler);
		void SetOnAudioDeviceRemovedHandler(AudioEventHandler handler);
		void SetOnCaptureHandler(AudioEventHandler handler);
	public:
#ifdef __ANDROID__
		Audio(JavaVM* jvm);
#else
		Audio();
#endif
		Audio(const Audio&) = delete;
		Audio& operator=(const Audio&) = delete;
		~Audio();
		HephAudio::Native::NativeAudio* operator->() const noexcept;
		HephAudio::Native::NativeAudio* GetNativeAudio() const;
		std::shared_ptr<AudioObject> Play(StringBuffer filePath);
		std::shared_ptr<AudioObject> Play(StringBuffer filePath, bool isPaused);
		std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount);
		std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount, bool isPaused);
		std::vector<std::shared_ptr<AudioObject>> Queue(StringBuffer queueName, hephaudio_float queueDelay_ms, const std::vector<StringBuffer>& filePaths);
		std::shared_ptr<AudioObject> Load(StringBuffer filePath);
		std::shared_ptr<AudioObject> CreateAO(StringBuffer name, size_t bufferFrameCount);
		bool DestroyAO(std::shared_ptr<AudioObject> pAudioObject);
		bool AOExists(std::shared_ptr<AudioObject> pAudioObject) const;
		void SetAOPosition(std::shared_ptr<AudioObject> pAudioObject, hephaudio_float position);
		hephaudio_float GetAOPosition(std::shared_ptr<AudioObject> pAudioObject) const;
		std::shared_ptr<AudioObject> GetAO(StringBuffer aoName) const;
		std::shared_ptr<AudioObject> GetAO(StringBuffer queueName, size_t index) const;
		void PauseCapture(bool pause);
		bool IsCapturePaused() const noexcept;
		void SetMasterVolume(hephaudio_float volume);
		hephaudio_float GetMasterVolume() const;
		void Skip(StringBuffer queueName, bool applyDelay);
		void Skip(size_t skipCount, StringBuffer queueName, bool applyDelay);
		AudioFormatInfo GetRenderFormat() const;
		AudioFormatInfo GetCaptureFormat() const;
		void InitializeRender(AudioDevice* device, AudioFormatInfo format);
		void StopRendering();
		void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
		void StopCapturing();
#if (defined(_WIN32) && defined(_WIN32_WINNT_VISTA))
		void SetDisplayName(StringBuffer displayName);
		void SetIconPath(StringBuffer iconPath);
#endif
#if defined(_WIN32)
		AudioDevice GetAudioDeviceById(StringBuffer deviceId) const;
		AudioDevice GetRenderDevice() const;
		AudioDevice GetCaptureDevice() const;
		AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
		std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
#endif
		bool SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer);
	};
}