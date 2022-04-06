#pragma once
#include "AudioDevice.h"
#include "AudioException.h"
#include "Category.h"
#include "IAudioObject.h"
#include "AudioFormats.h"
#include "AudioFormatInfo.h"
#include "EchoInfo.h"
#include <vector>
#include <thread>

using namespace HephAudio::Structs;

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
		typedef void (*AudioRenderEventHandler)(AudioBuffer& renderBuffer, std::wstring audioObjectName, bool isProcessed);
		typedef void (*AudioCaptureEventHandler)(AudioBuffer& capturedDataBuffer);
		class INativeAudio
		{
		protected:
			std::vector<std::shared_ptr<IAudioObject>> audioObjects;
			Categories categories;
			std::thread::id mainThreadId;
			std::thread renderThread;
			std::thread captureThread;
			std::vector<std::thread> queueThreads;
			AudioFormatInfo renderFormat;
			AudioFormatInfo captureFormat;
			bool disposing;
			bool isRenderInitialized;
			bool isCaptureInitialized;
			bool isCapturePaused;
			std::wstring displayName;
			std::wstring iconPath;
		public:
			Formats::AudioFormats audioFormats;
			AudioExceptionEventHandler OnException;
			AudioDeviceEventHandler OnDefaultAudioDeviceChange;
			AudioDeviceEventHandler OnAudioDeviceAdded;
			AudioDeviceEventHandler OnAudioDeviceRemoved;
			AudioRenderEventHandler OnRender;
			AudioCaptureEventHandler OnCapture;
		public:
			INativeAudio();
			INativeAudio(const INativeAudio&) = delete;
			INativeAudio& operator=(const INativeAudio&) = delete;
			virtual ~INativeAudio() = default;
			virtual std::shared_ptr<IAudioObject> Play(std::wstring filePath);
			virtual std::shared_ptr<IAudioObject> Play(std::wstring filePath, uint32_t loopCount);
			virtual std::vector<std::shared_ptr<IAudioObject>> Queue(std::wstring queueName, uint32_t queueDelay, std::vector<std::wstring> filePaths);
			virtual std::shared_ptr<IAudioObject> Load(std::wstring filePath);
			virtual std::shared_ptr<IAudioObject> CreateAO(std::wstring name, size_t bufferFrameCount);
			virtual bool DestroyAO(std::shared_ptr<IAudioObject> audioObject);
			virtual bool AOExists(std::shared_ptr<IAudioObject> audioObject) const;
			virtual void SetAOPosition(std::shared_ptr<IAudioObject> audioObject, double position);
			virtual double GetAOPosition(std::shared_ptr<IAudioObject> audioObject) const;
			virtual void PauseCapture(bool pause);
			virtual bool IsCapturePaused() const noexcept;
			virtual void SetMasterVolume(double volume) const = 0;
			virtual double GetMasterVolume() const = 0;
			virtual void SetCategoryVolume(std::wstring categoryName, double newVolume);
			virtual double GetCategoryVolume(std::wstring categoryName) const;
			virtual void RegisterCategory(Category category);
			virtual void UnregisterCategory(std::wstring categoryName);
			virtual bool CategoryExists(std::wstring categoryName) const;
			virtual void Skip(std::wstring queueName, bool applyDelay);
			virtual void Skip(size_t skipCount, std::wstring queueName, bool applyDelay);
			virtual AudioFormatInfo GetRenderFormat() const;
			virtual AudioFormatInfo GetCaptureFormat() const;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopRendering() = 0;
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) = 0;
			virtual void StopCapturing() = 0;
			virtual void SetDisplayName(std::wstring displayName) = 0;
			virtual void SetIconPath(std::wstring iconPath) = 0;
			virtual AudioDevice GetAudioDeviceById(std::wstring deviceId) const;
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const = 0;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const = 0;
			virtual bool SaveToFile(std::wstring filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat);
		protected:
			virtual void JoinRenderThread();
			virtual void JoinCaptureThread();
			virtual void JoinQueueThreads();
			virtual AudioExceptionThread GetCurrentThread() const;
			virtual std::vector<std::shared_ptr<IAudioObject>> GetQueue(std::wstring queueName) const;
			virtual void PlayNextInQueue(std::wstring queueName, uint32_t queueDelay, uint32_t decreaseQueueIndex);
			// Mixes audio objects that are currently playing into one buffer.
			virtual void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			virtual size_t GetAOCountToMix() const;
			virtual double GetFinalAOVolume(std::shared_ptr<IAudioObject> audioObject) const;
		};
#define	RAISE_AUDIO_EXCPT(pNativeAudio, audioException) if(pNativeAudio != nullptr && pNativeAudio->OnException != nullptr) { pNativeAudio->OnException(audioException, pNativeAudio->GetCurrentThread()); }
	}
}