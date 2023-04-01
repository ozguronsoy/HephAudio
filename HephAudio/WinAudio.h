#pragma once
#ifdef _WIN32
#include "framework.h"
#include "NativeAudio.h"
#include <wrl.h>
#include <Mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <mmeapi.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Uses WASAPI. Use WinAudioDS if you have Windows XP or lower.
		/// </summary>
		class WinAudio final : public NativeAudio
		{
		private:
			HANDLE hEvent;
			Microsoft::WRL::ComPtr<IMMDeviceEnumerator> pEnumerator;
			Microsoft::WRL::ComPtr<IAudioClient3> pRenderAudioClient;
			Microsoft::WRL::ComPtr<IAudioSessionManager2> pRenderSessionManager;
			Microsoft::WRL::ComPtr<IAudioSessionControl> pRenderSessionControl;
			Microsoft::WRL::ComPtr<IAudioClient3> pCaptureAudioClient;
		public:
			/// <summary>
			/// Creates and initalizes a WinAudio instance.
			/// </summary>
			WinAudio();
			WinAudio(const WinAudio&) = delete;
			WinAudio& operator=(const WinAudio&) = delete;
			/// <summary>
			/// Frees the WASAPI resources.
			/// </summary>
			~WinAudio();
			void SetMasterVolume(hephaudio_float volume) override;
			hephaudio_float GetMasterVolume() const override;
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
			static EDataFlow DeviceTypeToDataFlow(AudioDeviceType deviceType);
			static AudioDeviceType DataFlowToDeviceType(EDataFlow dataFlow);
		};
	}
}
#endif