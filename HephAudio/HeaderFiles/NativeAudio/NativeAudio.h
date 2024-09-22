#pragma once
#include "HephAudioShared.h"
#include "AudioDevice.h"
#include "AudioObject.h"
#include "AudioFormatInfo.h"
#include "IAudioDecoder.h"
#include "IAudioEncoder.h"
#include "Params/NativeAudioParams.h"
#include "Exception.h"
#include "Event.h"
#include "StringHelpers.h"
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief base class for the classes that interact with the native audio APIs.
		 * 
		 */
		class HEPH_API NativeAudio
		{
		protected:
			/**
			 * indicates the device enumeration has failed.
			 * 
			 */
			static constexpr bool DEVICE_ENUMERATION_FAIL = false;

			/**
			 * indicates the device enumeration has succeeded.
			 * 
			 */
			static constexpr bool DEVICE_ENUMERATION_SUCCESS = true;

		protected:
			/**
			 * shared pointer to the decoder instance that's used internally.
			 * 
			 */
			std::shared_ptr<IAudioDecoder> pAudioDecoder;

			/**
			 * shared pointer to the encoder instance that's used internally.
			 * 
			 */
			std::shared_ptr<IAudioEncoder> pAudioEncoder;

			/**
			 * a list of audio objects.
			 * 
			 */
			std::vector<AudioObject> audioObjects;

			/**
			 * a list of audio devices present in the system.
			 * 
			 */
			std::vector<AudioDevice> audioDevices;

			/**
			 * unique identifier of the thread that created the current isntance.
			 * 
			 */
			std::thread::id mainThreadId;

			/**
			 * the thread that's used for rendering audio data.
			 * 
			 */
			std::thread renderThread;

			/**
			 * the thread that's used for capturing audio data.
			 * 
			 */
			std::thread captureThread;

			/**
			 * the thread that's used for enumerating audio devices periodically.
			 * 
			 */
			std::thread deviceThread;

			/**
			 * unique identifier of the audio device that's currently being used for rendering.
			 * 
			 */
			std::string renderDeviceId;

			/**
			 * unique identifier of the audio device that's currently being used for capturing.
			 * 
			 */
			std::string captureDeviceId;

			/**
			 * render format.
			 * 
			 */
			AudioFormatInfo renderFormat;

			/**
			 * capture format.
			 * 
			 */
			AudioFormatInfo captureFormat;

			/**
			 * indicates whether the current instance is being destroyed.
			 * 
			 */
			bool disposing;

			/**
			 * indicates whether the current instance is ready for rendering.
			 * 
			 */
			bool isRenderInitialized;

			/**
			 * indicates whether the current instance is ready for capturing.
			 * 
			 */
			bool isCaptureInitialized;

			/**
			 * indicates whether the capture is paused.
			 * 
			 */
			bool isCapturePaused;

			/**
			 * time, in milliseconds, between each device enumeration.
			 * 
			 */
			uint32_t deviceEnumerationPeriod_ms;

			/**
			 * to prevent race condition when accessing/enumerating audio devices.
			 * 
			 */
			mutable std::mutex audioDevicesMutex;

			/**
			 * to prevent race condition when creating/accessing audio objects.
			 * 
			 */
			mutable std::recursive_mutex audioObjectsMutex;

		public:
			/**
			 * raised when an audio device is connected to the device or activated.
			 * 
			 */
			Heph::Event OnAudioDeviceAdded;

			/**
			 * raised when an audio device is disconnected from the device or deactivated.
			 * 
			 */
			Heph::Event OnAudioDeviceRemoved;

			/**
			 * raised when sufficient amount (typically 10 ms) audio data is captured.
			 * 
			 */
			Heph::Event OnCapture;

		public:
			/** @copydoc default_constructor */
			NativeAudio();

			NativeAudio(const NativeAudio&) = delete;
			NativeAudio& operator=(const NativeAudio&) = delete;

			/** @copydoc destructor */
			virtual ~NativeAudio() = default;

			/**
			 * gets the shared pointer to the audio decoder instance.
			 * 
			 */
			std::shared_ptr<IAudioDecoder> GetAudioDecoder() const;

			/**
			 * sets the decoder.
			 * 
			 * @param pNewDecoder shared pointer to the new decoder.
			 */
			void SetAudioDecoder(std::shared_ptr<IAudioDecoder> pNewDecoder);

			/**
			 * gets the shared pointer to the audio encoder instance.
			 * 
			 */
			std::shared_ptr<IAudioEncoder> GetAudioEncoder() const;

			/**
			 * sets the encoder.
			 * 
			 * @param pNewEncoder shared pointer to the new encoder.
			 */
			void SetAudioEncoder(std::shared_ptr<IAudioEncoder> pNewEncoder);

			/**
			 * reads the file, then starts playing it.
			 * 
			 * @param filePath path of the file which will be played.
			 * @return pointer to the audio object instance.
			 */
			AudioObject* Play(const std::filesystem::path& filePath);

			/**
			 * reads the file, then starts playing it.
			 * 
			 * @param filePath path of the file which will be played.
			 * @param playCount number of times the file will be played.
			 * @return pointer to the audio object instance.
			 */
			AudioObject* Play(const std::filesystem::path& filePath, uint32_t playCount);

			/**
			 * reads the file, then starts playing it.
			 * 
			 * @param filePath path of the file which will be played.
			 * @param playCount number of times the file will be played.
			 * @param isPaused indicates whether to start playing the audio data after reading it to memory.
			 * @return pointer to the audio object instance.
			 */
			AudioObject* Play(const std::filesystem::path& filePath, uint32_t playCount, bool isPaused);

			/**
			 * reads the file and converts the audio data to the render format but does not start playing it.
			 * 
			 * @param filePath path of the file which will be loaded.
			 * @return pointer to the audio object instance.
			 */
			AudioObject* Load(const std::filesystem::path& filePath);

			/**
			 * reads the file and converts the audio data to the render format but does not start playing it.
			 * 
			 * @param filePath path of the file which will be loaded.
			 * @param playCount number of times the file will be played.
			 * @return pointer to the audio object instance.
			 */			
			AudioObject* Load(const std::filesystem::path& filePath, uint32_t playCount);

			/**
			 * reads the file and converts the audio data to the render format but does not start playing it.
			 * 
			 * @param filePath path of the file which will be loaded.
			 * @param playCount number of times the file will be played.
			 * @param isPaused indicates whether to start playing the audio data after loading it.
			 * @return pointer to the audio object instance.
			 */
			AudioObject* Load(const std::filesystem::path& filePath, uint32_t playCount, bool isPaused);

			/**
			 * creates an audio object with the provided buffer info.
			 * 
			 * @param name name for the audio object.
			 * @param bufferFrameCount number of frames the buffer will have.
			 * @param channelLayout channel layout of the buffer.
			 * @param sampleRate sample rate of the buffer.
			 * @return pointer to the audio object instance.
			 */
			AudioObject* CreateAudioObject(const std::string& name, size_t bufferFrameCount, AudioChannelLayout channelLayout, uint16_t sampleRate);

			/**
			 * destroys the audio object.
			 * 
			 * @param pAudioObject pointer to the object which will be destroyed.
			 * @return true if the object is found and destroyed, otherwise false.
			 */
			bool DestroyAudioObject(AudioObject* pAudioObject);

			/**
			 * destroys the audio object.
			 * 
			 * @param audioObjectId unique identifier of the object which will be destroyed.
			 * @return true if the object is found and destroyed, otherwise false.
			 */
			bool DestroyAudioObject(const Heph::Guid& audioObjectId);

			/**
			 * checks whether an audio object exists.
			 * 
			 * @param pAudioObject pointer to the object which will checked.
			 * @return true if the object is found, otherwise false.
			 */
			bool AudioObjectExists(AudioObject* pAudioObject) const;

			/**
			 * checks whether an audio object exists.
			 * 
			 * @param audioObjectId unique identifier of the object which will be checked.
			 * @return true if the object is found, otherwise false.
			 */
			bool AudioObjectExists(const Heph::Guid& audioObjectId) const;

			/**
			 * gets the audio object.
			 * 
			 * @param index index of the audio object.
			 * @return pointer to the audio object if found, otherwise nullptr.
			 */
			AudioObject* GetAudioObject(size_t index);

			/**
			 * gets the audio object.
			 * 
			 * @param audioObjectId unique identifier of the audio object.
			 * @return pointer to the audio object if found, otherwise nullptr.
			 */
			AudioObject* GetAudioObject(const Heph::Guid& audioObjectId);

			/**
			 * gets the first audio object with the provided name.
			 * 
			 * @param audioObjectName name of the audio object.
			 * @return pointer to the audio object if found, otherwise nullptr.
			 */
			AudioObject* GetAudioObject(const std::string& audioObjectName);

			/**
			 * gets the number of audio objects currently present.
			 * 
			 */
			size_t GetAudioObjectCount() const;

			/**
			 * resumes capturing.
			 * 
			 */
			void ResumeCapture();

			/**
			 * pauses capturing.
			 * 
			 */
			void PauseCapture();

			/**
			 * checks whether the capturing is paused.
			 * 
			 * @return true if paused capturing, otherwise false.
			 */
			bool IsCapturePaused() const;

			/**
			 * gets the \link NativeAudio::deviceEnumerationPeriod_ms device enumeration period.
			 * 
			 */
			uint32_t GetDeviceEnumerationPeriod() const;

			/**
			 * sets the \link NativeAudio::deviceEnumerationPeriod_ms device enumeration period.
			 * 
			 */
			void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms);

			/**
			 * sets the master volume. 
			 * 
			 * @param volume new master volume.
			 * @important values above 1 may cause distortion.
			 */
			virtual void SetMasterVolume(double volume) = 0;

			/**
			 * gets the master volume. 
			 * 
			 */
			virtual double GetMasterVolume() const = 0;

			/**
			 * gets the audio format used for rendering.
			 * 
			 */
			const AudioFormatInfo& GetRenderFormat() const;

			/**
			 * gets the audio format used for capturing.
			 * 
			 */
			const AudioFormatInfo& GetCaptureFormat() const;

			/**
			 * initializes rendering with the default device and default format.
			 * 
			 */
			void InitializeRender();

			/**
			 * initializes rendering with the default device and the provided format.
			 * 
			 * @param channelLayout channel layout of the render format.
			 * @param sampleRate sample rate of the render format.
			 */
			void InitializeRender(AudioChannelLayout channelLayout, uint32_t sampleRate);

			/**
			 * initializes rendering with the default device and the provided format.
			 * 
			 * @param format render format.
			 */
			void InitializeRender(AudioFormatInfo format);

			/**
			 * initializes rendering with the provided device and the provided format.
			 * 
			 * @param device pointer to the audio render device.
			 * @param format render format.
			 */
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) = 0;

			/**
			 * stops rendering and releases the resources allocated for it.
			 * 
			 */
			virtual void StopRendering() = 0;

			/**
			 * initializes capturing with the default device and default format.
			 * 
			 */
			void InitializeCapture();

			/**
			 * initializes capturing with the default device and the provided format.
			 * 
			 * @param channelLayout channel layout of the capture format.
			 * @param sampleRate sample rate of the capture format.
			 */
			void InitializeCapture(AudioChannelLayout channelLayout, uint32_t sampleRate);

			/**
			 * initializes capturing with the default device and the provided format.
			 * 
			 * @param format capture format.
			 */
			void InitializeCapture(AudioFormatInfo format);

			/**
			 * initializes capturing with the provided device and the provided format.
			 * 
			 * @param device pointer to the audio capture device.
			 * @param format capture format.
			 */
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) = 0;

			/**
			 * stops capturing and releases the resources allocated for it.
			 * 
			 */
			virtual void StopCapturing() = 0;

			/**
			 * gets the native API specific parameters.
			 * 
			 */
			virtual void GetNativeParams(NativeAudioParams& nativeParams) const = 0;

			/**
			 * sets the native API specific parameters.
			 * 
			 */
			virtual void SetNativeParams(const NativeAudioParams& nativeParams) = 0;

			/**
			 * gets audio device by id.
			 * 
			 */
			AudioDevice GetAudioDeviceById(const std::string& deviceId) const;

			/**
			 * gets the audio device that's currently being used for rendering.
			 * 
			 */
			AudioDevice GetRenderDevice() const;

			/**
			 * gets the audio device that's currently being used for capturing.
			 * 
			 */
			AudioDevice GetCaptureDevice() const;

			/**
			 * gets the system default render/capture device.
			 * 
			 * @param deviceType type of the device. 
			 * Must be either \link AudioDeviceType::Render AudioDeviceType::Render \endlink or \link AudioDeviceType::Capture AudioDeviceType::Capture \endlink.
			 * @return AudioDevice 
			 */
			AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;

			/**
			 * gets the available audio devices of the requested type.
			 * 
			 * @param deviceType type of the audio devices.
			 */
			std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;

		protected:

			/**
			 * enumerates the audio devices.
			 * 
			 * @return #DEVICE_ENUMERATION_SUCCESS if succeded, otherwise #DEVICE_ENUMERATION_FAIL.
			 */
			virtual bool EnumerateAudioDevices() = 0;

			/**
			 * enumerates audio devices periodically.
			 * 
			 */
			virtual void CheckAudioDevices();

			/**
			 * waits for render thread to join.
			 * 
			 */
			void JoinRenderThread();

			/**
			 * waits for capture thread to join.
			 * 
			 */
			void JoinCaptureThread();

			/**
			 * waits for device thread to join.
			 * 
			 */
			void JoinDeviceThread();

			/**
			 * mixes the audio objects that are currently playing into one buffer.
			 * 
			 * @param frameCount number of frames the output buffer will have.
			 */
			EncodedAudioBuffer Mix(uint32_t frameCount);

			/**
			 * gets the number of audio objects that will are currently playing.
			 * 
			 */
			size_t GetAOCountToMix() const;

			/**
			 * calculates the volume of the audio object.
			 * 
			 */
			virtual double GetFinalAOVolume(AudioObject* pAudioObject) const;
		};
	}
}