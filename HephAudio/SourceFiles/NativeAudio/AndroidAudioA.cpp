#if defined(__ANDROID__) && (__ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL)
#include "NativeAudio/AndroidAudioA.h"
#include "File.h"
#include "HephMath.h"
#include "Stopwatch.h"
#include "ConsoleLogger.h"

#define ANDROIDAUDIO_EXCPT(ar, androidAudio, method, message) ares = ar;  if(ares != AAUDIO_OK) { RAISE_AND_THROW_HEPH_EXCEPTION(androidAudio, HephException(ares, method, message, "AAudio", AAudio_convertResultToText(ares))); }

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
			HEPH_SW_RESET;
			HEPHAUDIO_LOG("Destructing AndroidAudioA...", HEPH_CL_INFO);

			disposing = true;
			StopRendering();
			StopCapturing();

			HEPHAUDIO_LOG("AndroidAudioA destructed in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::SetMasterVolume(double volume)
		{
			if (volume < 0)
			{
				volume = -volume;
			}
			masterVolume = HEPH_MATH_MIN(volume, 1);
		}
		double AndroidAudioA::GetMasterVolume() const
		{
			return masterVolume;
		}
		void AndroidAudioA::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			StopRendering();

			aaudio_result_t ares;
			AAudioStreamBuilder* streamBuilder;
			ANDROIDAUDIO_EXCPT(AAudio_createStreamBuilder(&streamBuilder), this, "AndroidAudioA::InitializeRender", "An error occurred while creating the stream builder.");

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

			renderBufferFrameCount = this->renderFormat.sampleRate * this->params.renderBufferDuration_ms * 1e-3;

			AAudioStreamBuilder_setDirection(streamBuilder, AAUDIO_DIRECTION_OUTPUT);
			AAudioStreamBuilder_setSharingMode(streamBuilder, this->params.renderShareMode);
			AAudioStreamBuilder_setPerformanceMode(streamBuilder, this->params.renderPerformanceMode);
			AAudioStreamBuilder_setSampleRate(streamBuilder, format.sampleRate);
			AAudioStreamBuilder_setChannelCount(streamBuilder, format.channelLayout.count);
			AAudioStreamBuilder_setFramesPerDataCallback(streamBuilder, renderBufferFrameCount);
			AAudioStreamBuilder_setDataCallback(streamBuilder, &AndroidAudioA::RenderCallback, this);
			AAudioStreamBuilder_setErrorCallback(streamBuilder, &AndroidAudioA::ErrorCallback, this);

#if __ANDROID_API__ >= 28
			AAudioStreamBuilder_setContentType(streamBuilder, this->params.renderContentType);
			AAudioStreamBuilder_setUsage(streamBuilder, this->params.renderUsage);
#endif

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

			if (device != nullptr)
			{
				AAudioStreamBuilder_setDeviceId(streamBuilder, std::stoi(device->id.c_str()));
				renderDeviceId = device->id;
			}

			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pRenderStream), this, "AndroidAudioA::InitializeRender", "An error occurred while opening the render stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, "AndroidAudioA::InitializeRender", "An error occurred while deleting the stream builder.");

			if (device == nullptr)
			{
				renderDeviceId = StringHelpers::ToString(AAudioStream_getDeviceId(pRenderStream));
			}

			isRenderInitialized = true;
			ANDROIDAUDIO_EXCPT(AAudioStream_requestStart(pRenderStream), this, "AndroidAudioA::InitializeRender", "Failed to start the render stream.");

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = "";
				renderBufferFrameCount = 0;
				aaudio_result_t  ares;
				ANDROIDAUDIO_EXCPT(AAudioStream_close(pRenderStream), this, "AndroidAudioA::StopRendering", "An error occurred while closing the render stream.");
				JoinRenderThread();
				pRenderStream = nullptr;
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioA::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			StopCapturing();

			aaudio_result_t  ares;
			AAudioStreamBuilder* streamBuilder;
			ANDROIDAUDIO_EXCPT(AAudio_createStreamBuilder(&streamBuilder), this, "AndroidAudioA::InitializeCapture", "An error occurred while creating the stream builder.");

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

			captureBufferFrameCount = this->captureFormat.sampleRate * this->params.captureBufferDuration_ms * 1e-3;

			AAudioStreamBuilder_setDirection(streamBuilder, AAUDIO_DIRECTION_INPUT);
			AAudioStreamBuilder_setSharingMode(streamBuilder, this->params.captureShareMode);
			AAudioStreamBuilder_setPerformanceMode(streamBuilder, this->params.capturePerformanceMode);
			AAudioStreamBuilder_setSampleRate(streamBuilder, format.sampleRate);
			AAudioStreamBuilder_setChannelCount(streamBuilder, format.channelLayout.count);
			AAudioStreamBuilder_setFramesPerDataCallback(streamBuilder, captureBufferFrameCount);
			AAudioStreamBuilder_setDataCallback(streamBuilder, &AndroidAudioA::CaptureCallback, this);
			AAudioStreamBuilder_setErrorCallback(streamBuilder, &AndroidAudioA::ErrorCallback, this);

#if __ANDROID_API__ >= 28
			AAudioStreamBuilder_setContentType(streamBuilder, this->params.captureContentType);
			AAudioStreamBuilder_setUsage(streamBuilder, this->params.captureUsage);
#endif

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

			if (device != nullptr)
			{
				AAudioStreamBuilder_setDeviceId(streamBuilder, std::stoi(device->id.c_str()));
				captureDeviceId = device->id;
			}

			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_openStream(streamBuilder, &pCaptureStream), this, "AndroidAudioA::InitializeCapture", "An error occurred while opening the capture stream.");
			ANDROIDAUDIO_EXCPT(AAudioStreamBuilder_delete(streamBuilder), this, "AndroidAudioA::InitializeCapture", "An error occurred while deleting the stream builder.");

			if (device == nullptr)
			{
				captureDeviceId = StringHelpers::ToString(AAudioStream_getDeviceId(pCaptureStream));
			}

			isCaptureInitialized = true;
			ANDROIDAUDIO_EXCPT(AAudioStream_requestStart(pCaptureStream), this, "AndroidAudioA::InitializeCapture", "Failed to start the capture stream.");

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void AndroidAudioA::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = "";
				captureBufferFrameCount = 0;
				aaudio_result_t  ares;
				ANDROIDAUDIO_EXCPT(AAudioStream_close(pCaptureStream), this, "AndroidAudioA::StopCapturing", "An error occurred while closing the capture stream.");
				JoinCaptureThread();
				pCaptureStream = nullptr;
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void AndroidAudioA::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			AAudioParams* pAAudioParams = dynamic_cast<AAudioParams*>(&nativeParams);
			if (pAAudioParams == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AndroidAudioA::GetNativeParams", "nativeParams must be a AAudioParams instance."));
				return;
			}
			(*pAAudioParams) = this->params;
		}
		void AndroidAudioA::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			const AAudioParams* pAAudioParams = dynamic_cast<const AAudioParams*>(&nativeParams);
			if (pAAudioParams == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AndroidAudioA::SetNativeParams", "nativeParams must be a AAudioParams instance."));
				return;
			}
			this->params = *pAAudioParams;
		}
		double AndroidAudioA::GetFinalAOVolume(AudioObject* pAudioObject) const
		{
			return NativeAudio::GetFinalAOVolume(pAudioObject) * masterVolume;
		}
		aaudio_data_callback_result_t AndroidAudioA::RenderCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames)
		{
			AndroidAudioA* pAudio = (AndroidAudioA*)userData;
			if (pAudio != nullptr)
			{
				if (!pAudio->disposing && pAudio->isRenderInitialized)
				{
					EncodedAudioBuffer mixedBuffer = pAudio->Mix(numFrames);
					memcpy(audioData, mixedBuffer.begin(), numFrames * pAudio->renderFormat.FrameSize());
					return AAUDIO_CALLBACK_RESULT_CONTINUE;
				}
			}
			return AAUDIO_CALLBACK_RESULT_STOP;
		}
		aaudio_data_callback_result_t AndroidAudioA::CaptureCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames)
		{
			AndroidAudioA* pAudio = (AndroidAudioA*)userData;
			if (pAudio != nullptr)
			{
				if (!pAudio->disposing && pAudio->isCaptureInitialized)
				{
					if (!pAudio->isCapturePaused && pAudio->OnCapture)
					{
						EncodedAudioBuffer encodedBuffer((const uint8_t*)audioData, numFrames * this->captureFormat.FrameSize(), pAudio->captureFormat);
						AudioBuffer buffer = pAudio->pAudioDecoder->Decode(encodedBuffer);
						
						AudioCaptureEventArgs captureEventArgs(pAudio, buffer);
						pAudio->OnCapture(&captureEventArgs, nullptr);
					}
					return AAUDIO_CALLBACK_RESULT_CONTINUE;
				}
			}
			return AAUDIO_CALLBACK_RESULT_STOP;
		}
		void AndroidAudioA::ErrorCallback(AAudioStream* stream, void* userData, aaudio_result_t error)
		{
			const std::string direction = AAudioStream_getDirection(stream) == AAUDIO_DIRECTION_OUTPUT ? "rendering" : "capturing";
			RAISE_HEPH_EXCEPTION(userData, HephException(error, "AndroidAudioA", "An error occurred while " + direction, "AAudio", AAudio_convertResultToText(error)));
		}
	}
}
#endif