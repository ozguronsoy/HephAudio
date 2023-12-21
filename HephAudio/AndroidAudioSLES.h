#pragma once
#ifdef __ANDROID__
#include "HephAudioFramework.h"
#include "AndroidAudioBase.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Uses OpenSL ES, min api target = 21. Use AndroidAudioA for api level 27 or greater (uses AAudio).
		/// Add -lOpenSLES to the compiler flags.
		/// </summary>
		class AndroidAudioSLES final : public AndroidAudioBase
		{
		private:
			struct CallbackContext
			{
				AndroidAudioSLES* pAndroidAudio;
				SLint8* pDataBase;
				SLint8* pData;
				SLuint32 bufferSize;
			};
		private:
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
			AndroidAudioSLES(JavaVM* jvm);
			AndroidAudioSLES(const AndroidAudioSLES&) = delete;
			AndroidAudioSLES& operator=(const AndroidAudioSLES&) = delete;
			~AndroidAudioSLES();
			void SetMasterVolume(heph_float volume);
			heph_float GetMasterVolume() const;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			void StopRendering();
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
			void StopCapturing();
			void SetDisplayName(HephCommon::StringBuffer displayName);
			void SetIconPath(HephCommon::StringBuffer iconPath);
		private:
			SLAndroidDataFormat_PCM_EX ToSLFormat(AudioFormatInfo& formatInfo);
			static void BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext);
			static void RecordEventCallback(SLAndroidSimpleBufferQueueItf simpleBufferQueue, void* pContext);
		};
	}
}
#endif