#pragma once
#ifdef __ANDROID__
#include "framework.h"
#include "INativeAudio.h"
#include <SLES/OpenSLES.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses OpenSL ES, min api target = 14. Use AndroidAudioA for api level 26 or greater (uses AAudio).
		class AndroidAudio : public INativeAudio
		{
		protected:
			typedef struct CallbackContext {
				AndroidAudio* pAndroidAudio;
				SLint8* pDataBase; // Base adress of local audio data storage
				SLint8* pData; // Current adress of local audio data storage
				SLuint32 size;
			};
		protected:
			SLObjectItf audioEngineObject;
			SLEngineItf audioEngine;
			SLAudioIODeviceCapabilitiesItf audioDeviceCaps;
			SLObjectItf audioPlayerObject;
			SLPlayItf audioPlayer;
			SLObjectItf audioRecorderObject;
			SLRecordItf audioRecorder;
			double masterVolume;
			std::vector<AudioDevice> audioDevices;
			std::thread deviceThread;
			std::wstring renderDeviceId;
			std::wstring captureDeviceId;
		public:
			AndroidAudio();
			AndroidAudio(const AndroidAudio&) = delete;
			AndroidAudio& operator=(const AndroidAudio&) = delete;
			virtual ~AndroidAudio();
			virtual void SetMasterVolume(double volume);
			virtual double GetMasterVolume() const;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			virtual void StopRendering();
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format);
			virtual void StopCapturing();
			virtual void SetDisplayName(std::wstring displayName);
			virtual void SetIconPath(std::wstring iconPath);
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const;
		protected:
			virtual void JoinDeviceThread();
			virtual void RenderData(SLBufferQueueItf bufferQueue);
			virtual void CaptureData(void* dataBuffer);
			virtual double GetFinalAOVolume(std::shared_ptr<IAudioObject> audioObject) const;
			virtual void EnumerateAudioDevices();
			virtual void EnumerateRenderDevices();
			virtual void EnumerateCaptureDevices();
			static void BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext);
			static void RecordEventCallback(SLRecordItf audioRecorder, void* pContext, SLuint32 e);
		};
	}
}
#endif