#ifdef _WIN32
#include "WinAudioDS.h"
#include "AudioProcessor.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#pragma comment (lib, "Dsound.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "Winmm.lib")

using namespace Microsoft::WRL;

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE (HINSTANCE)&__ImageBase
#define WINAUDIODS_EXCPT(hr, winAudioDS, method, message) if(FAILED(hr)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hr, method, message)); throw AudioException(hr, method, message); }
#define WINAUDIODS_DEVICE_THREAD_EXCPT(hr, winAudioDS, method, message) if(FAILED(hr)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hr, method, message)); }
#define WINAUDIODS_RENDER_THREAD_EXCPT(hr, winAudioDS, method, message) if(FAILED(hr)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hr, method, message)); goto RENDER_EXIT; }
#define WINAUDIODS_CAPTURE_THREAD_EXCPT(hr, winAudioDS, method, message) if(FAILED(hr)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hr, method, message)); goto CAPTURE_EXIT; }

namespace HephAudio
{
	namespace Native
	{
		WinAudioDS::WinAudioDS() : NativeAudio()
		{
			CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			pDirectSound = nullptr;
			pDirectSoundBuffer = nullptr;
			pDirectSoundCapture = nullptr;
			pDirectSoundCaptureBuffer = nullptr;
			WINAUDIODS_EXCPT(DirectSoundEnumerateW(&WinAudioDS::RenderDeviceEnumerationCallback, (void*)this), this, L"WinAudioDS::WinAudioDS", L"An error occurred whilst enumerating render devices."); // Enumerate render devices.
			WINAUDIODS_EXCPT(DirectSoundCaptureEnumerateW(&WinAudioDS::CaptureDeviceEnumerationCallback, (void*)this), this, L"WinAudioDS::WinAudioDS", L"An error occurred whilst enumerating capture devices."); // Enumerate capture devices.
			deviceThread = std::thread(&WinAudioDS::EnumerateAudioDevices, this);
			WNDCLASSEXW wndw;
			wndw.lpfnWndProc = &WinAudioDS::WindowProc;
			wndw.lpszClassName = L"HephAudio";
			wndw.hInstance = DLL_INSTANCE;
			wndw.cbClsExtra = 0;
			wndw.cbSize = sizeof(WNDCLASSEXW);
			wndw.cbWndExtra = 0;
			wndw.hbrBackground = nullptr;
			wndw.hCursor = nullptr;
			wndw.hIcon = nullptr;
			wndw.hIconSm = nullptr;
			wndw.lpszMenuName = nullptr;
			wndw.style = 0;
			RegisterClassExW(&wndw);
			hwnd = CreateWindowExW(0, L"HephAudio", L"HephAudio", 0, 0, 0, 0, 0, nullptr, nullptr, DLL_INSTANCE, nullptr);
		}
		WinAudioDS::~WinAudioDS()
		{
			disposing = true;
			JoinDeviceThread();
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();
			pDirectSoundBuffer = nullptr;
			pDirectSound = nullptr;
			pDirectSoundCaptureBuffer = nullptr;
			pDirectSoundCapture = nullptr;
			CloseWindow(hwnd);
			UnregisterClassW(L"HephAudio", DLL_INSTANCE);
			CoUninitialize();
		}
		void WinAudioDS::SetMasterVolume(double volume)
		{
			if (!disposing && isRenderInitialized)
			{
				const uint16_t usv = volume * UINT16_MAX;
				const uint32_t dv = (usv << 16) | usv;
				waveOutSetVolume(nullptr, dv);
			}
		}
		double WinAudioDS::GetMasterVolume() const
		{
			if (!disposing && isRenderInitialized)
			{
				DWORD dv;
				waveOutGetVolume(nullptr, &dv);
				const uint16_t usv = dv & 0x0000FFFF;
				return (double)usv / (double)UINT16_MAX;
			}
			return -1.0;
		}
		void WinAudioDS::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			if (disposing) { return; }
			StopRendering();
			renderFormat = format;
			if (renderFormat.formatTag != 1) { renderFormat.formatTag = 1; }
			if (renderFormat.sampleRate != 22050 && renderFormat.sampleRate != 44100 && renderFormat.sampleRate != 88200 && renderFormat.sampleRate != 176400 &&
				renderFormat.sampleRate != 24000 && renderFormat.sampleRate != 48000 && renderFormat.sampleRate != 96000 && renderFormat.sampleRate != 192000)
			{
				renderFormat.sampleRate = 48000;
			}
			if (renderFormat.channelCount != 1 && renderFormat.channelCount != 2) { renderFormat.channelCount = 2; }
			if (renderFormat.bitsPerSample != 8 && renderFormat.bitsPerSample != 16 && renderFormat.bitsPerSample != 24 && renderFormat.bitsPerSample != 32) { renderFormat.bitsPerSample = 32; }
			renderFormat = AudioFormatInfo(renderFormat.formatTag, renderFormat.channelCount, renderFormat.bitsPerSample, renderFormat.sampleRate);
			GUID deviceId;
			if (device == nullptr || device->type != AudioDeviceType::Render)
			{
				deviceId = WStringToGuid(GetDefaultAudioDevice(AudioDeviceType::Render).id);
			}
			else
			{
				deviceId = WStringToGuid(device->id);
			}
			WINAUDIODS_EXCPT(DirectSoundCreate(&deviceId, pDirectSound.GetAddressOf(), nullptr), this, L"WinAudioDS::InitializeRender", L"An error occurred whilst initializing render.");
			WINAUDIODS_EXCPT(pDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY), this, L"WinAudioDS::InitializeRender", L"An error occurred whilst initializing render.");
			DSBUFFERDESC bufferDesc = DSBUFFERDESC();
			bufferDesc.dwSize = sizeof(DSBUFFERDESC);
			bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
			bufferDesc.dwBufferBytes = renderFormat.ByteRate() / 100 * 4;
			bufferDesc.dwReserved = 0;
			WAVEFORMATEX wrf = renderFormat;
			bufferDesc.lpwfxFormat = &wrf;
			bufferDesc.guid3DAlgorithm = GUID_NULL;
			WINAUDIODS_EXCPT(pDirectSound->CreateSoundBuffer(&bufferDesc, pDirectSoundBuffer.GetAddressOf(), nullptr), this, L"WinAudioDS::InitializeRender", L"An error occurred whilst creating a render buffer.");
			renderDeviceId = GuidToWString(&deviceId);
			isRenderInitialized = true;
			renderThread = std::thread(&WinAudioDS::RenderData, this);
		}
		void WinAudioDS::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = L"";
				JoinRenderThread();
				pDirectSoundBuffer = nullptr;
				pDirectSound = nullptr;
			}
		}
		void WinAudioDS::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			if (disposing) { return; }
			StopCapturing();
			captureFormat = format;
			if (captureFormat.formatTag != 1) { captureFormat.formatTag = 1; }
			if (captureFormat.sampleRate != 22050 && captureFormat.sampleRate != 44100 && captureFormat.sampleRate != 88200 && captureFormat.sampleRate != 176400 &&
				captureFormat.sampleRate != 24000 && captureFormat.sampleRate != 48000 && captureFormat.sampleRate != 96000 && captureFormat.sampleRate != 192000)
			{
				captureFormat.sampleRate = 48000;
			}
			if (captureFormat.channelCount != 1 && captureFormat.channelCount != 2) { captureFormat.channelCount = 2; }
			if (captureFormat.bitsPerSample != 8 && captureFormat.bitsPerSample != 16 && captureFormat.bitsPerSample != 24 && captureFormat.bitsPerSample != 32) { captureFormat.bitsPerSample = 32; }
			captureFormat = AudioFormatInfo(captureFormat.formatTag, captureFormat.channelCount, captureFormat.bitsPerSample, captureFormat.sampleRate);
			GUID deviceId;
			if (device == nullptr || device->type != AudioDeviceType::Capture)
			{
				deviceId = WStringToGuid(GetDefaultAudioDevice(AudioDeviceType::Capture).id);
			}
			else
			{
				deviceId = WStringToGuid(device->id);
			}
			WINAUDIODS_EXCPT(DirectSoundCaptureCreate(&deviceId, pDirectSoundCapture.GetAddressOf(), nullptr), this, L"WinAudioDS::InitializeCapture", L"An error occurred whilst initializing capture.");
			DSCBUFFERDESC bufferDesc = DSCBUFFERDESC();
			bufferDesc.dwSize = sizeof(DSCBUFFERDESC);
			bufferDesc.dwFlags = 0;
			bufferDesc.dwBufferBytes = captureFormat.ByteRate() * 2;
			WAVEFORMATEX wcf = captureFormat;
			bufferDesc.lpwfxFormat = &wcf;
			bufferDesc.dwFXCount = 0;
			bufferDesc.lpDSCFXDesc = nullptr;
			WINAUDIODS_EXCPT(pDirectSoundCapture->CreateCaptureBuffer(&bufferDesc, pDirectSoundCaptureBuffer.GetAddressOf(), nullptr), this, L"WinAudioDS::InitializeCapture", L"An error occurred whilst creating a capture buffer.");
			captureDeviceId = GuidToWString(&deviceId);
			isCaptureInitialized = true;
			captureThread = std::thread(&WinAudioDS::CaptureData, this);
		}
		void WinAudioDS::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false; // Set this to false to stop rendering.
				captureDeviceId = L"";
				JoinCaptureThread();
				pDirectSoundCaptureBuffer = nullptr;
				pDirectSoundCapture = nullptr;
			}
		}
		void WinAudioDS::SetDisplayName(std::wstring displayName)
		{
			if (disposing) { return; }
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"WinAudioDS::SetDisplayName", L"WinAudioDS does not support this method, use WinAudio instead."));
		}
		void WinAudioDS::SetIconPath(std::wstring iconPath)
		{
			if (disposing) { return; }
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"WinAudioDS::SetIconPath", L"WinAudioDS does not support this method, use WinAudio instead."));
		}
		AudioDevice WinAudioDS::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			if (disposing) { return AudioDevice(); }
			if (deviceType == AudioDeviceType::All || deviceType == AudioDeviceType::Null)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"WinAudioDS::GetDefaultAudioDevice", L"DeviceType must be either Render or Capture."));
				return AudioDevice();
			}
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (audioDevices.at(i).isDefault && audioDevices.at(i).type == deviceType)
				{
					return audioDevices.at(i);
				}
			}
			return AudioDevice();
		}
		std::vector<AudioDevice> WinAudioDS::GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const
		{
			std::vector<AudioDevice> result;
			if (deviceType == AudioDeviceType::Null)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"WinAudioDS::GetAudioDevices", L"DeviceType must not be Null."));
				return result;
			}
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (deviceType == AudioDeviceType::All || audioDevices.at(i).type == deviceType)
				{
					result.push_back(audioDevices.at(i));
				}
			}
			return result;
		}
		void WinAudioDS::JoinDeviceThread()
		{
			if (deviceThread.joinable())
			{
				deviceThread.join();
			}
		}
		void WinAudioDS::EnumerateAudioDevices()
		{
			constexpr uint32_t period = 250; // In ms.
			auto start = std::chrono::high_resolution_clock::now();
			auto deltaTime = std::chrono::milliseconds(0);
			while (!disposing)
			{
				deltaTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - start);
				if (deltaTime >= std::chrono::milliseconds(period))
				{
					std::vector<AudioDevice> oldDevices = audioDevices;
					AudioDevice oldDefaultRender = GetDefaultAudioDevice(AudioDeviceType::Render);
					AudioDevice oldDefaultCapture = GetDefaultAudioDevice(AudioDeviceType::Capture);
					start = std::chrono::high_resolution_clock::now();
					audioDevices.clear();
					WINAUDIODS_DEVICE_THREAD_EXCPT(DirectSoundEnumerateW(&WinAudioDS::RenderDeviceEnumerationCallback, (void*)this), this, L"WinAudioDS", L"An error occurred whilst enumerating render devices."); // Enumerate render devices.
					WINAUDIODS_DEVICE_THREAD_EXCPT(DirectSoundCaptureEnumerateW(&WinAudioDS::CaptureDeviceEnumerationCallback, (void*)this), this, L"WinAudioDS", L"An error occurred whilst enumerating capture devices."); // Enumerate capture devices.
					if (OnDefaultAudioDeviceChange != nullptr)
					{
						AudioDevice newDefaultRender = GetDefaultAudioDevice(AudioDeviceType::Render);
						AudioDevice newDefaultCapture = GetDefaultAudioDevice(AudioDeviceType::Capture);
						if (oldDefaultRender.id != newDefaultRender.id)
						{
							OnDefaultAudioDeviceChange(newDefaultRender);
						}
						if (oldDefaultCapture.id != newDefaultCapture.id)
						{
							OnDefaultAudioDeviceChange(newDefaultCapture);
						}
					}
					if (OnAudioDeviceAdded != nullptr)
					{
						for (size_t i = 0; i < audioDevices.size(); i++)
						{
							for (size_t j = 0; j < oldDevices.size(); j++)
							{
								if (audioDevices.at(i).id == oldDevices.at(j).id)
								{
									goto ADD_BREAK;
								}
							}
							OnAudioDeviceAdded(audioDevices.at(i));
						ADD_BREAK:;
						}
					}
					AudioDevice* removedDevice = nullptr;
					for (size_t i = 0; i < oldDevices.size(); i++)
					{
						for (size_t j = 0; j < audioDevices.size(); j++)
						{
							if (oldDevices.at(i).id == audioDevices.at(j).id)
							{
								goto REMOVE_BREAK;
							}
						}
						removedDevice = &oldDevices.at(i);
						if (isRenderInitialized && oldDevices.at(i).type == AudioDeviceType::Render && (renderDeviceId == L"" || removedDevice->id == renderDeviceId))
						{
							InitializeRender(nullptr, renderFormat);
						}
						if (isCaptureInitialized && oldDevices.at(i).type == AudioDeviceType::Capture && (captureDeviceId == L"" || removedDevice->id == captureDeviceId))
						{
							InitializeCapture(nullptr, captureFormat);
						}
						if (OnAudioDeviceRemoved != nullptr)
						{
							OnAudioDeviceRemoved(*removedDevice);
						}
					REMOVE_BREAK:;
					}
					start = std::chrono::high_resolution_clock::now();
				}
			}
		}
		void WinAudioDS::RenderData()
		{
			void* audioPtr1;
			void* audioPtr2;
			DWORD audioBytes1, audioBytes2;
			AudioBuffer dataBuffer(renderFormat.sampleRate / 100, renderFormat);
			ComPtr<IDirectSoundNotify> pDirectSoundNotify;
			const size_t notificationCount = 5;
			HANDLE hEvents[notificationCount]{ nullptr };
			DSBPOSITIONNOTIFY notifyInfos[notificationCount];
			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				notifyInfos[i].dwOffset = (i + 1) * dataBuffer.Size() - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[4].dwOffset = DSBPN_OFFSETSTOP;
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)pDirectSoundNotify.GetAddressOf()), this, L"WinAudioDS", L"An error occurred whilst rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), this, L"WinAudioDS", L"An error occurred whilst rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING), this, L"WinAudioDS", L"An error occurred whilst rendering the samples.");
			while (!disposing && isRenderInitialized)
			{
				uint32_t retval = WaitForMultipleObjects(notificationCount, hEvents, FALSE, 2000);
				if (retval == WAIT_OBJECT_0 || retval == WAIT_OBJECT_0 + 1 || retval == WAIT_OBJECT_0 + 2 || retval == WAIT_OBJECT_0 + 3 || retval == WAIT_OBJECT_0 + 4)
				{
					Mix(dataBuffer, dataBuffer.FrameCount());
					WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Lock(0, dataBuffer.Size(), &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, DSBLOCK_FROMWRITECURSOR), this, L"WinAudioDS", L"An error occurred whilst rendering the samples.");
					memcpy(audioPtr1, dataBuffer.Begin(), audioBytes1);
					if (audioPtr2 != nullptr)
					{
						memcpy(audioPtr2, (uint8_t*)dataBuffer.Begin() + audioBytes1, audioBytes2);
					}
					WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), this, L"WinAudioDS", L"An error occurred whilst rendering the samples.");
					dataBuffer.Reset();
				}
			}
		RENDER_EXIT:
			for (size_t i = 0; i < 4; i++)
			{
				if (hEvents[i] != nullptr)
				{
					CloseHandle(hEvents[i]);
				}
			}
			long hr = pDirectSoundBuffer->Stop();
			if (FAILED(hr))
			{
				RAISE_AUDIO_EXCPT(this, AudioException(hr, L"WinAudioDS", L"An error occurred whilst rendering the samples."));
			}
		}
		void WinAudioDS::CaptureData()
		{
			void* audioPtr1;
			void* audioPtr2;
			DWORD audioBytes1, audioBytes2, captureCursor, readCursor;
			AudioBuffer dataBuffer(captureFormat.sampleRate / 2, captureFormat);
			ComPtr<IDirectSoundNotify> pDirectSoundNotify;
			const size_t notificationCount = 5;
			HANDLE hEvents[notificationCount]{ nullptr };
			DSBPOSITIONNOTIFY notifyInfos[notificationCount];
			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				notifyInfos[i].dwOffset = (i + 1) * dataBuffer.Size() - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[4].dwOffset = DSBPN_OFFSETSTOP;
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)pDirectSoundNotify.GetAddressOf()), this, L"WinAudioDS", L"An error occurred whilst capturing the samples.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), this, L"WinAudioDS", L"An error occurred whilst capturing the samples.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Start(DSCBSTART_LOOPING), this, L"WinAudioDS", L"An error occurred whilst capturing the samples.");
			while (!disposing && isCaptureInitialized)
			{
				uint32_t retval = WaitForMultipleObjects(notificationCount, hEvents, FALSE, 2000);
				if (retval == WAIT_OBJECT_0 || retval == WAIT_OBJECT_0 + 1 || retval == WAIT_OBJECT_0 + 2 || retval == WAIT_OBJECT_0 + 3 || retval == WAIT_OBJECT_0 + 4)
				{
					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->GetCurrentPosition(&captureCursor, &readCursor), this, L"WinAudioDS", L"An error occurred whilst capturing the samples.");
					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Lock(readCursor, dataBuffer.Size(), &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0), this, L"WinAudioDS", L"An error occurred whilst capturing the samples.");
					memcpy(dataBuffer.Begin(), audioPtr1, audioBytes1);
					if (audioPtr2 != nullptr)
					{
						memcpy((uint8_t*)dataBuffer.Begin() + audioBytes1, audioPtr2, audioBytes2);
					}
					WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), this, L"WinAudioDS", L"An error occurred whilst capturing the samples.");
					if (OnCapture != nullptr)
					{
						AudioBuffer tempBuffer = dataBuffer;
						AudioProcessor::ConvertPcmToInnerFormat(tempBuffer);
						OnCapture(tempBuffer);
					}
				}
			}
		CAPTURE_EXIT:
			for (size_t i = 0; i < notificationCount; i++)
			{
				if (hEvents[i] != nullptr)
				{
					CloseHandle(hEvents[i]);
				}
			}
			long hr = pDirectSoundCaptureBuffer->Stop();
			if (FAILED(hr))
			{
				RAISE_AUDIO_EXCPT(this, AudioException(hr, L"WinAudioDS", L"An error occurred whilst capturing the samples."));
			}
		}
		BOOL CALLBACK WinAudioDS::RenderDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext)
		{
			WinAudioDS* wads = (WinAudioDS*)lpContext;
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
				WINAUDIODS_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, L"WinAudioDS::WinAudioDS", L"An error occurred whilst enumerating render devices."); // Get default render device id.
				AudioDevice device = AudioDevice();
				device.id = GuidToWString(lpGuid);
				device.name = std::wstring(lpcstrDescription);
				device.type = AudioDeviceType::Render;
				device.isDefault = device.id == GuidToWString(&defaultDeviceId);
				wads->audioDevices.push_back(device);
			}
			return TRUE;
		}
		BOOL CALLBACK WinAudioDS::CaptureDeviceEnumerationCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext)
		{
			WinAudioDS* wads = (WinAudioDS*)lpContext;
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
				WINAUDIODS_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, L"WinAudioDS::WinAudioDS", L"An error occurred whilst enumerating capture devices."); // Get default capture device id.
				AudioDevice device = AudioDevice();
				device.id = GuidToWString(lpGuid);
				device.name = std::wstring(lpcstrDescription);
				device.type = AudioDeviceType::Capture;
				device.isDefault = device.id == GuidToWString(&defaultDeviceId);
				wads->audioDevices.push_back(device);
			}
			return TRUE;
		}
		std::string WinAudioDS::GuidToString(LPGUID guid)
		{
			std::stringstream ss;
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned long) * 2) << std::hex << guid->Data1 << "-";
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned short) * 2) << std::hex << guid->Data2 << "-";
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned short) * 2) << std::hex << guid->Data3 << "-";
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[0];
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[1] << "-";
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[2];
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[3];
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[4];
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[5];
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[6];
			ss << std::noshowbase << std::setfill('0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[7];
			return ss.str();
		}
		std::wstring WinAudioDS::GuidToWString(LPGUID guid)
		{
			std::wstringstream wss;
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned long) * 2) << std::hex << guid->Data1 << "-";
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned short) * 2) << std::hex << guid->Data2 << "-";
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned short) * 2) << std::hex << guid->Data3 << "-";
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[0];
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[1] << "-";
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[2];
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[3];
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[4];
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[5];
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[6];
			wss << std::noshowbase << std::setfill(L'0') << std::setw(sizeof(unsigned char) * 2) << std::hex << (int)guid->Data4[7];
			return wss.str();
		}
		GUID WinAudioDS::StringToGuid(std::string str)
		{
			GUID guid = GUID();
			guid.Data1 = std::stoul(str.substr(0, 8), nullptr, 16);
			guid.Data2 = std::stoi(str.substr(9, 4), nullptr, 16);
			guid.Data3 = std::stoi(str.substr(14, 4), nullptr, 16);
			guid.Data4[0] = std::stoi(str.substr(19, 2), nullptr, 16);
			guid.Data4[1] = std::stoi(str.substr(21, 2), nullptr, 16);
			guid.Data4[2] = std::stoi(str.substr(24, 2), nullptr, 16);
			guid.Data4[3] = std::stoi(str.substr(26, 2), nullptr, 16);
			guid.Data4[4] = std::stoi(str.substr(28, 2), nullptr, 16);
			guid.Data4[5] = std::stoi(str.substr(30, 2), nullptr, 16);
			guid.Data4[6] = std::stoi(str.substr(32, 2), nullptr, 16);
			guid.Data4[7] = std::stoi(str.substr(34, 2), nullptr, 16);
			return guid;
		}
		GUID WinAudioDS::WStringToGuid(std::wstring str)
		{
			GUID guid = GUID();
			guid.Data1 = std::stoul(str.substr(0, 8), nullptr, 16);
			guid.Data2 = std::stoi(str.substr(9, 4), nullptr, 16);
			guid.Data3 = std::stoi(str.substr(14, 4), nullptr, 16);
			guid.Data4[0] = std::stoi(str.substr(19, 2), nullptr, 16);
			guid.Data4[1] = std::stoi(str.substr(21, 2), nullptr, 16);
			guid.Data4[2] = std::stoi(str.substr(24, 2), nullptr, 16);
			guid.Data4[3] = std::stoi(str.substr(26, 2), nullptr, 16);
			guid.Data4[4] = std::stoi(str.substr(28, 2), nullptr, 16);
			guid.Data4[5] = std::stoi(str.substr(30, 2), nullptr, 16);
			guid.Data4[6] = std::stoi(str.substr(32, 2), nullptr, 16);
			guid.Data4[7] = std::stoi(str.substr(34, 2), nullptr, 16);
			return guid;
		}
		LRESULT CALLBACK WinAudioDS::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			return TRUE;
		}
	}
}
#endif