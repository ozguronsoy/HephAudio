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
		/// <summary>
		/// Uses OpenSL ES, min api target = 16. Use AndroidAudioA for api level 27 or greater (uses AAudio).
		/// </summary>
		class AndroidAudioSLES : public AndroidAudioBase
		{
		protected:
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
		protected:
			/// <summary>
			/// The object that provides access to the audio engine interface.
			/// </summary>
			SLObjectItf audioEngineObject;
			/// <summary>
			/// The interface for creating an audio player/recorder.
			/// </summary>
			SLEngineItf audioEngine;
			/// <summary>
			/// The object that provides access to the audio player interface.
			/// </summary>
			SLObjectItf audioPlayerObject;
			/// <summary>
			/// The interface that renders the audio samples.
			/// </summary>
			SLPlayItf audioPlayer;
			/// <summary>
			/// The interface that sets or gets the master volume of the application.
			/// </summary>
			SLVolumeItf masterVolumeObject;
			/// <summary>
			/// The object that provides access to the audio recorder interface.
			/// </summary>
			SLObjectItf audioRecorderObject;
			/// <summary>
			/// The interface that captures the audio samples.
			/// </summary>
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
			virtual ~AndroidAudioSLES();
			virtual void SetMasterVolume(hephaudio_float volume);
			virtual hephaudio_float GetMasterVolume() const;
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