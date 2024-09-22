#ifdef _WIN32
#include "NativeAudio/WinAudioDS.h"
#include "Stopwatch.h"
#include "ConsoleLogger.h"
#include <VersionHelpers.h>
#include <comdef.h>

#if defined(_MSVC_LANG)
#pragma comment (lib, "Dsound.lib")
#pragma comment(lib, "Winmm.lib")
#endif

#define IID_IDS_NOTIFY {0xB0210783, 0x89CD, 0x11D0, {0xAF, 0x08, 0x00, 0xA0, 0xC9, 0x25, 0xCD, 0x16}}
#define WINAUDIODS_RENDER_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephException(hres, method, message, "DirectSound", WinAudioBase::GetComErrorMessage(hres))); goto RENDER_EXIT; }
#define WINAUDIODS_CAPTURE_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephException(hres, method, message, "DirectSound", WinAudioBase::GetComErrorMessage(hres))); goto CAPTURE_EXIT; }
#define WINAUDIODS_ENUMERATE_DEVICE_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephException(hres, method, message, "DirectSound", WinAudioBase::GetComErrorMessage(hres))); return NativeAudio::DEVICE_ENUMERATION_FAIL; }
#define WINAUDIODS_ENUMERATION_CALLBACK_EXCPT(hr, winAudioDS, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(winAudioDS, HephException(hres, method, message, "DirectSound", WinAudioBase::GetComErrorMessage(hres))); return NativeAudio::DEVICE_ENUMERATION_FAIL; }

using namespace Heph;
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
			HEPH_SW_RESET;
			HEPHAUDIO_LOG("Destructing WinAudioDS...", HEPH_CL_INFO);

			this->disposing = true;

			this->JoinDeviceThread();
			this->JoinRenderThread();
			this->JoinCaptureThread();

			CoUninitialize();

			HEPHAUDIO_LOG("WinAudioDS destructed in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioDS::SetMasterVolume(double volume)
		{
			if (this->isRenderInitialized)
			{
				const uint16_t usv = volume * UINT16_MAX;
				const MMRESULT mmResult = waveOutSetVolume(nullptr, (usv << 16) | usv);
				if (mmResult != MMSYSERR_NOERROR)
				{
					char errorMessage[MAXERRORLENGTH]{ };
					if (waveOutGetErrorTextA(mmResult, errorMessage, MAXERRORLENGTH) != MMSYSERR_NOERROR)
					{
						strcpy(errorMessage, "error message not found");
					}
					RAISE_HEPH_EXCEPTION(this, HephException(mmResult, HEPH_FUNC, "An error occurred while setting the master volume", "MMEAPI", errorMessage));
				}
			}
		}
		double WinAudioDS::GetMasterVolume() const
		{
			if (this->isRenderInitialized)
			{
				DWORD dv;
				const MMRESULT mmResult = waveOutGetVolume(nullptr, &dv);
				if (mmResult != MMSYSERR_NOERROR)
				{
					char errorMessage[MAXERRORLENGTH]{ };
					if (waveOutGetErrorTextA(mmResult, errorMessage, MAXERRORLENGTH) != MMSYSERR_NOERROR)
					{
						strcpy(errorMessage, "error message not found");
					}
					RAISE_HEPH_EXCEPTION(this, HephException(mmResult, HEPH_FUNC, "An error occurred while getting the master volume", "MMEAPI", errorMessage));
					return -1.0;
				}
				return (double)(dv & 0x0000FFFF) / (double)UINT16_MAX;
			}
			return -1.0;
		}
		void WinAudioDS::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

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

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioDS::StopRendering()
		{
			if (this->isRenderInitialized)
			{
				this->isRenderInitialized = false;
				this->renderDeviceId = "";
				this->JoinRenderThread();
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void WinAudioDS::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

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

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudioDS::StopCapturing()
		{
			if (this->isCaptureInitialized)
			{
				this->isCaptureInitialized = false;
				this->captureDeviceId = "";
				this->JoinCaptureThread();
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void WinAudioDS::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_OPERATION, HEPH_FUNC, "Native params not supported."));
		}
		void WinAudioDS::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_OPERATION, HEPH_FUNC, "Native params not supported."));
		}
		bool WinAudioDS::EnumerateAudioDevices()
		{
			HRESULT hres;
			WINAUDIODS_ENUMERATE_DEVICE_EXCPT(DirectSoundEnumerateW(&WinAudioDS::RenderDeviceEnumerationCallback, (void*)this), HEPH_FUNC, "An error occurred while enumerating render devices.");
			WINAUDIODS_ENUMERATE_DEVICE_EXCPT(DirectSoundCaptureEnumerateW(&WinAudioDS::CaptureDeviceEnumerationCallback, (void*)this), HEPH_FUNC, "An error occurred while enumerating capture devices.");
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
			EncodedAudioBuffer mixedBuffer;
			size_t mixedBufferSize_byte = 0;
			size_t nFramesToRead;
			HANDLE hEvents[notificationCount]{ nullptr };
			DSBPOSITIONNOTIFY notifyInfos[notificationCount]{ 0 };
			WAVEFORMATEXTENSIBLE wfx{ 0 };
			HRESULT hres;

			WINAUDIODS_RENDER_THREAD_EXCPT(DirectSoundCreate(&deviceId, pDirectSound.GetAddressOf(), nullptr), HEPH_FUNC, "An error occurred while initializing render.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSound->SetCooperativeLevel(this->hwnd, DSSCL_EXCLUSIVE), HEPH_FUNC, "An error occurred while initializing render.");

			dsCaps.dwSize = sizeof(DSCAPS);
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSound->GetCaps(&dsCaps), HEPH_FUNC, "An error occurred while reading the direct sound capacity.");
			WinAudioDS::RestrictAudioFormatInfo(this->renderFormat, dsCaps);
			nFramesToRead = this->renderFormat.ByteRate() / 100; // 10ms
			mixedBufferSize_byte = nFramesToRead * this->renderFormat.FrameSize();

			wfx = WinAudioBase::AFI2WFX(this->renderFormat);
			bufferDesc.dwSize = sizeof(DSBUFFERDESC);
			bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
			bufferDesc.dwBufferBytes = mixedBufferSize_byte * (notificationCount - 1);
			bufferDesc.dwReserved = 0;
			bufferDesc.lpwfxFormat = (WAVEFORMATEX*)&wfx;
			bufferDesc.guid3DAlgorithm = GUID_NULL;
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSound->CreateSoundBuffer(&bufferDesc, pDirectSoundBuffer.GetAddressOf(), nullptr), HEPH_FUNC, "An error occurred while creating a render buffer.");

			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
				if (hEvents[i] == nullptr)
				{
					WINAUDIODS_RENDER_THREAD_EXCPT(E_FAIL, HEPH_FUNC, "An error occurred while setting the render event handles.");
				}

				notifyInfos[i].dwOffset = (i + 1) * mixedBufferSize_byte - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[notificationCount - 1].dwOffset = DSBPN_OFFSETSTOP;

			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->QueryInterface(IID_IDS_NOTIFY, (void**)pDirectSoundNotify.GetAddressOf()), HEPH_FUNC, "An error occurred while rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), HEPH_FUNC, "An error occurred while rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING), HEPH_FUNC, "An error occurred while rendering the samples.");

			this->isRenderInitialized = true;

			while (!this->disposing && this->isRenderInitialized)
			{
				bool waitSuccessfull = false;
				const DWORD waitForNotificationResult = WaitForMultipleObjects(notificationCount, hEvents, FALSE, 1000);
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
					WINAUDIODS_RENDER_THREAD_EXCPT(E_FAIL, HEPH_FUNC, "Render time-out.");
				}

				mixedBuffer = this->Mix(nFramesToRead);

				WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Lock(0, mixedBufferSize_byte, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, DSBLOCK_FROMWRITECURSOR), HEPH_FUNC, "An error occurred while rendering the samples.");
				memcpy(audioPtr1, mixedBuffer.begin(), audioBytes1);
				if (audioPtr2 != nullptr)
				{
					memcpy(audioPtr2, mixedBuffer.begin() + audioBytes1, audioBytes2);
				}
				WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), HEPH_FUNC, "An error occurred while rendering the samples.");
			}

			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Stop(), HEPH_FUNC, "An error occurred while rendering the samples.");

		RENDER_EXIT:
			for (size_t i = 0; i < notificationCount; i++)
			{
				if (hEvents[i] != nullptr)
				{
					const BOOL eventResult = CloseHandle(hEvents[i]);
					if (eventResult == FALSE)
					{
						RAISE_HEPH_EXCEPTION(this, HephException(eventResult, HEPH_FUNC, "An error occurred while closing the render event handles."));
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
			WAVEFORMATEXTENSIBLE wfx{ 0 };
			void* audioPtr1 = nullptr;
			void* audioPtr2 = nullptr;
			DWORD audioBytes1, audioBytes2, captureCursor, readCursor;
			size_t nFramesToRead, nBytesToRead;
			HANDLE hEvents[notificationCount]{ nullptr };
			DSBPOSITIONNOTIFY notifyInfos[notificationCount]{ 0 };
			HRESULT hres;

			WINAUDIODS_CAPTURE_THREAD_EXCPT(DirectSoundCaptureCreate(&deviceId, pDirectSoundCapture.GetAddressOf(), nullptr), HEPH_FUNC, "An error occurred while initializing capture.");

			dscCaps.dwSize = sizeof(DSCCAPS);
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCapture->GetCaps(&dscCaps), HEPH_FUNC, "An error occurred while reading the direct sound capacity.");
			WinAudioDS::RestrictAudioFormatInfo(this->captureFormat, dscCaps);

			wfx = WinAudioBase::AFI2WFX(this->captureFormat);
			bufferDesc.dwSize = sizeof(DSCBUFFERDESC);
			bufferDesc.dwFlags = 0;
			bufferDesc.dwBufferBytes = this->captureFormat.ByteRate() * bufferDuration_s * (notificationCount - 1);
			bufferDesc.lpwfxFormat = (WAVEFORMATEX*)&wfx;
			bufferDesc.dwFXCount = 0;
			bufferDesc.lpDSCFXDesc = nullptr;
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCapture->CreateCaptureBuffer(&bufferDesc, pDirectSoundCaptureBuffer.GetAddressOf(), nullptr), HEPH_FUNC, "An error occurred while creating a capture buffer.");

			nFramesToRead = this->captureFormat.sampleRate * bufferDuration_s;
			nBytesToRead = nFramesToRead * this->captureFormat.FrameSize();

			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
				if (hEvents[i] == nullptr)
				{
					WINAUDIODS_CAPTURE_THREAD_EXCPT(E_FAIL, HEPH_FUNC, "An error occurred while setting the capture event handles.");
				}
				notifyInfos[i].dwOffset = (i + 1) * nBytesToRead - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[notificationCount - 1].dwOffset = DSBPN_OFFSETSTOP;

			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->QueryInterface(IID_IDS_NOTIFY, (void**)pDirectSoundNotify.GetAddressOf()), HEPH_FUNC, "An error occurred while initializing capture.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), HEPH_FUNC, "An error occurred while initializing capture.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Start(DSCBSTART_LOOPING), HEPH_FUNC, "An error occurred while initializing capture.");

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
						WINAUDIODS_CAPTURE_THREAD_EXCPT(E_FAIL, HEPH_FUNC, "Capture time-out.");
					}

					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->GetCurrentPosition(&captureCursor, &readCursor), HEPH_FUNC, "An error occurred while capturing the samples.");
					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Lock(readCursor, nBytesToRead, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0), HEPH_FUNC, "An error occurred while capturing the samples.");
					EncodedAudioBuffer encodedBuffer((const uint8_t*)audioPtr1, nFramesToRead * this->captureFormat.FrameSize(), this->captureFormat);
					if (audioPtr2 != nullptr)
					{
						memcpy(encodedBuffer.begin() + audioBytes1, audioPtr2, audioBytes2);
					}
					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), HEPH_FUNC, "An error occurred while capturing the samples.");

					AudioBuffer buffer = this->pAudioDecoder->Decode(encodedBuffer);
					AudioCaptureEventArgs captureEventArgs(this, buffer);
					this->OnCapture(&captureEventArgs, nullptr);
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)(bufferDuration_s * 1000)));
				}
			}

			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Stop(), HEPH_FUNC, "An error occurred while capturing the samples.");

		CAPTURE_EXIT:
			for (size_t i = 0; i < notificationCount; i++)
			{
				if (hEvents[i] != nullptr)
				{
					const BOOL eventResult = CloseHandle(hEvents[i]);
					if (eventResult == FALSE)
					{
						RAISE_HEPH_EXCEPTION(this, HephException(eventResult, HEPH_FUNC, "An error occurred while closing the capture event handles."));
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
				WINAUDIODS_ENUMERATION_CALLBACK_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, HEPH_FUNC, "An error occurred while enumerating render devices.");

				AudioDevice device;
				device.id = GuidToString(lpGuid);
				device.name = StringHelpers::WideToStr(lpcstrDescription);
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
				WINAUDIODS_ENUMERATION_CALLBACK_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, HEPH_FUNC, "An error occurred while enumerating capture devices."); // Get default capture device id.

				AudioDevice device = AudioDevice();
				device.id = GuidToString(lpGuid);
				device.name = StringHelpers::WideToStr(lpcstrDescription);
				device.type = AudioDeviceType::Capture;
				device.isDefault = device.id == GuidToString(&defaultDeviceId);
				wads->audioDevices.push_back(device);
			}

			return TRUE;
		}
		std::string WinAudioDS::GuidToString(LPGUID guid)
		{
			std::string result = "";
			std::string temp = StringHelpers::ToHexString((uint32_t)guid->Data1);
			result += temp.substr(2, temp.size() - 2) + "-";

			temp = StringHelpers::ToHexString(guid->Data2);
			result += temp.substr(2, temp.size() - 2) + "-";

			temp = StringHelpers::ToHexString(guid->Data3);
			result += temp.substr(2, temp.size() - 2) + "-";

			temp = StringHelpers::ToHexString(guid->Data4[0]);
			result += temp.substr(2, temp.size() - 2);

			temp = StringHelpers::ToHexString(guid->Data4[1]);
			result += temp.substr(2, temp.size() - 2) + "-";

			for (size_t i = 2; i < 8; i++)
			{
				temp = StringHelpers::ToHexString(guid->Data4[i]);
				result += temp.substr(2, temp.size() - 2);
			}

			return result;
		}
		GUID WinAudioDS::StringToGuid(const std::string& str)
		{
			GUID guid = GUID();
			guid.Data1 = StringHelpers::HexStringToU32(str.substr(0, 8));
			guid.Data2 = StringHelpers::HexStringToU16(str.substr(9, 4));
			guid.Data3 = StringHelpers::HexStringToU16(str.substr(14, 4));
			guid.Data4[0] = StringHelpers::HexStringToU16(str.substr(19, 2));
			guid.Data4[1] = StringHelpers::HexStringToU16(str.substr(21, 2));
			guid.Data4[2] = StringHelpers::HexStringToU16(str.substr(24, 2));
			guid.Data4[3] = StringHelpers::HexStringToU16(str.substr(26, 2));
			guid.Data4[4] = StringHelpers::HexStringToU16(str.substr(28, 2));
			guid.Data4[5] = StringHelpers::HexStringToU16(str.substr(30, 2));
			guid.Data4[6] = StringHelpers::HexStringToU16(str.substr(32, 2));
			guid.Data4[7] = StringHelpers::HexStringToU16(str.substr(34, 2));
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
				afi.channelLayout = (dsCaps.dwFlags & DSCAPS_SECONDARYSTEREO) ? HEPHAUDIO_CH_LAYOUT_STEREO : HEPHAUDIO_CH_LAYOUT_MONO;
				afi.bitsPerSample = (dsCaps.dwFlags & DSCAPS_SECONDARY16BIT) ? 16 : 8;
				afi.bitRate = AudioFormatInfo::CalculateBitrate(afi);
			}
		}
		void WinAudioDS::RestrictAudioFormatInfo(AudioFormatInfo& afi, const DSCCAPS& dscCaps)
		{
			if (!IsWindowsVistaOrGreater())
			{
				afi.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;
				afi.bitsPerSample = 16;
				afi.sampleRate = 44100;
				afi.channelLayout = dscCaps.dwChannels == 2 ? HEPHAUDIO_CH_LAYOUT_STEREO : HEPHAUDIO_CH_LAYOUT_MONO;
				afi.bitRate = AudioFormatInfo::CalculateBitrate(afi);
			}
		}
	}
}
#endif