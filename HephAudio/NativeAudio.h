#pragma once
#include "framework.h"
#include "AudioDevice.h"
#include "AudioException.h"
#include "Category.h"
#include "AudioObject.h"
#include "AudioFormats.h"
#include "AudioFormatInfo.h"
#include "EchoInfo.h"
#include "AudioEvent.h"
#include "AudioExceptionEventArgs.h"
#include "AudioDeviceEventArgs.h"
#include "AudioCaptureEventArgs.h"
#include <vector>
#include <thread>

namespace HephAudio
{
	namespace Native
	{
		class NativeAudio
		{
		protected:
			std::vector<std::shared_ptr<AudioObject>> audioObjects;
			Categories categories;
			std::thread::id mainThreadId;
			std::thread renderThread;
			std::thread captureThread;
			std::vector<std::thread> queueThreads;
			StringBuffer renderDeviceId;
			StringBuffer captureDeviceId;
			AudioFormatInfo renderFormat;
			AudioFormatInfo captureFormat;
			bool disposing;
			bool isRenderInitialized;
			bool isCaptureInitialized;
			bool isCapturePaused;
			StringBuffer displayName;
			StringBuffer iconPath;
			mutable AudioExceptionEventArgs audioExceptionEventArgs;
		public:
			Formats::AudioFormats audioFormats;
			AudioEvent OnException;
			AudioEvent OnAudioDeviceAdded;
			AudioEvent OnAudioDeviceRemoved;
			AudioEvent OnCapture;
		public:
			NativeAudio();
			NativeAudio(const NativeAudio&) = delete;
			NativeAudio& operator=(const NativeAudio&) = delete;
			virtual ~NativeAudio() = default;
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath);
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, bool isPaused);
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount);
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount, bool isPaused);
			virtual std::vector<std::shared_ptr<AudioObject>> Queue(StringBuffer queueName, HEPHAUDIO_DOUBLE queueDelay, const std::vector<StringBuffer>& filePaths);
			virtual std::shared_ptr<AudioObject> Load(StringBuffer filePath);
			virtual std::shared_ptr<AudioObject> CreateAO(StringBuffer name, size_t bufferFrameCount);
			virtual bool DestroyAO(std::shared_ptr<AudioObject> audioObject);
			virtual bool AOExists(std::shared_ptr<AudioObject> audioObject) const;
			virtual void SetAOPosition(std::shared_ptr<AudioObject> audioObject, HEPHAUDIO_DOUBLE position);
			virtual HEPHAUDIO_DOUBLE GetAOPosition(std::shared_ptr<AudioObject> audioObject) const;
			virtual std::shared_ptr<AudioObject> GetAO(StringBuffer aoName) const;
			virtual std::shared_ptr<AudioObject> GetAO(StringBuffer queueName, size_t index) const;
			virtual void PauseCapture(bool pause);
			virtual bool IsCapturePaused() const noexcept;
			virtual void SetMasterVolume(HEPHAUDIO_DOUBLE volume) = 0;
			virtual HEPHAUDIO_DOUBLE GetMasterVolume() const = 0;
			virtual void SetCategoryVolume(StringBuffer categoryName, HEPHAUDIO_DOUBLE newVolume);
			virtual HEPHAUDIO_DOUBLE GetCategoryVolume(StringBuffer categoryName) const;
			virtual void RegisterCategory(Category category);
			virtual void UnregisterCategory(StringBuffer categoryName);
			virtual bool CategoryExists(StringBuffer categoryName) const;
			virtual void Skip(StringBuffer queueName, bool applyDelay);
			virtual void Skip(size_t skipCount, StringBuffer queueName, bool applyDelay);
			virtual AudioFormatInfo GetRenderFormat() const;
			virtual AudioFormatInfo GetCaptureFormat() const;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopRendering() = 0;
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopCapturing() = 0;
			virtual void SetDisplayName(StringBuffer displayName) = 0;
			virtual void SetIconPath(StringBuffer iconPath) = 0;
			virtual AudioDevice GetAudioDeviceById(StringBuffer deviceId) const;
			virtual AudioDevice GetRenderDevice() const;
			virtual AudioDevice GetCaptureDevice() const;
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const = 0;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const = 0;
			virtual bool SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer);
		protected:
			virtual void JoinRenderThread();
			virtual void JoinCaptureThread();
			virtual void JoinQueueThreads();
			virtual AudioExceptionThread GetCurrentThread() const;
			virtual std::vector<std::shared_ptr<AudioObject>> GetQueue(StringBuffer queueName) const;
			virtual void PlayNextInQueue(StringBuffer queueName, HEPHAUDIO_DOUBLE queueDelay, uint32_t decreaseQueueIndex);
			// Mixes audio objects that are currently playing into one buffer.
			virtual void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			virtual size_t GetAOCountToMix() const;
			virtual HEPHAUDIO_DOUBLE GetFinalAOVolume(std::shared_ptr<AudioObject> audioObject) const;
		};
#define	RAISE_AUDIO_EXCPT(pNativeAudio, audioException) pNativeAudio->audioExceptionEventArgs = AudioExceptionEventArgs(pNativeAudio, audioException, pNativeAudio->GetCurrentThread()); pNativeAudio->OnException(&pNativeAudio->audioExceptionEventArgs, nullptr)
		StringBuffer AudioExceptionThreadName(const AudioExceptionThread& t);
	}
}