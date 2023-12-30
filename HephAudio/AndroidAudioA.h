#pragma once
#ifdef __ANDROID__
#include "HephAudioFramework.h"
#include "AndroidAudioBase.h"
#include <aaudio/AAudio.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses AAudio, min api target = 27. Use AndroidAudioSLES for api level 26 or lower (min 21) (uses OpenSL ES).
		// Add -laaudio to the compiler flags.
		class AndroidAudioA final : public AndroidAudioBase
		{
		private:
			AAudioStream* pRenderStream;
			AAudioStream* pCaptureStream;
			size_t renderBufferFrameCount;
			size_t captureBufferFrameCount;
			heph_float masterVolume;
		public:
			AndroidAudioA(JavaVM* jvm);
			AndroidAudioA(const AndroidAudioA&) = delete;
			AndroidAudioA& operator=(const AndroidAudioA&) = delete;
			~AndroidAudioA();
			void SetMasterVolume(heph_float volume) override;
			heph_float GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
		private:
			void RenderData();
			void CaptureData();
			heph_float GetFinalAOVolume(AudioObject* pAudioObject) const override;
		};
	}
}
#endif