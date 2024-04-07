#if defined(__linux__) && !defined(__ANDROID__)
#include "NativeAudio/LinuxAudio.h"
#include "AudioProcessor.h"
#include "File.h"
#include "ConsoleLogger.h"
#include "StopWatch.h"
#include "StringHelpers.h"
#include <unistd.h>

#define SND_OK 0
#define LINUX_ENUMERATE_DEVICE_EXCPT(r, linuxAudio, method, message)                                            \
	result = r;                                                                                                 \
	if (result != SND_OK)                                                                                       \
	{                                                                                                           \
		RAISE_HEPH_EXCEPTION(linuxAudio, HephException(result, method, message, "ALSA", snd_strerror(result))); \
		return NativeAudio::DEVICE_ENUMERATION_FAIL;                                                            \
	}
#define LINUX_EXCPT(r, linuxAudio, method, message)                                                                       \
	result = r;                                                                                                           \
	if (result < SND_OK)                                                                                                  \
	{                                                                                                                     \
		RAISE_AND_THROW_HEPH_EXCEPTION(linuxAudio, HephException(result, method, message, "ALSA", snd_strerror(result))); \
	}
#define LINUX_RENDER_CAPTURE_EXCPT(r, linuxAudio, method, message)                                              \
	result = r;                                                                                                 \
	if (result < SND_OK)                                                                                        \
	{                                                                                                           \
		RAISE_HEPH_EXCEPTION(linuxAudio, HephException(result, method, message, "ALSA", snd_strerror(result))); \
		return;                                                                                                 \
	}

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		LinuxAudio::LinuxAudio() : NativeAudio(), renderPcm(nullptr), capturePcm(nullptr)
		{
			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&LinuxAudio::CheckAudioDevices, this);
		}
		LinuxAudio::~LinuxAudio()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing LinuxAudio...", HEPH_CL_INFO);

			disposing = true;

			JoinDeviceThread();
			JoinRenderThread();
			JoinCaptureThread();

			if (renderPcm != nullptr)
			{
				snd_pcm_close(renderPcm);
			}

			if (capturePcm != nullptr)
			{
				snd_pcm_close(capturePcm);
			}

			HEPHAUDIO_LOG("LinuxAudio destructed in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void LinuxAudio::SetMasterVolume(heph_float volume)
		{
			if (isRenderInitialized && renderDeviceId != "")
			{
				snd_mixer_t* mixer;
				int result;
				const std::string cardID = renderDeviceId == "default" ? "hw:0" : ("hw:" + StringHelpers::Split(StringHelpers::Split(renderDeviceId, ",")[0], ":")[1]);

				LINUX_EXCPT(snd_mixer_open(&mixer, 0), this, "LinuxAudio::SetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_attach(mixer, cardID.c_str()), this, "LinuxAudio::SetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_selem_register(mixer, nullptr, nullptr), this, "LinuxAudio::SetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_load(mixer), this, "LinuxAudio::SetMasterVolume", "An error occurred while loading the mixer.");

				snd_mixer_elem_t* mixerElem = snd_mixer_first_elem(mixer);
				if (mixerElem == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "LinuxAudio::SetMasterVolume", "No mixer element found."));
				}

				if (!snd_mixer_selem_has_playback_volume(mixerElem))
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "LinuxAudio::SetMasterVolume", "No mixer playback volume element found."));
				}

				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_range(mixerElem, -LinuxAudio::volume_max, LinuxAudio::volume_max), this, "LinuxAudio::SetMasterVolume", "An error occurred while setting the volume range.");
				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_all(mixerElem, volume * LinuxAudio::volume_max * 2 - LinuxAudio::volume_max), this, "LinuxAudio::SetMasterVolume", "An error occurred while setting the volume.");
				LINUX_EXCPT(snd_mixer_close(mixer), this, "LinuxAudio::SetMasterVolume", "An error occurred while closing the mixer.");
			}
		}
		heph_float LinuxAudio::GetMasterVolume() const
		{
			if (isRenderInitialized && renderDeviceId != "")
			{
				snd_mixer_t* mixer;
				int result;
				long value;
				const std::string cardID = renderDeviceId == "default" ? "hw:0" : ("hw:" + StringHelpers::Split(StringHelpers::Split(renderDeviceId, ",")[0], ":")[1]);

				LINUX_EXCPT(snd_mixer_open(&mixer, 0), this, "LinuxAudio::GetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_attach(mixer, cardID.c_str()), this, "LinuxAudio::GetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_selem_register(mixer, nullptr, nullptr), this, "LinuxAudio::GetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_load(mixer), this, "LinuxAudio::GetMasterVolume", "An error occurred while loading the mixer.");

				snd_mixer_elem_t* mixerElem = snd_mixer_first_elem(mixer);
				if (mixerElem == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "LinuxAudio::GetMasterVolume", "No mixer element found."));
				}

				if (!snd_mixer_selem_has_playback_volume(mixerElem))
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "LinuxAudio::GetMasterVolume", "No mixer playback volume element found."));
				}

				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_range(mixerElem, -LinuxAudio::volume_max, LinuxAudio::volume_max), this, "LinuxAudio::GetMasterVolume", "An error occurred while getting the volume range.");
				LINUX_EXCPT(snd_mixer_selem_get_playback_volume(mixerElem, SND_MIXER_SCHN_MONO, &value), this, "LinuxAudio::GetMasterVolume", "An error occurred while getting the volume.");
				LINUX_EXCPT(snd_mixer_close(mixer), this, "LinuxAudio::GetMasterVolume", "An error occurred while closing the mixer.");

				return ((heph_float)value + LinuxAudio::volume_max) / (LinuxAudio::volume_max * 2.0);
			}
			return 1.0;
		}
		void LinuxAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			int result;
			snd_pcm_hw_params_t* pcmHwParams;
			snd_pcm_sw_params_t* pcmSwParams;

			StopRendering();

			renderDeviceId = device != nullptr ? device->id : "default";
			renderFormat = format;

			LINUX_EXCPT(snd_pcm_open(&renderPcm, renderDeviceId.c_str(), SND_PCM_STREAM_PLAYBACK, 0), this, "LinuxAudio::InitializeRender", "An error occurred while opening pcm.");

			LINUX_EXCPT(snd_pcm_hw_params_malloc(&pcmHwParams), this, "LinuxAudio::InitializeRender", "An error occurred while allocating space for pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_any(renderPcm, pcmHwParams), this, "LinuxAudio::InitializeRender", "An error occurred while reading the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_access(renderPcm, pcmHwParams, SND_PCM_ACCESS_RW_INTERLEAVED), this, "LinuxAudio::InitializeRender", "An error occurred while setting access to the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_format(renderPcm, pcmHwParams, ToPcmFormat(renderFormat)), this, "LinuxAudio::InitializeRender", "Unsupported bps.");
			LINUX_EXCPT(snd_pcm_hw_params_set_channels(renderPcm, pcmHwParams, renderFormat.channelLayout.count), this, "LinuxAudio::InitializeRender", "Unsupported channel count.");
			LINUX_EXCPT(snd_pcm_hw_params_set_rate(renderPcm, pcmHwParams, renderFormat.sampleRate, 0), this, "LinuxAudio::InitializeRender", "Unsupported sample rate.");
			LINUX_EXCPT(snd_pcm_hw_params_set_buffer_size(renderPcm, pcmHwParams, renderFormat.sampleRate * 0.5), this, "LinuxAudio::InitializeRender", "An error occurred while setting the pcm buffer size.");
			LINUX_EXCPT(snd_pcm_hw_params(renderPcm, pcmHwParams), this, "LinuxAudio::InitializeRender", "An error occurred while configuring the pcm hw params.");
			snd_pcm_hw_params_free(pcmHwParams);

			isRenderInitialized = true;
			renderThread = std::thread(&LinuxAudio::RenderData, this);

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void LinuxAudio::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = "";
				JoinRenderThread();
				if (renderPcm != nullptr)
				{
					snd_pcm_close(renderPcm);
					renderPcm = nullptr;
				}
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void LinuxAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			int result;
			snd_pcm_hw_params_t* pcmHwParams;

			StopCapturing();

			captureDeviceId = device != nullptr ? device->id : "default";
			captureFormat = format;

			LINUX_EXCPT(snd_pcm_open(&capturePcm, captureDeviceId.c_str(), SND_PCM_STREAM_CAPTURE, 0), this, "LinuxAudio::InitializeCapture", "An error occurred while opening pcm.");

			LINUX_EXCPT(snd_pcm_hw_params_malloc(&pcmHwParams), this, "LinuxAudio::InitializeCapture", "An error occurred while allocating space for pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_any(capturePcm, pcmHwParams), this, "LinuxAudio::InitializeCapture", "An error occurred while reading the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_access(capturePcm, pcmHwParams, SND_PCM_ACCESS_RW_INTERLEAVED), this, "LinuxAudio::InitializeCapture", "An error occurred while setting access to the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_format(capturePcm, pcmHwParams, ToPcmFormat(captureFormat)), this, "LinuxAudio::InitializeCapture", "Unsupported bps.");
			LINUX_EXCPT(snd_pcm_hw_params_set_channels(capturePcm, pcmHwParams, captureFormat.channelLayout.count), this, "LinuxAudio::InitializeCapture", "Unsupported channel count.");
			LINUX_EXCPT(snd_pcm_hw_params_set_rate(capturePcm, pcmHwParams, captureFormat.sampleRate, 0), this, "LinuxAudio::InitializeCapture", "Unsupported sample rate.");
			LINUX_EXCPT(snd_pcm_hw_params_set_buffer_size(capturePcm, pcmHwParams, captureFormat.sampleRate * 0.1), this, "LinuxAudio::InitializeCapture", "An error occurred while setting the pcm buffer size.");
			LINUX_EXCPT(snd_pcm_hw_params(capturePcm, pcmHwParams), this, "LinuxAudio::InitializeCapture", "An error occurred while configuring the pcm hw params.");
			snd_pcm_hw_params_free(pcmHwParams);

			isCaptureInitialized = true;
			captureThread = std::thread(&LinuxAudio::CaptureData, this);

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void LinuxAudio::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = "";
				JoinCaptureThread();
				if (capturePcm != nullptr)
				{
					snd_pcm_close(capturePcm);
					capturePcm = nullptr;
				}
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		bool LinuxAudio::EnumerateAudioDevices()
		{
			snd_pcm_stream_t streamTypes[2] = { SND_PCM_STREAM_PLAYBACK, SND_PCM_STREAM_CAPTURE };
			snd_ctl_card_info_t* cardInfo;
			snd_pcm_info_t* pcmInfo;
			int cardID = -1, deviceID = -1;
			int result;

			LINUX_ENUMERATE_DEVICE_EXCPT(snd_card_next(&cardID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while getting the next card.");
			LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_card_info_malloc(&cardInfo), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while allocating memory for card info.");
			LINUX_ENUMERATE_DEVICE_EXCPT(snd_pcm_info_malloc(&pcmInfo), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while allocating memory for device info.");
			while (cardID != -1)
			{
				if (snd_card_load(cardID)) // check if the card is available
				{
					std::string cardName = "hw:" + StringHelpers::ToString(cardID);
					snd_ctl_t* ctl;

					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_open(&ctl, cardName.c_str(), SND_CTL_READONLY), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while opening the card controls.");
					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_card_info(ctl, cardInfo), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while reading the card info.");
					cardName = snd_ctl_card_info_get_name(cardInfo);

					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_pcm_next_device(ctl, &deviceID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while getting the next device.");
					while (deviceID != -1)
					{
						snd_pcm_info_set_device(pcmInfo, deviceID);
						for (size_t i = 0; i < 2; i++)
						{
							snd_pcm_info_set_stream(pcmInfo, streamTypes[i]);
							if (snd_ctl_pcm_info(ctl, pcmInfo) == SND_OK)
							{
								AudioDevice device;
								device.id = "plughw:" + StringHelpers::ToString(cardID) + ',' + StringHelpers::ToString(deviceID);
								device.name = cardName + " " + snd_pcm_info_get_name(pcmInfo);
								device.type = i == 1 ? AudioDeviceType::Capture : AudioDeviceType::Render;
								device.isDefault = (cardID == 0 && deviceID == 0); // "plughw:0,0" is the default device
								this->audioDevices.push_back(device);
							}
						}

						LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_pcm_next_device(ctl, &deviceID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while getting the next device.");
					}

					snd_ctl_close(ctl);
				}
				LINUX_ENUMERATE_DEVICE_EXCPT(snd_card_next(&cardID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred while getting the next card.");
			}

			snd_ctl_card_info_free(cardInfo);
			snd_pcm_info_free(pcmInfo);

			return NativeAudio::DEVICE_ENUMERATION_SUCCESS;
		}
		void LinuxAudio::RenderData()
		{
			AudioBuffer buffer(renderFormat.sampleRate * 0.01f, renderFormat);
			snd_pcm_uframes_t availableFrameCount;
			int result;

			LINUX_RENDER_CAPTURE_EXCPT(snd_pcm_start(renderPcm), this, "LinuxAudio", "Failed to start rendering.");

			{
				while (snd_pcm_state(renderPcm) != SND_PCM_STATE_RUNNING);

				const size_t tempFrameCount = snd_pcm_avail_update(renderPcm);
				const size_t tempSize = tempFrameCount * renderFormat.FrameSize();
				void* pTempData = malloc(tempSize);
				if (pTempData == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "LinuxAudio", "Insufficient memory."));
					return;
				}
				memset(pTempData, 0, tempSize);

				snd_pcm_writei(renderPcm, pTempData, tempFrameCount);
				free(pTempData);
				usleep(5000);
			}

			while (!disposing && isRenderInitialized)
			{
				availableFrameCount = snd_pcm_avail_update(renderPcm);
				if (availableFrameCount >= buffer.FrameCount())
				{
					Mix(buffer, buffer.FrameCount());
					snd_pcm_writei(renderPcm, buffer.Begin(), buffer.FrameCount());
				}
				usleep(5000);
			}
		}
		void LinuxAudio::CaptureData()
		{
			snd_pcm_uframes_t availableFrameCount;
			int result;

			LINUX_RENDER_CAPTURE_EXCPT(snd_pcm_start(capturePcm), this, "LinuxAudio", "Failed to start capturing.");
			while (!disposing && isCaptureInitialized)
			{
				if (!isCapturePaused)
				{
					availableFrameCount = snd_pcm_avail_update(capturePcm);
					if (availableFrameCount > 0)
					{
						AudioBuffer buffer(availableFrameCount, captureFormat);
						snd_pcm_readi(capturePcm, buffer.Begin(), buffer.FrameCount());

						if (OnCapture)
						{
							AudioProcessor::ConvertToInnerFormat(buffer);
							AudioCaptureEventArgs captureEventArgs(this, buffer);
							OnCapture(&captureEventArgs, nullptr);
						}
					}
				}
			}
		}
		snd_pcm_format_t LinuxAudio::ToPcmFormat(const AudioFormatInfo& format) const
		{
			if (format.formatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT)
			{
				return format.bitsPerSample == sizeof(double) ? snd_pcm_format_t::SND_PCM_FORMAT_FLOAT64 : snd_pcm_format_t::SND_PCM_FORMAT_FLOAT;
			}
			else
			{
				switch (format.bitsPerSample)
				{
				case 8:
					return snd_pcm_format_t::SND_PCM_FORMAT_U8;
				case 16:
					return snd_pcm_format_t::SND_PCM_FORMAT_S16;
				case 24:
					return snd_pcm_format_t::SND_PCM_FORMAT_S24;
				case 32:
					return snd_pcm_format_t::SND_PCM_FORMAT_S32;
				default:
					return snd_pcm_format_t::SND_PCM_FORMAT_S16;
				}
			}
		}
	}
}
#endif