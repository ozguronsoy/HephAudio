#pragma once
#ifdef __ANDROID__
#include "framework.h"
#include "AndroidAudioBase.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses OpenSL ES, min api target = 16. Use AndroidAudioA for api level 27 or greater (uses AAudio).
		class AndroidAudioSLES : public AndroidAudioBase
		{
		protected:
			struct CallbackContext {
				AndroidAudioSLES* pAndroidAudio;
				SLint8* pDataBase; // Base address of local audio data storage
				SLint8* pData; // Current address of local audio data storage
				SLuint32 size;
			};
		protected:
			SLObjectItf audioEngineObject;
			SLEngineItf audioEngine;
			SLObjectItf audioPlayerObject;
			SLPlayItf audioPlayer;
			SLVolumeItf masterVolumeObject;
			SLObjectItf audioRecorderObject;
			SLRecordItf audioRecorder;
			uint32_t renderBufferSize;
			uint32_t captureBufferSize;
		public:
			AndroidAudioSLES(JavaVM* jvm);
			AndroidAudioSLES(const AndroidAudioSLES&) = delete;
			AndroidAudioSLES& operator=(const AndroidAudioSLES&) = delete;
			virtual ~AndroidAudioSLES();
			virtual void SetMasterVolume(HEPHAUDIO_DOUBLE volume);
			virtual HEPHAUDIO_DOUBLE GetMasterVolume() const;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			virtual void StopRendering();
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
			virtual void StopCapturing();
			virtual void SetDisplayName(StringBuffer displayName);
			virtual void SetIconPath(StringBuffer iconPath);
		protected:
			virtual void RenderData(SLBufferQueueItf bufferQueue);
			virtual void CaptureData(SLAndroidSimpleBufferQueueItf simpleBufferQueue);
			virtual SLAndroidDataFormat_PCM_EX ToSLFormat(AudioFormatInfo& formatInfo);
			static void BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext);
			static void RecordEventCallback(SLAndroidSimpleBufferQueueItf simpleBufferQueue, void* pContext);
		};
	}
}
#endif