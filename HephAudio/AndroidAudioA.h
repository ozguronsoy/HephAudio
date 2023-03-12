#pragma once
#ifdef __ANDROID__
#include "framework.h"
#include "AndroidAudioBase.h"
#include <aaudio/AAudio.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Uses AAudio, min api target = 27. Use AndroidAudioSLES for api level 26 or lower (min 16) (uses OpenSL ES).
		/// </summary>
		class AndroidAudioA : public AndroidAudioBase
		{
		protected:
			/// <summary>
			/// Pointer to the AAudio stream rendering audio samples.
			/// </summary>
			AAudioStream* pRenderStream;
			/// <summary>
			/// Pointer to the AAudio stream capturing audio samples.
			/// </summary>
			AAudioStream* pCaptureStream;
			/// <summary>
			/// The number of frames the buffer AAudio uses for rendering audio samples consists of.
			/// </summary>
			size_t renderBufferFrameCount;
			/// <summary>
			/// The number of frames the buffer AAudio uses for capturing audio samples consists of.
			/// </summary>
			size_t captureBufferFrameCount;
			/// <summary>
			/// The master volume.
			/// </summary>
			hephaudio_float masterVolume;
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
			virtual ~AndroidAudioA();
			virtual void SetMasterVolume(hephaudio_float volume) override;
			virtual hephaudio_float GetMasterVolume() const override;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopRendering() override;
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopCapturing() override;
			virtual void SetDisplayName(StringBuffer displayName) override;
			virtual void SetIconPath(StringBuffer iconPath) override;
		protected:
			virtual void RenderData();
			virtual void CaptureData();
			virtual hephaudio_float GetFinalAOVolume(std::shared_ptr<AudioObject> pAudioObject) const override;
		};
	}
}
#endif