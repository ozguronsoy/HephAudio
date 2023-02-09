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
		enum class AudioExceptionThread : uint8_t;
		typedef void (*AudioExceptionEventHandler)(AudioException exception, AudioExceptionThread exceptionThread);
		typedef void (*AudioDeviceEventHandler)(AudioDevice device);
		typedef void (*AudioCaptureEventHandler)(AudioBuffer& capturedDataBuffer);
		class NativeAudio;
	}

	class Audio final
	{
	private:
		HephAudio::Native::NativeAudio* pNativeAudio;
	public:
		Formats::AudioFormats* GetAudioFormats() const;
		void SetOnExceptionHandler(HephAudio::Native::AudioExceptionEventHandler handler);
		void SetOnDefaultAudioDeviceChangeHandler(HephAudio::Native::AudioDeviceEventHandler handler);
		void SetOnAudioDeviceAddedHandler(HephAudio::Native::AudioDeviceEventHandler handler);
		void SetOnAudioDeviceRemovedHandler(HephAudio::Native::AudioDeviceEventHandler handler);
		void SetOnCaptureHandler(HephAudio::Native::AudioCaptureEventHandler handler);
	public:
#ifdef __ANDROID__
		Audio(JavaVM* jvm);
#else
		Audio();
#endif
		Audio(const Audio&) = delete;
		Audio& operator=(const Audio&) = delete;
		~Audio();
		HephAudio::Native::NativeAudio* GetNativeAudio() const;
		std::shared_ptr<AudioObject> Play(StringBuffer filePath);
		std::shared_ptr<AudioObject> Play(StringBuffer filePath, bool isPaused);
		std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount);
		std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount, bool isPaused);
		void Queue(StringBuffer queueName, double queueDelay, const std::vector<StringBuffer>& filePaths);
		std::shared_ptr<AudioObject> Load(StringBuffer filePath);
		std::shared_ptr<AudioObject> CreateAO(StringBuffer name, size_t bufferFrameCount);
		bool DestroyAO(std::shared_ptr<AudioObject> audioObject);
		bool AOExists(std::shared_ptr<AudioObject> audioObject) const;
		void SetAOPosition(std::shared_ptr<AudioObject> audioObject, double position);
		double GetAOPosition(std::shared_ptr<AudioObject> audioObject) const;
		void PauseCapture(bool pause);
		bool IsCapturePaused() const noexcept;
		void SetMasterVolume(double volume);
		double GetMasterVolume() const;
		void SetCategoryVolume(StringBuffer categoryName, double newVolume);
		double GetCategoryVolume(StringBuffer categoryName) const;
		void RegisterCategory(Category category);
		void UnregisterCategory(StringBuffer categoryName);
		bool CategoryExists(StringBuffer categoryName) const;
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
		bool SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat);
	};
}