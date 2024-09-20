#pragma once
#if defined(_WIN32)
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "WinAudioBase.h"
#include <mmeapi.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief uses MMEAPI (waveIn and waveOut)
		 * 
		 */
		class HEPH_API WinAudioMME final : public WinAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;

		private:
			static constexpr uint32_t HDR_COUNT = 2;

		private:
			HWAVEOUT hwo;
			HWAVEIN hwi;
			WAVEHDR renderHdrs[HDR_COUNT];
			WAVEHDR captureHdrs[HDR_COUNT];

		public:
			/** @copydoc default_constructor */
			WinAudioMME();

			WinAudioMME(const WinAudioMME&) = delete;
			WinAudioMME& operator=(const WinAudioMME&) = delete;

			/** @copydoc destructor */
			~WinAudioMME();
			
			void SetMasterVolume(double volume) override;
			double GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;
		private:
			bool EnumerateAudioDevices() override;
			AudioFormatInfo GetClosestFormat(DWORD dwFormats, const AudioFormatInfo& format) const;
			static size_t CalculateBufferSize(uint32_t byteRate, uint32_t sampleRate);
			static void CALLBACK RenderCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
			static void CALLBACK CaptureCallback(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
			static std::string GetErrorString(MMRESULT mmResult);
		};
	}
}
#endif