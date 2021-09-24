#pragma once
#ifdef _WIN32
#include "framework.h"
#include "INativeAudio.h"
#include <wrl.h>
#include <dsound.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses DirectSound. Use WinAudio if you have Windows Vista or higher.
		class WinAudioDS : public INativeAudio
		{
		protected:
			HWND hwnd;
			Microsoft::WRL::ComPtr<IDirectSound> pDirectSound;
			Microsoft::WRL::ComPtr<IDirectSoundBuffer> pDirectSoundBuffer;
			Microsoft::WRL::ComPtr<IDirectSoundCapture> pDirectSoundCapture;
			Microsoft::WRL::ComPtr<IDirectSoundCaptureBuffer> pDirectSoundCaptureBuffer;
			std::vector<AudioDevice> audioDevices;
			std::thread deviceThread;
			std::wstring renderDeviceId;
			std::wstring captureDeviceId;
		public:
			WinAudioDS();
			WinAudioDS(const WinAudioDS&) = delete;
			WinAudioDS& operator=(const WinAudioDS&) = delete;
			virtual ~WinAudioDS();
			virtual void SetMasterVolume(double volume) const;
			virtual double GetMasterVolume() const;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			virtual void StopRendering();
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
			virtual void StopCapturing();
			virtual void SetDisplayName(std::wstring displayName);
			virtual void SetIconPath(std::wstring iconPath);
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const;
		protected:
			virtual void JoinDeviceThread();
			virtual void EnumerateAudioDevices();
			virtual void RenderData();
			virtual void CaptureData();
		protected:
			// lpContext = a pointer to the current WinAudioDS that calls this method.
			static BOOL CALLBACK RenderDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext);
			// lpContext = a pointer to the current WinAudioDS that calls this method.
			static BOOL CALLBACK CaptureDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext);
			static std::string GuidToString(LPGUID guid);
			static std::wstring GuidToWString(LPGUID guid);
			static GUID StringToGuid(std::string str);
			static GUID WStringToGuid(std::wstring str);
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		};
	}
}
#endif