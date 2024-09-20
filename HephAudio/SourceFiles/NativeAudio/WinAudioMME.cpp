#if defined(_WIN32)
#include "NativeAudio/WinAudioMME.h"
#include "Stopwatch.h"
#include "ConsoleLogger.h"
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
			HEPH_SW_RESET;
			HEPHAUDIO_LOG("Destructing WinAudioMME...", HEPH_CL_INFO);

			this->disposing = true;

			this->JoinDeviceThread();

			this->StopRendering();
			this->StopCapturing();

			HEPHAUDIO_LOG("WinAudioMME destructed in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioMME::SetMasterVolume(double volume)
		{
			if (this->isRenderInitialized)
			{
				const uint16_t usv = volume * UINT16_MAX;
				MMRESULT mmres;
				WINAUDIOMME_EXCPT(waveOutSetVolume(this->hwo, (usv << 16) | usv), this, HEPH_FUNC, "An error occurred while setting the master volume");
			}
		}
		double WinAudioMME::GetMasterVolume() const
		{
			if (this->isRenderInitialized)
			{
				DWORD dv;
				MMRESULT mmres;
				WINAUDIOMME_EXCPT(waveOutGetVolume(this->hwo, &dv), this, HEPH_FUNC, "An error occurred while getting the master volume");
				return (double)(dv & 0x0000FFFF) / (double)UINT16_MAX;
			}
			return -1.0;
		}
		void WinAudioMME::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopRendering();

			MMRESULT mmres = MMSYSERR_NOERROR;

			const UINT deviceID = device == nullptr ? 0 : StringHelpers::StringToU32(device->id);
			this->renderDeviceId = StringHelpers::ToString(deviceID);
			if (deviceID >= waveOutGetNumDevs())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "Render device not found."));
			}

			WAVEOUTCAPS deviceCaps{ 0 };
			WINAUDIOMME_EXCPT(waveOutGetDevCaps(deviceID, &deviceCaps, sizeof(WAVEOUTCAPS)), this, HEPH_FUNC, "An error occurred while getting the render device caps.");

			this->renderFormat = this->GetClosestFormat(deviceCaps.dwFormats, format);
			if (this->renderFormat.channelLayout.count > deviceCaps.wChannels || this->renderFormat.channelLayout.count == 0)
			{
				this->renderFormat.channelLayout = deviceCaps.wChannels == 2 ? HEPHAUDIO_CH_LAYOUT_STEREO : HEPHAUDIO_CH_LAYOUT_MONO;
			}
			const WAVEFORMATEXTENSIBLE wfx = this->AFI2WFX(this->renderFormat);

			WINAUDIOMME_EXCPT(waveOutOpen(&this->hwo, deviceID, (WAVEFORMATEX*)&wfx, (DWORD_PTR)&WinAudioMME::RenderCallback, (DWORD_PTR)this, CALLBACK_FUNCTION), this, HEPH_FUNC, "An error occurred while openning the render device.");

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
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, HEPH_FUNC, "Insufficient memory."));
				}
				memset(this->renderHdrs[i].lpData, 0, this->renderHdrs[i].dwBufferLength);
			}

			for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
			{
				WINAUDIOMME_EXCPT(waveOutWrite(this->hwo, &this->renderHdrs[i], sizeof(WAVEHDR)), this, HEPH_FUNC, "An error occurred while starting render.");
			}

			this->isRenderInitialized = true;

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
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
					WINAUDIOMME_EXCPT(waveOutClose(this->hwo), this, HEPH_FUNC, "An error occurred while closing the waveOut handle.");
					this->hwo = nullptr;
				}

				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void WinAudioMME::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopCapturing();

			MMRESULT mmres = MMSYSERR_NOERROR;

			const UINT deviceID = device == nullptr ? 0 : StringHelpers::StringToU32(device->id);
			this->captureDeviceId = StringHelpers::ToString(deviceID);
			if (deviceID >= waveInGetNumDevs())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "Capture device not found."));
			}

			WAVEINCAPS deviceCaps{ 0 };
			WINAUDIOMME_EXCPT(waveInGetDevCaps(deviceID, &deviceCaps, sizeof(WAVEINCAPS)), this, HEPH_FUNC, "An error occurred while getting the capture device caps.");

			this->captureFormat = this->GetClosestFormat(deviceCaps.dwFormats, format);
			if (this->captureFormat.channelLayout.count > deviceCaps.wChannels || this->captureFormat.channelLayout.count == 0)
			{
				this->captureFormat.channelLayout = deviceCaps.wChannels == 2 ? HEPHAUDIO_CH_LAYOUT_STEREO : HEPHAUDIO_CH_LAYOUT_MONO;
			}
			const WAVEFORMATEXTENSIBLE wfx = this->AFI2WFX(this->captureFormat);

			WINAUDIOMME_EXCPT(waveInOpen(&this->hwi, deviceID, (WAVEFORMATEX*)&wfx, (DWORD_PTR)&WinAudioMME::CaptureCallback, (DWORD_PTR)this, CALLBACK_FUNCTION), this, HEPH_FUNC, "An error occurred while starting capture.");

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
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, HEPH_FUNC, "Insufficient memory."));
				}
				memset(this->captureHdrs[i].lpData, 0, this->captureHdrs[i].dwBufferLength);
			}

			for (size_t i = 0; i < WinAudioMME::HDR_COUNT; i++)
			{
				WINAUDIOMME_EXCPT(waveInAddBuffer(this->hwi, &this->captureHdrs[i], sizeof(WAVEHDR)), this, HEPH_FUNC, "An error occurred while starting capture.");
			}

			WINAUDIOMME_EXCPT(waveInStart(this->hwi), this, HEPH_FUNC, "An error occurred while starting capture.");

			this->isCaptureInitialized = true;

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
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
					WINAUDIOMME_EXCPT(waveInClose(this->hwi), this, HEPH_FUNC, "An error occurred while closing the waveIn handle.");
					this->hwi = nullptr;
				}

				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void WinAudioMME::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_OPERATION, HEPH_FUNC, "Native params not supported."));
		}
		void WinAudioMME::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_OPERATION, HEPH_FUNC, "Native params not supported."));
		}
		bool WinAudioMME::EnumerateAudioDevices()
		{
			MMRESULT mmres = MMSYSERR_NOERROR;
			UINT deviceCount = waveOutGetNumDevs();
			for (size_t i = 0; i < deviceCount; i++)
			{
				WAVEOUTCAPS deviceCaps{ 0 };
				WINAUDIOMME_ENUMERATION_CALLBACK_EXCPT(waveOutGetDevCaps(i, &deviceCaps, sizeof(WAVEOUTCAPS)), this, HEPH_FUNC, "An error occurred while enumerating render devices.");

				AudioDevice device;
				device.id = StringHelpers::ToString(i);

				if (sizeof(TCHAR) == sizeof(wchar_t))
				{
					device.name = StringHelpers::WideToStr((wchar_t*)deviceCaps.szPname);
				}
				else
				{
					device.name = (char*)deviceCaps.szPname;
				}
				
				device.type = AudioDeviceType::Render;
				device.isDefault = (i == 0);

				this->audioDevices.push_back(device);
			}

			deviceCount = waveInGetNumDevs();
			for (size_t i = 0; i < deviceCount; i++)
			{
				WAVEINCAPS deviceCaps{ 0 };
				WINAUDIOMME_ENUMERATION_CALLBACK_EXCPT(waveInGetDevCaps(i, &deviceCaps, sizeof(WAVEINCAPS)), this, HEPH_FUNC, "An error occurred while enumerating capture devices.");

				AudioDevice device;
				device.id = StringHelpers::ToString(i);

				if (sizeof(TCHAR) == sizeof(wchar_t))
				{
					device.name = StringHelpers::WideToStr((wchar_t*)deviceCaps.szPname);
				}
				else
				{
					device.name = (char*)deviceCaps.szPname;
				}

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
					closestFormat.bitRate = AudioFormatInfo::CalculateBitrate(closestFormat);
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
					closestFormat.bitRate = AudioFormatInfo::CalculateBitrate(closestFormat);
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
					closestFormat.bitRate = AudioFormatInfo::CalculateBitrate(closestFormat);
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
					closestFormat.bitRate = AudioFormatInfo::CalculateBitrate(closestFormat);
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
					closestFormat.bitRate = AudioFormatInfo::CalculateBitrate(closestFormat);
					return closestFormat;
				}
				else
				{
					if (retry)
					{
						MMRESULT mmres;
						WINAUDIOMME_EXCPT(WAVERR_BADFORMAT, this, HEPH_FUNC, "Could not find a supported sample rate.");
					}

					closestFormat.sampleRate = 96000;
					retry = true;
					goto CLOSEST_SAMPLE_RATE; // retry once with all possible sample rates
				}
			}

			closestFormat.bitRate = AudioFormatInfo::CalculateBitrate(closestFormat);
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

					EncodedAudioBuffer mixedBuffer = pAudio->Mix(pwhd->dwBufferLength / pAudio->renderFormat.FrameSize());
					memcpy(pwhd->lpData, mixedBuffer.begin(), pwhd->dwBufferLength);

					pwhd->dwFlags = WHDR_PREPARED;
					const MMRESULT mmres = waveOutWrite(hwo, pwhd, sizeof(WAVEHDR));
					if (mmres != MMSYSERR_NOERROR)
					{
						RAISE_HEPH_EXCEPTION(pAudio, HephException(mmres, HEPH_FUNC, "An error occurred while rendering.", "MMEAPI", WinAudioMME::GetErrorString(mmres)));
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
						EncodedAudioBuffer encodedBuffer((const uint8_t*)pwhd->lpData, pwhd->dwBufferLength, pAudio->captureFormat);
						AudioBuffer buffer = pAudio->pAudioDecoder->Decode(encodedBuffer);
						
						AudioCaptureEventArgs captureEventArgs(pAudio, buffer);
						pAudio->OnCapture(&captureEventArgs, nullptr);
					}

					pwhd->dwFlags = WHDR_PREPARED;
					const MMRESULT mmres = waveInAddBuffer(hwi, pwhd, sizeof(WAVEHDR));
					if (mmres != MMSYSERR_NOERROR)
					{
						RAISE_HEPH_EXCEPTION(pAudio, HephException(mmres, HEPH_FUNC, "An error occurred while capturing.", "MMEAPI", WinAudioMME::GetErrorString(mmres)));
					}
				}
			}
		}
		std::string WinAudioMME::GetErrorString(MMRESULT mmResult)
		{
			char errorMessage[MAXERRORLENGTH]{ };
			if (waveOutGetErrorTextA(mmResult, errorMessage, MAXERRORLENGTH) != MMSYSERR_NOERROR)
			{
				return "error message not found";
			}
			return errorMessage;
		}
	}
}
#endif