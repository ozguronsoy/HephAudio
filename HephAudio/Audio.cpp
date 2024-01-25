#include "Audio.h"
#include "WinAudio.h"
#include "WinAudioDS.h"
#include "AndroidAudioA.h"
#include "AndroidAudioSLES.h"
#include "LinuxAudio.h"
#if defined(_WIN32)
#include <VersionHelpers.h>
#elif defined(__ANDROID__)
#include <android/api-level.h>
#endif

using namespace HephCommon;
using namespace HephAudio::Native;

namespace HephAudio
{
	void Audio::SetOnAudioDeviceAddedHandler(EventHandler handler)
	{
		this->pNativeAudio->OnAudioDeviceAdded = handler;
	}
	void Audio::AddOnAudioDeviceAddedHandler(EventHandler handler)
	{
		this->pNativeAudio->OnAudioDeviceAdded += handler;
	}
	void Audio::SetOnAudioDeviceRemovedHandler(EventHandler handler)
	{
		this->pNativeAudio->OnAudioDeviceRemoved = handler;
	}
	void Audio::AddOnAudioDeviceRemovedHandler(EventHandler handler)
	{
		this->pNativeAudio->OnAudioDeviceRemoved += handler;
	}
	void Audio::SetOnCaptureHandler(EventHandler handler)
	{
		this->pNativeAudio->OnCapture = handler;
	}
	void Audio::AddOnCaptureHandler(EventHandler handler)
	{
		this->pNativeAudio->OnCapture += handler;
	}
#ifdef __ANDROID__
	Audio::Audio(JavaVM* jvm)
	{
		const uint32_t androidApiLevel = android_get_device_api_level();
		if (androidApiLevel >= 27)
		{
			this->pNativeAudio = new AndroidAudioA(jvm);
		}
		else if (androidApiLevel >= 21)
		{
			this->pNativeAudio = new AndroidAudioSLES(jvm);
		}
		else
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "Audio::Audio", "API levels under 21 are not supported."));
		}
	}
#else
	Audio::Audio()
	{
#if defined(_WIN32)
		if (IsWindowsVistaOrGreater())
		{
			this->pNativeAudio = new WinAudio();
		}
		else
		{
			this->pNativeAudio = new WinAudioDS();
		}
#elif defined(__linux__)
		this->pNativeAudio = new LinuxAudio();
#elif defined(__APPLE__)
		this->pNativeAudio = new AppleAudio();
#else
		RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "Audio::Audio", "Unsupported platform."));
#endif
	}
#endif
	Audio::~Audio()
	{
		if (this->pNativeAudio != nullptr)
		{
			delete this->pNativeAudio;
			this->pNativeAudio = nullptr;
		}
	}
	NativeAudio* Audio::GetNativeAudio() const
	{
		return this->pNativeAudio;
	}
	AudioObject* Audio::Play(const StringBuffer& filePath)
	{
		return this->pNativeAudio->Play(filePath);
	}
	AudioObject* Audio::Play(const StringBuffer& filePath, uint32_t playCount)
	{
		return this->pNativeAudio->Play(filePath, playCount);
	}
	AudioObject* Audio::Play(const StringBuffer& filePath, uint32_t playCount, bool isPaused)
	{
		return this->pNativeAudio->Play(filePath, playCount, isPaused);
	}
	AudioObject* Audio::Load(const StringBuffer& filePath)
	{
		return this->pNativeAudio->Load(filePath);
	}
	AudioObject* Audio::Load(const StringBuffer& filePath, uint32_t playCount)
	{
		return this->pNativeAudio->Load(filePath, playCount);
	}
	AudioObject* Audio::Load(const StringBuffer& filePath, uint32_t playCount, bool isPaused)
	{
		return this->pNativeAudio->Load(filePath, playCount, isPaused);
	}
	AudioObject* Audio::CreateAudioObject(const StringBuffer& name, size_t bufferFrameCount)
	{
		return this->pNativeAudio->CreateAudioObject(name, bufferFrameCount);
	}
	bool Audio::DestroyAudioObject(AudioObject* pAudioObject)
	{
		return this->pNativeAudio->DestroyAudioObject(pAudioObject);
	}
	bool Audio::AudioObjectExists(AudioObject* pAudioObject) const
	{
		return this->pNativeAudio->AudioObjectExists(pAudioObject);
	}
	AudioObject* Audio::GetAudioObject(size_t index) const
	{
		return this->pNativeAudio->GetAudioObject(index);
	}
	AudioObject* Audio::GetAudioObject(const StringBuffer& audioObjectName) const
	{
		return this->pNativeAudio->GetAudioObject(audioObjectName);
	}
	void Audio::PauseCapture(bool pause)
	{
		this->pNativeAudio->PauseCapture(pause);
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
	void Audio::SetMasterVolume(heph_float volume)
	{
		this->pNativeAudio->SetMasterVolume(volume);
	}
	heph_float Audio::GetMasterVolume() const
	{
		return this->pNativeAudio->GetMasterVolume();
	}
	AudioFormatInfo Audio::GetRenderFormat() const
	{
		return this->pNativeAudio->GetRenderFormat();
	}
	AudioFormatInfo Audio::GetCaptureFormat() const
	{
		return this->pNativeAudio->GetCaptureFormat();
	}
	void Audio::InitializeRender() 
	{
		this->pNativeAudio->InitializeRender();
	}
	void Audio::InitializeRender(uint16_t channelCount, uint32_t sampleRate) 
	{
		this->pNativeAudio->InitializeRender(channelCount, sampleRate);
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
	void Audio::InitializeCapture(uint16_t channelCount, uint32_t sampleRate)
	{
		this->pNativeAudio->InitializeCapture(channelCount, sampleRate);
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
	AudioDevice Audio::GetAudioDeviceById(const StringBuffer& deviceId) const
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
	bool Audio::SaveToFile(const StringBuffer& filePath, bool overwrite, AudioBuffer& buffer)
	{
		return this->pNativeAudio->SaveToFile(filePath, overwrite, buffer);
	}
}