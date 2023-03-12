#pragma once
#include "framework.h"
#include "AudioDevice.h"
#include "AudioException.h"
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
		/// <summary>
		/// Base class for native audio wrapper classes.
		/// </summary>
		class NativeAudio
		{
		protected:
			/// <summary>
			/// The internal AudioObject list.
			/// </summary>
			std::vector<std::shared_ptr<AudioObject>> audioObjects;
			/// <summary>
			/// The unique identifier of the main thread.
			/// </summary>
			std::thread::id mainThreadId;
			/// <summary>
			/// The thread where playing audio signals occurs.
			/// </summary>
			std::thread renderThread;
			/// <summary>
			/// The thread where recording audio signals occurs.
			/// </summary>
			std::thread captureThread;
			/// <summary>
			/// The threads that are responsible for waiting for the "AudioObject::queueDelay_ms" amount of time before playing the next audio object in the queue without blocking any other threads.
			/// </summary>
			std::vector<std::thread> queueThreads;
			/// <summary>
			/// The unique identifier of the audio output device.
			/// </summary>
			StringBuffer renderDeviceId;
			/// <summary>
			/// The unique identifier of the audio input device.
			/// </summary>
			StringBuffer captureDeviceId;
			/// <summary>
			/// The audio rendering format information.
			/// </summary>
			AudioFormatInfo renderFormat;
			/// <summary>
			/// The audio capturing format information.
			/// </summary>
			AudioFormatInfo captureFormat;
			/// <summary>
			/// true when the native audio is being destructed.
			/// </summary>
			bool disposing;
			/// <summary>
			/// true if the render is initialized.
			/// </summary>
			bool isRenderInitialized;
			/// <summary>
			/// true if the capture is initialized.
			/// </summary>
			bool isCaptureInitialized;
			/// <summary>
			/// true if capturing is paused.
			/// </summary>
			bool isCapturePaused;
			/// <summary>
			/// The name that's displayed in the windows volume mixer.
			/// </summary>
			StringBuffer displayName;
			/// <summary>
			/// The path of the icon that's displayed in the windows volume mixer.
			/// </summary>
			StringBuffer iconPath;
			/// <summary>
			/// Arguments for OnException.
			/// </summary>
			mutable AudioExceptionEventArgs audioExceptionEventArgs;
		public:
			Formats::AudioFormats audioFormats;
			/// <summary>
			/// Called when an audio exception occurs.
			/// </summary>
			AudioEvent OnException;
			/// <summary>
			/// Called when an audio device is connected.
			/// </summary>
			AudioEvent OnAudioDeviceAdded;
			/// <summary>
			/// Called when an audio device loses connection.
			/// </summary>
			AudioEvent OnAudioDeviceRemoved;
			/// <summary>
			/// Called while capturing audio data.
			/// </summary>
			AudioEvent OnCapture;
		public:
			/// <summary>
			/// Creates and initalizes a NativeAudio instance.
			/// </summary>
			NativeAudio();
			NativeAudio(const NativeAudio&) = delete;
			NativeAudio& operator=(const NativeAudio&) = delete;
			/// <summary>
			/// Frees the resources and waits for the threads to join.
			/// </summary>
			virtual ~NativeAudio() = default;
			/// <summary>
			/// Starts playing the audio file at the provided path.
			/// </summary>
			/// <param name="filePath">The file path.</param>
			/// <returns>The shared AudioObject pointer.</returns>
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath);
			/// <summary>
			/// Loads the audio file at the provided path and starts playing it if "isPaused" is false.
			/// </summary>
			/// <param name="filePath">The file path.</param>
			/// <param name="isPaused">Determines whether to start playing the audio file.</param>
			/// <returns>The shared AudioObject pointer.</returns>
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, bool isPaused);
			/// <summary>
			/// Loops the audio file at the provided path.
			/// </summary>
			/// <param name="filePath">The file path</param>
			/// <param name="loopCount">The number of times the file will be played.</param>
			/// <returns>The shared AudioObject pointer.</returns>
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount);
			/// <summary>
			/// Loops the audio file at the provided path and starts playing it if "isPaused" is false.
			/// </summary>
			/// <param name="filePath">The file path.</param>
			/// <param name="loopCount">The number of times the file will be played.</param>
			/// <param name="isPaused">Determines whether to start playing the audio file.</param>
			/// <returns>The shared AudioObject pointer.</returns>
			virtual std::shared_ptr<AudioObject> Play(StringBuffer filePath, uint32_t loopCount, bool isPaused);
			/// <summary>
			/// Adds the files to the end of the queue.
			/// </summary>
			/// <param name="queueName">The unique name of the queue.</param>
			/// <param name="queueDelay_ms">The time, in milliseconds, between the end of the current item in the queue and the start of the next object in the queue.</param>
			/// <param name="filePaths">The paths of the files that will be added to the queue.</param>
			/// <returns>The shared pointers of AudioObjects that are successfully added to the queue.</returns>
			virtual std::vector<std::shared_ptr<AudioObject>> Queue(StringBuffer queueName, hephaudio_float queueDelay_ms, const std::vector<StringBuffer>& filePaths);
			/// <summary>
			/// Loads and converts the audio file to the render format.
			/// </summary>
			/// <param name="filePath">The file path.</param>
			/// <returns>The shared AudioObject pointer.</returns>
			virtual std::shared_ptr<AudioObject> Load(StringBuffer filePath);
			/// <summary>
			/// Creates an AudioObject.
			/// </summary>
			/// <param name="name">The name of the AudioObject.</param>
			/// <param name="bufferFrameCount">The number of frames the object's buffer will consist of.</param>
			/// <returns>The shared AudioObject pointer.</returns>
			virtual std::shared_ptr<AudioObject> CreateAO(StringBuffer name, size_t bufferFrameCount);
			/// <summary>
			/// Removes the object from the internal AudioObject list.
			/// </summary>
			/// <param name="pAudioObject">The object to be removed.</param>
			/// <returns>true if the object is found and removed.</returns>
			virtual bool DestroyAO(std::shared_ptr<AudioObject> pAudioObject);
			/// <summary>
			/// Checks whether the object exists in the internal AudioObject list.
			/// </summary>
			/// <param name="pAudioObject">The object to look for.</param>
			/// <returns>true if the object exists in the internal list.</returns>
			virtual bool AOExists(std::shared_ptr<AudioObject> pAudioObject) const;
			/// <summary>
			/// Changes the position of the object.
			/// </summary>
			/// <param name="pAudioObject">The object whose position will be changed.</param>
			/// <param name="position">The new position of the object. Must be between 0 and 1.</param>
			virtual void SetAOPosition(std::shared_ptr<AudioObject> pAudioObject, hephaudio_float position);
			/// <summary>
			/// Gets the position of the object between 0 and 1.
			/// </summary>
			/// <param name="pAudioObject">The audio object.</param>
			/// <returns>The position of the object between 0 and 1.</returns>
			virtual hephaudio_float GetAOPosition(std::shared_ptr<AudioObject> pAudioObject) const;
			/// <summary>
			/// Gets the first audio object with the provided name.
			/// </summary>
			/// <param name="aoName">The name to search for.</param>
			/// <returns>if found, the object; otherwise, nullptr.</returns>
			virtual std::shared_ptr<AudioObject> GetAO(StringBuffer aoName) const;
			/// <summary>
			/// Gets the object at the provided index in a queue.
			/// </summary>
			/// <param name="queueName">The name of the queue.</param>
			/// <param name="index">The index of the object.</param>
			/// <returns>the shared AudioObject pointer.</returns>
			virtual std::shared_ptr<AudioObject> GetAO(StringBuffer queueName, size_t index) const;
			/// <summary>
			/// Pauses or unpauses capturing.
			/// </summary>
			/// <param name="pause">Determines whether to pause capturing.</param>
			virtual void PauseCapture(bool pause);
			/// <summary>
			/// Checks whether capturing is paused.
			/// </summary>
			/// <returns>true if capturing is paused.</returns>
			virtual bool IsCapturePaused() const noexcept;
			/// <summary>
			/// Sets the master volume.
			/// </summary>
			/// <param name="volume">The new master volume.</param>
			virtual void SetMasterVolume(hephaudio_float volume) = 0;
			/// <summary>
			/// Gets the master volume.
			/// </summary>
			/// <returns>The master volume.</returns>
			virtual hephaudio_float GetMasterVolume() const = 0;
			/// <summary>
			/// Skips the current playing object of the queue.
			/// </summary>
			/// <param name="queueName">The name of the queue.</param>
			/// <param name="applyDelay">Determines whether to apply delay before starting the next object in the queue.</param>
			virtual void Skip(StringBuffer queueName, bool applyDelay);
			/// <summary>
			/// Skips "skipCount" amount of objects in the queue.
			/// </summary>
			/// <param name="skipCount">The number of objects to skip.</param>
			/// <param name="queueName">The name of the queue.</param>
			/// <param name="applyDelay">Determines whether to apply delay before starting the next object in the queue.</param>
			virtual void Skip(size_t skipCount, StringBuffer queueName, bool applyDelay);
			/// <summary>
			/// Gets the rendering format information.
			/// </summary>
			/// <returns>The render format information.</returns>
			virtual AudioFormatInfo GetRenderFormat() const;
			/// <summary>
			/// Gets the capturing format information.
			/// </summary>
			/// <returns>The capture format information.</returns>
			virtual AudioFormatInfo GetCaptureFormat() const;
			/// <summary>
			/// Initializes the native API for rendering with the provided audio device.
			/// </summary>
			/// <param name="device">The pointer to the desired rendering device. Set this to nullptr for initializing with the default device.</param>
			/// <param name="format">The render format.</param>
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) = 0;
			/// <summary>
			/// Tells the native API to stop rendering.
			/// </summary>
			virtual void StopRendering() = 0;
			/// <summary>
			/// Initializes the native API for capturing with the provided audio device.
			/// </summary>
			/// <param name="device">The pointer to the desired capturing device. Set this to nullptr for initializing with the default device.</param>
			/// <param name="format">The capture format.</param>
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) = 0;
			/// <summary>
			/// Tells the native API to stop capturing.
			/// </summary>
			virtual void StopCapturing() = 0;
			/// <summary>
			/// Sets the display name for the windows volume mixer.
			/// </summary>
			/// <param name="displayName">The new name.</param>
			virtual void SetDisplayName(StringBuffer displayName) = 0;
			/// <summary>
			/// Sets the path of the icon that will be displayed at the windows volume mixer.
			/// </summary>
			/// <param name="iconPath">The icon path.</param>
			virtual void SetIconPath(StringBuffer iconPath) = 0;
			/// <summary>
			/// Finds the audio device by its unique identifier.
			/// </summary>
			/// <param name="deviceId">The unique identifier of the desired audio device.</param>
			/// <returns>If found, the audio device; otherwise, the default AudioDevice instance.</returns>
			virtual AudioDevice GetAudioDeviceById(StringBuffer deviceId) const;
			/// <summary>
			/// Gets the rendering device.
			/// </summary>
			/// <returns>The render device.</returns>
			virtual AudioDevice GetRenderDevice() const;
			/// <summary>
			/// Gets the capturing device.
			/// </summary>
			/// <returns>The capture device.</returns>
			virtual AudioDevice GetCaptureDevice() const;
			/// <summary>
			/// Gets the default audio device of the provided type.
			/// </summary>
			/// <param name="deviceType">The type of the device.</param>
			/// <returns>The default audio device.</returns>
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const = 0;
			/// <summary>
			/// Gets all available audio devices of the provided type.
			/// </summary>
			/// <param name="deviceType">The device type.</param>
			/// <returns>The available audio devices.</returns>
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const = 0;
			/// <summary>
			/// Saves the audio data to the provided path.
			/// </summary>
			/// <param name="filePath">The file path.</param>
			/// <param name="overwrite">Determines whether to overwrite when a file already exists at the provided path.</param>
			/// <param name="buffer">The buffer that contains the audio data to save.</param>
			/// <returns>true if successfully saved.</returns>
			virtual bool SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer);
		protected:
			/// <summary>
			/// Waits for the render thread to join.
			/// </summary>
			virtual void JoinRenderThread();
			/// <summary>
			/// Waits for the capture thread to join.
			/// </summary>
			virtual void JoinCaptureThread();
			/// <summary>
			/// Waits for the queue threads to join.
			/// </summary>
			virtual void JoinQueueThreads();
			/// <summary>
			/// Finds the thread that this method is called from. Used when an exception occurs.
			/// </summary>
			virtual AudioExceptionThread GetCurrentThread() const;
			/// <summary>
			/// Gets all the objects in the queue.
			/// </summary>
			/// <param name="queueName">The unique name of the queue.</param>
			/// <returns>The objects in the queue.</returns>
			virtual std::vector<std::shared_ptr<AudioObject>> GetQueue(StringBuffer queueName) const;
			/// <summary>
			/// Starts playing the next object in the queue.
			/// </summary>
			/// <param name="queueName">The unique name of the queue.</param>
			/// <param name="queueDelay">The delay before starting the next object.</param>
			/// <param name="decreaseQueueIndex">The number of objects skipped.</param>
			virtual void PlayNextInQueue(StringBuffer queueName, hephaudio_float queueDelay, uint32_t decreaseQueueIndex);
			/// <summary>
			/// Mixes all of the playing objects into the output buffer.
			/// </summary>
			/// <param name="outputBuffer">The output buffer.</param>
			/// <param name="frameCount">The number of frames the output buffer consists of.</param>
			virtual void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
			/// <summary>
			/// Gets the number of audio objects that will be mixed.
			/// </summary>
			/// <returns>The number of audio objects that will be mixed.</returns>
			virtual size_t GetAOCountToMix() const;
			/// <summary>
			/// Calculates the volume of the audio object.
			/// </summary>
			/// <param name="pAudioObject">The shared AudioObject pointer.</param>
			/// <returns>The volume of the audio object.</returns>
			virtual hephaudio_float GetFinalAOVolume(std::shared_ptr<AudioObject> pAudioObject) const;
		};
#define	RAISE_AUDIO_EXCPT(pNativeAudio, audioException) pNativeAudio->audioExceptionEventArgs = AudioExceptionEventArgs(pNativeAudio, audioException, pNativeAudio->GetCurrentThread()); pNativeAudio->OnException(&pNativeAudio->audioExceptionEventArgs, nullptr)
		StringBuffer AudioExceptionThreadName(const AudioExceptionThread& t);
	}
}