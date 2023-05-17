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
		/// Uses OpenSL ES, min api target = 16. Use AndroidAudioA for api level 27 or greater (uses AAudio).
		/// </summary>
		class AndroidAudioSLES final : public AndroidAudioBase
		{
		private:
			/// <summary>
			/// Holds information to be used in a render or a capture callback.
			/// </summary>
			struct CallbackContext
			{
				/// <summary>
				/// Pointer to the AndroidAudioSLES instance that called the method.
				/// </summary>
				AndroidAudioSLES* pAndroidAudio;
				/// <summary>
				/// Pointer to the start of the render/capture buffer.
				/// </summary>
				SLint8* pDataBase;
				/// <summary>
				/// Pointer to the current write/read position of the render/capture buffer.
				/// </summary>
				SLint8* pData;
				/// <summary>
				/// Size of the render/capture buffer.
				/// </summary>
				SLuint32 size;
			};
		private:
			SLObjectItf audioEngineObject;
			SLEngineItf audioEngine;
			SLObjectItf audioPlayerObject;
			SLPlayItf audioPlayer;
			SLVolumeItf masterVolumeObject;
			SLObjectItf audioRecorderObject;
			SLRecordItf audioRecorder;
			/// <summary>
			/// The number of frames the buffer OpenSL ES uses for rendering audio samples consists of.
			/// </summary>
			uint32_t renderBufferSize;
			/// <summary>
			/// The number of frames the buffer OpenSL ES uses for capturing audio samples consists of.
			/// </summary>
			uint32_t captureBufferSize;
		public:
			/// <summary>
			/// Creates and initalizes an AndroidAudioSLES instance.
			/// </summary>
			AndroidAudioSLES(JavaVM* jvm);
			AndroidAudioSLES(const AndroidAudioSLES&) = delete;
			AndroidAudioSLES& operator=(const AndroidAudioSLES&) = delete;
			/// <summary>
			/// Frees the OpenSL ES resources.
			/// </summary>
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
			void RenderData(SLBufferQueueItf bufferQueue);
			void CaptureData(SLAndroidSimpleBufferQueueItf simpleBufferQueue);
			SLAndroidDataFormat_PCM_EX ToSLFormat(AudioFormatInfo& formatInfo);
			static void BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext);
			static void RecordEventCallback(SLAndroidSimpleBufferQueueItf simpleBufferQueue, void* pContext);
		};
	}
}
#endif