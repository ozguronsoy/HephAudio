#if defined(__linux__) && !defined(__ANDROID__)
#include "NativeAudio/LinuxAudio.h"
#include "ConsoleLogger.h"
#include "Stopwatch.h"
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
			HEPH_SW_RESET;
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

			HEPHAUDIO_LOG("LinuxAudio destructed in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void LinuxAudio::SetMasterVolume(double volume)
		{
			if (isRenderInitialized && renderDeviceId != "")
			{
				snd_mixer_t* mixer;
				int result;
				const std::string cardID = renderDeviceId == "default" ? "hw:0" : ("hw:" + StringHelpers::Split(StringHelpers::Split(renderDeviceId, ",")[0], ":")[1]);

				LINUX_EXCPT(snd_mixer_open(&mixer, 0), this, HEPH_FUNC, "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_attach(mixer, cardID.c_str()), this, HEPH_FUNC, "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_selem_register(mixer, nullptr, nullptr), this, HEPH_FUNC, "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_load(mixer), this, HEPH_FUNC, "An error occurred while loading the mixer.");

				snd_mixer_elem_t* mixerElem = snd_mixer_first_elem(mixer);
				if (mixerElem == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "No mixer element found."));
				}

				if (!snd_mixer_selem_has_playback_volume(mixerElem))
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "No mixer playback volume element found."));
				}

				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_range(mixerElem, -LinuxAudio::volume_max, LinuxAudio::volume_max), this, HEPH_FUNC, "An error occurred while setting the volume range.");
				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_all(mixerElem, volume * LinuxAudio::volume_max * 2 - LinuxAudio::volume_max), this, HEPH_FUNC, "An error occurred while setting the volume.");
				LINUX_EXCPT(snd_mixer_close(mixer), this, HEPH_FUNC, "An error occurred while closing the mixer.");
			}
		}
		double LinuxAudio::GetMasterVolume() const
		{
			if (isRenderInitialized && renderDeviceId != "")
			{
				snd_mixer_t* mixer;
				int result;
				long value;
				const std::string cardID = renderDeviceId == "default" ? "hw:0" : ("hw:" + StringHelpers::Split(StringHelpers::Split(renderDeviceId, ",")[0], ":")[1]);

				LINUX_EXCPT(snd_mixer_open(&mixer, 0), this, HEPH_FUNC, "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_attach(mixer, cardID.c_str()), this, HEPH_FUNC, "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_selem_register(mixer, nullptr, nullptr), this, HEPH_FUNC, "Failed to open the mixer.");
				LINUX_EXCPT(snd_mixer_load(mixer), this, HEPH_FUNC, "An error occurred while loading the mixer.");

				snd_mixer_elem_t* mixerElem = snd_mixer_first_elem(mixer);
				if (mixerElem == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "No mixer element found."));
				}

				if (!snd_mixer_selem_has_playback_volume(mixerElem))
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "No mixer playback volume element found."));
				}

				LINUX_EXCPT(snd_mixer_selem_set_playback_volume_range(mixerElem, -LinuxAudio::volume_max, LinuxAudio::volume_max), this, HEPH_FUNC, "An error occurred while getting the volume range.");
				LINUX_EXCPT(snd_mixer_selem_get_playback_volume(mixerElem, SND_MIXER_SCHN_MONO, &value), this, HEPH_FUNC, "An error occurred while getting the volume.");
				LINUX_EXCPT(snd_mixer_close(mixer), this, HEPH_FUNC, "An error occurred while closing the mixer.");

				return ((double)value + LinuxAudio::volume_max) / (LinuxAudio::volume_max * 2.0);
			}
			return 1.0;
		}
		void LinuxAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			int result;

			StopRendering();

			renderDeviceId = device != nullptr ? device->id : "default";
			renderFormat = format;

			LINUX_EXCPT(snd_pcm_open(&renderPcm, renderDeviceId.c_str(), SND_PCM_STREAM_PLAYBACK, 0), this, HEPH_FUNC, "An error occurred while opening pcm.");

			const useconds_t bufferDuration = this->params.renderBufferDuration_ms * 1000;
			const useconds_t latency = bufferDuration * 2;

			LINUX_EXCPT(snd_pcm_set_params(renderPcm,
				ToPcmFormat(renderFormat),
				SND_PCM_ACCESS_RW_INTERLEAVED,
				renderFormat.channelLayout.count,
				renderFormat.sampleRate,
				1,
				latency),
				this, HEPH_FUNC, "An error occurred while setting the snd_pcm params");

			snd_pcm_chmap* pcm_chmap = ToPcmChmap(renderFormat);
			LINUX_EXCPT(snd_pcm_set_chmap(renderPcm, pcm_chmap), this, HEPH_FUNC, "An error occurred while setting the channel mapping");
			free(pcm_chmap);

			isRenderInitialized = true;
			renderThread = std::thread(&LinuxAudio::RenderData, this, bufferDuration);

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
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
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			int result;

			StopCapturing();

			captureDeviceId = device != nullptr ? device->id : "default";
			captureFormat = format;

			LINUX_EXCPT(snd_pcm_open(&capturePcm, captureDeviceId.c_str(), SND_PCM_STREAM_CAPTURE, 0), this, HEPH_FUNC, "An error occurred while opening pcm.");

			const useconds_t bufferDuration = this->params.captureBufferDuration_ms * 1000;
			const useconds_t latency = bufferDuration * 2;

			LINUX_EXCPT(snd_pcm_set_params(capturePcm,
				ToPcmFormat(captureFormat),
				SND_PCM_ACCESS_RW_INTERLEAVED,
				captureFormat.channelLayout.count,
				captureFormat.sampleRate,
				1,
				latency),
				this, HEPH_FUNC, "An error occurred while setting the snd_pcm params");

			snd_pcm_chmap* pcm_chmap = ToPcmChmap(captureFormat);
			LINUX_EXCPT(snd_pcm_set_chmap(capturePcm, pcm_chmap), this, HEPH_FUNC, "An error occurred while setting the channel mapping");
			free(pcm_chmap);

			isCaptureInitialized = true;
			captureThread = std::thread(&LinuxAudio::CaptureData, this, bufferDuration);

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
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
		void LinuxAudio::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			AlsaParams* pAlsaParams = dynamic_cast<AlsaParams*>(&nativeParams);
			if (pAlsaParams == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "nativeParams must be a AlsaParams instance."));
				return;
			}
			(*pAlsaParams) = this->params;
		}
		void LinuxAudio::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			const AlsaParams* pAlsaParams = dynamic_cast<const AlsaParams*>(&nativeParams);
			if (pAlsaParams == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "nativeParams must be a AlsaParams instance."));
				return;
			}
			this->params = *pAlsaParams;
		}
		bool LinuxAudio::EnumerateAudioDevices()
		{
			snd_pcm_stream_t streamTypes[2] = { SND_PCM_STREAM_PLAYBACK, SND_PCM_STREAM_CAPTURE };
			snd_ctl_card_info_t* cardInfo;
			snd_pcm_info_t* pcmInfo;
			int cardID = -1, deviceID = -1;
			int result;

			LINUX_ENUMERATE_DEVICE_EXCPT(snd_card_next(&cardID), this, HEPH_FUNC, "An error occurred while getting the next card.");
			LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_card_info_malloc(&cardInfo), this, HEPH_FUNC, "An error occurred while allocating memory for card info.");
			LINUX_ENUMERATE_DEVICE_EXCPT(snd_pcm_info_malloc(&pcmInfo), this, HEPH_FUNC, "An error occurred while allocating memory for device info.");
			while (cardID != -1)
			{
				if (snd_card_load(cardID)) // check if the card is available
				{
					std::string cardName = "hw:" + StringHelpers::ToString(cardID);
					snd_ctl_t* ctl;

					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_open(&ctl, cardName.c_str(), SND_CTL_READONLY), this, HEPH_FUNC, "An error occurred while opening the card controls.");
					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_card_info(ctl, cardInfo), this, HEPH_FUNC, "An error occurred while reading the card info.");
					cardName = snd_ctl_card_info_get_name(cardInfo);

					LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_pcm_next_device(ctl, &deviceID), this, HEPH_FUNC, "An error occurred while getting the next device.");
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

						LINUX_ENUMERATE_DEVICE_EXCPT(snd_ctl_pcm_next_device(ctl, &deviceID), this, HEPH_FUNC, "An error occurred while getting the next device.");
					}

					snd_ctl_close(ctl);
				}
				LINUX_ENUMERATE_DEVICE_EXCPT(snd_card_next(&cardID), this, HEPH_FUNC, "An error occurred while getting the next card.");
			}

			snd_ctl_card_info_free(cardInfo);
			snd_pcm_info_free(pcmInfo);

			return NativeAudio::DEVICE_ENUMERATION_SUCCESS;
		}
		void LinuxAudio::RenderData(useconds_t bufferDuration_us)
		{
			const snd_pcm_sframes_t bufferDuration_frame = renderFormat.sampleRate * (bufferDuration_us / 1e6f);
			snd_pcm_sframes_t availableFrameCount;
			snd_pcm_sframes_t writtenFrameCount;
			int result;

			LINUX_RENDER_CAPTURE_EXCPT(snd_pcm_start(renderPcm), this, HEPH_FUNC, "Failed to start rendering.");
			while (snd_pcm_state(renderPcm) != SND_PCM_STATE_RUNNING);
			while (!disposing && isRenderInitialized)
			{
				availableFrameCount = snd_pcm_avail_update(renderPcm);
				if (availableFrameCount >= bufferDuration_frame)
				{
					const EncodedAudioBuffer mixedBuffer = Mix(bufferDuration_frame);
					writtenFrameCount = snd_pcm_writei(renderPcm, mixedBuffer.begin(), bufferDuration_frame);
					if (writtenFrameCount < 0)
					{
						HEPHAUDIO_LOG("An error occurred while rendering, attempting to recover.", HEPH_CL_WARNING);
						result = snd_pcm_recover(renderPcm, writtenFrameCount, 1);
						if (result < 0)
						{
							LINUX_RENDER_CAPTURE_EXCPT(result, this, HEPH_FUNC, "Failed to recover from the render error.");
						}
					}
				}
				usleep(bufferDuration_us / 2);
			}
		}
		void LinuxAudio::CaptureData(useconds_t bufferDuration_us)
		{
			const snd_pcm_sframes_t bufferDuration_frame = captureFormat.sampleRate * (bufferDuration_us / 1e6f);
			snd_pcm_sframes_t availableFrameCount;
			snd_pcm_sframes_t readFrameCount;
			int result;

			LINUX_RENDER_CAPTURE_EXCPT(snd_pcm_start(capturePcm), this, HEPH_FUNC, "Failed to start capturing.");
			while (snd_pcm_state(capturePcm) != SND_PCM_STATE_RUNNING);
			while (!disposing && isCaptureInitialized)
			{
				if (!isCapturePaused)
				{
					availableFrameCount = snd_pcm_avail_update(capturePcm);
					if (availableFrameCount >= bufferDuration_frame)
					{
						EncodedAudioBuffer encodedBuffer(bufferDuration_frame * this->captureFormat.FrameSize(), this->captureFormat);
						readFrameCount = snd_pcm_readi(capturePcm, encodedBuffer.begin(), bufferDuration_frame);
						if (readFrameCount < 0)
						{
							HEPHAUDIO_LOG("An error occurred while capturing, attempting to recover.", HEPH_CL_WARNING);
							result = snd_pcm_recover(capturePcm, readFrameCount, 1);
							if (result < 0)
							{
								LINUX_RENDER_CAPTURE_EXCPT(result, this, HEPH_FUNC, "Failed to recover from the capture error.");
							}
						}
						else
						{
							if (OnCapture)
							{
								AudioBuffer buffer = this->pAudioDecoder->Decode(encodedBuffer);
								AudioCaptureEventArgs captureEventArgs(this, buffer);
								OnCapture(&captureEventArgs, nullptr);
							}
						}
					}
					usleep(bufferDuration_us / 2);
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
		snd_pcm_chmap* LinuxAudio::ToPcmChmap(const AudioFormatInfo& format) const
		{
			snd_pcm_chmap* pcm_chmap = (snd_pcm_chmap*)malloc(sizeof(int) * (format.channelLayout.count + 1));
			pcm_chmap->channels = renderFormat.channelLayout.count;

			const std::vector<AudioChannelMask> channelMapping = AudioChannelLayout::GetChannelMapping(renderFormat.channelLayout);
			for (size_t i = 0; i < channelMapping.size(); i++)
			{
				switch (channelMapping[i])
				{
				case AudioChannelMask::FrontLeft:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_FL;
					break;
				case AudioChannelMask::FrontRight:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_FR;
					break;
				case AudioChannelMask::FrontCenter:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_FC;
					break;
				case AudioChannelMask::LowFrequency:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_LFE;
					break;
				case AudioChannelMask::BackLeft:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_RL;
					break;
				case AudioChannelMask::BackRight:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_RR;
					break;
				case AudioChannelMask::FrontLeftOfCenter:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_FLC;
					break;
				case AudioChannelMask::FrontRightOfCenter:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_FRC;
					break;
				case AudioChannelMask::BackCenter:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_RC;
					break;
				case AudioChannelMask::SideLeft:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_SL;
					break;
				case AudioChannelMask::SideRight:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_SR;
					break;
				case AudioChannelMask::TopCenter:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_TC;
					break;
				case AudioChannelMask::TopFrontLeft:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_TFL;
					break;
				case AudioChannelMask::TopFrontCenter:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_TFC;
					break;
				case AudioChannelMask::TopFrontRight:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_TFR;
					break;
				case AudioChannelMask::TopBackLeft:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_TRL;
					break;
				case AudioChannelMask::TopBackCenter:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_TRC;
					break;
				case AudioChannelMask::TopBackRight:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_TRR;
					break;
				default:
					pcm_chmap->pos[i] = snd_pcm_chmap_position::SND_CHMAP_UNKNOWN;
					break;
				}
			}

			return pcm_chmap;
		}
	}
}
#endif