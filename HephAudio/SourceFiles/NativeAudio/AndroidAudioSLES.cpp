#ifdef __ANDROID__
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

			ANDROIDAUDIO_EXCPT(slCreateEngine(&audioEngineObject, 1, &engineOption, 0, nullptr, nullptr), this, "AndroidAudioSLES::AndroidAudioSLES", "An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->Realize(audioEngineObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::AndroidAudioSLES", "An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->GetInterface(audioEngineObject, SL_IID_ENGINE, &audioEngine), this, "AndroidAudioSLES::AndroidAudioSLES", "An error occurred whilst getting the audio engine interface.");

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
		void AndroidAudioSLES::SetMasterVolume(heph_float volume)
		{
			SLresult slres;
			ANDROIDAUDIO_EXCPT((*masterVolumeObject)->SetVolumeLevel(masterVolumeObject, 2000 * log10(abs(volume))), this, "AndroidAudioSLES::SetMasterVolume", "An error occurred whilst setting the master volume.");
		}
		heph_float AndroidAudioSLES::GetMasterVolume() const
		{
			SLresult slres;
			SLmillibel volume = 0;
			ANDROIDAUDIO_EXCPT((*masterVolumeObject)->GetVolumeLevel(masterVolumeObject, &volume), this, "AndroidAudioSLES::GetMasterVolume", "An error occurred whilst getting the master volume.");
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
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateOutputMix(audioEngine, &outputMixObject, 0, nullptr, nullptr), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst creating audio player.");
			outputMixLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
			outputMixLocator.outputMix = outputMixObject;
			dataSink.pLocator = &outputMixLocator;
			dataSink.pFormat = &pcmFormat;

			SLboolean audioPlayerBools[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			SLInterfaceID audioPlayerIIDs[3] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioPlayer(audioEngine, &audioPlayerObject, &dataSource, &dataSink, 3, audioPlayerIIDs, audioPlayerBools), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->Realize(audioPlayerObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_VOLUME, &masterVolumeObject), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst creating master volume object.");

			SetMasterVolume(1.0);

			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_PLAY, &audioPlayer), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst creating audio player.");
			SLBufferQueueItf bufferQueue;
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_BUFFERQUEUE, &bufferQueue), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst creating render buffer.");

			renderCallbackContext.bufferSize = renderFormat.ByteRate() * 0.03;
			renderCallbackContext.pDataBase = (SLint8*)malloc(renderCallbackContext.bufferSize);
			renderCallbackContext.pData = renderCallbackContext.pDataBase;

			if (renderCallbackContext.pDataBase == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AndroidAudioSLES::InitializeRender", "Insufficient memory."));
				return;
			}
			memset(renderCallbackContext.pDataBase, 0, renderCallbackContext.bufferSize);

			ANDROIDAUDIO_EXCPT((*bufferQueue)->RegisterCallback(bufferQueue, &AndroidAudioSLES::BufferQueueCallback, &renderCallbackContext), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst rendering data.");
			ANDROIDAUDIO_EXCPT((*bufferQueue)->Enqueue(bufferQueue, renderCallbackContext.pData, renderCallbackContext.bufferSize * 0.01), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst rendering data.");
			ANDROIDAUDIO_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING), this, "AndroidAudioSLES::InitializeRender", "An error occurred whilst rendering data.");

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
				ANDROIDAUDIO_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED), this, "AndroidAudioSLES::StopRendering", "An error occurred whilst rendering data.");
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
				free(renderCallbackContext.pDataBase);
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
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioRecorder(audioEngine, &audioRecorderObject, &dataSource, &dataSink, 2, audioRecorderIIDs, audioRecorderBools), this, "AndroidAudioSLES::InitializeCapture", "An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->Realize(audioRecorderObject, SL_BOOLEAN_FALSE), this, "AndroidAudioSLES::InitializeCapture", "An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->GetInterface(audioRecorderObject, SL_IID_RECORD, &audioRecorder), this, "AndroidAudioSLES::InitializeCapture", "An error occurred whilst initializing capture.");

			SLAndroidSimpleBufferQueueItf simpleBufferQueue;
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->GetInterface(audioRecorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &simpleBufferQueue), this, "AndroidAudioSLES::InitializeCapture", "An error occurred whilst creating capture buffer.");

			captureCallbackContext.bufferSize = captureFormat.ByteRate() * 0.03;
			captureCallbackContext.pDataBase = (SLint8*)malloc(captureCallbackContext.bufferSize);
			captureCallbackContext.pData = captureCallbackContext.pDataBase;

			if (captureCallbackContext.pDataBase == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AndroidAudioSLES::InitializeCapture", "Insufficient memory."));
				return;
			}
			memset(captureCallbackContext.pDataBase, 0, captureCallbackContext.bufferSize);

			ANDROIDAUDIO_EXCPT((*simpleBufferQueue)->RegisterCallback(simpleBufferQueue, &AndroidAudioSLES::RecordEventCallback, &captureCallbackContext), this, "AndroidAudioSLES::InitializeCapture", "An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*simpleBufferQueue)->Enqueue(simpleBufferQueue, captureCallbackContext.pData, captureCallbackContext.bufferSize * 0.01), this, "AndroidAudioSLES::InitializeCapture", "An error occurred whilst capturing data.");
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_RECORDING), this, "AndroidAudioSLES::InitializeCapture", "An error occurred whilst capturing data.");

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
				ANDROIDAUDIO_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_STOPPED), this, "AndroidAudioSLES::StopCapturing", "An error occurred whilst capturing data.");
				if (audioRecorderObject != nullptr)
				{
					(*audioRecorderObject)->Destroy(audioRecorderObject);
					audioRecorderObject = nullptr;
				}
				free(captureCallbackContext.pDataBase);
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioSLES::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			RAISE_AND_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioSLES::GetNativeParams", "Not implemented."));
		}
		void AndroidAudioSLES::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioSLES::SetNativeParams", "Not implemented."));
		}
		SLAndroidDataFormat_PCM_EX  AndroidAudioSLES::ToSLFormat(AudioFormatInfo& formatInfo)
		{
			formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;

			SLAndroidDataFormat_PCM_EX pcmFormat;
			pcmFormat.formatType = SL_DATAFORMAT_PCM;
			pcmFormat.sampleRate = formatInfo.sampleRate * 1000;
			pcmFormat.bitsPerSample = formatInfo.bitsPerSample;
			pcmFormat.containerSize = formatInfo.bitsPerSample;
			pcmFormat.numChannels = formatInfo.channelLayout.count;
			pcmFormat.channelMask = (SLuint32)formatInfo.channelLayout.mask;
			pcmFormat.endianness = HEPH_SYSTEM_ENDIAN == Endian::Little ? SL_BYTEORDER_LITTLEENDIAN : SL_BYTEORDER_BIGENDIAN;
			pcmFormat.representation = formatInfo.bitsPerSample == 8 ? SL_ANDROID_PCM_REPRESENTATION_UNSIGNED_INT : SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
			return pcmFormat;
		}
		void AndroidAudioSLES::BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->isRenderInitialized)
			{
				const uint32_t renderCallbackFrameCount = pCallbackContext->bufferSize * 0.01 / pCallbackContext->pAndroidAudio->renderFormat.FrameSize();

				AudioBuffer dataBuffer(renderCallbackFrameCount, pCallbackContext->pAndroidAudio->renderFormat);
				pCallbackContext->pAndroidAudio->Mix(dataBuffer, renderCallbackFrameCount);
				memcpy(pCallbackContext->pData, dataBuffer.Begin(), dataBuffer.Size());

				SLresult slres = (*bufferQueue)->Enqueue(bufferQueue, pCallbackContext->pData, dataBuffer.Size());
				if (slres != 0)
				{
					RAISE_HEPH_EXCEPTION(pCallbackContext->pAndroidAudio, HephException(slres, "AndroidAudioSLES", "An error occurred whilst rendering data.", "OpenSL ES", ""));
					return;
				}

				pCallbackContext->pData += dataBuffer.Size();
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->bufferSize)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
				}
			}
		}
		void AndroidAudioSLES::RecordEventCallback(SLAndroidSimpleBufferQueueItf simpleBufferQueue, void* pContext)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->isCaptureInitialized && pCallbackContext->pAndroidAudio->OnCapture)
			{
				const uint32_t captureCallbackSize = pCallbackContext->bufferSize * 0.01;
				const uint32_t captureCallbackFrameCount = captureCallbackSize / pCallbackContext->pAndroidAudio->captureFormat.FrameSize();

				AudioBuffer captureBuffer(captureCallbackFrameCount, pCallbackContext->pAndroidAudio->captureFormat);
				memcpy(captureBuffer.Begin(), pCallbackContext->pData, captureCallbackSize);

				AudioCaptureEventArgs captureEventArgs(pCallbackContext->pAndroidAudio, captureBuffer);
				pCallbackContext->pAndroidAudio->OnCapture(&captureEventArgs, nullptr);

				SLresult slres = (*simpleBufferQueue)->Enqueue(simpleBufferQueue, pCallbackContext->pData, captureCallbackSize);
				if (slres != 0)
				{
					RAISE_HEPH_EXCEPTION(pCallbackContext->pAndroidAudio, HephException(slres, "AndroidAudioSLES", "An error occurred whilst capturing data.", "OpenSL ES", ""));
					return;
				}

				pCallbackContext->pData += captureCallbackSize;
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->bufferSize)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
				}
			}
		}
	}
}
#endif