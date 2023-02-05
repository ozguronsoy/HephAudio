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

	using namespace HephAudio::Native;

	class Audio final
	{
	private:
		NativeAudio* pNativeAudio;
	public:
		Formats::AudioFormats* GetAudioFormats() const;
		void SetOnExceptionHandler(AudioExceptionEventHandler handler);
		void SetOnDefaultAudioDeviceChangeHandler(AudioDeviceEventHandler handler);
		void SetOnAudioDeviceAddedHandler(AudioDeviceEventHandler handler);
		void SetOnAudioDeviceRemovedHandler(AudioDeviceEventHandler handler);
		void SetOnCaptureHandler(AudioCaptureEventHandler handler);
	public:
#ifdef __ANDROID__
		Audio(JavaVM* jvm);
#else
		Audio();
#endif
		Audio(const Audio&) = delete;
		Audio& operator=(const Audio&) = delete;
		~Audio();
		NativeAudio* GetNativeAudio() const;
		std::shared_ptr<AudioObject> Play(std::wstring filePath);
		std::shared_ptr<AudioObject> Play(std::wstring filePath, bool isPaused);
		std::shared_ptr<AudioObject> Play(std::wstring filePath, uint32_t loopCount);
		std::shared_ptr<AudioObject> Play(std::wstring filePath, uint32_t loopCount, bool isPaused);
		std::vector<std::shared_ptr<AudioObject>> Queue(std::wstring queueName, uint32_t queueDelay, std::vector<std::wstring> filePaths);
		std::shared_ptr<AudioObject> Load(std::wstring filePath);
		std::shared_ptr<AudioObject> CreateAO(std::wstring name, size_t bufferFrameCount);
		bool DestroyAO(std::shared_ptr<AudioObject> audioObject);
		bool AOExists(std::shared_ptr<AudioObject> audioObject) const;
		void SetAOPosition(std::shared_ptr<AudioObject> audioObject, double position);
		double GetAOPosition(std::shared_ptr<AudioObject> audioObject) const;
		void PauseCapture(bool pause);
		bool IsCapturePaused() const noexcept;
		void SetMasterVolume(double volume);
		double GetMasterVolume() const;
		void SetCategoryVolume(std::wstring categoryName, double newVolume);
		double GetCategoryVolume(std::wstring categoryName) const;
		void RegisterCategory(Category category);
		void UnregisterCategory(std::wstring categoryName);
		bool CategoryExists(std::wstring categoryName) const;
		void Skip(std::wstring queueName, bool applyDelay);
		void Skip(size_t skipCount, std::wstring queueName, bool applyDelay);
		AudioFormatInfo GetRenderFormat() const;
		AudioFormatInfo GetCaptureFormat() const;
		void InitializeRender(AudioDevice* device, AudioFormatInfo format);
		void StopRendering();
		void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
		void StopCapturing();
#if (defined(_WIN32) && defined(_WIN32_WINNT_VISTA))
		void SetDisplayName(std::wstring displayName);
		void SetIconPath(std::wstring iconPath);
#endif
#if defined(_WIN32)
		AudioDevice GetAudioDeviceById(std::wstring deviceId) const;
		AudioDevice GetRenderDevice() const;
		AudioDevice GetCaptureDevice() const;
		AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
		std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const;
#endif
		bool SaveToFile(std::wstring filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat);
	};
}