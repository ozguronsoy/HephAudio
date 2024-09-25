#pragma once
#include "HephAudioShared.h"
#include "NativeAudio/NativeAudio.h"
#include "AudioEvents/AudioDeviceEventArgs.h"
#include "AudioEvents/AudioCaptureEventArgs.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief available native audio APIs.
	 * 
	 */
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
#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL
		AAudio,
#endif
		OpenSLES,
#elif defined(__linux__)
		ALSA,
#endif
	};

	/**
	 * @brief manages the \link HephAudio::Native::NativeAudio native audio \endlink classes.
	 * 
	 */
	class HEPH_API Audio final
	{
	private:
		Native::NativeAudio* pNativeAudio;

	public:
		void SetOnAudioDeviceAddedHandler(Heph::EventHandler handler);
		void AddOnAudioDeviceAddedHandler(Heph::EventHandler handler);
		void SetOnAudioDeviceRemovedHandler(Heph::EventHandler handler);
		void AddOnAudioDeviceRemovedHandler(Heph::EventHandler handler);
		void SetOnCaptureHandler(Heph::EventHandler handler);
		void AddOnCaptureHandler(Heph::EventHandler handler);
		
	public:
		/** @copydoc default_constructor */
		Audio();

		/**
		 * @copydoc constructor
		 * 
		 * @param api the native audio API that will be used.
		 */
		Audio(AudioAPI api);
		
		Audio(const Audio&) = delete;
		Audio& operator=(const Audio&) = delete;
		
		/** @copydoc destructor */
		~Audio();
		
		/**
		 * gets the pointer to the native audio instance that's internally used.
		 * 
		 */
		Native::NativeAudio* GetNativeAudio() const;

		/** @copydoc HephAudio::Native::NativeAudio::GetAudioDecoder */
		std::shared_ptr<IAudioDecoder> GetAudioDecoder() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::SetAudioDecoder */
		void SetAudioDecoder(std::shared_ptr<IAudioDecoder> pNewDecoder);
		
		/** @copydoc HephAudio::Native::NativeAudio::GetAudioEncoder */
		std::shared_ptr<IAudioEncoder> GetAudioEncoder() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::SetAudioEncoder */
		void SetAudioEncoder(std::shared_ptr<IAudioEncoder> pNewEncoder);
		
		/** @copydoc HephAudio::Native::NativeAudio::Play(const std::filesystem::path&) */
		AudioObject* Play(const std::filesystem::path& filePath);
		
		/** @copydoc HephAudio::Native::NativeAudio::Play(const std::filesystem::path&,uint32_t) */
		AudioObject* Play(const std::filesystem::path& filePath, uint32_t playCount);
		
		/** @copydoc HephAudio::Native::NativeAudio::Play(const std::filesystem::path&,uint32_t,bool) */
		AudioObject* Play(const std::filesystem::path& filePath, uint32_t playCount, bool isPaused);
		
		/** @copydoc HephAudio::Native::NativeAudio::Load(const std::filesystem::path&) */
		AudioObject* Load(const std::filesystem::path& filePath);
		
		/** @copydoc HephAudio::Native::NativeAudio::Load(const std::filesystem::path&,uint32_t) */
		AudioObject* Load(const std::filesystem::path& filePath, uint32_t playCount);
		
		/** @copydoc HephAudio::Native::NativeAudio::Load(const std::filesystem::path&,uint32_t,bool) */
		AudioObject* Load(const std::filesystem::path& filePath, uint32_t playCount, bool isPaused);
		
		/** @copydoc HephAudio::Native::NativeAudio::CreateAudioObject */
		AudioObject* CreateAudioObject(const std::string& name, size_t bufferFrameCount, AudioChannelLayout channelLayout, uint16_t sampleRate);
		
		/** @copydoc HephAudio::Native::NativeAudio::DestroyAudioObject(AudioObject*) */
		bool DestroyAudioObject(AudioObject* pAudioObject);
		
		/** @copydoc HephAudio::Native::NativeAudio::DestroyAudioObject(const Heph::Guid&) */
		bool DestroyAudioObject(const Heph::Guid& audioObjectId);
		
		/** @copydoc HephAudio::Native::NativeAudio::AudioObjectExists(AudioObject*) */
		bool AudioObjectExists(AudioObject* pAudioObject) const;
		
		/** @copydoc HephAudio::Native::NativeAudio::AudioObjectExists(const Heph::Guid&) */
		bool AudioObjectExists(const Heph::Guid& audioObjectId) const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetAudioObject(size_t) */
		AudioObject* GetAudioObject(size_t index) const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetAudioObject(const Heph::Guid&) */
		AudioObject* GetAudioObject(const Heph::Guid& audioObjectId);
		
		/** @copydoc HephAudio::Native::NativeAudio::GetAudioObject(const std::string&) */
		AudioObject* GetAudioObject(const std::string& audioObjectName) const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetAudioObjectCount */
		size_t GetAudioObjectCount() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::ResumeCapture */
		void ResumeCapture();
		
		/** @copydoc HephAudio::Native::NativeAudio::PauseCapture */
		void PauseCapture();
		
		/** @copydoc HephAudio::Native::NativeAudio::IsCapturePaused */
		bool IsCapturePaused() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetDeviceEnumerationPeriod */
		uint32_t GetDeviceEnumerationPeriod() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::SetDeviceEnumerationPeriod */
		void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms);
		
		/** @copydoc HephAudio::Native::NativeAudio::SetMasterVolume */
		void SetMasterVolume(double volume);
		
		/** @copydoc HephAudio::Native::NativeAudio::GetMasterVolume */
		double GetMasterVolume() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetRenderFormat */
		const AudioFormatInfo& GetRenderFormat() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetCaptureFormat */
		const AudioFormatInfo& GetCaptureFormat() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeRender() */
		void InitializeRender();
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeRender(AudioChannelLayout,uint32_t) */
		void InitializeRender(AudioChannelLayout channelLayout, uint32_t sampleRate);
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeRender(AudioFormatInfo) */
		void InitializeRender(AudioFormatInfo format);
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeRender(AudioDevice*,AudioFormatInfo) */
		void InitializeRender(AudioDevice* device, AudioFormatInfo format);
		
		/** @copydoc HephAudio::Native::NativeAudio::StopRendering */
		void StopRendering();
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeCapture() */
		void InitializeCapture();
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeCapture(AudioChannelLayout,uint32_t) */
		void InitializeCapture(AudioChannelLayout channelLayout, uint32_t sampleRate);
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeCapture(AudioFormatInfo) */
		void InitializeCapture(AudioFormatInfo format);
		
		/** @copydoc HephAudio::Native::NativeAudio::InitializeCapture(AudioDevice*,AudioFormatInfo) */
		void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
		
		/** @copydoc HephAudio::Native::NativeAudio::StopCapturing */
		void StopCapturing();
		
		/** @copydoc HephAudio::Native::NativeAudio::GetNativeParams */
		void GetNativeParams(Native::NativeAudioParams& nativeParams) const;
		
		/** @copydoc HephAudio::Native::NativeAudio::SetNativeParams */
		void SetNativeParams(const Native::NativeAudioParams& nativeParams);
		
		/** @copydoc HephAudio::Native::NativeAudio::GetAudioDeviceById */
		AudioDevice GetAudioDeviceById(const std::string& deviceId) const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetRenderDevice */
		AudioDevice GetRenderDevice() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetCaptureDevice */
		AudioDevice GetCaptureDevice() const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetDefaultAudioDevice */
		AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
		
		/** @copydoc HephAudio::Native::NativeAudio::GetAudioDevices */
		std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
	};
}