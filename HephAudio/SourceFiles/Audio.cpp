#include "Audio.h"
#include "NativeAudio/WinAudio.h"
#include "NativeAudio/WinAudioDS.h"
#include "NativeAudio/WinAudioMME.h"
#include "NativeAudio/AndroidAudioA.h"
#include "NativeAudio/AndroidAudioSLES.h"
#include "NativeAudio/LinuxAudio.h"
#include "NativeAudio/AppleAudio.h"
#include "StringHelpers.h"
#if defined(_WIN32)
#include <VersionHelpers.h>
#elif defined(__ANDROID__)
#include <android/api-level.h>
#endif

using namespace Heph;
using namespace HephAudio::Native;

namespace HephAudio
{
	Audio::Audio() : Audio(AudioAPI::Default) {}

	Audio::Audio(AudioAPI api) 
		: pNativeAudio(Audio::CreateNativeAudio(api)), 
		OnAudioDeviceAdded(this->pNativeAudio->OnAudioDeviceAdded),
		OnAudioDeviceRemoved(this->pNativeAudio->OnAudioDeviceRemoved),
		OnCapture(this->pNativeAudio->OnCapture) {}

	std::shared_ptr<NativeAudio> Audio::GetNativeAudio() const
	{
		return this->pNativeAudio;
	}

	std::shared_ptr<IAudioDecoder> Audio::GetAudioDecoder() const
	{
		return this->pNativeAudio->GetAudioDecoder();
	}

	void Audio::SetAudioDecoder(std::shared_ptr<IAudioDecoder> pNewDecoder)
	{
		this->pNativeAudio->SetAudioDecoder(pNewDecoder);
	}

	std::shared_ptr<IAudioEncoder> Audio::GetAudioEncoder() const
	{
		return this->pNativeAudio->GetAudioEncoder();
	}

	void Audio::SetAudioEncoder(std::shared_ptr<IAudioEncoder> pNewEncoder)
	{
		this->pNativeAudio->SetAudioEncoder(pNewEncoder);
	}

	AudioObject* Audio::Play(const std::filesystem::path& filePath)
	{
		return this->pNativeAudio->Play(filePath);
	}

	AudioObject* Audio::Play(const std::filesystem::path& filePath, uint32_t playCount)
	{
		return this->pNativeAudio->Play(filePath, playCount);
	}

	AudioObject* Audio::Load(const std::filesystem::path& filePath)
	{
		return this->pNativeAudio->Load(filePath);
	}

	AudioObject* Audio::Load(const std::filesystem::path& filePath, uint32_t playCount)
	{
		return this->pNativeAudio->Load(filePath, playCount);
	}

	AudioObject* Audio::CreateAudioObject(const std::string& name, size_t bufferFrameCount, AudioChannelLayout channelLayout, uint32_t sampleRate)
	{
		return this->pNativeAudio->CreateAudioObject(name, bufferFrameCount, channelLayout, sampleRate);
	}

	bool Audio::DestroyAudioObject(AudioObject* pAudioObject)
	{
		return this->pNativeAudio->DestroyAudioObject(pAudioObject);
	}

	bool Audio::DestroyAudioObject(const Heph::Guid& audioObjectId)
	{
		return this->pNativeAudio->DestroyAudioObject(audioObjectId);
	}

	bool Audio::AudioObjectExists(AudioObject* pAudioObject) const
	{
		return this->pNativeAudio->AudioObjectExists(pAudioObject);
	}

	bool Audio::AudioObjectExists(const Heph::Guid& audioObjectId) const
	{
		return this->pNativeAudio->AudioObjectExists(audioObjectId);
	}

	AudioObject* Audio::GetAudioObject(size_t index) const
	{
		return this->pNativeAudio->GetAudioObject(index);
	}

	AudioObject* Audio::GetAudioObject(const Heph::Guid& audioObjectId)
	{
		return this->pNativeAudio->GetAudioObject(audioObjectId);
	}

	AudioObject* Audio::GetAudioObject(const std::string& audioObjectName) const
	{
		return this->pNativeAudio->GetAudioObject(audioObjectName);
	}

	size_t Audio::GetAudioObjectCount() const
	{
		return this->pNativeAudio->GetAudioObjectCount();
	}

	void Audio::ResumeCapture()
	{
		this->pNativeAudio->ResumeCapture();
	}

	void Audio::PauseCapture()
	{
		this->pNativeAudio->PauseCapture();
	}

	bool Audio::IsCapturePaused() const
	{
		return this->pNativeAudio->IsCapturePaused();
	}

	uint32_t Audio::GetDeviceEnumerationPeriod() const
	{
		return this->pNativeAudio->GetDeviceEnumerationPeriod();
	}

	void Audio::SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms)
	{
		this->pNativeAudio->SetDeviceEnumerationPeriod(deviceEnumerationPeriod_ms);
	}

	void Audio::SetMasterVolume(double volume)
	{
		this->pNativeAudio->SetMasterVolume(volume);
	}

	double Audio::GetMasterVolume() const
	{
		return this->pNativeAudio->GetMasterVolume();
	}

	const AudioFormatInfo& Audio::GetRenderFormat() const
	{
		return this->pNativeAudio->GetRenderFormat();
	}

	const AudioFormatInfo& Audio::GetCaptureFormat() const
	{
		return this->pNativeAudio->GetCaptureFormat();
	}

	void Audio::InitializeRender()
	{
		this->pNativeAudio->InitializeRender();
	}

	void Audio::InitializeRender(AudioChannelLayout channelLayout, uint32_t sampleRate)
	{
		this->pNativeAudio->InitializeRender(channelLayout, sampleRate);
	}

	void Audio::InitializeRender(AudioFormatInfo format)
	{
		this->pNativeAudio->InitializeRender(format);
	}

	void Audio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
	{
		this->pNativeAudio->InitializeRender(device, format);
	}

	void Audio::StopRendering()
	{
		this->pNativeAudio->StopRendering();
	}

	void Audio::InitializeCapture()
	{
		this->pNativeAudio->InitializeCapture();
	}

	void Audio::InitializeCapture(AudioChannelLayout channelLayout, uint32_t sampleRate)
	{
		this->pNativeAudio->InitializeCapture(channelLayout, sampleRate);
	}

	void Audio::InitializeCapture(AudioFormatInfo format)
	{
		this->pNativeAudio->InitializeCapture(format);
	}

	void Audio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
	{
		this->pNativeAudio->InitializeCapture(device, format);
	}

	void Audio::StopCapturing()
	{
		this->pNativeAudio->StopCapturing();
	}

	void Audio::GetNativeParams(Native::NativeAudioParams& nativeParams) const
	{
		this->pNativeAudio->GetNativeParams(nativeParams);
	}

	void Audio::SetNativeParams(const NativeAudioParams& nativeParams)
	{
		this->pNativeAudio->SetNativeParams(nativeParams);
	}

	AudioDevice Audio::GetAudioDeviceById(const std::string& deviceId) const
	{
		return this->pNativeAudio->GetAudioDeviceById(deviceId);
	}

	AudioDevice Audio::GetRenderDevice() const
	{
		return this->pNativeAudio->GetRenderDevice();
	}

	AudioDevice Audio::GetCaptureDevice() const
	{
		return this->pNativeAudio->GetCaptureDevice();
	}

	AudioDevice Audio::GetDefaultAudioDevice(AudioDeviceType deviceType) const
	{
		return this->pNativeAudio->GetDefaultAudioDevice(deviceType);
	}

	std::vector<AudioDevice> Audio::GetAudioDevices(AudioDeviceType deviceType) const
	{
		return this->pNativeAudio->GetAudioDevices(deviceType);
	}

	std::shared_ptr<Native::NativeAudio> Audio::CreateNativeAudio(AudioAPI api)
	{
#if defined(_WIN32)

		switch (api)
		{
		case AudioAPI::WASAPI:
			return std::shared_ptr<NativeAudio>(new WinAudio());
		case AudioAPI::DirectSound:
			return std::shared_ptr<NativeAudio>(new WinAudioDS());
		case AudioAPI::MMEAPI:
			return std::shared_ptr<NativeAudio>(new WinAudioMME());
		case AudioAPI::Default:
		default:
			return
				(IsWindowsVistaOrGreater())
				? (std::shared_ptr<NativeAudio>(new WinAudio()))
				: (std::shared_ptr<NativeAudio>(new WinAudioDS()));
		}

#elif defined(__ANDROID__)

#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL

		switch (api)
		{
		case AudioAPI::AAudio:
			return std::shared_ptr<NativeAudio>(new AndroidAudioA());
		case AudioAPI::OpenSLES:
			return std::shared_ptr<NativeAudio>(new AndroidAudioSLES());
		case AudioAPI::Default:
		default:
			return std::shared_ptr<NativeAudio>(new AndroidAudioA());
		}

#elif __ANDROID_API__ >= HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL

		return std::shared_ptr<NativeAudio>(new AndroidAudioSLES());

#else

#error unsupported API level

#endif

#elif defined(__linux__)

		return std::shared_ptr<NativeAudio>(new LinuxAudio());

#elif defined(__APPLE__)

		return std::shared_ptr<NativeAudio>(new AppleAudio());

#else

#error unsupported platform

#endif
	}
}