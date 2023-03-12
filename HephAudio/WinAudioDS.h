#pragma once
#ifdef _WIN32
#include "framework.h"
#include "NativeAudio.h"
#include <mutex>
#include <wrl.h>
#include <dsound.h>
#include <mmeapi.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Uses DirectSound. Use WinAudio if you have Windows Vista or higher.
		/// </summary>
		class WinAudioDS : public NativeAudio
		{
		protected:
			/// <summary>
			/// The handle to the audio window.
			/// </summary>
			HWND hwnd;
			/// <summary>
			/// the DirectSound interface for rendering.
			/// </summary>
			Microsoft::WRL::ComPtr<IDirectSound> pDirectSound;
			/// <summary>
			/// The DirectSound interface for the render buffer.
			/// </summary>
			Microsoft::WRL::ComPtr<IDirectSoundBuffer> pDirectSoundBuffer;
			/// <summary>
			/// the DirectSound interface for capturing.
			/// </summary>
			Microsoft::WRL::ComPtr<IDirectSoundCapture> pDirectSoundCapture;
			/// <summary>
			/// The DirectSound interface for the capture buffer.
			/// </summary>
			Microsoft::WRL::ComPtr<IDirectSoundCaptureBuffer> pDirectSoundCaptureBuffer;
			/// <summary>
			/// The available audio devices.
			/// </summary>
			std::vector<AudioDevice> audioDevices;
			/// <summary>
			/// Enumerates the audio devices periodically to detect any change in available devices.
			/// </summary>
			std::thread deviceThread;
			/// <summary>
			/// Locks to prevent race condition while accessing and enumerating audio devices.
			/// </summary>
			mutable std::mutex deviceMutex;
		public:
			/// <summary>
			/// Creates and initalizes an WinAudioDS instance.
			/// </summary>
			WinAudioDS();
			WinAudioDS(const WinAudioDS&) = delete;
			WinAudioDS& operator=(const WinAudioDS&) = delete;
			/// <summary>
			/// Frees the DirectSound resources.
			/// </summary>
			virtual ~WinAudioDS();
			virtual void SetMasterVolume(hephaudio_float volume) override;
			virtual hephaudio_float GetMasterVolume() const override;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopRendering() override;
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopCapturing() override;
			virtual void SetDisplayName(StringBuffer displayName) override;
			virtual void SetIconPath(StringBuffer iconPath) override;
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const override;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const override;
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