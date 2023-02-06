#pragma once
#include "framework.h"
#include "AudioDevice.h"
#include "AudioException.h"
#include "Category.h"
#include "AudioObject.h"
#include "AudioFormats.h"
#include "AudioFormatInfo.h"
#include "EchoInfo.h"
#include "Audio.h"
#include <vector>
#include <thread>

namespace HephAudio
{
	namespace Native
	{
		enum class AudioExceptionThread : uint8_t
		{
			MainThread = 0x00,
			RenderThread = 0x01,
			CaptureThread = 0x02,
			QueueThread = 0x04,
			Other = 0xFF
		};
		typedef void (*AudioExceptionEventHandler)(AudioException exception, AudioExceptionThread exceptionThread);
		typedef void (*AudioDeviceEventHandler)(AudioDevice device);
		typedef void (*AudioCaptureEventHandler)(AudioBuffer& capturedDataBuffer);
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
		public:
			Formats::AudioFormats audioFormats;
			AudioExceptionEventHandler OnException;
			AudioDeviceEventHandler OnDefaultAudioDeviceChange;
			AudioDeviceEventHandler OnAudioDeviceAdded;
			AudioDeviceEventHandler OnAudioDeviceRemoved;
			AudioCaptureEventHandler OnCapture;
		public:
			NativeAudio();
			NativeAudio(const NativeAudio&) = delete;
			NativeAudio& operator=(const NativeAudio&) = delete;
			virtual ~NativeAudio() = default;
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath);
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, bool isPaused);
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount);
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount, bool isPaused);
			virtual std::vector<std::shared_ptr<AudioObject>> Queue(StringBuffer queueName, uint32_t queueDelay, std::vector<StringBuffer> filePaths);
			virtual std::shared_ptr<AudioObject> Load(StringBuffer filePath);
			virtual std::shared_ptr<AudioObject> CreateAO(StringBuffer name, size_t bufferFrameCount);
			virtual bool DestroyAO(std::shared_ptr<AudioObject> audioObject);
			virtual bool AOExists(std::shared_ptr<AudioObject> audioObject) const;
			virtual void SetAOPosition(std::shared_ptr<AudioObject> audioObject, double position);
			virtual double GetAOPosition(std::shared_ptr<AudioObject> audioObject) const;
			virtual void PauseCapture(bool pause);
			virtual bool IsCapturePaused() const noexcept;
			virtual void SetMasterVolume(double volume) = 0;
			virtual double GetMasterVolume() const = 0;
			virtual void SetCategoryVolume(StringBuffer categoryName, double newVolume);
			virtual double GetCategoryVolume(StringBuffer categoryName) const;
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
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const = 0;
			virtual bool SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat);
		protected:
			virtual void JoinRenderThread();
			virtual void JoinCaptureThread();
			virtual void JoinQueueThreads();
			virtual AudioExceptionThread GetCurrentThread() const;
			virtual std::vector<std::shared_ptr<AudioObject>> GetQueue(StringBuffer queueName) const;
			virtual void PlayNextInQueue(StringBuffer queueName, uint32_t queueDelay, uint32_t decreaseQueueIndex);
			// Mixes audio objects that are currently playing into one buffer.
			virtual void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			virtual size_t GetAOCountToMix() const;
			virtual double GetFinalAOVolume(std::shared_ptr<AudioObject> audioObject) const;
		};
#define	RAISE_AUDIO_EXCPT(pNativeAudio, audioException) if(pNativeAudio != nullptr && pNativeAudio->OnException != nullptr) { pNativeAudio->OnException(audioException, pNativeAudio->GetCurrentThread()); }
		StringBuffer AudioExceptionThreadName(const AudioExceptionThread& t);
	}
}