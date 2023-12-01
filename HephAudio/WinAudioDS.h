#pragma once
#ifdef _WIN32
#include "HephAudioFramework.h"
#include "NativeAudio.h"
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
		class WinAudioDS final : public NativeAudio
		{
		private:
			HWND hwnd;
			Microsoft::WRL::ComPtr<IDirectSound> pDirectSound;
			Microsoft::WRL::ComPtr<IDirectSoundBuffer> pDirectSoundBuffer;
			Microsoft::WRL::ComPtr<IDirectSoundCapture> pDirectSoundCapture;
			Microsoft::WRL::ComPtr<IDirectSoundCaptureBuffer> pDirectSoundCaptureBuffer;
		public:
			WinAudioDS();
			WinAudioDS(const WinAudioDS&) = delete;
			WinAudioDS& operator=(const WinAudioDS&) = delete;
			~WinAudioDS();
			void SetMasterVolume(heph_float volume) override;
			heph_float GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void SetDisplayName(HephCommon::StringBuffer displayName) override;
			void SetIconPath(HephCommon::StringBuffer iconPath) override;
		private:
			bool EnumerateAudioDevices() override;
			void RenderData();
			void CaptureData();
			// lpContext = a pointer to the current WinAudioDS that calls this method.
			static BOOL CALLBACK RenderDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext);
			// lpContext = a pointer to the current WinAudioDS that calls this method.
			static BOOL CALLBACK CaptureDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext);
			static HephCommon::StringBuffer GuidToString(LPGUID guid);
			static GUID StringToGuid(HephCommon::StringBuffer str);
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		};
	}
}
#endif