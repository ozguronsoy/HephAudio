#pragma once
#ifdef _WIN32
#include "framework.h"
#include "NativeAudio.h"
#include <wrl.h>
#include <dsound.h>
#include <mmeapi.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses DirectSound. Use WinAudio if you have Windows Vista or higher.
		class WinAudioDS : public NativeAudio
		{
		protected:
			HWND hwnd;
			Microsoft::WRL::ComPtr<IDirectSound> pDirectSound;
			Microsoft::WRL::ComPtr<IDirectSoundBuffer> pDirectSoundBuffer;
			Microsoft::WRL::ComPtr<IDirectSoundCapture> pDirectSoundCapture;
			Microsoft::WRL::ComPtr<IDirectSoundCaptureBuffer> pDirectSoundCaptureBuffer;
			std::vector<AudioDevice> audioDevices;
			std::thread deviceThread;
		public:
			WinAudioDS();
			WinAudioDS(const WinAudioDS&) = delete;
			WinAudioDS& operator=(const WinAudioDS&) = delete;
			virtual ~WinAudioDS();
			virtual void SetMasterVolume(HEPHAUDIO_DOUBLE volume);
			virtual HEPHAUDIO_DOUBLE GetMasterVolume() const;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			virtual void StopRendering();
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
			virtual void StopCapturing();
			virtual void SetDisplayName(StringBuffer displayName);
			virtual void SetIconPath(StringBuffer iconPath);
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
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
			static StringBuffer GuidToString(LPGUID guid);
			static GUID StringToGuid(StringBuffer str);
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		};
	}
}
#endif