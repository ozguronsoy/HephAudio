#ifdef __ANDROID__
#include "NativeAudio/AndroidAudioA.h"
#include "File.h"
#include "HephMath.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"

#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL

#define ANDROIDAUDIO_EXCPT(ar, androidAudio, method, message) ares = ar;  if(ares != AAUDIO_OK) { RAISE_AND_THROW_HEPH_EXCEPTION(androidAudio, HephException(ares, method, message, "AAudio", AAudio_convertResultToText(ares))); }
#define ANDROIDAUDIO_RENDER_THREAD_EXCPT(ar, androidAudio, method, message) ares = ar; if(ares != AAUDIO_OK) { RAISE_HEPH_EXCEPTION(androidAudio, HephException(ares, method, message,"AAudio", AAudio_convertResultToText(ares))); goto RENDER_EXIT; }
#define ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(ar, androidAudio, method, message) ares = ar; if(ares != AAUDIO_OK) { RAISE_HEPH_EXCEPTION(androidAudio, HephException(ares, method, message, "AAudio", AAudio_convertResultToText(ares))); goto CAPTURE_EXIT; }

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioA::AndroidAudioA() : AndroidAudioBase()
			, pRenderStream(nullptr), pCaptureStream(nullptr)
			, renderBufferFrameCount(0), captureBufferFrameCount(0), masterVolume(1.0)
		{
			if (deviceApiLevel < HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::AndroidAudioA", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
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
			StopRendering();
			StopCapturing();

			HEPHAUDIO_LOG("AndroidAudioA destructed in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::SetMasterVolume(heph_float volume)
		{
			masterVolume = HephMath::Max(0.0, HephMath::Min((double)volume, 1.0));
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

			aaudio_result_t ares;
			AAudioStreamBuilder* streamBuilder;
			ANDROIDAUDIO_EXCPT(AAudio_createStreamBuilder(&streamBuilder), this, "AndroidAudioA::InitializeRender", "An error occurred whilst creating the stream builder.");

			renderFormat = format;

			if (format.formatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT)
			{
				renderFormat.bitsPerSample = 32;
				AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_FLOAT);
			}
			else
			{
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
			}

			renderFormat.bitRate = AudioFormatInfo::CalculateBitrate(renderFormat);

			renderBufferFrameCount = renderFormat.ByteRate() * 0.03;

			AAudioStreamBuilder_setDirection(streamBuilder, AAUDIO_DIRECTION_OUTPUT);
			AAudioStreamBuilder_setSharingMode(streamBuilder, AAUDIO_SHARING_MODE_SHARED);
			AAudioStreamBuilder_setSampleRate(streamBuilder, format.sampleRate);
			AAudioStreamBuilder_setChannelCount(streamBuilder, format.channelLayout.count);

#if __ANDROID_API__ >= 32
			if (format.channelLayout.count == 1)
			{
				AAudioStreamBuilder_setChannelMask(streamBuilder, AAUDIO_CHANNEL_MONO);
			}
			else
			{
				AAudioStreamBuilder_setChannelMask(streamBuilder, (aaudio_channel_mask_t)format.channelLayout.mask);
			}
#endif

			AAudioStreamBuilder_setBufferCapacityInFrames(streamBuilder, renderBufferFrameCount);

			if (device != nullptr)
			{
				AAudioStreamBuilder_setDeviceId(streamBuilder, std::stoi(device->id.c_str()));
				renderDeviceId = device->id;
			}

			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pRenderStream), this, "AndroidAudioA::InitializeRender", "An error occurred whilst opening the render stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, "AndroidAudioA::InitializeRender", "An error occurred whilst deleting the stream builder.");

			if (device == nullptr)
			{
				renderDeviceId = StringHelpers::ToString(AAudioStream_getDeviceId(pRenderStream));
			}

			isRenderInitialized = true;
			renderThread = std::thread(&AndroidAudioA::RenderData, this);

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = "";
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

			if (format.formatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT)
			{
				captureFormat.bitsPerSample = 32;
				AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_FLOAT);
			}
			else {
				if (deviceApiLevel >= 31) {
					switch (format.bitsPerSample) {
					case 16:
						captureFormat.bitsPerSample = 16;
						AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
						break;
					case 24:
						captureFormat.bitsPerSample = 24;
						AAudioStreamBuilder_setFormat(streamBuilder,
							AAUDIO_FORMAT_PCM_I24_PACKED);
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
				else {
					captureFormat.bitsPerSample = 16;
					AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_I16);
				}
			}

			captureFormat.bitRate = AudioFormatInfo::CalculateBitrate(captureFormat);

			captureBufferFrameCount = captureFormat.ByteRate() * 0.03;

			AAudioStreamBuilder_setDirection(streamBuilder, AAUDIO_DIRECTION_INPUT);
			AAudioStreamBuilder_setSharingMode(streamBuilder, AAUDIO_SHARING_MODE_SHARED);
			AAudioStreamBuilder_setSampleRate(streamBuilder, format.sampleRate);
			AAudioStreamBuilder_setChannelCount(streamBuilder, format.channelLayout.count);

#if __ANDROID_API__ >= 32
			if (format.channelLayout.count == 1)
			{
				AAudioStreamBuilder_setChannelMask(streamBuilder, AAUDIO_CHANNEL_MONO);
			}
			else
			{
				AAudioStreamBuilder_setChannelMask(streamBuilder, (aaudio_channel_mask_t)format.channelLayout.mask);
			}
#endif

			AAudioStreamBuilder_setBufferCapacityInFrames(streamBuilder, captureBufferFrameCount);

			if (device != nullptr)
			{
				AAudioStreamBuilder_setDeviceId(streamBuilder, std::stoi(device->id.c_str()));
				captureDeviceId = device->id;
			}

			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pCaptureStream), this, "AndroidAudioA::InitializeCapture", "An error occurred whilst opening the capture stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, "AndroidAudioA::InitializeCapture", "An error occurred whilst deleting the stream builder.");

			if (device == nullptr)
			{
				captureDeviceId = StringHelpers::ToString(AAudioStream_getDeviceId(pCaptureStream));
			}

			isCaptureInitialized = true;
			captureThread = std::thread(&AndroidAudioA::CaptureData, this);

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = "";
				captureBufferFrameCount = 0;
				aaudio_result_t  ares;
				ANDROIDAUDIO_EXCPT(AAudioStream_close(pCaptureStream), this, "AndroidAudioA::StopCapturing", "An error occurred whilst closing the capture stream.");
				JoinCaptureThread();
				pCaptureStream = nullptr;
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioA::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			RAISE_AND_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::GetNativeParams", "Not implemented."));
		}
		void AndroidAudioA::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::SetNativeParams", "Not implemented."));
		}
		void AndroidAudioA::RenderData()
		{
			constexpr uint64_t stateChangeTimeoutNanos = 200 * 1000000;
			constexpr uint64_t writeTimeoutNanos = 10 * 1000000;

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
					RAISE_HEPH_EXCEPTION(this, HephException(result, "AndroidAudioA", "An error occurred whilst rendering.", "AAudio", AAudio_convertResultToText(result)));
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
			constexpr uint64_t stateChangeTimeoutNanos = 200e6;
			constexpr uint64_t readTimeoutNanos = 10e6;

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
					RAISE_HEPH_EXCEPTION(this, HephException(result, "AndroidAudioA", "An error occurred whilst capturing.", "AAudio", AAudio_convertResultToText(result)));
					goto CAPTURE_EXIT;
				}

				if (OnCapture)
				{
					AudioBuffer tempBuffer = dataBuffer;
					AudioCaptureEventArgs captureEventArgs(this, tempBuffer);
					OnCapture(&captureEventArgs, nullptr);
				}

				dataBuffer.Reset();
			}
		CAPTURE_EXIT:
			AAudioStream_requestStop(pCaptureStream);
			AAudioStream_waitForStateChange(pCaptureStream, AAUDIO_STREAM_STATE_STOPPING, &currentState, stateChangeTimeoutNanos);
		}
		heph_float AndroidAudioA::GetFinalAOVolume(AudioObject* pAudioObject) const
		{
			return NativeAudio::GetFinalAOVolume(pAudioObject) * masterVolume;
		}
	}
}

#else

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		AndroidAudioA::AndroidAudioA() : AndroidAudioBase()
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::AndroidAudioA", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
		}
		AndroidAudioA::~AndroidAudioA()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing AndroidAudioA...", HEPH_CL_INFO);

			disposing = true;
			JoinRenderThread();
			JoinCaptureThread();
			StopRendering();
			StopCapturing();

			HEPHAUDIO_LOG("AndroidAudioA destructed in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::SetMasterVolume(heph_float volume)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::SetMasterVolume", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
		}
		heph_float AndroidAudioA::GetMasterVolume() const
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::GetMasterVolume", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
		}
		void AndroidAudioA::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::InitializeRender", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
		}
		void AndroidAudioA::StopRendering()
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::StopRendering", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
		}
		void AndroidAudioA::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::InitializeCapture", "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
		}
		void AndroidAudioA::StopCapturing()
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "The minimum supported API level is " + StringHelpers::ToString(HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL) + "."));
		}
		void AndroidAudioA::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			RAISE_AND_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::GetNativeParams", "Not implemented."));
		}
		void AndroidAudioA::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AndroidAudioA::SetNativeParams", "Not implemented."));
		}
	}
}

#endif

#endif