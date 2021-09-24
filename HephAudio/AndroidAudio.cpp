#ifdef __ANDROID__
#include "AndroidAudio.h"
#include "AudioProcessor.h"

#define RENDER_BUFFER_SIZE 352800 
#define RENDER_CALLBACK_SIZE_FRAMES 882
#define RENDER_CALLBACK_SIZE 3528
#define CAPTURE_BUFFER_SIZE RENDER_BUFFER_SIZE
#define CAPTURE_CALLBACK_SIZE_FRAMES RENDER_CALLBACK_SIZE_FRAMES
#define CAPTURE_CALLBACK_SIZE RENDER_CALLBACK_SIZE
#define ANDROIDAUDIO_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); throw AudioException(slres, method, message); }
#define ANDROIDAUDIO_RENDER_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); goto RENDER_EXIT; }
#define ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); goto CAPTURE_EXIT; }
#define ANDROIDAUDIO_DEVICE_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); }
#define DEFAULT_AUDIO_FORMAT AudioFormatInfo(1, 2, 16, 44100)

namespace HephAudio
{
	namespace Native
	{
		AndroidAudio::AndroidAudio() : INativeAudio()
		{
			if (__ANDROID_API__ < 21)
			{
				throw AudioException(E_FAIL, L"AndroidAudio::AndroidAudio", L"Api level must be 21 or greater.");
			}
			renderFormat = DEFAULT_AUDIO_FORMAT;
			masterVolume = 1.0;
			SLEngineOption engineOption;
			engineOption.feature = SL_ENGINEOPTION_THREADSAFE;
			engineOption.data = SL_BOOLEAN_TRUE;
			SLInterfaceID engineInterfaceIds[2]{ SL_IID_ENGINE, SL_IID_AUDIOIODEVICECAPABILITIES };
			SLboolean engineInterfaceBools[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			ANDROIDAUDIO_EXCPT(slCreateEngine(&audioEngineObject, 1, &engineOption, 2, engineInterfaceIds, engineInterfaceBools), this, L"AndroidAudio::AndroidAudio", L"An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->Realize(audioEngineObject, SL_BOOLEAN_FALSE), this, L"AndroidAudio::AndroidAudio", L"An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->GetInterface(audioEngineObject, SL_IID_ENGINE, &audioEngine), this, L"AndroidAudio::AndroidAudio", L"An error occurred whilst getting the audio engine interface.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->GetInterface(audioEngineObject, SL_IID_AUDIOIODEVICECAPABILITIES, &audioDeviceCaps), this, L"AndroidAudio::AndroidAudio", L"An error occurred whilst initializing capture.");
			EnumerateRenderDevices();
			EnumerateCaptureDevices();
			deviceThread = std::thread(&AndroidAudio::EnumerateAudioDevices, this);
			renderDeviceId = L"";
			captureDeviceId = L"";
		}
		AndroidAudio::~AndroidAudio()
		{
			disposing = true;
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();
			JoinDeviceThread();
			StopRendering();
			StopCapturing();
			(*audioEngineObject)->Destroy(audioEngineObject);
		}
		void AndroidAudio::SetMasterVolume(double volume) const
		{
			masterVolume = volume;
		}
		double AndroidAudio::GetMasterVolume() const
		{
			return masterVolume;
		}
		void AndroidAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			if (isRenderInitialized)
			{
				StopRendering();
			}
			SLuint32 selectedDeviceId = 0;
			if (device == nullptr)
			{
				AudioDevice defaultRenderDevice = GetDefaultAudioDevice(AudioDeviceType::Render);
				selectedDeviceId = std::stoul(defaultRenderDevice.id);
				renderDeviceId = defaultRenderDevice.id;
			}
			else
			{
				selectedDeviceId = std::stoul(device->id);
				renderDeviceId = device->id;
			}
			SLDataSource dataSource;
			SLDataFormat_PCM pcmFormat;
			pcmFormat.formatType = SL_DATAFORMAT_PCM;
			pcmFormat.numChannels = 2;
			pcmFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
			pcmFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
			pcmFormat.containerSize = 16;
			pcmFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
			pcmFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
			SLDataLocator_BufferQueue bufferQueueLocator;
			bufferQueueLocator.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
			bufferQueueLocator.numBuffers = 1;
			dataSource.pLocator = &bufferQueueLocator;
			dataSource.pFormat = &pcmFormat;
			SLDataSink dataSink;
			SLDataLocator_IODevice deviceLocator;
			deviceLocator.locatorType = SL_DATALOCATOR_IODEVICE;
			deviceLocator.deviceType = SL_IODEVICE_AUDIOINPUT;
			deviceLocator.deviceID = selectedDeviceId;
			deviceLocator.device = nullptr;
			dataSink.pLocator = &deviceLocator;
			dataSink.pFormat = &pcmFormat;
			SLboolean audioPlayerBools[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			SLInterfaceID audioPlayerIIDs[2] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE };
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioPlayer(audioEngine, &audioPlayerObject, &dataSource, &dataSink, 2, audioPlayerIIDs, audioPlayerBools), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->Realize(audioPlayerObject, SL_BOOLEAN_FALSE), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_PLAY, &audioPlayer), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			SLBufferQueueItf bufferQueue;
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_BUFFERQUEUE, &bufferQueue), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating render buffer.");
			isRenderInitialized = true;
			renderThread = std::thread(&AndroidAudio::RenderData, this, bufferQueue);
		}
		void AndroidAudio::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				JoinRenderThread();
				if (audioPlayerObject != nullptr)
				{
					(*audioPlayerObject)->Destroy(audioPlayerObject);
				}
			}
		}
		void AndroidAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			if (isCaptureInitialized)
			{
				StopCapturing();
			}
			SLuint32 selectedDeviceId = 0;
			if (device == nullptr)
			{
				AudioDevice defaultCaptureDevice = GetDefaultAudioDevice(AudioDeviceType::Capture);
				selectedDeviceId = std::stoul(defaultCaptureDevice.id);
				captureDeviceId = defaultCaptureDevice.id;
			}
			else
			{
				selectedDeviceId = std::stoul(device->id);
				captureDeviceId = device->id;
			}
			SLDataLocator_IODevice deviceLocator;
			deviceLocator.locatorType = SL_DATALOCATOR_IODEVICE;
			deviceLocator.deviceType = SL_IODEVICE_AUDIOINPUT;
			deviceLocator.deviceID = selectedDeviceId;
			deviceLocator.device = nullptr;
			SLDataSource dataSource;
			dataSource.pLocator = &deviceLocator;
			dataSource.pFormat = NULL;
			SLDataSink dataSink;
			SLDataFormat_PCM pcmFormat;
			pcmFormat.formatType = SL_DATAFORMAT_PCM;
			pcmFormat.numChannels = 2;
			pcmFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
			pcmFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
			pcmFormat.containerSize = 16;
			pcmFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
			pcmFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
			void* dataBuffer = malloc(CAPTURE_BUFFER_SIZE);
			if (dataBuffer == nullptr)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, L"AndroidAudio::InitializeCapture", L"Insufficient memory."));
				return;
			}
			SLDataLocator_Address addressLocator;
			addressLocator.locatorType = SL_DATALOCATOR_ADDRESS;
			addressLocator.pAddress = dataBuffer;
			addressLocator.length = CAPTURE_BUFFER_SIZE;
			dataSink.pLocator = &addressLocator;
			dataSink.pFormat = &pcmFormat;
			SLboolean audiorecordertrue = true;
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioRecorder(audioEngine, &audioRecorderObject, &dataSource, &dataSink, 1, &SL_IID_RECORD, &audiorecordertrue), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->Realize(audioRecorderObject, SL_BOOLEAN_FALSE), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->GetInterface(audioRecorderObject, SL_IID_RECORD, &audioRecorder), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)dataBuffer;
			context.pData = context.pDataBase;
			context.size = CAPTURE_BUFFER_SIZE;
			ANDROIDAUDIO_EXCPT((*audioRecorder)->RegisterCallback(audioRecorder, &AndroidAudio::RecordEventCallback, &context), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetPositionUpdatePeriod(audioRecorder, 200), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture."); // update callback every 200 ms.
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetCallbackEventsMask(audioRecorder, SL_RECORDEVENT_HEADATNEWPOS), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			captureFormat = format;
			isCaptureInitialized = true;
			captureThread = std::thread(&AndroidAudio::CaptureData, this, dataBuffer);
		}
		void AndroidAudio::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				JoinCaptureThread();
				if (audioRecorderObject != nullptr)
				{
					(*audioRecorderObject)->Destroy(audioRecorderObject);
				}
			}
		}
		void AndroidAudio::SetDisplayName(std::wstring displayName)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudio::SetDisplayName", L"AndroidAudio does not support this method."));
		}
		void AndroidAudio::SetIconPath(std::wstring iconPath)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudio::SetIconPath", L"AndroidAudio does not support this method."));
		}
		AudioDevice AndroidAudio::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			if (deviceType == AudioDeviceType::All || deviceType == AudioDeviceType::Null)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"AndroidAudio::GetDefaultAudioDevice", L"DeviceType must be either Render or Capture."));
				return AudioDevice();
			}
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (audioDevices.at(i).isDefault && audioDevices.at(i).type == deviceType)
				{
					return audioDevices.at(i);
				}
			}
			return AudioDevice();
		}
		std::vector<AudioDevice> AndroidAudio::GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const
		{
			std::vector<AudioDevice> result;
			if (deviceType == AudioDeviceType::Null)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"AndroidAudio::GetAudioDevices", L"DeviceType must not be Null."));
				return result;
			}
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (deviceType == AudioDeviceType::All || audioDevices.at(i).type == deviceType)
				{
					result.push_back(audioDevices.at(i));
				}
			}
			return result;
		}
		void AndroidAudio::JoinDeviceThread()
		{
			if (deviceThread.joinable())
			{
				deviceThread.join();
			}
		}
		void AndroidAudio::RenderData(SLBufferQueueItf bufferQueue)
		{
			void* audioBuffer = malloc(RENDER_BUFFER_SIZE);
			if (audioBuffer == nullptr)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, L"AndroidAudio", L"Insufficient memory."));
				return;
			}
			memset(audioBuffer, 0, RENDER_BUFFER_SIZE);
			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)audioBuffer;
			context.pData = context.pDataBase;
			context.size = RENDER_BUFFER_SIZE;
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->RegisterCallback(bufferQueue, &AndroidAudio::BufferQueueCallback, &context), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->Enqueue(bufferQueue, context.pData, RENDER_CALLBACK_SIZE), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
			while (!disposing && isRenderInitialized);
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
		RENDER_EXIT:
			free(audioBuffer);
		}
		void AndroidAudio::CaptureData(void* dataBuffer)
		{
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_RECORDING), this, L"AndroidAudio", L"An error occurred whilst capturing data.");
			while (!disposing && isCaptureInitialized);
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_STOPPED), this, L"AndroidAudio", L"An error occurred whilst capturing data.");
		CAPTURE_EXIT:
			free(dataBuffer);
		}
		double AndroidAudio::GetFinalAOVolume(std::shared_ptr<IAudioObject> audioObject) const
		{
			return INativeAudio::GetFinalAOVolume(audioObject) * masterVolume;
		}
		void AndroidAudio::EnumerateAudioDevices()
		{
			const uint32_t period = 1000; // In ms.
			auto start = std::chrono::high_resolution_clock::now();
			auto passedTime = std::chrono::milliseconds(0);
			while (!disposing)
			{
				passedTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - start);
				if (passedTime >= std::chrono::milliseconds(period))
				{
					start = std::chrono::high_resolution_clock::now();
					std::vector<AudioDevice> oldDevices = audioDevices;
					AudioDevice oldDefaultRender = GetDefaultAudioDevice(AudioDeviceType::Render);
					AudioDevice oldDefaultCapture = GetDefaultAudioDevice(AudioDeviceType::Capture);
					audioDevices.clear();
					EnumerateRenderDevices();
					EnumerateCaptureDevices();
					if (OnDefaultAudioDeviceChange != nullptr)
					{
						AudioDevice newDefaultRender = GetDefaultAudioDevice(AudioDeviceType::Render);
						AudioDevice newDefaultCapture = GetDefaultAudioDevice(AudioDeviceType::Capture);
						if (oldDefaultRender.id != newDefaultRender.id)
						{
							OnDefaultAudioDeviceChange(newDefaultRender);
						}
						if (oldDefaultCapture.id != newDefaultCapture.id)
						{
							OnDefaultAudioDeviceChange(newDefaultCapture);
						}
					}
					if (OnAudioDeviceAdded != nullptr)
					{
						for (size_t i = 0; i < audioDevices.size(); i++)
						{
							bool added = true;
							for (size_t j = 0; j < oldDevices.size(); j++)
							{
								if (audioDevices.at(i).id == oldDevices.at(j).id)
								{
									added = false;
									break;
								}
							}
							if (added && OnAudioDeviceAdded != nullptr)
							{
								OnAudioDeviceAdded(audioDevices.at(i));
							}
						}
					}
					for (size_t i = 0; i < oldDevices.size(); i++)
					{
						bool removed = true;
						for (size_t j = 0; j < audioDevices.size(); j++)
						{
							if (oldDevices.at(i).id == audioDevices.at(j).id)
							{
								removed = false;
								break;
							}
						}
						if (removed)
						{
							AudioDevice removedDevice = oldDevices.at(i);
							if (removedDevice.id == renderDeviceId)
							{
								InitializeRender(nullptr, renderFormat);
							}
							if (removedDevice.id == captureDeviceId)
							{
								InitializeCapture(nullptr, captureFormat);
							}
							if (OnAudioDeviceRemoved != nullptr)
							{
								OnAudioDeviceRemoved(removedDevice);
							}
						}
					}
				}
			}
		}
		void AndroidAudio::EnumerateRenderDevices()
		{
			SLint32 deviceCount = 20;
			SLint32 defaultDeviceCount = 20;
			SLuint32 deviceIds[20]{ 0 };
			SLuint32 defaultDeviceIds[20]{ 0 };
			ANDROIDAUDIO_DEVICE_THREAD_EXCPT((*audioDeviceCaps)->GetDefaultAudioDevices(audioDeviceCaps, SL_DEFAULTDEVICEID_AUDIOOUTPUT, &defaultDeviceCount, defaultDeviceIds), this, L"AndroidAudio", L"An error occurred whilst enumerating render devices.");
			ANDROIDAUDIO_DEVICE_THREAD_EXCPT((*audioDeviceCaps)->GetAvailableAudioOutputs(audioDeviceCaps, &deviceCount, deviceIds), this, L"AndroidAudio", L"An error occurred whilst enumerating render devices.");
			for (size_t i = 0; i < deviceCount; i++)
			{
				SLAudioOutputDescriptor audioDeviceDesc;
				ANDROIDAUDIO_EXCPT((*audioDeviceCaps)->QueryAudioOutputCapabilities(audioDeviceCaps, deviceIds[i], &audioDeviceDesc), this, L"AndroidAudio", L"An error occurred whilst enumerating render devices.");
				AudioDevice device;
				device.id = std::to_wstring(deviceIds[i]);
				std::string deviceName = (char*)audioDeviceDesc.pDeviceName;
				device.name = std::wstring(deviceName.begin(), deviceName.end());
				device.type = AudioDeviceType::Render;
				device.isDefault = false;
				for (size_t j = 0; j < defaultDeviceCount; j++)
				{
					if (deviceIds[i] == defaultDeviceIds[j])
					{
						device.isDefault = true;
						break;
					}
				}
				audioDevices.push_back(device);
			}
		}
		void AndroidAudio::EnumerateCaptureDevices()
		{
			SLint32 deviceCount = 20;
			SLint32 defaultDeviceCount = 20;
			SLuint32 deviceIds[20]{ 0 };
			SLuint32 defaultDeviceIds[20]{ 0 };
			ANDROIDAUDIO_DEVICE_THREAD_EXCPT((*audioDeviceCaps)->GetDefaultAudioDevices(audioDeviceCaps, SL_DEFAULTDEVICEID_AUDIOINPUT, &defaultDeviceCount, defaultDeviceIds), this, L"AndroidAudio", L"An error occurred whilst enumerating capture devices.");
			ANDROIDAUDIO_DEVICE_THREAD_EXCPT((*audioDeviceCaps)->GetAvailableAudioInputs(audioDeviceCaps, &deviceCount, deviceIds), this, L"AndroidAudio", L"An error occurred whilst enumerating capture devices.");
			for (size_t i = 0; i < deviceCount; i++)
			{
				SLAudioInputDescriptor audioDeviceDesc;
				ANDROIDAUDIO_EXCPT((*audioDeviceCaps)->QueryAudioInputCapabilities(audioDeviceCaps, deviceIds[i], &audioDeviceDesc), this, L"AndroidAudio", L"An error occurred whilst enumerating capture devices.");
				AudioDevice device;
				device.id = std::to_wstring(deviceIds[i]);
				std::string deviceName = (char*)audioDeviceDesc.deviceName;
				device.name = std::wstring(deviceName.begin(), deviceName.end());
				device.type = AudioDeviceType::Capture;
				device.isDefault = false;
				for (size_t j = 0; j < defaultDeviceCount; j++)
				{
					if (deviceIds[i] == defaultDeviceIds[j])
					{
						device.isDefault = true;
						break;
					}
				}
				audioDevices.push_back(device);
			}
		}
		void AndroidAudio::BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr)
			{
				AudioBuffer dataBuffer(RENDER_CALLBACK_SIZE_FRAMES, pCallbackContext->pAndroidAudio->renderFormat);
				pCallbackContext->pAndroidAudio->Mix(dataBuffer, RENDER_CALLBACK_SIZE_FRAMES);
				memcpy(pCallbackContext->pData, dataBuffer.GetInnerBufferAddress(), dataBuffer.Size());
				SLresult slres = (*bufferQueue)->Enqueue(bufferQueue, pCallbackContext->pData, dataBuffer.Size());
				if (slres != 0)
				{
					RAISE_AUDIO_EXCPT(pCallbackContext->pAndroidAudio, AudioException(E_FAIL, L"AndroidAudio", L"An error occurred whilst rendering data."));
					return;
				}
				pCallbackContext->pData += dataBuffer.Size();
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + RENDER_BUFFER_SIZE)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
				}
			}
		}
		void AndroidAudio::RecordEventCallback(SLRecordItf audioRecorder, void* pContext, SLuint32 e)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->OnCapture != nullptr)
			{
				AudioBuffer captureBuffer(CAPTURE_CALLBACK_SIZE_FRAMES, DEFAULT_AUDIO_FORMAT);
				memcpy(captureBuffer.GetInnerBufferAddress(), pCallbackContext->pData, CAPTURE_CALLBACK_SIZE);
				AudioProcessor audioProcessor(pCallbackContext->pAndroidAudio->captureFormat);
				audioProcessor.ConvertSampleRate(captureBuffer, CAPTURE_CALLBACK_SIZE_FRAMES);
				audioProcessor.ConvertBPS(captureBuffer);
				audioProcessor.ConvertChannels(captureBuffer);
				pCallbackContext->pAndroidAudio->OnCapture(captureBuffer);
				pCallbackContext->pData += CAPTURE_CALLBACK_SIZE;
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + CAPTURE_BUFFER_SIZE - CAPTURE_CALLBACK_SIZE)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
					SLresult slres = (*audioRecorder)->SetMarkerPosition(audioRecorder, 0);
					if (slres != 0)
					{
						RAISE_AUDIO_EXCPT(pCallbackContext->pAndroidAudio, AudioException(E_FAIL, L"AndroidAudio", L"An error occurred whilst capturing data."));
					}
				}
			}
		}
	}
}
#endif