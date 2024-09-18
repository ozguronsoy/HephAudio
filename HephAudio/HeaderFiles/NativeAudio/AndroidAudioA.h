#pragma once
#if  defined(__ANDROID__) && (__ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL)
#include "HephAudioShared.h"
#include "AndroidAudioBase.h"
#include "Params/AAudioParams.h"
#include <aaudio/AAudio.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief uses AAudio.
		 * 
		 */
		class AndroidAudioA final : public AndroidAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;

		private:
			AAudioParams params;
			AAudioStream* pRenderStream;
			AAudioStream* pCaptureStream;
			size_t renderBufferFrameCount;
			size_t captureBufferFrameCount;
			double masterVolume;

		public:
			/**
			 * creates a new instance and initializes it with default values.
			 * 
			 */
			AndroidAudioA();

			AndroidAudioA(const AndroidAudioA&) = delete;
			AndroidAudioA& operator=(const AndroidAudioA&) = delete;

			/**
			 * releases the resources and destroys the instance.
			 * 
			 */
			~AndroidAudioA();

			void SetMasterVolume(double volume) override;
			double GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;

		private:
			double GetFinalAOVolume(AudioObject* pAudioObject) const override;
			static aaudio_data_callback_result_t RenderCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames);
			static aaudio_data_callback_result_t CaptureCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames);
			static void ErrorCallback(AAudioStream* stream, void* userData, aaudio_result_t error);
		};
	}
}
#endif