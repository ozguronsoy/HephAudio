#if defined(__ANDROID__) && __ANDROID_API__ >= HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL
#include "NativeAudio/AndroidAudioSLES.h"
#include "File.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"

#define ANDROIDAUDIO_EXCPT(sr, androidAudio, method, message) slres = sr; if(slres != 0) { RAISE_AND_THROW_HEPH_EXCEPTION(androidAudio, HephException(slres, method, message, "OpenSL ES", "")); }

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioSLES::AndroidAudioSLES() : AndroidAudioBase()
			, audioEngineObject(nullptr), audioEngine(nullptr), outputMixObject(nullptr), audioPlayerObject(nullptr), audioPlayer(nullptr)
			, audioRecorderObject(nullptr), audioRecorder(nullptr), masterVolumeObject(nullptr)
		{
			if (deviceApiLevel < HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AndroidAudioSLES::AndroidAudioSLES", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL) + "."));
			}

			renderCallbackContext.pAndroidAudio = this;
			captureCallbackContext.pAndroidAudio = this;

			SLresult slres;
			SLEngineOption engineOption;
			engineOption.feature = SL_ENGINEOPTION_THREADSAFE;
			engineOption.data = SL_BOOLEAN_TRUE;

			ANDROIDAUDIO_EXCPT(slCreateEngine(&audioEngineObject, 1, &engineOption, 0, nullptr, nullptr), this, "AndroidAudioSLES::AndroidAudioSLES", "An error occurred while creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->Realize(audioEngineObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::AndroidAudioSLES", "An error occurred while creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->GetInterface(audioEngineObject, SL_IID_ENGINE, &audioEngine), this, "AndroidAudioSLES::AndroidAudioSLES", "An error occurred while getting the audio engine interface.");

			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&AndroidAudioSLES::CheckAudioDevices, this);
		}
		AndroidAudioSLES::~AndroidAudioSLES()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing AndroidAudioSLES...", HEPH_CL_INFO);

			disposing = true;
			StopRendering();
			StopCapturing();

			(*audioEngineObject)->Destroy(audioEngineObject);

			HEPHAUDIO_LOG("AndroidAudioSLES destructed in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioSLES::SetMasterVolume(double volume)
		{
			SLresult slres;
			ANDROIDAUDIO_EXCPT((*masterVolumeObject)->SetVolumeLevel(masterVolumeObject, 2000 * log10(abs(volume))), this, "AndroidAudioSLES::SetMasterVolume", "An error occurred while setting the master volume.");
		}
		double AndroidAudioSLES::GetMasterVolume() const
		{
			SLresult slres;
			SLmillibel volume = 0;
			ANDROIDAUDIO_EXCPT((*masterVolumeObject)->GetVolumeLevel(masterVolumeObject, &volume), this, "AndroidAudioSLES::GetMasterVolume", "An error occurred while getting the master volume.");
			return pow(10.0, volume * 0.0005);
		}
		void AndroidAudioSLES::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			StopRendering();

			SLDataSource dataSource;
			SLAndroidDataFormat_PCM_EX pcmFormat = ToSLFormat(format);
			renderFormat = format;

			SLDataLocator_BufferQueue bufferQueueLocator;
			bufferQueueLocator.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
			bufferQueueLocator.numBuffers = 1;
			dataSource.pLocator = &bufferQueueLocator;
			dataSource.pFormat = &pcmFormat;

			SLDataSink dataSink;
			SLDataLocator_OutputMix outputMixLocator;
			SLresult slres;
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateOutputMix(audioEngine, &outputMixObject, 0, nullptr, nullptr), this, "AndroidAudioSLES::InitializeRender", "An error occurred while creating audio player.");
			ANDROIDAUDIO_EXCPT((*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::InitializeRender", "An error occurred while creating audio player.");
			outputMixLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
			outputMixLocator.outputMix = outputMixObject;
			dataSink.pLocator = &outputMixLocator;
			dataSink.pFormat = &pcmFormat;

			SLboolean audioPlayerBools[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			SLInterfaceID audioPlayerIIDs[3] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioPlayer(audioEngine, &audioPlayerObject, &dataSource, &dataSink, 3, audioPlayerIIDs, audioPlayerBools), this, "AndroidAudioSLES::InitializeRender", "An error occurred while creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->Realize(audioPlayerObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::InitializeRender", "An error occurred while creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_VOLUME, &masterVolumeObject), this, "AndroidAudioSLES::InitializeRender", "An error occurred while creating master volume object.");

			SetMasterVolume(1.0);

			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_PLAY, &audioPlayer), this, "AndroidAudioSLES::InitializeRender", "An error occurred while creating audio player.");
			SLBufferQueueItf bufferQueue;
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_BUFFERQUEUE, &bufferQueue), this, "AndroidAudioSLES::InitializeRender", "An error occurred while creating render buffer.");

			renderCallbackContext.bufferSize_frame = this->renderFormat.sampleRate * (this->params.renderBufferDuration_ms * 2 * 1e-3);
			renderCallbackContext.bufferSize_byte = renderCallbackContext.bufferSize_frame * this->renderFormat.FrameSize();
			renderCallbackContext.pData = (SLint8*)malloc(renderCallbackContext.bufferSize_byte);
			renderCallbackContext.index = 0;

			if (renderCallbackContext.pData == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AndroidAudioSLES::InitializeRender", "Insufficient memory."));
				return;
			}
			(void)memset(renderCallbackContext.pData, 0, renderCallbackContext.bufferSize_byte);

			ANDROIDAUDIO_EXCPT((*bufferQueue)->RegisterCallback(bufferQueue, &AndroidAudioSLES::BufferQueueCallback, &renderCallbackContext), this, "AndroidAudioSLES::InitializeRender", "An error occurred while rendering data.");
			ANDROIDAUDIO_EXCPT((*bufferQueue)->Enqueue(bufferQueue, renderCallbackContext.pData, renderCallbackContext.bufferSize_byte), this, "AndroidAudioSLES::InitializeRender", "An error occurred while rendering data.");
			ANDROIDAUDIO_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING), this, "AndroidAudioSLES::InitializeRender", "An error occurred while rendering data.");

			isRenderInitialized = true;

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioSLES::StopRendering()
		{
			if (isRenderInitialized)
			{
				SLresult slres;
				isRenderInitialized = false;
				renderDeviceId = "";
				ANDROIDAUDIO_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED), this, "AndroidAudioSLES::StopRendering", "An error occurred while rendering data.");
				if (audioPlayerObject != nullptr)
				{
					(*audioPlayerObject)->Destroy(audioPlayerObject);
					audioPlayerObject = nullptr;
				}
				if (outputMixObject != nullptr)
				{
					(*outputMixObject)->Destroy(outputMixObject);
					outputMixObject = nullptr;
				}
				free(renderCallbackContext.pData);
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioSLES::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			StopCapturing();

			SLDataLocator_IODevice deviceLocator;
			deviceLocator.locatorType = SL_DATALOCATOR_IODEVICE;
			deviceLocator.deviceType = SL_IODEVICE_AUDIOINPUT;
			deviceLocator.deviceID = SL_DEFAULTDEVICEID_AUDIOINPUT;
			deviceLocator.device = nullptr;

			SLDataSource dataSource;
			dataSource.pLocator = &deviceLocator;
			dataSource.pFormat = nullptr;

			SLAndroidDataFormat_PCM_EX pcmFormat = ToSLFormat(format);
			captureFormat = format;

			SLDataSink dataSink;
			SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueueLocator;
			simpleBufferQueueLocator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
			simpleBufferQueueLocator.numBuffers = 1;
			dataSink.pLocator = &simpleBufferQueueLocator;
			dataSink.pFormat = &pcmFormat;

			SLInterfaceID audioRecorderIIDs[2] = { SL_IID_RECORD, SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
			SLboolean audioRecorderBools[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			SLresult slres;
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioRecorder(audioEngine, &audioRecorderObject, &dataSource, &dataSink, 2, audioRecorderIIDs, audioRecorderBools), this, "AndroidAudioSLES::InitializeCapture", "An error occurred while initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->Realize(audioRecorderObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::InitializeCapture", "An error occurred while initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->GetInterface(audioRecorderObject, SL_IID_RECORD, &audioRecorder), this, "AndroidAudioSLES::InitializeCapture", "An error occurred while initializing capture.");

			SLAndroidSimpleBufferQueueItf simpleBufferQueue;
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->GetInterface(audioRecorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &simpleBufferQueue), this, "AndroidAudioSLES::InitializeCapture", "An error occurred while creating capture buffer.");

			captureCallbackContext.bufferSize_frame = this->captureFormat.sampleRate * (this->params.captureBufferDuration_ms * 2 * 1e-3);
			captureCallbackContext.bufferSize_byte = captureCallbackContext.bufferSize_frame * this->captureFormat.FrameSize();
			captureCallbackContext.pData = (SLint8*)malloc(captureCallbackContext.bufferSize_byte);
			captureCallbackContext.index = 0;

			if (captureCallbackContext.pData == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AndroidAudioSLES::InitializeCapture", "Insufficient memory."));
				return;
			}
			(void)memset(captureCallbackContext.pData, 0, captureCallbackContext.bufferSize_byte);

			ANDROIDAUDIO_EXCPT((*simpleBufferQueue)->RegisterCallback(simpleBufferQueue, &AndroidAudioSLES::RecordEventCallback, &captureCallbackContext), this, "AndroidAudioSLES::InitializeCapture", "An error occurred while initializing capture.");
			ANDROIDAUDIO_EXCPT((*simpleBufferQueue)->Enqueue(simpleBufferQueue, captureCallbackContext.pData, captureCallbackContext.bufferSize_byte), this, "AndroidAudioSLES::InitializeCapture", "An error occurred while capturing data.");
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_RECORDING), this, "AndroidAudioSLES::InitializeCapture", "An error occurred while capturing data.");

			isCaptureInitialized = true;

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioSLES::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				SLresult slres;
				isCaptureInitialized = false;
				captureDeviceId = "";
				ANDROIDAUDIO_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_STOPPED), this, "AndroidAudioSLES::StopCapturing", "An error occurred while capturing data.");
				if (audioRecorderObject != nullptr)
				{
					(*audioRecorderObject)->Destroy(audioRecorderObject);
					audioRecorderObject = nullptr;
				}
				free(captureCallbackContext.pData);
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioSLES::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			OpenSLParams* pOpenSLParams = dynamic_cast<OpenSLParams*>(&nativeParams);
			if (pOpenSLParams == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AndroidAudioSLES::GetNativeParams", "nativeParams must be a OpenSLParams instance."));
				return;
			}
			(*pOpenSLParams) = this->params;
		}
		void AndroidAudioSLES::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			const OpenSLParams* pOpenSLParams = dynamic_cast<const OpenSLParams*>(&nativeParams);
			if (pOpenSLParams == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AndroidAudioSLES::SetNativeParams", "nativeParams must be a OpenSLParams instance."));
				return;
			}
			this->params = *pOpenSLParams;
		}
		SLAndroidDataFormat_PCM_EX  AndroidAudioSLES::ToSLFormat(AudioFormatInfo& formatInfo)
		{
			SLAndroidDataFormat_PCM_EX pcmFormat;
			pcmFormat.formatType = SL_DATAFORMAT_PCM;
			pcmFormat.sampleRate = formatInfo.sampleRate * 1000;
			pcmFormat.bitsPerSample = formatInfo.bitsPerSample;
			pcmFormat.containerSize = formatInfo.bitsPerSample;
			pcmFormat.numChannels = formatInfo.channelLayout.count;
			pcmFormat.channelMask = (SLuint32)formatInfo.channelLayout.mask;
			pcmFormat.endianness = HEPH_SYSTEM_ENDIAN == Endian::Little ? SL_BYTEORDER_LITTLEENDIAN : SL_BYTEORDER_BIGENDIAN;

			if (formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT)
			{
				pcmFormat.formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
				pcmFormat.representation = SL_ANDROID_PCM_REPRESENTATION_FLOAT;
			}
			else
			{
				pcmFormat.representation = formatInfo.bitsPerSample == 8 ? SL_ANDROID_PCM_REPRESENTATION_UNSIGNED_INT : SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
				formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;
			}

			return pcmFormat;
		}
		void AndroidAudioSLES::BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->isRenderInitialized)
			{
				const size_t frameCount = pCallbackContext->bufferSize_frame / 2;
				AudioBuffer dataBuffer(frameCount, pCallbackContext->pAndroidAudio->renderFormat);
				const size_t bufferSize = dataBuffer.Size();

				pCallbackContext->pAndroidAudio->Mix(dataBuffer, frameCount);
				(void)memcpy(pCallbackContext->pData + pCallbackContext->index, dataBuffer.Begin(), bufferSize);
				pCallbackContext->index = (pCallbackContext->index + bufferSize) % pCallbackContext->bufferSize_byte;

				SLresult slres = (*bufferQueue)->Enqueue(bufferQueue, pCallbackContext->pData + pCallbackContext->index, bufferSize);
				if (slres != 0)
				{
					RAISE_HEPH_EXCEPTION(pCallbackContext->pAndroidAudio, HephException(slres, "AndroidAudioSLES", "An error occurred while rendering data.", "OpenSL ES", ""));
					(void)(*pCallbackContext->pAndroidAudio->audioPlayer)->SetPlayState(pCallbackContext->pAndroidAudio->audioPlayer, SL_PLAYSTATE_STOPPED);
				}
			}
		}
		void AndroidAudioSLES::RecordEventCallback(SLAndroidSimpleBufferQueueItf simpleBufferQueue, void* pContext)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->isCaptureInitialized && pCallbackContext->pAndroidAudio->OnCapture)
			{
				const size_t frameCount = pCallbackContext->bufferSize_frame / 2;
				AudioBuffer captureBuffer(frameCount, pCallbackContext->pAndroidAudio->captureFormat);
				const size_t bufferSize = captureBuffer.Size();

				(void)memcpy(captureBuffer.Begin(), pCallbackContext->pData + pCallbackContext->index, bufferSize);

				AudioCaptureEventArgs captureEventArgs(pCallbackContext->pAndroidAudio, captureBuffer);
				pCallbackContext->pAndroidAudio->OnCapture(&captureEventArgs, nullptr);

				pCallbackContext->index = (pCallbackContext->index + bufferSize) % pCallbackContext->bufferSize_byte;

				SLresult slres = (*simpleBufferQueue)->Enqueue(simpleBufferQueue, pCallbackContext->pData + pCallbackContext->index, bufferSize);
				if (slres != 0)
				{
					RAISE_HEPH_EXCEPTION(pCallbackContext->pAndroidAudio, HephException(slres, "AndroidAudioSLES", "An error occurred while capturing data.", "OpenSL ES", ""));
					(void)(*pCallbackContext->pAndroidAudio->audioRecorder)->SetRecordState(pCallbackContext->pAndroidAudio->audioRecorder, SL_RECORDSTATE_STOPPED);
				}
			}
		}
	}
}
#endif