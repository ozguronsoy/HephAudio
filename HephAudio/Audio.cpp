#include "Audio.h"
#include "WinAudio.h"
#include "WinAudioDS.h"
#include "AndroidAudioSLES.h"
#if defined(_WIN32)
#include <VersionHelpers.h>
#endif

namespace HephAudio
{
	Formats::AudioFormats* Audio::GetAudioFormats() const
	{
		return &pNativeAudio->audioFormats;
	}
	void Audio::SetOnExceptionHandler(AudioExceptionEventHandler handler)
	{
		pNativeAudio->OnException = handler;
	}
	void Audio::SetOnDefaultAudioDeviceChangeHandler(AudioDeviceEventHandler handler)
	{
		pNativeAudio->OnDefaultAudioDeviceChange = handler;
	}
	void Audio::SetOnAudioDeviceAddedHandler(AudioDeviceEventHandler handler)
	{
		pNativeAudio->OnAudioDeviceAdded = handler;
	}
	void Audio::SetOnAudioDeviceRemovedHandler(AudioDeviceEventHandler handler)
	{
		pNativeAudio->OnAudioDeviceRemoved = handler;
	}
	void Audio::SetOnCaptureHandler(AudioCaptureEventHandler handler)
	{
		pNativeAudio->OnCapture = handler;
	}
#ifdef __ANDROID__
	Audio::Audio(JNIEnv* env)
	{
#if __ANDROID_API__ >= 27
		pNativeAudio = new AndroidAudioA(env);
#elif __ANDROID_API__ >= 9
		pNativeAudio = new AndroidAudioSLES();
#endif
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
	INativeAudio* Audio::GetNativeAudio() const
	{
		return pNativeAudio;
	}
	std::shared_ptr<IAudioObject> Audio::Play(std::wstring filePath)
	{
		return pNativeAudio->Play(filePath);
	}
	std::shared_ptr<IAudioObject> Audio::Play(std::wstring filePath, bool isPaused)
	{
		return pNativeAudio->Play(filePath, isPaused);
	}
	std::shared_ptr<IAudioObject> Audio::Play(std::wstring filePath, uint32_t loopCount)
	{
		return pNativeAudio->Play(filePath, loopCount);
	}
	std::shared_ptr<IAudioObject> Audio::Play(std::wstring filePath, uint32_t loopCount, bool isPaused)
	{
		return pNativeAudio->Play(filePath, loopCount, isPaused);
	}
	std::vector<std::shared_ptr<IAudioObject>> Audio::Queue(std::wstring queueName, uint32_t queueDelay, std::vector<std::wstring> filePaths)
	{
		return pNativeAudio->Queue(queueName, queueDelay, filePaths);
	}
	std::shared_ptr<IAudioObject> Audio::Load(std::wstring filePath)
	{
		return pNativeAudio->Load(filePath);
	}
	std::shared_ptr<IAudioObject> Audio::CreateAO(std::wstring name, size_t bufferFrameCount)
	{
		return pNativeAudio->CreateAO(name, bufferFrameCount);
	}
	bool Audio::DestroyAO(std::shared_ptr<IAudioObject> audioObject)
	{
		return pNativeAudio->DestroyAO(audioObject);
	}
	bool Audio::AOExists(std::shared_ptr<IAudioObject> audioObject) const
	{
		return pNativeAudio->AOExists(audioObject);
	}
	void Audio::SetAOPosition(std::shared_ptr<IAudioObject> audioObject, double position)
	{
		pNativeAudio->SetAOPosition(audioObject, position);
	}
	double Audio::GetAOPosition(std::shared_ptr<IAudioObject> audioObject) const
	{
		return pNativeAudio->GetAOPosition(audioObject);
	}
	void Audio::PauseCapture(bool pause)
	{
		pNativeAudio->PauseCapture(pause);
	}
	bool Audio::IsCapturePaused() const noexcept
	{
		return pNativeAudio->IsCapturePaused();
	}
	void Audio::SetMasterVolume(double volume)
	{
		pNativeAudio->SetMasterVolume(volume);
	}
	double Audio::GetMasterVolume() const
	{
		return pNativeAudio->GetMasterVolume();
	}
	void Audio::SetCategoryVolume(std::wstring categoryName, double newVolume)
	{
		pNativeAudio->SetCategoryVolume(categoryName, newVolume);
	}
	double Audio::GetCategoryVolume(std::wstring categoryName) const
	{
		return pNativeAudio->GetCategoryVolume(categoryName);
	}
	void Audio::RegisterCategory(Category category)
	{
		pNativeAudio->RegisterCategory(category);
	}
	void Audio::UnregisterCategory(std::wstring categoryName)
	{
		pNativeAudio->UnregisterCategory(categoryName);
	}
	bool Audio::CategoryExists(std::wstring categoryName) const
	{
		return pNativeAudio->CategoryExists(categoryName);
	}
	void Audio::Skip(std::wstring queueName, bool applyDelay)
	{
		pNativeAudio->Skip(queueName, applyDelay);
	}
	void Audio::Skip(size_t skipCount, std::wstring queueName, bool applyDelay)
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
	void Audio::SetDisplayName(std::wstring displayName)
	{
		pNativeAudio->SetDisplayName(displayName);
	}
	void Audio::SetIconPath(std::wstring iconPath)
	{
		pNativeAudio->SetIconPath(iconPath);
	}
#endif
#if defined(_WIN32)
	AudioDevice Audio::GetAudioDeviceById(std::wstring deviceId) const
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
	std::vector<AudioDevice> Audio::GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const
	{
		return pNativeAudio->GetAudioDevices(deviceType, includeInactive);
	}
#endif
	bool Audio::SaveToFile(std::wstring filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat)
	{
		return pNativeAudio->SaveToFile(filePath, overwrite, buffer, targetFormat);
	}
}