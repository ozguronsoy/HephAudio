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
		return &pNativeAudio->audioFormats;
	}
	void Audio::SetOnExceptionHandler(AudioEventHandler handler)
	{
		pNativeAudio->OnException += handler;
	}
	void Audio::SetOnAudioDeviceAddedHandler(AudioEventHandler handler)
	{
		pNativeAudio->OnAudioDeviceAdded += handler;
	}
	void Audio::SetOnAudioDeviceRemovedHandler(AudioEventHandler handler)
	{
		pNativeAudio->OnAudioDeviceRemoved += handler;
	}
	void Audio::SetOnCaptureHandler(AudioEventHandler handler)
	{
		pNativeAudio->OnCapture += handler;
	}
#ifdef __ANDROID__
	Audio::Audio(JavaVM* jvm)
	{
		const uint32_t androidApiLevel = android_get_device_api_level();
		if (androidApiLevel >= 27)
		{
			pNativeAudio = new AndroidAudioA(jvm);
		}
		else
		{
			pNativeAudio = new AndroidAudioSLES(jvm);
		}
	}
#else
	Audio::Audio()
	{
		pNativeAudio = nullptr;
#if defined(_WIN32)
		if (IsWindowsVistaOrGreater())
		{
			pNativeAudio = new WinAudio();
		}
		else
		{
			pNativeAudio = new WinAudioDS();
		}
#endif
	}
#endif
	Audio::~Audio()
	{
		if (pNativeAudio != nullptr)
		{
			delete pNativeAudio;
			pNativeAudio = nullptr;
		}
	}
	NativeAudio* Audio::GetNativeAudio() const
	{
		return pNativeAudio;
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath)
	{
		return pNativeAudio->Play(filePath);
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath, bool isPaused)
	{
		return pNativeAudio->Play(filePath, isPaused);
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath, uint32_t loopCount)
	{
		return pNativeAudio->Play(filePath, loopCount);
	}
	std::shared_ptr<AudioObject> Audio::Play(StringBuffer filePath, uint32_t loopCount, bool isPaused)
	{
		return pNativeAudio->Play(filePath, loopCount, isPaused);
	}
	std::vector<std::shared_ptr<AudioObject>> Audio::Queue(StringBuffer queueName, HEPHAUDIO_DOUBLE queueDelay, const std::vector<StringBuffer>& filePaths)
	{
		return pNativeAudio->Queue(queueName, queueDelay, filePaths);
	}
	std::shared_ptr<AudioObject> Audio::Load(StringBuffer filePath)
	{
		return pNativeAudio->Load(filePath);
	}
	std::shared_ptr<AudioObject> Audio::CreateAO(StringBuffer name, size_t bufferFrameCount)
	{
		return pNativeAudio->CreateAO(name, bufferFrameCount);
	}
	bool Audio::DestroyAO(std::shared_ptr<AudioObject> audioObject)
	{
		return pNativeAudio->DestroyAO(audioObject);
	}
	bool Audio::AOExists(std::shared_ptr<AudioObject> audioObject) const
	{
		return pNativeAudio->AOExists(audioObject);
	}
	void Audio::SetAOPosition(std::shared_ptr<AudioObject> audioObject, HEPHAUDIO_DOUBLE position)
	{
		pNativeAudio->SetAOPosition(audioObject, position);
	}
	HEPHAUDIO_DOUBLE Audio::GetAOPosition(std::shared_ptr<AudioObject> audioObject) const
	{
		return pNativeAudio->GetAOPosition(audioObject);
	}
	std::shared_ptr<AudioObject> Audio::GetAO(StringBuffer aoName) const
	{
		return pNativeAudio->GetAO(aoName);
	}
	std::shared_ptr<AudioObject> Audio::GetAO(StringBuffer queueName, size_t index) const
	{
		return pNativeAudio->GetAO(queueName, index);
	}
	void Audio::PauseCapture(bool pause)
	{
		pNativeAudio->PauseCapture(pause);
	}
	bool Audio::IsCapturePaused() const noexcept
	{
		return pNativeAudio->IsCapturePaused();
	}
	void Audio::SetMasterVolume(HEPHAUDIO_DOUBLE volume)
	{
		pNativeAudio->SetMasterVolume(volume);
	}
	HEPHAUDIO_DOUBLE Audio::GetMasterVolume() const
	{
		return pNativeAudio->GetMasterVolume();
	}
	void Audio::SetCategoryVolume(StringBuffer categoryName, HEPHAUDIO_DOUBLE newVolume)
	{
		pNativeAudio->SetCategoryVolume(categoryName, newVolume);
	}
	HEPHAUDIO_DOUBLE Audio::GetCategoryVolume(StringBuffer categoryName) const
	{
		return pNativeAudio->GetCategoryVolume(categoryName);
	}
	void Audio::RegisterCategory(Category category)
	{
		pNativeAudio->RegisterCategory(category);
	}
	void Audio::UnregisterCategory(StringBuffer categoryName)
	{
		pNativeAudio->UnregisterCategory(categoryName);
	}
	bool Audio::CategoryExists(StringBuffer categoryName) const
	{
		return pNativeAudio->CategoryExists(categoryName);
	}
	void Audio::Skip(StringBuffer queueName, bool applyDelay)
	{
		pNativeAudio->Skip(queueName, applyDelay);
	}
	void Audio::Skip(size_t skipCount, StringBuffer queueName, bool applyDelay)
	{
		pNativeAudio->Skip(skipCount, queueName, applyDelay);
	}
	AudioFormatInfo Audio::GetRenderFormat() const
	{
		return pNativeAudio->GetRenderFormat();
	}
	AudioFormatInfo Audio::GetCaptureFormat() const
	{
		return pNativeAudio->GetCaptureFormat();
	}
	void Audio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
	{
		pNativeAudio->InitializeRender(device, format);
	}
	void Audio::StopRendering()
	{
		pNativeAudio->StopRendering();
	}
	void Audio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
	{
		pNativeAudio->InitializeCapture(device, format);
	}
	void Audio::StopCapturing()
	{
		pNativeAudio->StopCapturing();
	}
#if (defined(_WIN32) && defined(_WIN32_WINNT_VISTA))
	void Audio::SetDisplayName(StringBuffer displayName)
	{
		pNativeAudio->SetDisplayName(displayName);
	}
	void Audio::SetIconPath(StringBuffer iconPath)
	{
		pNativeAudio->SetIconPath(iconPath);
	}
#endif
#if defined(_WIN32)
	AudioDevice Audio::GetAudioDeviceById(StringBuffer deviceId) const
	{
		return pNativeAudio->GetAudioDeviceById(deviceId);
	}
	AudioDevice Audio::GetRenderDevice() const
	{
		return pNativeAudio->GetRenderDevice();
	}
	AudioDevice Audio::GetCaptureDevice() const
	{
		return pNativeAudio->GetCaptureDevice();
	}
	AudioDevice Audio::GetDefaultAudioDevice(AudioDeviceType deviceType) const
	{
		return pNativeAudio->GetDefaultAudioDevice(deviceType);
	}
	std::vector<AudioDevice> Audio::GetAudioDevices(AudioDeviceType deviceType) const
	{
		return pNativeAudio->GetAudioDevices(deviceType);
	}
#endif
	bool Audio::SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat)
	{
		return pNativeAudio->SaveToFile(filePath, overwrite, buffer, targetFormat);
	}
}