#ifdef __ANDROID__
#include "AndroidAudioSLES.h"
#include "AudioProcessor.h"

#define ANDROIDAUDIO_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); throw AudioException(slres, method, message); }
#define ANDROIDAUDIO_RENDER_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); goto RENDER_EXIT; }
#define ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); goto CAPTURE_EXIT; }

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioSLES::AndroidAudioSLES(JNIEnv* env) : AndroidAudioBase(env)
		{
#if __ANDROID_API__ < 9
			throw AudioException(E_FAIL, L"AndroidAudioSLES::AndroidAudioSLES", L"The minimum supported Api level is 9.");
#endif
			audioEngineObject = nullptr;
			audioEngine = nullptr;
			audioPlayerObject = nullptr;
			audioPlayer = nullptr;
			audioRecorderObject = nullptr;
			audioRecorder = nullptr;
			masterVolumeObject = nullptr;
			renderBufferSize = 0;
			captureBufferSize = 0;
			SLEngineOption engineOption;
			engineOption.feature = SL_ENGINEOPTION_THREADSAFE;
			engineOption.data = SL_BOOLEAN_TRUE;
			ANDROIDAUDIO_EXCPT(slCreateEngine(&audioEngineObject, 1, &engineOption, 0, nullptr, nullptr), this, L"AndroidAudioSLES::AndroidAudioSLES", L"An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->Realize(audioEngineObject, SL_BOOLEAN_FALSE), this, L"AndroidAudioSLES::AndroidAudioSLES", L"An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->GetInterface(audioEngineObject, SL_IID_ENGINE, &audioEngine), this, L"AndroidAudioSLES::AndroidAudioSLES", L"An error occurred whilst getting the audio engine interface.");
		}
		AndroidAudioSLES::~AndroidAudioSLES()
		{
			disposing = true;
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();
			(*audioEngineObject)->Destroy(audioEngineObject);
		}
		void AndroidAudioSLES::SetMasterVolume(double volume)
		{
			ANDROIDAUDIO_EXCPT((*masterVolumeObject)->SetVolumeLevel(masterVolumeObject, 2000 * log10(abs(volume))), this, L"AndroidAudioSLES::SetMasterVolume", L"An error occurred whilst setting the master volume.");
		}
		double AndroidAudioSLES::GetMasterVolume() const
		{
			SLmillibel volume = 0;
			ANDROIDAUDIO_EXCPT((*masterVolumeObject)->GetVolumeLevel(masterVolumeObject, &volume), this, L"AndroidAudioSLES::GetMasterVolume", L"An error occurred whilst getting the master volume.");
			return pow(10.0, volume * 0.0005);
		}
		void AndroidAudioSLES::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			StopRendering();
			SLDataSource dataSource;
			SLAndroidDataFormat_PCM_EX pcmFormat = ToSLFormat(format);
			renderFormat = format;
			SLDataLocator_BufferQueue bufferQueueLocator;
			bufferQueueLocator.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
			bufferQueueLocator.numBuffers = 1;
			dataSource.pLocator = &bufferQueueLocator;
			dataSource.pFormat = &pcmFormat;
			renderBufferSize = renderFormat.ByteRate();
			SLDataSink dataSink;
			SLObjectItf outputMixObject;
			SLDataLocator_OutputMix outputMixLocator;
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateOutputMix(audioEngine, &outputMixObject, 0, nullptr, nullptr), this, L"AndroidAudioSLES::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE), this, L"AndroidAudioSLES::InitializeRender", L"An error occurred whilst creating audio player.");
			outputMixLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
			outputMixLocator.outputMix = outputMixObject;
			dataSink.pLocator = &outputMixLocator;
			dataSink.pFormat = &pcmFormat;
			SLboolean audioPlayerBools[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			SLInterfaceID audioPlayerIIDs[3] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioPlayer(audioEngine, &audioPlayerObject, &dataSource, &dataSink, 3, audioPlayerIIDs, audioPlayerBools), this, L"AndroidAudioSLES::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->Realize(audioPlayerObject, SL_BOOLEAN_FALSE), this, L"AndroidAudioSLES::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_VOLUME, &masterVolumeObject), this, L"AndroidAudioSLES::InitializeRender", L"An error occurred whilst creating master volume object.");
			SetMasterVolume(1.0);
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_PLAY, &audioPlayer), this, L"AndroidAudioSLES::InitializeRender", L"An error occurred whilst creating audio player.");
			SLBufferQueueItf bufferQueue;
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_BUFFERQUEUE, &bufferQueue), this, L"AndroidAudioSLES::InitializeRender", L"An error occurred whilst creating render buffer.");
			isRenderInitialized = true;
			renderThread = std::thread(&AndroidAudioSLES::RenderData, this, bufferQueue);
		}
		void AndroidAudioSLES::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = L"";
				JoinRenderThread();
				if (audioPlayerObject != nullptr)
				{
					(*audioPlayerObject)->Destroy(audioPlayerObject);
				}
			}
		}
		void AndroidAudioSLES::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			throw AudioException(E_FAIL, L"AndroidAudioSLES::AndroidAudioSLES", L"AndroidAudioSLES does not support this method, use AndroidAudio instead.");
			StopCapturing();
			SLDataLocator_IODevice deviceLocator;
			deviceLocator.locatorType = SL_DATALOCATOR_IODEVICE;
			deviceLocator.deviceType = SL_IODEVICE_AUDIOINPUT;
			deviceLocator.deviceID = SL_DEFAULTDEVICEID_AUDIOINPUT;
			deviceLocator.device = nullptr;
			SLDataSource dataSource;
			dataSource.pLocator = &deviceLocator;
			dataSource.pFormat = nullptr;
			SLDataSink dataSink;
			SLAndroidDataFormat_PCM_EX pcmFormat = ToSLFormat(format);
			captureFormat = format;
			captureBufferSize = captureFormat.ByteRate() * 0.01;
			void* dataBuffer = malloc(captureBufferSize);
			if (dataBuffer == nullptr)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, L"AndroidAudioSLES::InitializeCapture", L"Insufficient memory."));
				return;
			}
			SLDataLocator_Address addressLocator;
			addressLocator.locatorType = SL_DATALOCATOR_ADDRESS;
			addressLocator.pAddress = dataBuffer;
			addressLocator.length = captureBufferSize;
			dataSink.pLocator = &addressLocator;
			dataSink.pFormat = &pcmFormat;
			SLInterfaceID recId = SL_IID_RECORD;
			SLboolean audioRecorderTrue = SL_BOOLEAN_TRUE;
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioRecorder(audioEngine, &audioRecorderObject, &dataSource, &dataSink, 1, &recId, &audioRecorderTrue), this, L"AndroidAudioSLES::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->Realize(audioRecorderObject, SL_BOOLEAN_FALSE), this, L"AndroidAudioSLES::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->GetInterface(audioRecorderObject, SL_IID_RECORD, &audioRecorder), this, L"AndroidAudioSLES::InitializeCapture", L"An error occurred whilst initializing capture.");
			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)dataBuffer;
			context.pData = context.pDataBase;
			context.size = captureBufferSize;
			ANDROIDAUDIO_EXCPT((*audioRecorder)->RegisterCallback(audioRecorder, &AndroidAudioSLES::RecordEventCallback, &context), this, L"AndroidAudioSLES::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetPositionUpdatePeriod(audioRecorder, 200), this, L"AndroidAudioSLES::InitializeCapture", L"An error occurred whilst initializing capture."); // update callback every 200 ms.
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetCallbackEventsMask(audioRecorder, SL_RECORDEVENT_HEADATNEWPOS), this, L"AndroidAudioSLES::InitializeCapture", L"An error occurred whilst initializing capture.");
			captureFormat = format;
			isCaptureInitialized = true;
			captureThread = std::thread(&AndroidAudioSLES::CaptureData, this, dataBuffer);
		}
		void AndroidAudioSLES::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = L"";
				JoinCaptureThread();
				if (audioRecorderObject != nullptr)
				{
					(*audioRecorderObject)->Destroy(audioRecorderObject);
				}
			}
		}
		void AndroidAudioSLES::SetDisplayName(std::wstring displayName)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudioSLES::SetDisplayName", L"AndroidAudioSLES does not support this method."));
		}
		void AndroidAudioSLES::SetIconPath(std::wstring iconPath)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudioSLES::SetIconPath", L"AndroidAudioSLES does not support this method."));
		}
		void AndroidAudioSLES::RenderData(SLBufferQueueItf bufferQueue)
		{
			void* audioBuffer = malloc(renderBufferSize);
			if (audioBuffer == nullptr)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, L"AndroidAudioSLES", L"Insufficient memory."));
				return;
			}
			memset(audioBuffer, 0, renderBufferSize);
			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)audioBuffer;
			context.pData = context.pDataBase;
			context.size = renderBufferSize;
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->RegisterCallback(bufferQueue, &AndroidAudioSLES::BufferQueueCallback, &context), this, L"AndroidAudioSLES", L"An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->Enqueue(bufferQueue, context.pData, renderBufferSize * 0.01), this, L"AndroidAudioSLES", L"An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING), this, L"AndroidAudioSLES", L"An error occurred whilst rendering data.");
			while (!disposing && isRenderInitialized);
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED), this, L"AndroidAudioSLES", L"An error occurred whilst rendering data.");
		RENDER_EXIT:
			free(audioBuffer);
		}
		void AndroidAudioSLES::CaptureData(void* dataBuffer)
		{
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_RECORDING), this, L"AndroidAudioSLES", L"An error occurred whilst capturing data.");
			while (!disposing && isCaptureInitialized);
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_STOPPED), this, L"AndroidAudioSLES", L"An error occurred whilst capturing data.");
		CAPTURE_EXIT:
			free(dataBuffer);
		}
		SLAndroidDataFormat_PCM_EX  AndroidAudioSLES::ToSLFormat(AudioFormatInfo& formatInfo)
		{
			SLAndroidDataFormat_PCM_EX pcmFormat;
			pcmFormat.formatType = SL_DATAFORMAT_PCM;
			pcmFormat.numChannels = formatInfo.channelCount;
			pcmFormat.sampleRate = formatInfo.sampleRate * 1000;
			pcmFormat.bitsPerSample = formatInfo.bitsPerSample;
			pcmFormat.containerSize = formatInfo.bitsPerSample;
			pcmFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
			pcmFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
			pcmFormat.representation = formatInfo.bitsPerSample == 8 ? SL_ANDROID_PCM_REPRESENTATION_UNSIGNED_INT : SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
			return pcmFormat;
		}
		void AndroidAudioSLES::BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr)
			{
				const uint32_t renderCallbackFrameCount = pCallbackContext->pAndroidAudio->renderBufferSize * 0.01 / pCallbackContext->pAndroidAudio->renderFormat.FrameSize();
				AudioBuffer dataBuffer(renderCallbackFrameCount, pCallbackContext->pAndroidAudio->renderFormat);
				pCallbackContext->pAndroidAudio->Mix(dataBuffer, renderCallbackFrameCount);
				memcpy(pCallbackContext->pData, dataBuffer.Begin(), dataBuffer.Size());
				SLresult slres = (*bufferQueue)->Enqueue(bufferQueue, pCallbackContext->pData, dataBuffer.Size());
				if (slres != 0)
				{
					RAISE_AUDIO_EXCPT(pCallbackContext->pAndroidAudio, AudioException(E_FAIL, L"AndroidAudioSLES", L"An error occurred whilst rendering data."));
					return;
				}
				pCallbackContext->pData += dataBuffer.Size();
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->pAndroidAudio->renderBufferSize)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
				}
			}
		}
		void AndroidAudioSLES::RecordEventCallback(SLRecordItf audioRecorder, void* pContext, SLuint32 e)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->OnCapture != nullptr)
			{
				const uint32_t captureCallbackSize = pCallbackContext->pAndroidAudio->captureBufferSize * 0.01;
				const uint32_t captureCallbackFrameCount = captureCallbackSize / pCallbackContext->pAndroidAudio->captureFormat.FrameSize();
				AudioBuffer captureBuffer(captureCallbackFrameCount, pCallbackContext->pAndroidAudio->captureFormat);
				memcpy(captureBuffer.Begin(), pCallbackContext->pData, captureCallbackSize);
				AudioProcessor::ConvertPcmToInnerFormat(captureBuffer);
				pCallbackContext->pAndroidAudio->OnCapture(captureBuffer);
				pCallbackContext->pData += captureCallbackSize;
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->pAndroidAudio->captureBufferSize - captureCallbackSize)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
					SLresult slres = (*audioRecorder)->SetMarkerPosition(audioRecorder, 0);
					if (slres != 0)
					{
						RAISE_AUDIO_EXCPT(pCallbackContext->pAndroidAudio, AudioException(E_FAIL, L"AndroidAudioSLES", L"An error occurred whilst capturing data."));
					}
				}
			}
		}
	}
}
#endif