#pragma once
#ifdef _WIN32
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "WinAudioBase.h"
#include <dsound.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief uses DirectSound
		 * 
		 */
		class WinAudioDS final : public WinAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;

		private:
			HWND hwnd;

		public:
			/**
			 * creates a new instance and initializes it with default values.
			 * 
			 */
			WinAudioDS();

			WinAudioDS(const WinAudioDS&) = delete;
			WinAudioDS& operator=(const WinAudioDS&) = delete;

			/**
			 * releases the resources and destroys the instance.
			 * 
			 */
			~WinAudioDS();
			
			void SetMasterVolume(double volume) override;
			double GetMasterVolume() const override;
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