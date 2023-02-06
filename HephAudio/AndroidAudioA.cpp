#ifdef __ANDROID__
#include "AndroidAudioA.h"
#include "AudioProcessor.h"

#define ANDROIDAUDIO_EXCPT(ares, androidAudio, method, message) if(ares != AAUDIO_OK) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(ares, method, message)); throw AudioException(ares, method, message); }
#define ANDROIDAUDIO_RENDER_THREAD_EXCPT(ares, androidAudio, method, message) if(ares != AAUDIO_OK) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(ares, method, message)); goto RENDER_EXIT; }
#define ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(ares, androidAudio, method, message) if(ares != AAUDIO_OK) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(ares, method, message)); goto CAPTURE_EXIT; }

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioA::AndroidAudioA(JavaVM* jvm) : AndroidAudioBase(jvm)
		{
			if (deviceApiLevel < 27)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, L"AndroidAudioSLES::AndroidAudioA", L"The minimum supported Api level is 27."));
				throw AudioException(E_FAIL, L"AndroidAudioSLES::AndroidAudioA", L"The minimum supported Api level is 27.");
			}
			pRenderStream = nullptr;
			pCaptureStream = nullptr;
			renderBufferFrameCount = 0;
			captureBufferFrameCount = 0;
			masterVolume = 1.0;
		}
		AndroidAudioA::~AndroidAudioA()
		{
			disposing = true;
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();
			StopRendering();
			StopCapturing();
		}
		void AndroidAudioA::SetMasterVolume(double volume)
		{
			masterVolume = max(0.0, min(volume, 1.0));
		}
		double AndroidAudioA::GetMasterVolume() const
		{
			return masterVolume;
		}
		void AndroidAudioA::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			StopRendering();
			AAudioStreamBuilder* streamBuilder;
			ANDROIDAUDIO_EXCPT(AAudio_createStreamBuilder(&streamBuilder), this, L"AndroidAudioA::InitializeRender", L"An error occurred whilst creating the stream builder.");
			renderFormat = format;
			if (deviceApiLevel >= 31)
			{
				switch (format.bitsPerSample)
				{
				case 16:
					renderFormat.bitsPerSample = 16;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
					break;
				case 24:
					renderFormat.bitsPerSample = 24;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I24_PACKED);
					break;
				case 32:
					renderFormat.bitsPerSample = 32;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I32);
					break;
				default:
					renderFormat.bitsPerSample = 16;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
					break;
				}
			}
			else
			{
				renderFormat.bitsPerSample = 16;
				AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
			}
			renderBufferFrameCount = renderFormat.ByteRate();
			AAudioStreamBuilder_setDirection(streamBuilder, AAUDIO_DIRECTION_OUTPUT);
			AAudioStreamBuilder_setSharingMode(streamBuilder, AAUDIO_SHARING_MODE_SHARED);
			AAudioStreamBuilder_setSampleRate(streamBuilder, format.sampleRate);
			AAudioStreamBuilder_setChannelCount(streamBuilder, format.channelCount);
			AAudioStreamBuilder_setBufferCapacityInFrames(streamBuilder, renderBufferFrameCount);
			if (device != nullptr)
			{
				AAudioStreamBuilder_setDeviceId(streamBuilder, device->id.GetStringType() == StringType::Normal ? std::stoi(device->id.c_str()) : std::stoi(device->id.wc_str()));
				renderDeviceId = device->id;
			}
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pRenderStream), this, L"AndroidAudioA::InitializeRender", L"An error occurred whilst opening the render stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, L"AndroidAudioA::InitializeRender", L"An error occurred whilst deleting the stream builder.");
			isRenderInitialized = true;
			renderThread = std::thread(&AndroidAudioA::RenderData, this);
		}
		void AndroidAudioA::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = L"";
				renderBufferFrameCount = 0;
				ANDROIDAUDIO_EXCPT(AAudioStream_close(pRenderStream), this, L"AndroidAudioA::StopRendering", L"An error occurred whilst closing the render stream.");
				JoinRenderThread();
				pRenderStream = nullptr;
			}
		}
		void AndroidAudioA::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			StopCapturing();
			AAudioStreamBuilder* streamBuilder;
			ANDROIDAUDIO_EXCPT(AAudio_createStreamBuilder(&streamBuilder), this, L"AndroidAudioA::InitializeCapture", L"An error occurred whilst creating the stream builder.");
			captureFormat = format;
			if (deviceApiLevel >= 31)
			{
				switch (format.bitsPerSample)
				{
				case 16:
					captureFormat.bitsPerSample = 16;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
					break;
				case 24:
					captureFormat.bitsPerSample = 24;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I24_PACKED);
					break;
				case 32:
					captureFormat.bitsPerSample = 32;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I32);
					break;
				default:
					captureFormat.bitsPerSample = 16;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
					break;
				}
			}
			else
			{
				captureFormat.bitsPerSample = 16;
				AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
			}
			captureBufferFrameCount = captureFormat.ByteRate();
			AAudioStreamBuilder_setDirection(streamBuilder, AAUDIO_DIRECTION_INPUT);
			AAudioStreamBuilder_setSharingMode(streamBuilder, AAUDIO_SHARING_MODE_SHARED);
			AAudioStreamBuilder_setSampleRate(streamBuilder, format.sampleRate);
			AAudioStreamBuilder_setChannelCount(streamBuilder, format.channelCount);
			AAudioStreamBuilder_setBufferCapacityInFrames(streamBuilder, captureBufferFrameCount);
			if (device != nullptr)
			{
				AAudioStreamBuilder_setDeviceId(streamBuilder, device->id.GetStringType() == StringType::Normal ? std::stoi(device->id.c_str()) : std::stoi(device->id.wc_str()));
				captureDeviceId = device->id;
			}
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pCaptureStream), this, L"AndroidAudioA::InitializeCapture", L"An error occurred whilst opening the capture stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, L"AndroidAudioA::InitializeCapture", L"An error occurred whilst deleting the stream builder.");
			isCaptureInitialized = true;
			captureThread = std::thread(&AndroidAudioA::CaptureData, this);
		}
		void AndroidAudioA::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = L"";
				captureBufferFrameCount = 0;
				ANDROIDAUDIO_EXCPT(AAudioStream_close(pCaptureStream), this, L"AndroidAudioA::StopCapturing", L"An error occurred whilst closing the capture stream.");
				JoinCaptureThread();
				pCaptureStream = nullptr;
			}
		}
		void AndroidAudioA::SetDisplayName(StringBuffer displayName)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudioA::SetDisplayName", L"AndroidAudioA does not support this method."));
		}
		void AndroidAudioA::SetIconPath(StringBuffer iconPath)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudioA::SetIconPath", L"AndroidAudioA does not support this method."));
		}
		void AndroidAudioA::RenderData()
		{
			constexpr uint64_t stateChangeTimeoutNanos = 200 * 1000000;
			constexpr uint64_t writeTimeoutNanos = 10 * 1000000;
			aaudio_stream_state_t initialState = AAudioStream_getState(pRenderStream);
			aaudio_stream_state_t currentState = AAUDIO_STREAM_STATE_UNINITIALIZED;
			AudioBuffer dataBuffer = AudioBuffer(renderFormat.sampleRate * 0.01, renderFormat);
			ANDROIDAUDIO_RENDER_THREAD_EXCPT(AAudioStream_requestStart(pRenderStream), this, L"AndroidAudioA", L"An error occurred whilst starting the render stream.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT(AAudioStream_waitForStateChange(pRenderStream, initialState, &currentState, stateChangeTimeoutNanos), this, L"AndroidAudioA", L"An error occurred whilst starting the render stream.");
			while (!disposing && isRenderInitialized)
			{
				Mix(dataBuffer, dataBuffer.FrameCount());
				aaudio_result_t result = AAudioStream_write(pRenderStream, dataBuffer.Begin(), dataBuffer.FrameCount(), writeTimeoutNanos);
				if (result < 0)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(result, L"AndroidAudioA", L"An error occurred whilst rendering."));
					goto RENDER_EXIT;
				}
				dataBuffer.Reset();
			}
		RENDER_EXIT:
			AAudioStream_requestStop(pRenderStream);
			AAudioStream_waitForStateChange(pRenderStream, AAUDIO_STREAM_STATE_STOPPING, &currentState, stateChangeTimeoutNanos);
		}
		void AndroidAudioA::CaptureData()
		{
			constexpr uint64_t stateChangeTimeoutNanos = 200 * 1000000;
			constexpr uint64_t readTimeoutNanos = 10 * 1000000;
			aaudio_stream_state_t initialState = AAudioStream_getState(pCaptureStream);
			aaudio_stream_state_t currentState = AAUDIO_STREAM_STATE_UNINITIALIZED;
			AudioBuffer dataBuffer = AudioBuffer(captureFormat.sampleRate * 0.01, captureFormat);
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(AAudioStream_requestStart(pCaptureStream), this, L"AndroidAudioA", L"An error occurred whilst starting the capture stream.");
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(AAudioStream_waitForStateChange(pCaptureStream, initialState, &currentState, stateChangeTimeoutNanos), this, L"AndroidAudioA", L"An error occurred whilst starting the render stream.");
			while (!disposing && isCaptureInitialized)
			{
				aaudio_result_t result = AAudioStream_read(pCaptureStream, dataBuffer.Begin(), dataBuffer.FrameCount(), readTimeoutNanos);
				if (result < 0)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(result, L"AndroidAudioA", L"An error occurred whilst capturing."));
					goto CAPTURE_EXIT;
				}
				if (OnCapture != nullptr)
				{
					AudioBuffer tempBuffer = dataBuffer;
					AudioProcessor::ConvertPcmToInnerFormat(tempBuffer);
					OnCapture(tempBuffer);
				}
				dataBuffer.Reset();
			}
		CAPTURE_EXIT:
			AAudioStream_requestStop(pCaptureStream);
			AAudioStream_waitForStateChange(pCaptureStream, AAUDIO_STREAM_STATE_STOPPING, &currentState, stateChangeTimeoutNanos);
		}
		double AndroidAudioA::GetFinalAOVolume(std::shared_ptr<AudioObject> audioObject) const
		{
			return NativeAudio::GetFinalAOVolume(audioObject) * masterVolume;
		}
	}
}
#endif