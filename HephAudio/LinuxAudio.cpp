#if defined(__linux__)
#include "LinuxAudio.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include <unistd.h>

#define SND_OK 0
#define LINUX_ENUMERATE_DEVICE_EXCPT(r, linuxAudio, method, message) result = r; if (result != SND_OK) { RAISE_HEPH_EXCEPTION(linuxAudio, HephException(result, method, message)); return NativeAudio::DEVICE_ENUMERATION_FAIL; }
#define LINUX_EXCPT(r, linuxAudio, method, message) result = r; if (result < SND_OK) { RAISE_AND_THROW_HEPH_EXCEPTION(linuxAudio, HephException(result, method, message)); }
#define LINUX_RENDER_CAPTURE_EXCPT(r, linuxAudio, method, message) result = r; if (result < SND_OK) { RAISE_HEPH_EXCEPTION(linuxAudio, HephException(result, method, message)); return; }

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
			JoinQueueThreads();

			if (renderPcm != nullptr)
			{
				snd_pcm_close(renderPcm);
			}

			if (capturePcm != nullptr)
			{
				snd_pcm_close(capturePcm);
			}

			HEPHAUDIO_LOG("LinuxAudio destructed in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void LinuxAudio::SetMasterVolume(heph_float volume)
		{
			if (isRenderInitialized && renderDeviceId != "")
			{
				snd_mixer_t* mixer;
				int result;
				const StringBuffer cardID = renderDeviceId == "default" ? "hw:0" : ("hw:" + renderDeviceId.Split(',')[0].Split(':')[1]);

				LINUX_EXCPT(snd_mixer_open(&mixer, 0), this, "LinuxAudio::SetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_attach(mixer, cardID.c_str()), this, "LinuxAudio::SetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_selem_register(mixer, nullptr, nullptr), this, "LinuxAudio::SetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_load(mixer), this, "LinuxAudio::SetMasterVolume", "An error occurred whilst loading the mixer.");

				snd_mixer_elem_t* mixerElem = snd_mixer_first_elem(mixer);
				if (mixerElem == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "LinuxAudio::SetMasterVolume", "No mixer element found."));
				}

				if (!snd_mixer_selem_has_playback_volume(mixerElem))
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "LinuxAudio::SetMasterVolume", "No mixer playback volume element found."));
				}

				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_range(mixerElem, -LinuxAudio::volume_max, LinuxAudio::volume_max), this, "LinuxAudio::SetMasterVolume", "An error occurred whilst setting the volume range.");
				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_all(mixerElem, volume * LinuxAudio::volume_max * 2 - LinuxAudio::volume_max), this, "LinuxAudio::SetMasterVolume", "An error occurred whilst setting the volume.");
				LINUX_EXCPT(snd_mixer_close(mixer), this, "LinuxAudio::SetMasterVolume", "An error occurred whilst closing the mixer.");
			}
		}
		heph_float LinuxAudio::GetMasterVolume() const
		{
			if (isRenderInitialized && renderDeviceId != "")
			{
				snd_mixer_t* mixer;
				int result;
				long value;
				const StringBuffer cardID = renderDeviceId == "default" ? "hw:0" : ("hw:" + renderDeviceId.Split(',')[0].Split(':')[1]);

				LINUX_EXCPT(snd_mixer_open(&mixer, 0), this, "LinuxAudio::GetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_attach(mixer, cardID.c_str()), this, "LinuxAudio::GetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_selem_register(mixer, nullptr, nullptr), this, "LinuxAudio::GetMasterVolume", "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_load(mixer), this, "LinuxAudio::GetMasterVolume", "An error occurred whilst loading the mixer.");

				snd_mixer_elem_t* mixerElem = snd_mixer_first_elem(mixer);
				if (mixerElem == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "LinuxAudio::GetMasterVolume", "No mixer element found."));
				}

				if (!snd_mixer_selem_has_playback_volume(mixerElem))
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "LinuxAudio::GetMasterVolume", "No mixer playback volume element found."));
				}

				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_range(mixerElem, -LinuxAudio::volume_max, LinuxAudio::volume_max), this, "LinuxAudio::GetMasterVolume", "An error occurred whilst getting the volume range.");
				LINUX_EXCPT(snd_mixer_selem_get_playback_volume(mixerElem, SND_MIXER_SCHN_MONO, &value), this, "LinuxAudio::GetMasterVolume", "An error occurred whilst getting the volume.");
				LINUX_EXCPT(snd_mixer_close(mixer), this, "LinuxAudio::GetMasterVolume", "An error occurred whilst closing the mixer.");

				return ((heph_float)value + LinuxAudio::volume_max) / (LinuxAudio::volume_max * 2.0);
			}
			return 1.0;
		}
		void LinuxAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			int result;
			snd_pcm_hw_params_t* pcmHwParams;

			StopRendering();

			renderDeviceId = device != nullptr ? device->id : "default";
			renderFormat = format;

			LINUX_EXCPT(snd_pcm_open(&renderPcm, renderDeviceId.c_str(), SND_PCM_STREAM_PLAYBACK, 0), this, "LinuxAudio::InitializeRender", "An error occurred whilst opening pcm.");

			LINUX_EXCPT(snd_pcm_hw_params_malloc(&pcmHwParams), this, "LinuxAudio::InitializeRender", "An error occurred whilst allocating space for pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_any(renderPcm, pcmHwParams), this, "LinuxAudio::InitializeRender", "An error occurred whilst reading the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_access(renderPcm, pcmHwParams, SND_PCM_ACCESS_RW_INTERLEAVED), this, "LinuxAudio::InitializeRender", "An error occurred whilst setting access to the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_format(renderPcm, pcmHwParams, ToPcmFormat(renderFormat)), this, "LinuxAudio::InitializeRender", "Unsupported bps.");
			LINUX_EXCPT(snd_pcm_hw_params_set_channels(renderPcm, pcmHwParams, renderFormat.channelCount), this, "LinuxAudio::InitializeRender", "Unsupported channel count.");
			LINUX_EXCPT(snd_pcm_hw_params_set_rate(renderPcm, pcmHwParams, renderFormat.sampleRate, 0), this, "LinuxAudio::InitializeRender", "Unsupported sample rate.");
			LINUX_EXCPT(snd_pcm_hw_params_set_buffer_size(renderPcm, pcmHwParams, renderFormat.sampleRate * 0.05), this, "LinuxAudio::InitializeRender", "An error occurred whilst setting the pcm buffer size.");
			LINUX_EXCPT(snd_pcm_hw_params(renderPcm, pcmHwParams), this, "LinuxAudio::InitializeRender", "An error occurred whilst configuring the pcm hw params.");
			snd_pcm_hw_params_free(pcmHwParams);

			isRenderInitialized = true;
			renderThread = std::thread(&LinuxAudio::RenderData, this);

			HEPHAUDIO_LOG("Render initialized in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
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
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			int result;
			snd_pcm_hw_params_t* pcmHwParams;

			StopCapturing();

			captureDeviceId = device != nullptr ? device->id : "default";
			captureFormat = format;

			LINUX_EXCPT(snd_pcm_open(&capturePcm, captureDeviceId.c_str(), SND_PCM_STREAM_CAPTURE, 0), this, "LinuxAudio::InitializeCapture", "An error occurred whilst opening pcm.");

			LINUX_EXCPT(snd_pcm_hw_params_malloc(&pcmHwParams), this, "LinuxAudio::InitializeCapture", "An error occurred whilst allocating space for pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_any(capturePcm, pcmHwParams), this, "LinuxAudio::InitializeCapture", "An error occurred whilst reading the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_access(capturePcm, pcmHwParams, SND_PCM_ACCESS_RW_INTERLEAVED), this, "LinuxAudio::InitializeCapture", "An error occurred whilst setting access to the pcm hw params.");
			LINUX_EXCPT(snd_pcm_hw_params_set_format(capturePcm, pcmHwParams, ToPcmFormat(captureFormat)), this, "LinuxAudio::InitializeCapture", "Unsupported bps.");
			LINUX_EXCPT(snd_pcm_hw_params_set_channels(capturePcm, pcmHwParams, captureFormat.channelCount), this, "LinuxAudio::InitializeCapture", "Unsupported channel count.");
			LINUX_EXCPT(snd_pcm_hw_params_set_rate(capturePcm, pcmHwParams, captureFormat.sampleRate, 0), this, "LinuxAudio::InitializeCapture", "Unsupported sample rate.");
			LINUX_EXCPT(snd_pcm_hw_params_set_buffer_size(capturePcm, pcmHwParams, captureFormat.sampleRate * 0.05), this, "LinuxAudio::InitializeCapture", "An error occurred whilst setting the pcm buffer size.");
			LINUX_EXCPT(snd_pcm_hw_params(capturePcm, pcmHwParams), this, "LinuxAudio::InitializeCapture", "An error occurred whilst configuring the pcm hw params.");
			snd_pcm_hw_params_free(pcmHwParams);

			isCaptureInitialized = true;
			captureThread = std::thread(&LinuxAudio::CaptureData, this);

			HEPHAUDIO_LOG("Capture initialized in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
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
		void LinuxAudio::SetDisplayName(HephCommon::StringBuffer displayName)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "LinuxAudio::SetDisplayName", "LinuxAudio does not support this method."));
		}
		void LinuxAudio::SetIconPath(HephCommon::StringBuffer iconPath)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "LinuxAudio::SetIconPath", "LinuxAudio does not support this method."));
		}
		bool LinuxAudio::EnumerateAudioDevices()
		{
			snd_pcm_stream_t streamTypes[2] = { SND_PCM_STREAM_PLAYBACK, SND_PCM_STREAM_CAPTURE };
			snd_ctl_card_info_t* cardInfo;
			snd_pcm_info_t* pcmInfo;
			int cardID = -1, deviceID = -1;
			int result;

			this->audioDevices.clear();

			LINUX_ENUMERATE_DEVICE_EXCPT(snd_card_next(&cardID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst getting the next card.");
			LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_card_info_malloc(&cardInfo), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst allocating memory for card info.");
			LINUX_ENUMERATE_DEVICE_EXCPT(snd_pcm_info_malloc(&pcmInfo), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst allocating memory for device info.");
			while (cardID != -1)
			{
				if (snd_card_load(cardID)) // check if the card is available
				{
					StringBuffer cardName = "hw:" + StringBuffer::ToString(cardID);
					snd_ctl_t* ctl;

					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_open(&ctl, cardName.c_str(), SND_CTL_READONLY), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst opening the card controls.");
					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_card_info(ctl, cardInfo), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst reading the card info.");
					cardName = snd_ctl_card_info_get_name(cardInfo);

					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_pcm_next_device(ctl, &deviceID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst getting the next device.");
					while (deviceID != -1)
					{
						snd_pcm_info_set_device(pcmInfo, deviceID);
						for (size_t i = 0; i < 2; i++)
						{
							snd_pcm_info_set_stream(pcmInfo, streamTypes[i]);
							if (snd_ctl_pcm_info(ctl, pcmInfo) == SND_OK)
							{
								AudioDevice device;
								device.id = "plughw:" + StringBuffer::ToString(cardID) + ',' + StringBuffer::ToString(deviceID);
								device.name = cardName + " " + snd_pcm_info_get_name(pcmInfo);
								device.type = i == 1 ? AudioDeviceType::Capture : AudioDeviceType::Render;
								device.isDefault = (cardID == 0 && deviceID == 0); // "plughw:0,0" is the default device
								this->audioDevices.push_back(device);
							}
						}

						LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_pcm_next_device(ctl, &deviceID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst getting the next device.");
					}

					snd_ctl_close(ctl);
				}
				LINUX_ENUMERATE_DEVICE_EXCPT(snd_card_next(&cardID), this, "LinuxAudio::EnumerateAudioDevices", "An error occurred whilst getting the next card.");
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
			while (!disposing && isRenderInitialized)
			{
				availableFrameCount = snd_pcm_avail_update(renderPcm);
				if (availableFrameCount >= buffer.FrameCount())
				{
					Mix(buffer, buffer.FrameCount());
					snd_pcm_writei(renderPcm, buffer.Begin(), buffer.FrameCount());
				}
			}
		}
		void LinuxAudio::CaptureData()
		{
			AudioBuffer buffer(captureFormat.sampleRate * 0.01f, captureFormat);
			snd_pcm_uframes_t availableFrameCount;
			int result;

			LINUX_RENDER_CAPTURE_EXCPT(snd_pcm_start(capturePcm), this, "LinuxAudio", "Failed to start capturing.");
			while (!disposing && isCaptureInitialized)
			{
				if (!isCapturePaused && OnCapture)
				{
					availableFrameCount = snd_pcm_avail_update(capturePcm);
					if (availableFrameCount >= buffer.FrameCount())
					{
						snd_pcm_readi(capturePcm, buffer.Begin(), buffer.FrameCount());

						AudioBuffer temp(buffer.FrameCount(), captureFormat);
						memcpy(temp.Begin(), buffer.Begin(), temp.Size());
						AudioProcessor::ConvertToInnerFormat(temp);
						AudioCaptureEventArgs captureEventArgs = AudioCaptureEventArgs(this, temp);
						OnCapture(&captureEventArgs, nullptr);
					}
				}
			}
		}
		snd_pcm_format_t LinuxAudio::ToPcmFormat(const AudioFormatInfo& format) const noexcept
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
				break;
			}
			return snd_pcm_format_t::SND_PCM_FORMAT_S16;
		}
	}
}
#endif