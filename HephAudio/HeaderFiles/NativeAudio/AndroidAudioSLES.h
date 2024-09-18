#pragma once
#if defined(__ANDROID__) && __ANDROID_API__ >= HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL
#include "HephAudioShared.h"
#include "AndroidAudioBase.h"
#include "Params/OpenSLParams.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief uses OpenSL ES
		 * 
		 */
		class AndroidAudioSLES final : public AndroidAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;

		private:
			struct CallbackContext
			{
				AndroidAudioSLES* pAndroidAudio;
				SLint8* pData;
				SLuint32 index;
				SLuint32 bufferSize_byte;
				SLuint32 bufferSize_frame;
			};

		private:
			OpenSLParams params;
			SLObjectItf audioEngineObject;
			SLEngineItf audioEngine;
			SLObjectItf outputMixObject;
			SLObjectItf audioPlayerObject;
			SLPlayItf audioPlayer;
			SLVolumeItf masterVolumeObject;
			SLObjectItf audioRecorderObject;
			SLRecordItf audioRecorder;
			CallbackContext renderCallbackContext;
			CallbackContext captureCallbackContext;

		public:
			/** @copydoc default_constructor */
			AndroidAudioSLES();

			AndroidAudioSLES(const AndroidAudioSLES&) = delete;
			AndroidAudioSLES& operator=(const AndroidAudioSLES&) = delete;
			
			/** @copydoc destructor */
			~AndroidAudioSLES();

			void SetMasterVolume(double volume);
			double GetMasterVolume() const;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			void StopRendering();
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
			void StopCapturing();
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;

		private:
			SLAndroidDataFormat_PCM_EX ToSLFormat(AudioFormatInfo& formatInfo);
			static void BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext);
			static void RecordEventCallback(SLAndroidSimpleBufferQueueItf simpleBufferQueue, void* pContext);
		};
	}
}
#endif