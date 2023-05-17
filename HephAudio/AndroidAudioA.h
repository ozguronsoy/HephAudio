#pragma once
#ifdef __ANDROID__
#include "HephAudioFramework.h"
#include "AndroidAudioBase.h"
#include <aaudio/AAudio.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Uses AAudio, min api target = 27. Use AndroidAudioSLES for api level 26 or lower (min 16) (uses OpenSL ES).
		/// </summary>
		class AndroidAudioA final : public AndroidAudioBase
		{
		private:
			AAudioStream* pRenderStream;
			AAudioStream* pCaptureStream;
			/// <summary>
			/// The number of frames the buffer AAudio uses for rendering audio samples consists of.
			/// </summary>
			size_t renderBufferFrameCount;
			/// <summary>
			/// The number of frames the buffer AAudio uses for capturing audio samples consists of.
			/// </summary>
			size_t captureBufferFrameCount;
			heph_float masterVolume;
		public:
			/// <summary>
			/// Creates and initalizes an AndroidAudioA instance.
			/// </summary>
			AndroidAudioA(JavaVM* jvm);
			AndroidAudioA(const AndroidAudioA&) = delete;
			AndroidAudioA& operator=(const AndroidAudioA&) = delete;
			/// <summary>
			/// Frees the AAudio resources.
			/// </summary>
			~AndroidAudioA();
			void SetMasterVolume(heph_float volume) override;
			heph_float GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void SetDisplayName(HephCommon::StringBuffer displayName) override;
			void SetIconPath(HephCommon::StringBuffer iconPath) override;
		private:
			void RenderData();
			void CaptureData();
			heph_float GetFinalAOVolume(std::shared_ptr<AudioObject> pAudioObject) const override;
		};
	}
}
#endif