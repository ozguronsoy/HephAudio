#ifdef _WIN32
#include "WinAudioDS.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"
#include <VersionHelpers.h>

#pragma comment (lib, "Dsound.lib")
#pragma comment(lib, "Winmm.lib")

#define IID_IDS_NOTIFY {0xB0210783, 0x89CD, 0x11D0, {0xAF, 0x08, 0x00, 0xA0, 0xC9, 0x25, 0xCD, 0x16}}
#define WINAUDIODS_RENDER_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephException(hres, method, message)); goto RENDER_EXIT; }
#define WINAUDIODS_CAPTURE_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephException(hres, method, message)); goto CAPTURE_EXIT; }
#define WINAUDIODS_ENUMERATE_DEVICE_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephException(hres, method, message)); return NativeAudio::DEVICE_ENUMERATION_FAIL; }
#define WINAUDIODS_ENUMERATION_CALLBACK_EXCPT(hr, winAudioDS, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(winAudioDS, HephException(hres, method, message)); return FALSE; }

using namespace HephCommon;
using namespace Microsoft::WRL;

namespace HephAudio
{
	namespace Native
	{
		WinAudioDS::WinAudioDS() : WinAudioBase(), hwnd(nullptr)
		{
			this->InitializeCOM();

			this->hwnd = GetForegroundWindow();
			if (this->hwnd == nullptr)
			{
				this->hwnd = GetDesktopWindow();
			}

			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&WinAudioDS::CheckAudioDevices, this);
		}
		WinAudioDS::~WinAudioDS()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing WinAudioDS...", HEPH_CL_INFO);

			this->disposing = true;

			this->JoinDeviceThread();
			this->JoinRenderThread();
			this->JoinCaptureThread();

			CoUninitialize();

			HEPHAUDIO_LOG("WinAudioDS destructed in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioDS::SetMasterVolume(heph_float volume)
		{
			if (this->isRenderInitialized)
			{
				const uint16_t usv = volume * UINT16_MAX;
				const MMRESULT mmResult = waveOutSetVolume(nullptr, (usv << 16) | usv);
				if (mmResult != MMSYSERR_NOERROR)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(mmResult, "WinAudioDS::SetMasterVolume", "An error occurred whilst setting the master volume"));
				}
			}
		}
		heph_float WinAudioDS::GetMasterVolume() const
		{
			if (this->isRenderInitialized)
			{
				DWORD dv;
				const MMRESULT mmResult = waveOutGetVolume(nullptr, &dv);
				if (mmResult != MMSYSERR_NOERROR)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(mmResult, "WinAudioDS::GetMasterVolume", "An error occurred whilst getting the master volume"));
					return -1.0;
				}
				return (heph_float)(dv & 0x0000FFFF) / (heph_float)UINT16_MAX;
			}
			return -1.0;
		}
		void WinAudioDS::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopRendering();

			this->renderFormat = format;

			GUID deviceId;
			if (device == nullptr || device->type != AudioDeviceType::Render)
			{
				deviceId = WinAudioDS::StringToGuid(this->GetDefaultAudioDevice(AudioDeviceType::Render).id);
			}
			else
			{
				deviceId = WinAudioDS::StringToGuid(device->id);
			}
			this->renderDeviceId = WinAudioDS::GuidToString(&deviceId);

			this->renderThread = std::thread(&WinAudioDS::RenderData, this, deviceId);
			size_t i = 0;
			while (!this->isRenderInitialized && i < 20) // wait for initialization to complete
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				i++;
			}

			HEPHAUDIO_LOG("Render initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioDS::StopRendering()
		{
			if (this->isRenderInitialized)
			{
				this->isRenderInitialized = false;
				this->renderDeviceId = L"";
				this->JoinRenderThread();
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void WinAudioDS::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopCapturing();

			this->captureFormat = format;

			GUID deviceId;
			if (device == nullptr || device->type != AudioDeviceType::Capture)
			{
				deviceId = WinAudioDS::StringToGuid(this->GetDefaultAudioDevice(AudioDeviceType::Capture).id);
			}
			else
			{
				deviceId = WinAudioDS::StringToGuid(device->id);
			}
			this->captureDeviceId = WinAudioDS::GuidToString(&deviceId);

			this->captureThread = std::thread(&WinAudioDS::CaptureData, this, deviceId);
			size_t i = 0;
			while (!this->isCaptureInitialized && i < 20) // wait for initialization to complete
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				i++;
			}

			HEPHAUDIO_LOG("Capture initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioDS::StopCapturing()
		{
			if (this->isCaptureInitialized)
			{
				this->isCaptureInitialized = false;
				this->captureDeviceId = L"";
				this->JoinCaptureThread();
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		bool WinAudioDS::EnumerateAudioDevices()
		{
			HRESULT hres;
			WINAUDIODS_ENUMERATE_DEVICE_EXCPT(DirectSoundEnumerateW(&WinAudioDS::RenderDeviceEnumerationCallback, (void*)this), "WinAudioDS", "An error occurred whilst enumerating render devices.");
			WINAUDIODS_ENUMERATE_DEVICE_EXCPT(DirectSoundCaptureEnumerateW(&WinAudioDS::CaptureDeviceEnumerationCallback, (void*)this), "WinAudioDS", "An error occurred whilst enumerating capture devices.");
			return NativeAudio::DEVICE_ENUMERATION_SUCCESS;
		}
		void WinAudioDS::CheckAudioDevices()
		{
			this->InitializeCOM();
			NativeAudio::CheckAudioDevices();
			CoUninitialize();
		}
		void WinAudioDS::RenderData(GUID deviceId)
		{
			this->InitializeCOM();

			constexpr size_t notificationCount = 4;

			ComPtr<IDirectSound> pDirectSound = nullptr;
			ComPtr<IDirectSoundBuffer> pDirectSoundBuffer = nullptr;
			ComPtr<IDirectSoundNotify> pDirectSoundNotify = nullptr;
			DSCAPS dsCaps{ 0 };
			DSBUFFERDESC bufferDesc{ 0 };
			void* audioPtr1 = nullptr;
			void* audioPtr2 = nullptr;
			DWORD audioBytes1 = 0, audioBytes2 = 0;
			AudioBuffer dataBuffer;
			size_t nFramesToRead;
			HANDLE hEvents[notificationCount]{ nullptr };
			DSBPOSITIONNOTIFY notifyInfos[notificationCount]{ 0 };
			WAVEFORMATEX wfx{ 0 };
			HRESULT hres;

			WINAUDIODS_RENDER_THREAD_EXCPT(DirectSoundCreate(&deviceId, pDirectSound.GetAddressOf(), nullptr), "WinAudioDS::InitializeRender", "An error occurred whilst initializing render.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSound->SetCooperativeLevel(this->hwnd, DSSCL_PRIORITY), "WinAudioDS::InitializeRender", "An error occurred whilst initializing render.");

			dsCaps.dwSize = sizeof(DSCAPS);
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSound->GetCaps(&dsCaps), "WinAudioDS::InitializeRender", "An error occurred whilst reading the direct sound capacity.");
			WinAudioDS::RestrictAudioFormatInfo(this->renderFormat, dsCaps);
			nFramesToRead = this->renderFormat.sampleRate / 100;
			dataBuffer = AudioBuffer(nFramesToRead, this->renderFormat);

			wfx = WinAudioBase::AFI2WFX(this->renderFormat);
			bufferDesc.dwSize = sizeof(DSBUFFERDESC);
			bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
			bufferDesc.dwBufferBytes = this->renderFormat.ByteRate() / 100 * (notificationCount - 1);
			bufferDesc.dwReserved = 0;
			bufferDesc.lpwfxFormat = &wfx;
			bufferDesc.guid3DAlgorithm = GUID_NULL;
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSound->CreateSoundBuffer(&bufferDesc, pDirectSoundBuffer.GetAddressOf(), nullptr), "WinAudioDS::InitializeRender", "An error occurred whilst creating a render buffer.");

			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
				if (hEvents[i] == nullptr)
				{
					WINAUDIODS_RENDER_THREAD_EXCPT(E_FAIL, "WinAudioDS::InitializeRender", "An error occurred whilst setting the render event handles.");
				}

				notifyInfos[i].dwOffset = (i + 1) * dataBuffer.Size() - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[notificationCount - 1].dwOffset = DSBPN_OFFSETSTOP;

			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->QueryInterface(IID_IDS_NOTIFY, (void**)pDirectSoundNotify.GetAddressOf()), "WinAudioDS::InitializeRender", "An error occurred whilst rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), "WinAudioDS::InitializeRender", "An error occurred whilst rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING), "WinAudioDS::InitializeRender", "An error occurred whilst rendering the samples.");

			this->isRenderInitialized = true;

			while (!this->disposing && this->isRenderInitialized)
			{
				bool waitSuccessfull = false;
				const DWORD waitForNotificationResult = WaitForMultipleObjects(notificationCount, hEvents, FALSE, 2000);
				for (size_t i = 0; i < notificationCount; i++)
				{
					if (waitForNotificationResult == WAIT_OBJECT_0 + i)
					{
						waitSuccessfull = true;
						break;
					}
				}
				if (!waitSuccessfull)
				{
					WINAUDIODS_RENDER_THREAD_EXCPT(E_FAIL, "WinAudioDS", "Render time-out.");
				}

				Mix(dataBuffer, nFramesToRead);

				WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Lock(0, dataBuffer.Size(), &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, DSBLOCK_FROMWRITECURSOR), "WinAudioDS", "An error occurred whilst rendering the samples.");
				memcpy(audioPtr1, dataBuffer.Begin(), audioBytes1);
				if (audioPtr2 != nullptr)
				{
					memcpy(audioPtr2, (uint8_t*)dataBuffer.Begin() + audioBytes1, audioBytes2);
				}
				WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), "WinAudioDS", "An error occurred whilst rendering the samples.");

				dataBuffer.Reset();
			}

			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Stop(), "WinAudioDS", "An error occurred whilst rendering the samples.");

		RENDER_EXIT:
			for (size_t i = 0; i < notificationCount; i++)
			{
				if (hEvents[i] != nullptr)
				{
					const BOOL eventResult = CloseHandle(hEvents[i]);
					if (eventResult == FALSE)
					{
						RAISE_HEPH_EXCEPTION(this, HephException(eventResult, "WinAudioDS", "An error occurred whilst closing the render event handles."));
					}
					hEvents[i] = nullptr;
				}
			}
			pDirectSoundNotify = nullptr;
			pDirectSoundBuffer = nullptr;
			pDirectSound = nullptr;
			CoUninitialize();
		}
		void WinAudioDS::CaptureData(GUID deviceId)
		{
			this->InitializeCOM();

			constexpr size_t notificationCount = 2;
			constexpr float bufferDuration_s = 0.3; // 300ms

			ComPtr<IDirectSoundCapture> pDirectSoundCapture = nullptr;
			ComPtr<IDirectSoundCaptureBuffer> pDirectSoundCaptureBuffer = nullptr;
			ComPtr<IDirectSoundNotify> pDirectSoundNotify = nullptr;
			DSCCAPS dscCaps{ 0 };
			DSCBUFFERDESC bufferDesc{ 0 };
			WAVEFORMATEX wfx{ 0 };
			void* audioPtr1 = nullptr;
			void* audioPtr2 = nullptr;
			DWORD audioBytes1, audioBytes2, captureCursor, readCursor;
			size_t nFramesToRead;
			AudioBuffer dataBuffer;
			HANDLE hEvents[notificationCount]{ nullptr };
			DSBPOSITIONNOTIFY notifyInfos[notificationCount]{ 0 };
			HRESULT hres;

			WINAUDIODS_CAPTURE_THREAD_EXCPT(DirectSoundCaptureCreate(&deviceId, pDirectSoundCapture.GetAddressOf(), nullptr), "WinAudioDS::InitializeCapture", "An error occurred whilst initializing capture.");

			dscCaps.dwSize = sizeof(DSCCAPS);
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCapture->GetCaps(&dscCaps), "WinAudioDS::InitializeCapture", "An error occurred whilst reading the direct sound capacity.");
			WinAudioDS::RestrictAudioFormatInfo(this->captureFormat, dscCaps);

			wfx = WinAudioBase::AFI2WFX(this->captureFormat);
			bufferDesc.dwSize = sizeof(DSCBUFFERDESC);
			bufferDesc.dwFlags = 0;
			bufferDesc.dwBufferBytes = this->captureFormat.ByteRate() * bufferDuration_s * (notificationCount - 1);
			bufferDesc.lpwfxFormat = &wfx;
			bufferDesc.dwFXCount = 0;
			bufferDesc.lpDSCFXDesc = nullptr;
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCapture->CreateCaptureBuffer(&bufferDesc, pDirectSoundCaptureBuffer.GetAddressOf(), nullptr), "WinAudioDS::InitializeCapture", "An error occurred whilst creating a capture buffer.");

			nFramesToRead = this->captureFormat.sampleRate * bufferDuration_s;
			dataBuffer = AudioBuffer(nFramesToRead, this->captureFormat);

			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
				if (hEvents[i] == nullptr)
				{
					WINAUDIODS_CAPTURE_THREAD_EXCPT(E_FAIL, "WinAudioDS::InitializeCapture", "An error occurred whilst setting the capture event handles.");
				}
				notifyInfos[i].dwOffset = (i + 1) * dataBuffer.Size() - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[notificationCount - 1].dwOffset = DSBPN_OFFSETSTOP;

			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->QueryInterface(IID_IDS_NOTIFY, (void**)pDirectSoundNotify.GetAddressOf()), "WinAudioDS::InitializeCapture", "An error occurred whilst initializing capture.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), "WinAudioDS::InitializeCapture", "An error occurred whilst initializing capture.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Start(DSCBSTART_LOOPING), "WinAudioDS::InitializeCapture", "An error occurred whilst initializing capture.");

			this->isCaptureInitialized = true;

			while (!this->disposing && this->isCaptureInitialized)
			{
				if (!this->isCapturePaused && this->OnCapture)
				{
					bool waitSuccessfull = false;
					const DWORD waitForNotificationResult = WaitForMultipleObjects(notificationCount, hEvents, FALSE, 2000);
					for (size_t i = 0; i < notificationCount; i++)
					{
						if (waitForNotificationResult == WAIT_OBJECT_0 + i)
						{
							waitSuccessfull = true;
							break;
						}
					}
					if (!waitSuccessfull)
					{
						WINAUDIODS_CAPTURE_THREAD_EXCPT(E_FAIL, "WinAudioDS", "Capture time-out.");
					}

					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->GetCurrentPosition(&captureCursor, &readCursor), "WinAudioDS", "An error occurred whilst capturing the samples.");
					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Lock(readCursor, dataBuffer.Size(), &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0), "WinAudioDS", "An error occurred whilst capturing the samples.");
					memcpy(dataBuffer.Begin(), audioPtr1, audioBytes1);
					if (audioPtr2 != nullptr)
					{
						memcpy((uint8_t*)dataBuffer.Begin() + audioBytes1, audioPtr2, audioBytes2);
					}
					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), "WinAudioDS", "An error occurred whilst capturing the samples.");

					AudioBuffer tempBuffer = dataBuffer;
					AudioProcessor::ConvertToInnerFormat(tempBuffer);
					AudioCaptureEventArgs captureEventArgs(this, tempBuffer);
					this->OnCapture(&captureEventArgs, nullptr);
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)(bufferDuration_s * 1000)));
				}
			}

			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Stop(), "WinAudioDS", "An error occurred whilst capturing the samples.");

		CAPTURE_EXIT:
			for (size_t i = 0; i < notificationCount; i++)
			{
				if (hEvents[i] != nullptr)
				{
					const BOOL eventResult = CloseHandle(hEvents[i]);
					if (eventResult == FALSE)
					{
						RAISE_HEPH_EXCEPTION(this, HephException(eventResult, "WinAudioDS", "An error occurred whilst closing the capture event handles."));
					}
					hEvents[i] = nullptr;
				}
			}
			pDirectSoundNotify = nullptr;
			pDirectSoundCaptureBuffer = nullptr;
			pDirectSoundCapture = nullptr;
			CoUninitialize();
		}
		BOOL CALLBACK WinAudioDS::RenderDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID pWinAudioDS)
		{
			WinAudioDS* wads = (WinAudioDS*)pWinAudioDS;
			if (wads != nullptr && lpGuid != nullptr)
			{
				GUID defaultDeviceId;
				GUID src = GUID(); // DSDEVID_DefaultPlayback
				src.Data1 = 3740270592;
				src.Data2 = 40045;
				src.Data3 = 18413;
				src.Data4[0] = 170;
				src.Data4[1] = 241;
				src.Data4[2] = 77;
				src.Data4[3] = 218;
				src.Data4[4] = 143;
				src.Data4[5] = 43;
				src.Data4[6] = 92;
				src.Data4[7] = 3;

				HRESULT hres;
				WINAUDIODS_ENUMERATION_CALLBACK_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, "WinAudioDS", "An error occurred whilst enumerating render devices.");

				AudioDevice device = AudioDevice();
				device.id = GuidToString(lpGuid);
				device.name = StringBuffer(lpcstrDescription);
				device.type = AudioDeviceType::Render;
				device.isDefault = device.id == GuidToString(&defaultDeviceId);
				wads->audioDevices.push_back(device);
			}

			return TRUE;
		}
		BOOL CALLBACK WinAudioDS::CaptureDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID pWinAudioDS)
		{
			WinAudioDS* wads = (WinAudioDS*)pWinAudioDS;
			if (wads != nullptr && lpGuid != nullptr)
			{
				GUID defaultDeviceId;
				GUID src = GUID(); // DSDEVID_DefaultCapture
				src.Data1 = 3740270593;
				src.Data2 = 40045;
				src.Data3 = 18413;
				src.Data4[0] = 170;
				src.Data4[1] = 241;
				src.Data4[2] = 77;
				src.Data4[3] = 218;
				src.Data4[4] = 143;
				src.Data4[5] = 43;
				src.Data4[6] = 92;
				src.Data4[7] = 3;

				HRESULT hres;
				WINAUDIODS_ENUMERATION_CALLBACK_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, "WinAudioDS", "An error occurred whilst enumerating capture devices."); // Get default capture device id.

				AudioDevice device = AudioDevice();
				device.id = GuidToString(lpGuid);
				device.name = StringBuffer(lpcstrDescription);
				device.type = AudioDeviceType::Capture;
				device.isDefault = device.id == GuidToString(&defaultDeviceId);
				wads->audioDevices.push_back(device);
			}

			return TRUE;
		}
		StringBuffer WinAudioDS::GuidToString(LPGUID guid)
		{
			StringBuffer result = "";
			StringBuffer temp = StringBuffer::ToHexString((uint32_t)guid->Data1);
			temp.ToLower();
			result += temp.SubString(2, temp.Size() - 2) + "-";

			temp = StringBuffer::ToHexString(guid->Data2);
			temp.ToLower();
			result += temp.SubString(2, temp.Size() - 2) + "-";

			temp = StringBuffer::ToHexString(guid->Data3);
			temp.ToLower();
			result += temp.SubString(2, temp.Size() - 2) + "-";

			temp = StringBuffer::ToHexString(guid->Data4[0]);
			temp.ToLower();
			result += temp.SubString(2, temp.Size() - 2);

			temp = StringBuffer::ToHexString(guid->Data4[1]);
			temp.ToLower();
			result += temp.SubString(2, temp.Size() - 2) + "-";

			for (size_t i = 2; i < 8; i++)
			{
				temp = StringBuffer::ToHexString(guid->Data4[i]);
				temp.ToLower();
				result += temp.SubString(2, temp.Size() - 2);
			}

			return result;
		}
		GUID WinAudioDS::StringToGuid(StringBuffer str)
		{
			GUID guid = GUID();
			guid.Data1 = StringBuffer::HexStringToU32(str.SubString(0, 8));
			guid.Data2 = StringBuffer::HexStringToU16(str.SubString(9, 4));
			guid.Data3 = StringBuffer::HexStringToU16(str.SubString(14, 4));
			guid.Data4[0] = StringBuffer::HexStringToU16(str.SubString(19, 2));
			guid.Data4[1] = StringBuffer::HexStringToU16(str.SubString(21, 2));
			guid.Data4[2] = StringBuffer::HexStringToU16(str.SubString(24, 2));
			guid.Data4[3] = StringBuffer::HexStringToU16(str.SubString(26, 2));
			guid.Data4[4] = StringBuffer::HexStringToU16(str.SubString(28, 2));
			guid.Data4[5] = StringBuffer::HexStringToU16(str.SubString(30, 2));
			guid.Data4[6] = StringBuffer::HexStringToU16(str.SubString(32, 2));
			guid.Data4[7] = StringBuffer::HexStringToU16(str.SubString(34, 2));
			return guid;
		}
		void WinAudioDS::RestrictAudioFormatInfo(AudioFormatInfo& afi, const DSCAPS& dsCaps)
		{
			if ((dsCaps.dwFlags & DSCAPS_CONTINUOUSRATE) == DSCAPS_CONTINUOUSRATE)
			{
				if (afi.sampleRate > dsCaps.dwMaxSecondarySampleRate)
				{
					afi.sampleRate = dsCaps.dwMaxSecondarySampleRate;
				}
				else if (afi.sampleRate < dsCaps.dwMinSecondarySampleRate)
				{
					afi.sampleRate = dsCaps.dwMinSecondarySampleRate;
				}
			}
			else
			{
				afi.sampleRate = dsCaps.dwMaxSecondarySampleRate;
			}

			if (!IsWindowsVistaOrGreater())
			{
				afi.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;
				afi.channelCount = (dsCaps.dwFlags & DSCAPS_SECONDARYSTEREO) == DSCAPS_SECONDARYSTEREO ? 2 : 1;
				afi.bitsPerSample = (dsCaps.dwFlags & DSCAPS_SECONDARY16BIT) != DSCAPS_SECONDARY16BIT ? 8 : 16;
			}
		}
		void WinAudioDS::RestrictAudioFormatInfo(AudioFormatInfo& afi, const DSCCAPS& dscCaps)
		{
			if (!IsWindowsVistaOrGreater())
			{
				afi.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;
				afi.bitsPerSample = 16;
				afi.sampleRate = 44100;
				afi.channelCount = dscCaps.dwChannels;
			}
		}
	}
}
#endif