#include "Audio.h"
#include "WinAudio.h"
#include "WinAudioDS.h"
#include "AndroidAudioA.h"
#include "AndroidAudioSLES.h"
#if defined(_WIN32)
#include <VersionHelpers.h>
#elif defined(__ANDROID__)
#include <android/api-level.h>
#endif

using namespace HephAudio::Native;

namespace HephAudio
{
	Formats::AudioFormats* Audio::GetAudioFormats() const
	{
		return &this->pNativeAudio->audioFormats;
	}
	void Audio::SetOnExceptionHandler(AudioEventHandler handler)
	{
		this->pNativeAudio->OnException += handler;
	}
	void Audio::SetOnAudioDeviceAddedHandler(AudioEventHandler handler)
	{
		this->pNativeAudio->OnAudioDeviceAdded += handler;
	}
	void Audio::SetOnAudioDeviceRemovedHandler(AudioEventHandler handler)
	{
		this->pNativeAudio->OnAudioDeviceRemoved += handler;
	}
	void Audio::SetOnCaptureHandler(AudioEventHandler handler)
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
		else
		{
			this->pNativeAudio = new AndroidAudioSLES(jvm);
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
#endif
	}
#endif
	Audio::~Audio()
	{
		if (pNativeAudio != nullptr)
		{
			delete this->pNativeAudio;
			this->pNativeAudio = nullptr;
		}
	}
	HephAudio::Native::NativeAudio* Audio::operator->() const noexcept
	{
		return this->pNativeAudio;
	}
	NativeAudio* Audio::GetNativeAudio() const
	{
		return this->pNativeAudio;
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath)
	{
		return this->pNativeAudio->Play(filePath);
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath, bool isPaused)
	{
		return this->pNativeAudio->Play(filePath, isPaused);
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath, uint32_t loopCount)
	{
		return this->pNativeAudio->Play(filePath, loopCount);
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath, uint32_t loopCount, bool isPaused)
	{
		return this->pNativeAudio->Play(filePath, loopCount, isPaused);
	}
	std::vector<std::shared_ptr<AudioObject>> Audio::Queue(StringBuffer queueName, hephaudio_float queueDelay_ms, const std::vector<StringBuffer>& filePaths)
	{
		return this->pNativeAudio->Queue(queueName, queueDelay_ms, filePaths);
	}
	std::shared_ptr<AudioObject> Audio::Load(StringBuffer filePath)
	{
		return this->pNativeAudio->Load(filePath);
	}
	std::shared_ptr<AudioObject> Audio::CreateAO(StringBuffer name, size_t bufferFrameCount)
	{
		return this->pNativeAudio->CreateAO(name, bufferFrameCount);
	}
	bool Audio::DestroyAO(std::shared_ptr<AudioObject> audioObject)
	{
		return this->pNativeAudio->DestroyAO(audioObject);
	}
	bool Audio::AOExists(std::shared_ptr<AudioObject> audioObject) const
	{
		return this->pNativeAudio->AOExists(audioObject);
	}
	void Audio::SetAOPosition(std::shared_ptr<AudioObject> audioObject, hephaudio_float position)
	{
		this->pNativeAudio->SetAOPosition(audioObject, position);
	}
	hephaudio_float Audio::GetAOPosition(std::shared_ptr<AudioObject> audioObject) const
	{
		return this->pNativeAudio->GetAOPosition(audioObject);
	}
	std::shared_ptr<AudioObject> Audio::GetAO(StringBuffer aoName) const
	{
		return this->pNativeAudio->GetAO(aoName);
	}
	std::shared_ptr<AudioObject> Audio::GetAO(StringBuffer queueName, size_t index) const
	{
		return this->pNativeAudio->GetAO(queueName, index);
	}
	void Audio::PauseCapture(bool pause)
	{
		this->pNativeAudio->PauseCapture(pause);
	}
	bool Audio::IsCapturePaused() const noexcept
	{
		return this->pNativeAudio->IsCapturePaused();
	}
	void Audio::SetMasterVolume(hephaudio_float volume)
	{
		this->pNativeAudio->SetMasterVolume(volume);
	}
	hephaudio_float Audio::GetMasterVolume() const
	{
		return this->pNativeAudio->GetMasterVolume();
	}
	void Audio::Skip(StringBuffer queueName, bool applyDelay)
	{
		this->pNativeAudio->Skip(queueName, applyDelay);
	}
	void Audio::Skip(size_t skipCount, StringBuffer queueName, bool applyDelay)
	{
		this->pNativeAudio->Skip(skipCount, queueName, applyDelay);
	}
	AudioFormatInfo Audio::GetRenderFormat() const
	{
		return this->pNativeAudio->GetRenderFormat();
	}
	AudioFormatInfo Audio::GetCaptureFormat() const
	{
		return this->pNativeAudio->GetCaptureFormat();
	}
	void Audio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
	{
		this->pNativeAudio->InitializeRender(device, format);
	}
	void Audio::StopRendering()
	{
		this->pNativeAudio->StopRendering();
	}
	void Audio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
	{
		this->pNativeAudio->InitializeCapture(device, format);
	}
	void Audio::StopCapturing()
	{
		this->pNativeAudio->StopCapturing();
	}
#if (defined(_WIN32) && defined(_WIN32_WINNT_VISTA))
	void Audio::SetDisplayName(StringBuffer displayName)
	{
		this->pNativeAudio->SetDisplayName(displayName);
	}
	void Audio::SetIconPath(StringBuffer iconPath)
	{
		this->pNativeAudio->SetIconPath(iconPath);
	}
#endif
#if defined(_WIN32)
	AudioDevice Audio::GetAudioDeviceById(StringBuffer deviceId) const
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
#endif
	bool Audio::SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer)
	{
		return this->pNativeAudio->SaveToFile(filePath, overwrite, buffer);
	}
}