#ifdef __ANDROID__
#include "AndroidAudioSLES.h"
#include "AudioProcessor.h"
#include "File.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"

#define ANDROIDAUDIO_EXCPT(sr, androidAudio, method, message) slres = sr; if(slres != 0) { RAISE_AND_THROW_HEPH_EXCEPTION(androidAudio, HephException(slres, method, message)); }
#define ANDROIDAUDIO_RENDER_THREAD_EXCPT(sr, androidAudio, method, message) slres = sr; if(slres != 0) { RAISE_HEPH_EXCEPTION(androidAudio, HephException(slres, method, message)); goto RENDER_EXIT; }
#define ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(sr, androidAudio, method, message) slres = sr; if(slres != 0) { RAISE_HEPH_EXCEPTION(androidAudio, HephException(slres, method, message)); goto CAPTURE_EXIT; }

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioSLES::AndroidAudioSLES(JavaVM* jvm) : AndroidAudioBase(jvm)
			, audioEngineObject(nullptr), audioEngine(nullptr), audioPlayerObject(nullptr), audioPlayer(nullptr), audioRecorderObject(nullptr), audioRecorder(nullptr)
			, masterVolumeObject(nullptr), renderBufferSize(0), captureBufferSize(0)
		{
			if (deviceApiLevel < 16)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "AndroidAudioSLES::AndroidAudioSLES", "The minimum supported Api level is 16."));
			}

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
			HEPHAUDIO_LOG("Destructing AndroidAudioSLES...", ConsoleLogger::info);

			disposing = true;
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();
			StopRendering();
			StopCapturing();

			(*audioEngineObject)->Destroy(audioEngineObject);

			HEPHAUDIO_LOG("AndroidAudioSLES destructed in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
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
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), ConsoleLogger::info);

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
			SLObjectItf outputMixObject;
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

			isRenderInitialized = true;
			renderThread = std::thread(&AndroidAudioSLES::RenderData, this, bufferQueue);

			HEPHAUDIO_LOG("Render initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
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
				HEPHAUDIO_LOG("Stopped rendering.", ConsoleLogger::info);
			}
		}
		void AndroidAudioSLES::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), ConsoleLogger::info);

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

			isCaptureInitialized = true;
			captureThread = std::thread(&AndroidAudioSLES::CaptureData, this, simpleBufferQueue);

			HEPHAUDIO_LOG("Capture initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
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
				HEPHAUDIO_LOG("Stopped capturing.", ConsoleLogger::info);
			}
		}
		void AndroidAudioSLES::SetDisplayName(StringBuffer displayName)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "AndroidAudioSLES::SetDisplayName", "AndroidAudioSLES does not support this method."));
		}
		void AndroidAudioSLES::SetIconPath(StringBuffer iconPath)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "AndroidAudioSLES::SetIconPath", "AndroidAudioSLES does not support this method."));
		}
		void AndroidAudioSLES::RenderData(SLBufferQueueItf bufferQueue)
		{
			renderBufferSize = renderFormat.ByteRate();
			void* audioBuffer = malloc(renderBufferSize);
			if (audioBuffer == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "AndroidAudioSLES", "Insufficient memory."));
				return;
			}
			memset(audioBuffer, 0, renderBufferSize);

			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)audioBuffer;
			context.pData = context.pDataBase;
			context.size = renderBufferSize;

			SLresult slres;
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->RegisterCallback(bufferQueue, &AndroidAudioSLES::BufferQueueCallback, &context), this, "AndroidAudioSLES", "An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->Enqueue(bufferQueue, context.pData, renderBufferSize * 0.01), this, "AndroidAudioSLES", "An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING), this, "AndroidAudioSLES", "An error occurred whilst rendering data.");

			while (!disposing && isRenderInitialized);

			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED), this, "AndroidAudioSLES", "An error occurred whilst rendering data.");
		RENDER_EXIT:
			free(audioBuffer);
		}
		void AndroidAudioSLES::CaptureData(SLAndroidSimpleBufferQueueItf simpleBufferQueue)
		{
			captureBufferSize = captureFormat.ByteRate();
			void* audioBuffer = malloc(captureBufferSize);
			if (audioBuffer == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "AndroidAudioSLES::InitializeCapture", "Insufficient memory."));
				return;
			}
			memset(audioBuffer, 0, captureBufferSize);

			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)audioBuffer;
			context.pData = context.pDataBase;
			context.size = captureBufferSize;

			SLresult slres;
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*simpleBufferQueue)->RegisterCallback(simpleBufferQueue, &AndroidAudioSLES::RecordEventCallback, &context), this, "AndroidAudioSLES", "An error occurred whilst initializing capture.");
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*simpleBufferQueue)->Enqueue(simpleBufferQueue, context.pData, captureBufferSize * 0.01), this, "AndroidAudioSLES", "An error occurred whilst capturing data.");
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_RECORDING), this, "AndroidAudioSLES", "An error occurred whilst capturing data.");

			while (!disposing && isCaptureInitialized);

			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_STOPPED), this, "AndroidAudioSLES", "An error occurred whilst capturing data.");
		CAPTURE_EXIT:
			free(audioBuffer);
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
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->isRenderInitialized)
			{
				const uint32_t renderCallbackFrameCount = pCallbackContext->size * 0.01 / pCallbackContext->pAndroidAudio->renderFormat.FrameSize();

				AudioBuffer dataBuffer(renderCallbackFrameCount, pCallbackContext->pAndroidAudio->renderFormat);
				pCallbackContext->pAndroidAudio->Mix(dataBuffer, renderCallbackFrameCount);
				memcpy(pCallbackContext->pData, dataBuffer.Begin(), dataBuffer.Size());

				SLresult slres = (*bufferQueue)->Enqueue(bufferQueue, pCallbackContext->pData, dataBuffer.Size());
				if (slres != 0)
				{
					RAISE_HEPH_EXCEPTION(pCallbackContext->pAndroidAudio, HephException(slres, "AndroidAudioSLES", "An error occurred whilst rendering data."));
					return;
				}

				pCallbackContext->pData += dataBuffer.Size();
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->size)
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
				const uint32_t captureCallbackSize = pCallbackContext->size * 0.01;
				const uint32_t captureCallbackFrameCount = captureCallbackSize / pCallbackContext->pAndroidAudio->captureFormat.FrameSize();

				AudioBuffer captureBuffer(captureCallbackFrameCount, pCallbackContext->pAndroidAudio->captureFormat);
				memcpy(captureBuffer.Begin(), pCallbackContext->pData, captureCallbackSize);

				AudioProcessor::ConvertToInnerFormat(captureBuffer, File::GetSystemEndian());
				AudioCaptureEventArgs captureEventArgs = AudioCaptureEventArgs(pCallbackContext->pAndroidAudio, captureBuffer);
				pCallbackContext->pAndroidAudio->OnCapture(&captureEventArgs, nullptr);

				SLresult slres = (*simpleBufferQueue)->Enqueue(simpleBufferQueue, pCallbackContext->pData, captureCallbackSize);
				if (slres != 0)
				{
					RAISE_HEPH_EXCEPTION(pCallbackContext->pAndroidAudio, HephException(slres, "AndroidAudioSLES", "An error occurred whilst capturing data."));
					return;
				}

				pCallbackContext->pData += captureCallbackSize;
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->size)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
				}
			}
		}
	}
}
#endif