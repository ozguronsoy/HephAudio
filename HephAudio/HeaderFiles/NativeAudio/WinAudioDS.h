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
		// Uses DirectSound
		class WinAudioDS final : public WinAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;
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
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;
		private:
			bool EnumerateAudioDevices() override;
			void CheckAudioDevices() override;
			void RenderData(GUID deviceId);
			void CaptureData(GUID deviceId);
			static BOOL CALLBACK RenderDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID pWinAudioDS);
			static BOOL CALLBACK CaptureDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID pWinAudioDS);
			static std::string GuidToString(LPGUID guid);
			static GUID StringToGuid(const std::string& str);
			static void RestrictAudioFormatInfo(AudioFormatInfo& afi, const DSCAPS& dsCaps);
			static void RestrictAudioFormatInfo(AudioFormatInfo& afi, const DSCCAPS& dscCaps);
		};
	}
}
#endif