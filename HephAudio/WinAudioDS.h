#pragma once
#ifdef _WIN32
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "WinAudioBase.h"
#include <dsound.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses DirectSound; add -lDsound, -ldxguid, and -lWinmm to the compiler flags.
		class WinAudioDS final : public WinAudioBase
		{
		private:
			HWND hwnd;
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
		private:
			bool EnumerateAudioDevices() override;
			void CheckAudioDevices() override;
			void RenderData(GUID deviceId);
			void CaptureData(GUID deviceId);
			static BOOL CALLBACK RenderDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID pWinAudioDS);
			static BOOL CALLBACK CaptureDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID pWinAudioDS);
			static HephCommon::StringBuffer GuidToString(LPGUID guid);
			static GUID StringToGuid(HephCommon::StringBuffer str);
			static void RestrictAudioFormatInfo(AudioFormatInfo& afi, const DSCAPS& dsCaps);
			static void RestrictAudioFormatInfo(AudioFormatInfo& afi, const DSCCAPS& dscCaps);
		};
	}
}
#endif