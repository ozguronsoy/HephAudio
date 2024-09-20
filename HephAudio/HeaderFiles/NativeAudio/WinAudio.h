#pragma once
#ifdef _WIN32
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "WinAudioBase.h"
#include "Params/WasapiParams.h"
#include <wrl.h>
#include <Mmdeviceapi.h>
#include <audiopolicy.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief uses WASAPI.
		 * 
		 */
		class HEPH_API WinAudio final : public WinAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;

		private:
			WasapiParams params;
			Microsoft::WRL::ComPtr<IMMDeviceEnumerator> pEnumerator;
			Microsoft::WRL::ComPtr<IAudioSessionManager> pRenderSessionManager;
			Microsoft::WRL::ComPtr<IAudioSessionControl> pRenderSessionControl;

		public:
			/** @copydoc default_constructor */
			WinAudio();

			WinAudio(const WinAudio&) = delete;
			WinAudio& operator=(const WinAudio&) = delete;

			/** @copydoc destructor */
			~WinAudio();

			void SetMasterVolume(double volume) override;
			double GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;

			/**
			 * sets the display name in the volume mixer.
			 * 
			 */
			void SetDisplayName(const std::string& displayName);

			/**
			 * sets the icon displayed in the volume mixer.
			 * 
			 */
			void SetIconPath(const std::filesystem::path& iconPath);

		private:
			bool EnumerateAudioDevices() override;
			void CheckAudioDevices() override;
			void RenderData(AudioDevice* device, AudioFormatInfo format);
			void CaptureData(AudioDevice* device, AudioFormatInfo format);
			static AudioDeviceType DataFlowToDeviceType(EDataFlow dataFlow);
		};
	}
}
#endif