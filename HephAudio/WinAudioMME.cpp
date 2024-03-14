#if defined(_WIN32)
#include "WinAudioMME.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
#include <VersionHelpers.h>

#define MAX_STOP_WAIT 200
#define HAS_FORMAT_TAG(formatTag) ((dwFormats & formatTag) == formatTag)
#define WINAUDIOMME_EXCPT(mmr, winAudioMME, method, message) mmres = mmr; if(mmres != MMSYSERR_NOERROR) { RAISE_AND_THROW_HEPH_EXCEPTION(winAudioMME, HephException(mmres, method, message, "MMEAPI", WinAudioMME::GetErrorString(mmres))); }
#define WINAUDIOMME_RENDER_EXCPT(mmr, winAudioMME, method, message) mmres = mmr; if(mmres != MMSYSERR_NOERROR) { RAISE_HEPH_EXCEPTION(winAudioMME, HephException(mmres, method, message, "MMEAPI", WinAudioMME::GetErrorString(mmres))); goto RENDER_EXIT; }
#define WINAUDIOMME_ENUMERATION_CALLBACK_EXCPT(mmr, winAudioMME, method, message) mmres = mmr; if(mmres != MMSYSERR_NOERROR) { RAISE_HEPH_EXCEPTION(winAudioMME, HephException(mmres, method, message, "MMEAPI", WinAudioMME::GetErrorString(mmres))); return NativeAudio::DEVICE_ENUMERATION_FAIL; }

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		WinAudioMME::WinAudioMME() : WinAudioBase(), hwo(nullptr), hwi(nullptr)
		{
			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&WinAudioMME::CheckAudioDevices, this);
		}
		WinAudioMME::~WinAudioMME()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing WinAudioMME...", HEPH_CL_INFO);

			this->disposing = true;

			this->JoinDeviceThread();

			this->StopRendering();
			this->StopCapturing();

			HEPHAUDIO_LOG("WinAudioMME destructed in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioMME::SetMasterVolume(heph_float volume)
		{
			if (this->isRenderInitialized)
			{
				const uint16_t usv = volume * UINT16_MAX;
				MMRESULT mmres;
				WINAUDIOMME_EXCPT(waveOutSetVolume(this->hwo, (usv << 16) | usv), this, "WinAudioMME::GetMasterVolume", "An error occurred whilst setting the master volume");
			}
		}
		heph_float WinAudioMME::GetMasterVolume() const
		{
			if (this->isRenderInitialized)
			{
				DWORD dv;
				MMRESULT mmres;
				WINAUDIOMME_EXCPT(waveOutGetVolume(this->hwo, &dv), this, "WinAudioMME::GetMasterVolume", "An error occurred whilst getting the master volume");
				return (heph_float)(dv & 0x0000FFFF) / (heph_float)UINT16_MAX;
			}
			return -1.0;
		}
		void WinAudioMME::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopRendering();

			MMRESULT mmres = MMSYSERR_NOERROR;

			const UINT deviceID = device == nullptr ? 0 : StringBuffer::StringToU32(device->id);
			this->renderDeviceId = StringBuffer::ToString(deviceID);
			if (deviceID >= waveOutGetNumDevs())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "WinAudioMME::InitializeRender", "Render device not found."));
			}

			WAVEOUTCAPS deviceCaps{ 0 };
			WINAUDIOMME_EXCPT(waveOutGetDevCaps(deviceID, &deviceCaps, sizeof(WAVEOUTCAPS)), this, "WinAudioMME::InitializeRender", "An error occurred whilst getting the render device caps.");

			this->renderFormat = this->GetClosestFormat(deviceCaps.dwFormats, format);
			if (this->renderFormat.channelCount > deviceCaps.wChannels || this->renderFormat.channelCount == 0)
			{
				this->renderFormat.channelCount = deviceCaps.wChannels;
			}
			const WAVEFORMATEX wfx = this->AFI2WFX(this->renderFormat);

			WINAUDIOMME_EXCPT(waveOutOpen(&this->hwo, deviceID, &wfx, (DWORD_PTR)&WinAudioMME::RenderCallback, (DWORD_PTR)this, CALLBACK_FUNCTION), this, "WinAudioMME::InitializeRender", "An error occurred whilst openning the render device.");

			const size_t bufferSize_byte = WinAudioMME::CalculateBufferSize(this->renderFormat.ByteRate(), this->renderFormat.sampleRate);
			for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
			{
				this->renderHdrs[i] = { 0 };
				this->renderHdrs[i].dwBufferLength = bufferSize_byte;
				this->renderHdrs[i].dwUser = i;
				this->renderHdrs[i].dwFlags = WHDR_PREPARED;
				this->renderHdrs[i].lpData = (LPSTR)malloc(bufferSize_byte);
				if (this->renderHdrs[i].lpData == nullptr)
				{
					for (size_t j = 0; j < i; j++)
					{
						free(this->renderHdrs[j].lpData);
						this->renderHdrs[j].lpData = nullptr;
					}
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "WinAudioMME::InitializeRender", "Insufficient memory."));
				}
				memset(this->renderHdrs[i].lpData, 0, this->renderHdrs[i].dwBufferLength);
			}

			for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
			{
				WINAUDIOMME_EXCPT(waveOutWrite(this->hwo, &this->renderHdrs[i], sizeof(WAVEHDR)), this, "WinAudioMME::InitializeRender", "An error occurred whilst starting render.");
			}

			this->isRenderInitialized = true;

			HEPHAUDIO_LOG("Render initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioMME::StopRendering()
		{
			if (this->isRenderInitialized)
			{
				this->isRenderInitialized = false;
				this->renderDeviceId = "";

				for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
				{
					if (this->renderHdrs[i].lpData != nullptr)
					{
						size_t j = 0;
						while ((this->renderHdrs[i].dwFlags & WHDR_DONE) != WHDR_DONE && j < MAX_STOP_WAIT)
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							j++;
						}
						free(this->renderHdrs[i].lpData);
						this->renderHdrs[i] = { 0 };
					}
				}

				if (this->hwo != nullptr)
				{
					MMRESULT mmres;
					WINAUDIOMME_EXCPT(waveOutClose(this->hwo), this, "WinAudioMME::StopRendering", "An error occurred while closing the waveOut handle.");
					this->hwo = nullptr;
				}

				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void WinAudioMME::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopCapturing();

			MMRESULT mmres = MMSYSERR_NOERROR;

			const UINT deviceID = device == nullptr ? 0 : StringBuffer::StringToU32(device->id);
			this->captureDeviceId = StringBuffer::ToString(deviceID);
			if (deviceID >= waveInGetNumDevs())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "WinAudioMME::InitializeCapture", "Capture device not found."));
			}

			WAVEINCAPS deviceCaps{ 0 };
			WINAUDIOMME_EXCPT(waveInGetDevCaps(deviceID, &deviceCaps, sizeof(WAVEINCAPS)), this, "WinAudioMME::InitializeCapture", "An error occurred whilst getting the capture device caps.");

			this->captureFormat = this->GetClosestFormat(deviceCaps.dwFormats, format);
			if (this->captureFormat.channelCount > deviceCaps.wChannels || this->captureFormat.channelCount == 0)
			{
				this->captureFormat.channelCount = deviceCaps.wChannels;
			}
			const WAVEFORMATEX wfx = this->AFI2WFX(this->captureFormat);

			WINAUDIOMME_EXCPT(waveInOpen(&this->hwi, deviceID, &wfx, (DWORD_PTR)&WinAudioMME::CaptureCallback, (DWORD_PTR)this, CALLBACK_FUNCTION), this, "WinAudioMME::InitializeCapture", "An error occurred whilst starting capture.");

			const size_t bufferSize_byte = WinAudioMME::CalculateBufferSize(this->captureFormat.ByteRate(), this->captureFormat.sampleRate);

			for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
			{
				this->captureHdrs[i] = { 0 };
				this->captureHdrs[i].dwBufferLength = bufferSize_byte;
				this->captureHdrs[i].dwUser = i;
				this->captureHdrs[i].dwFlags = WHDR_PREPARED;
				this->captureHdrs[i].lpData = (LPSTR)malloc(bufferSize_byte);
				if (this->captureHdrs[i].lpData == nullptr)
				{
					for (size_t j = 0; j < i; j++)
					{
						free(this->captureHdrs[j].lpData);
						this->captureHdrs[j].lpData = nullptr;
					}
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "WinAudioMME::InitializeCapture", "Insufficient memory."));
				}
				memset(this->captureHdrs[i].lpData, 0, this->captureHdrs[i].dwBufferLength);
			}

			for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
			{
				WINAUDIOMME_EXCPT(waveInAddBuffer(this->hwi, &this->captureHdrs[i], sizeof(WAVEHDR)), this, "WinAudioMME::InitializeCapture", "An error occurred whilst starting capture.");
			}

			WINAUDIOMME_EXCPT(waveInStart(this->hwi), this, "WinAudioMME::InitializeCapture", "An error occurred whilst starting capture.");

			this->isCaptureInitialized = true;

			HEPHAUDIO_LOG("Capture initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioMME::StopCapturing()
		{
			if (this->isCaptureInitialized)
			{
				this->isCaptureInitialized = false;
				this->captureDeviceId = "";

				for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
				{
					if (this->captureHdrs[i].lpData != nullptr)
					{
						size_t j = 0;
						while ((this->captureHdrs[i].dwFlags & WHDR_DONE) != WHDR_DONE && j < MAX_STOP_WAIT)
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							j++;
						}
						free(this->captureHdrs[i].lpData);
						this->captureHdrs[i] = { 0 };
					}
				}

				if (this->hwi != nullptr)
				{
					MMRESULT mmres;
					WINAUDIOMME_EXCPT(waveInClose(this->hwi), this, "WinAudioMME::StopCapturing", "An error occurred while closing the waveIn handle.");
					this->hwi = nullptr;
				}

				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		bool WinAudioMME::EnumerateAudioDevices()
		{
			MMRESULT mmres = MMSYSERR_NOERROR;
			UINT deviceCount = waveOutGetNumDevs();
			for (size_t i = 0; i < deviceCount; i++)
			{
				WAVEOUTCAPS deviceCaps{ 0 };
				WINAUDIOMME_ENUMERATION_CALLBACK_EXCPT(waveOutGetDevCaps(i, &deviceCaps, sizeof(WAVEOUTCAPS)), this, "WinAudioMME", "An error occurred whilst enumerating render devices.");

				AudioDevice device;
				device.id = StringBuffer::ToString(i);
				device.name = deviceCaps.szPname;
				device.type = AudioDeviceType::Render;
				device.isDefault = (i == 0);

				this->audioDevices.push_back(device);
			}

			deviceCount = waveInGetNumDevs();
			for (size_t i = 0; i < deviceCount; i++)
			{
				WAVEINCAPS deviceCaps{ 0 };
				WINAUDIOMME_ENUMERATION_CALLBACK_EXCPT(waveInGetDevCaps(i, &deviceCaps, sizeof(WAVEINCAPS)), this, "WinAudioMME", "An error occurred whilst enumerating capture devices.");

				AudioDevice device;
				device.id = StringBuffer::ToString(i);
				device.name = deviceCaps.szPname;
				device.type = AudioDeviceType::Render;
				device.isDefault = (i == 0);

				this->audioDevices.push_back(device);
			}

			return NativeAudio::DEVICE_ENUMERATION_SUCCESS;
		}
		AudioFormatInfo WinAudioMME::GetClosestFormat(DWORD dwFormats, const AudioFormatInfo& format) const
		{
			AudioFormatInfo closestFormat = format;
			closestFormat.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;

			if (format.bitsPerSample > 8)
			{
				if (HAS_FORMAT_TAG(WAVE_FORMAT_96M16) || HAS_FORMAT_TAG(WAVE_FORMAT_96S16)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_48M16) || HAS_FORMAT_TAG(WAVE_FORMAT_48S16)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_4M16) || HAS_FORMAT_TAG(WAVE_FORMAT_4S16)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_2M16) || HAS_FORMAT_TAG(WAVE_FORMAT_2S16)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_1M16) || HAS_FORMAT_TAG(WAVE_FORMAT_1S16))
				{
					closestFormat.bitsPerSample = 16;
				}
				else
				{
					closestFormat.bitsPerSample = 8;
				}
			}
			else
			{
				if (HAS_FORMAT_TAG(WAVE_FORMAT_96M08) || HAS_FORMAT_TAG(WAVE_FORMAT_96S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_48M08) || HAS_FORMAT_TAG(WAVE_FORMAT_48S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_4M08) || HAS_FORMAT_TAG(WAVE_FORMAT_4S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_2M08) || HAS_FORMAT_TAG(WAVE_FORMAT_2S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_1M08) || HAS_FORMAT_TAG(WAVE_FORMAT_1S08))
				{
					closestFormat.bitsPerSample = 8;
				}
				else
				{
					closestFormat.bitsPerSample = 16;
				}
			}

			bool retry = false;
		CLOSEST_SAMPLE_RATE:

			if (format.sampleRate >= 96000)
			{
				if (HAS_FORMAT_TAG(WAVE_FORMAT_96S16) || HAS_FORMAT_TAG(WAVE_FORMAT_96S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_96M16) || HAS_FORMAT_TAG(WAVE_FORMAT_96M08))
				{
					closestFormat.sampleRate = 96000;
					return closestFormat;
				}
				else
				{
					closestFormat.sampleRate = 48000;
				}
			}

			if (format.sampleRate >= 48000)
			{
				if (HAS_FORMAT_TAG(WAVE_FORMAT_48S16) || HAS_FORMAT_TAG(WAVE_FORMAT_48S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_48M16) || HAS_FORMAT_TAG(WAVE_FORMAT_48M08))
				{
					closestFormat.sampleRate = 48000;
					return closestFormat;
				}
				else
				{
					closestFormat.sampleRate = 44100;
				}
			}

			if (format.sampleRate >= 44100)
			{
				if (HAS_FORMAT_TAG(WAVE_FORMAT_4S16) || HAS_FORMAT_TAG(WAVE_FORMAT_4S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_4M16) || HAS_FORMAT_TAG(WAVE_FORMAT_4M08))
				{
					closestFormat.sampleRate = 44100;
					return closestFormat;
				}
				else
				{
					closestFormat.sampleRate = 22050;
				}
			}

			if (format.sampleRate >= 22050)
			{
				if (HAS_FORMAT_TAG(WAVE_FORMAT_2S16) || HAS_FORMAT_TAG(WAVE_FORMAT_2S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_2M16) || HAS_FORMAT_TAG(WAVE_FORMAT_2M08))
				{
					closestFormat.sampleRate = 22050;
					return closestFormat;
				}
				else
				{
					closestFormat.sampleRate = 11025;
				}
			}

			if (format.sampleRate >= 11025)
			{
				if (HAS_FORMAT_TAG(WAVE_FORMAT_1S16) || HAS_FORMAT_TAG(WAVE_FORMAT_1S08)
					|| HAS_FORMAT_TAG(WAVE_FORMAT_1M16) || HAS_FORMAT_TAG(WAVE_FORMAT_1M08))
				{
					closestFormat.sampleRate = 11025;
					return closestFormat;
				}
				else
				{
					if (retry)
					{
						MMRESULT mmres;
						WINAUDIOMME_EXCPT(WAVERR_BADFORMAT, this, "WinAudioMME", "Could not find a supported sample rate.");
					}

					closestFormat.sampleRate = 96000;
					retry = true;
					goto CLOSEST_SAMPLE_RATE; // retry once with all possible sample rates
				}
			}

			return closestFormat;
		}
		size_t WinAudioMME::CalculateBufferSize(uint32_t byteRate, uint32_t sampleRate)
		{
			switch (sampleRate)
			{
			case 22050:
				return byteRate / 10; // 100 ms
			case 11025:
				return byteRate * 0.8; // 800 ms
			default:
				return byteRate / 20; // 50 ms
			}
		}
		void CALLBACK WinAudioMME::RenderCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
		{
			if (uMsg == WOM_DONE)
			{
				WinAudioMME* pAudio = (WinAudioMME*)dwInstance;
				if (!pAudio->disposing && pAudio->isRenderInitialized)
				{
					WAVEHDR* pwhd = (WAVEHDR*)dwParam1;
					AudioBuffer buffer(pwhd->dwBufferLength / pAudio->renderFormat.FrameSize(), pAudio->renderFormat);

					pAudio->Mix(buffer, buffer.FrameCount());
					memcpy(pwhd->lpData, buffer.Begin(), pwhd->dwBufferLength);

					pwhd->dwFlags = WHDR_PREPARED;
					const MMRESULT mmres = waveOutWrite(hwo, pwhd, sizeof(WAVEHDR));
					if (mmres != MMSYSERR_NOERROR)
					{
						RAISE_HEPH_EXCEPTION(pAudio, HephException(mmres, "WinAudioMME", "An error occurred whilst rendering.", "MMEAPI", WinAudioMME::GetErrorString(mmres)));
					}
				}
			}
		}
		void CALLBACK WinAudioMME::CaptureCallback(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
		{
			if (uMsg == WIM_DATA)
			{
				WinAudioMME* pAudio = (WinAudioMME*)dwInstance;
				if (!pAudio->disposing && pAudio->isCaptureInitialized)
				{
					WAVEHDR* pwhd = (WAVEHDR*)dwParam1;

					if (!pAudio->isCapturePaused && pAudio->OnCapture)
					{
						AudioBuffer buffer(pwhd->dwBufferLength / pAudio->captureFormat.FrameSize(), pAudio->captureFormat);
						memcpy(buffer.Begin(), pwhd->lpData, pwhd->dwBufferLength);
						AudioProcessor::ConvertToInnerFormat(buffer);
						AudioCaptureEventArgs captureEventArgs(pAudio, buffer);
						pAudio->OnCapture(&captureEventArgs, nullptr);
					}

					pwhd->dwFlags = WHDR_PREPARED;
					const MMRESULT mmres = waveInAddBuffer(hwi, pwhd, sizeof(WAVEHDR));
					if (mmres != MMSYSERR_NOERROR)
					{
						RAISE_HEPH_EXCEPTION(pAudio, HephException(mmres, "WinAudioMME", "An error occurred whilst capturing.", "MMEAPI", WinAudioMME::GetErrorString(mmres)));
					}
				}
			}
		}
		StringBuffer WinAudioMME::GetErrorString(MMRESULT mmResult)
		{
			wchar_t errorMessage[MAXERRORLENGTH]{ };
			if (waveOutGetErrorText(mmResult, errorMessage, MAXERRORLENGTH) != MMSYSERR_NOERROR)
			{
				return L"error message not found";
			}
			return errorMessage;
		}
	}
}
#endif