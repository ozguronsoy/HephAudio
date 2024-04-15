#pragma once
#ifdef __ANDROID__
#define HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL 26

#include "HephAudioShared.h"
#include "AndroidAudioBase.h"
#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL
#include <aaudio/AAudio.h>
#endif


namespace HephAudio
{
	namespace Native
	{
		// Uses AAudio
		class AndroidAudioA final : public AndroidAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;
		private:
#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL
			AAudioStream* pRenderStream;
			AAudioStream* pCaptureStream;
			size_t renderBufferFrameCount;
			size_t captureBufferFrameCount;
			heph_float masterVolume;
#endif
		public:
			AndroidAudioA();
			AndroidAudioA(const AndroidAudioA&) = delete;
			AndroidAudioA& operator=(const AndroidAudioA&) = delete;
			~AndroidAudioA();
			void SetMasterVolume(heph_float volume) override;
			heph_float GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;
#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL
		private:
			void RenderData();
			void CaptureData();
			heph_float GetFinalAOVolume(AudioObject* pAudioObject) const override;
#endif
		};
	}
}
#endif