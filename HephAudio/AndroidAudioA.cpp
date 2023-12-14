#ifdef __ANDROID__
#include "AndroidAudioA.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/HephMath.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"

#define ANDROIDAUDIO_EXCPT(ar, androidAudio, method, message) ares = ar;  if(ares != AAUDIO_OK) { RAISE_AND_THROW_HEPH_EXCEPTION(androidAudio, HephException(ares, method, message)); }
#define ANDROIDAUDIO_RENDER_THREAD_EXCPT(ar, androidAudio, method, message) ares = ar; if(ares != AAUDIO_OK) { RAISE_HEPH_EXCEPTION(androidAudio, HephException(ares, method, message)); goto RENDER_EXIT; }
#define ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(ar, androidAudio, method, message) ares = ar; if(ares != AAUDIO_OK) { RAISE_HEPH_EXCEPTION(androidAudio, HephException(ares, method, message)); goto CAPTURE_EXIT; }

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioA::AndroidAudioA(JavaVM* jvm) : AndroidAudioBase(jvm)
			, pRenderStream(nullptr), pCaptureStream(nullptr), renderBufferFrameCount(0), captureBufferFrameCount(0), masterVolume(1.0)
		{
			if (deviceApiLevel < 27)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "AndroidAudioA::AndroidAudioA", "The minimum supported Api level is 27."));
			}

			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&AndroidAudioA::CheckAudioDevices, this);
		}
		AndroidAudioA::~AndroidAudioA()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing AndroidAudioA...", HEPH_CL_INFO);

			disposing = true;
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();
			StopRendering();
			StopCapturing();

			HEPHAUDIO_LOG("AndroidAudioA destructed in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::SetMasterVolume(heph_float volume)
		{
			masterVolume = Math::Max(0.0, Math::Min((double)volume, 1.0));
		}
		heph_float AndroidAudioA::GetMasterVolume() const
		{
			return masterVolume;
		}
		void AndroidAudioA::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			StopRendering();

			aaudio_result_t  ares;
			AAudioStreamBuilder* streamBuilder;
			ANDROIDAUDIO_EXCPT(AAudio_createStreamBuilder(&streamBuilder), this, "AndroidAudioA::InitializeRender", "An error occurred whilst creating the stream builder.");

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
				AAudioStreamBuilder_setDeviceId(streamBuilder, device->id.GetStringType() == StringType::ASCII ? std::stoi(device->id.c_str()) : std::stoi(device->id.wc_str()));
				renderDeviceId = device->id;
			}

			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pRenderStream), this, "AndroidAudioA::InitializeRender", "An error occurred whilst opening the render stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, "AndroidAudioA::InitializeRender", "An error occurred whilst deleting the stream builder.");

			if (device == nullptr)
			{
				renderDeviceId = StringBuffer::ToString(AAudioStream_getDeviceId(pRenderStream));
			}

			isRenderInitialized = true;
			renderThread = std::thread(&AndroidAudioA::RenderData, this);

			HEPHAUDIO_LOG("Render initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = L"";
				renderBufferFrameCount = 0;
				aaudio_result_t  ares;
				ANDROIDAUDIO_EXCPT(AAudioStream_close(pRenderStream), this, "AndroidAudioA::StopRendering", "An error occurred whilst closing the render stream.");
				JoinRenderThread();
				pRenderStream = nullptr;
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioA::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			StopCapturing();

			aaudio_result_t  ares;
			AAudioStreamBuilder* streamBuilder;
			ANDROIDAUDIO_EXCPT(AAudio_createStreamBuilder(&streamBuilder), this, "AndroidAudioA::InitializeCapture", "An error occurred whilst creating the stream builder.");

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
				AAudioStreamBuilder_setDeviceId(streamBuilder, device->id.GetStringType() == StringType::ASCII ? std::stoi(device->id.c_str()) : std::stoi(device->id.wc_str()));
				captureDeviceId = device->id;
			}

			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pCaptureStream), this, "AndroidAudioA::InitializeCapture", "An error occurred whilst opening the capture stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, "AndroidAudioA::InitializeCapture", "An error occurred whilst deleting the stream builder.");

			if (device == nullptr)
			{
				captureDeviceId = StringBuffer::ToString(AAudioStream_getDeviceId(pCaptureStream));
			}

			isCaptureInitialized = true;
			captureThread = std::thread(&AndroidAudioA::CaptureData, this);

			HEPHAUDIO_LOG("Capture initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = L"";
				captureBufferFrameCount = 0;
				aaudio_result_t  ares;
				ANDROIDAUDIO_EXCPT(AAudioStream_close(pCaptureStream), this, "AndroidAudioA::StopCapturing", "An error occurred whilst closing the capture stream.");
				JoinCaptureThread();
				pCaptureStream = nullptr;
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioA::SetDisplayName(StringBuffer displayName)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "AndroidAudioA::SetDisplayName", "AndroidAudioA does not support this method."));
		}
		void AndroidAudioA::SetIconPath(StringBuffer iconPath)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "AndroidAudioA::SetIconPath", "AndroidAudioA does not support this method."));
		}
		void AndroidAudioA::RenderData()
		{
			HEPH_CONSTEXPR uint64_t stateChangeTimeoutNanos = 200 * 1000000;
			HEPH_CONSTEXPR uint64_t writeTimeoutNanos = 10 * 1000000;

			AudioBuffer dataBuffer = AudioBuffer(renderFormat.sampleRate * 0.01, renderFormat);
			aaudio_result_t  ares;

			aaudio_stream_state_t initialState = AAudioStream_getState(pRenderStream);
			aaudio_stream_state_t currentState = AAUDIO_STREAM_STATE_UNINITIALIZED;
			ANDROIDAUDIO_RENDER_THREAD_EXCPT(AAudioStream_requestStart(pRenderStream), this, "AndroidAudioA", "An error occurred whilst starting the render stream.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT(AAudioStream_waitForStateChange(pRenderStream, initialState, &currentState, stateChangeTimeoutNanos), this, "AndroidAudioA", "An error occurred whilst starting the render stream.");

			while (!disposing && isRenderInitialized)
			{
				Mix(dataBuffer, dataBuffer.FrameCount());

				aaudio_result_t result = AAudioStream_write(pRenderStream, dataBuffer.Begin(), dataBuffer.FrameCount(), writeTimeoutNanos);
				if (result < 0)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(result, "AndroidAudioA", "An error occurred whilst rendering."));
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
			HEPH_CONSTEXPR uint64_t stateChangeTimeoutNanos = 200e6;
			HEPH_CONSTEXPR uint64_t readTimeoutNanos = 10e6;

			AudioBuffer dataBuffer = AudioBuffer(captureFormat.sampleRate * 0.01, captureFormat);
			aaudio_result_t  ares;

			aaudio_stream_state_t initialState = AAudioStream_getState(pCaptureStream);
			aaudio_stream_state_t currentState = AAUDIO_STREAM_STATE_UNINITIALIZED;
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(AAudioStream_requestStart(pCaptureStream), this, "AndroidAudioA", "An error occurred whilst starting the capture stream.");
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(AAudioStream_waitForStateChange(pCaptureStream, initialState, &currentState, stateChangeTimeoutNanos), this, "AndroidAudioA", "An error occurred whilst starting the render stream.");

			while (!disposing && isCaptureInitialized)
			{
				aaudio_result_t result = AAudioStream_read(pCaptureStream, dataBuffer.Begin(), dataBuffer.FrameCount(), readTimeoutNanos);

				if (result < 0)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(result, "AndroidAudioA", "An error occurred whilst capturing."));
					goto CAPTURE_EXIT;
				}

				if (OnCapture)
				{
					AudioBuffer tempBuffer = dataBuffer;
					AudioProcessor::ConvertToInnerFormat(tempBuffer);
					AudioCaptureEventArgs captureEventArgs = AudioCaptureEventArgs(this, tempBuffer);
					OnCapture(&captureEventArgs, nullptr);
				}

				dataBuffer.Reset();
			}
		CAPTURE_EXIT:
			AAudioStream_requestStop(pCaptureStream);
			AAudioStream_waitForStateChange(pCaptureStream, AAUDIO_STREAM_STATE_STOPPING, &currentState, stateChangeTimeoutNanos);
		}
		heph_float AndroidAudioA::GetFinalAOVolume(std::shared_ptr<AudioObject> pAudioObject) const
		{
			return NativeAudio::GetFinalAOVolume(pAudioObject) * masterVolume;
		}
	}
}
#endif