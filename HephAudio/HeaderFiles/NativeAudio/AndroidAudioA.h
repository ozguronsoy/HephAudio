#pragma once
#ifdef __ANDROID__
#include "HephAudioShared.h"
#include "AndroidAudioBase.h"
#include "Params/AAudioParams.h"
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
			AAudioParams params;
			AAudioStream* pRenderStream;
			AAudioStream* pCaptureStream;
			size_t renderBufferFrameCount;
			size_t captureBufferFrameCount;
			double masterVolume;
#endif
		public:
			AndroidAudioA();
			AndroidAudioA(const AndroidAudioA&) = delete;
			AndroidAudioA& operator=(const AndroidAudioA&) = delete;
			~AndroidAudioA();
			void SetMasterVolume(double volume) override;
			double GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;
#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL
		private:
			double GetFinalAOVolume(AudioObject* pAudioObject) const override;
			static aaudio_data_callback_result_t RenderCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames);
			static aaudio_data_callback_result_t CaptureCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames);
			static void ErrorCallback(AAudioStream* stream, void* userData, aaudio_result_t error);
#endif
		};
	}
}
#endif