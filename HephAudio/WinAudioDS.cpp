#ifdef _WIN32
#include "WinAudioDS.h"
#include "AudioProcessor.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"

#pragma comment (lib, "Dsound.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "Winmm.lib")

using namespace Microsoft::WRL;

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE (HINSTANCE)&__ImageBase
#define WINAUDIODS_EXCPT(hr, winAudioDS, method, message) hres = hr; if(FAILED(hres)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hres, method, message)); throw AudioException(hres, method, message); }
#define WINAUDIODS_DEVICE_THREAD_EXCPT(hr, winAudioDS, method, message) hres = hr; if(FAILED(hres)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hres, method, message)); }
#define WINAUDIODS_RENDER_THREAD_EXCPT(hr, winAudioDS, method, message) hres = hr; if(FAILED(hres)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hres, method, message)); goto RENDER_EXIT; }
#define WINAUDIODS_CAPTURE_THREAD_EXCPT(hr, winAudioDS, method, message) hres = hr; if(FAILED(hres)) { RAISE_AUDIO_EXCPT(winAudioDS, AudioException(hres, method, message)); goto CAPTURE_EXIT; }

namespace HephAudio
{
	namespace Native
	{
		WinAudioDS::WinAudioDS() : NativeAudio()
			, pDirectSound(nullptr), pDirectSoundBuffer(nullptr), pDirectSoundCapture(nullptr), pDirectSoundCaptureBuffer(nullptr)
		{
			CoInitializeEx(nullptr, COINIT_MULTITHREADED);

			HRESULT hres;
			WINAUDIODS_EXCPT(DirectSoundEnumerateW(&WinAudioDS::RenderDeviceEnumerationCallback, (void*)this), this, "WinAudioDS::WinAudioDS", "An error occurred whilst enumerating render devices.");
			WINAUDIODS_EXCPT(DirectSoundCaptureEnumerateW(&WinAudioDS::CaptureDeviceEnumerationCallback, (void*)this), this, "WinAudioDS::WinAudioDS", "An error occurred whilst enumerating capture devices.");
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
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing WinAudioDS...", ConsoleLogger::info);

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

			HEPHAUDIO_LOG("WinAudioDS destructed in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
		}
		void WinAudioDS::SetMasterVolume(hephaudio_float volume)
		{
			if (!disposing && isRenderInitialized)
			{
				const uint16_t usv = volume * UINT16_MAX;
				const uint32_t dv = (usv << 16) | usv;
				waveOutSetVolume(nullptr, dv);
			}
		}
		hephaudio_float WinAudioDS::GetMasterVolume() const
		{
			if (!disposing && isRenderInitialized)
			{
				DWORD dv;
				waveOutGetVolume(nullptr, &dv);
				const uint16_t usv = dv & 0x0000FFFF;
				return (hephaudio_float)usv / (hephaudio_float)UINT16_MAX;
			}
			return -1.0;
		}
		void WinAudioDS::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), ConsoleLogger::info);

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
				deviceId = StringToGuid(GetDefaultAudioDevice(AudioDeviceType::Render).id);
			}
			else
			{
				deviceId = StringToGuid(device->id);
			}

			HRESULT hres;
			WINAUDIODS_EXCPT(DirectSoundCreate(&deviceId, pDirectSound.GetAddressOf(), nullptr), this, "WinAudioDS::InitializeRender", "An error occurred whilst initializing render.");
			WINAUDIODS_EXCPT(pDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY), this, "WinAudioDS::InitializeRender", "An error occurred whilst initializing render.");

			DSBUFFERDESC bufferDesc = DSBUFFERDESC();
			bufferDesc.dwSize = sizeof(DSBUFFERDESC);
			bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
			bufferDesc.dwBufferBytes = renderFormat.ByteRate() / 100 * 4;
			bufferDesc.dwReserved = 0;
			WAVEFORMATEX wrf = renderFormat;
			bufferDesc.lpwfxFormat = &wrf;
			bufferDesc.guid3DAlgorithm = GUID_NULL;
			WINAUDIODS_EXCPT(pDirectSound->CreateSoundBuffer(&bufferDesc, pDirectSoundBuffer.GetAddressOf(), nullptr), this, "WinAudioDS::InitializeRender", "An error occurred whilst creating a render buffer.");

			renderDeviceId = GuidToString(&deviceId);
			isRenderInitialized = true;
			renderThread = std::thread(&WinAudioDS::RenderData, this);

			HEPHAUDIO_LOG("Render initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
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
				HEPHAUDIO_LOG("Stopped rendering.", ConsoleLogger::info);
			}
		}
		void WinAudioDS::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), ConsoleLogger::info);

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
				deviceId = StringToGuid(GetDefaultAudioDevice(AudioDeviceType::Capture).id);
			}
			else
			{
				deviceId = StringToGuid(device->id);
			}

			HRESULT hres;
			WINAUDIODS_EXCPT(DirectSoundCaptureCreate(&deviceId, pDirectSoundCapture.GetAddressOf(), nullptr), this, "WinAudioDS::InitializeCapture", "An error occurred whilst initializing capture.");

			DSCBUFFERDESC bufferDesc = DSCBUFFERDESC();
			bufferDesc.dwSize = sizeof(DSCBUFFERDESC);
			bufferDesc.dwFlags = 0;
			bufferDesc.dwBufferBytes = captureFormat.ByteRate() * 2;
			WAVEFORMATEX wcf = captureFormat;
			bufferDesc.lpwfxFormat = &wcf;
			bufferDesc.dwFXCount = 0;
			bufferDesc.lpDSCFXDesc = nullptr;
			WINAUDIODS_EXCPT(pDirectSoundCapture->CreateCaptureBuffer(&bufferDesc, pDirectSoundCaptureBuffer.GetAddressOf(), nullptr), this, "WinAudioDS::InitializeCapture", "An error occurred whilst creating a capture buffer.");

			captureDeviceId = GuidToString(&deviceId);
			isCaptureInitialized = true;
			captureThread = std::thread(&WinAudioDS::CaptureData, this);

			HEPHAUDIO_LOG("Capture initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
		}
		void WinAudioDS::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = L"";
				JoinCaptureThread();
				pDirectSoundCaptureBuffer = nullptr;
				pDirectSoundCapture = nullptr;
				HEPHAUDIO_LOG("Stopped capturing.", ConsoleLogger::info);
			}
		}
		void WinAudioDS::SetDisplayName(StringBuffer displayName)
		{
			if (disposing) { return; }
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, "WinAudioDS::SetDisplayName", "WinAudioDS does not support this method, use WinAudio instead."));
		}
		void WinAudioDS::SetIconPath(StringBuffer iconPath)
		{
			if (disposing) { return; }
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, "WinAudioDS::SetIconPath", "WinAudioDS does not support this method, use WinAudio instead."));
		}
		AudioDevice WinAudioDS::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			if (disposing) { return AudioDevice(); }

			if (deviceType == AudioDeviceType::All || deviceType == AudioDeviceType::Null)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "WinAudioDS::GetDefaultAudioDevice", "DeviceType must be either Render or Capture."));
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
		std::vector<AudioDevice> WinAudioDS::GetAudioDevices(AudioDeviceType deviceType) const
		{
			std::vector<AudioDevice> result;

			if (deviceType == AudioDeviceType::Null)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "WinAudioDS::GetAudioDevices", "DeviceType must not be Null."));
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
			constexpr hephaudio_float period = 250.0; // In ms.
			StopWatch::Start();
			AudioDeviceEventArgs deviceEventArgs = AudioDeviceEventArgs(this, AudioDevice());
			HRESULT hres;

			while (!disposing)
			{
				if (StopWatch::DeltaTime(StopWatch::milli) >= period)
				{
					std::vector<AudioDevice> oldDevices = audioDevices;

					audioDevices.clear();

					WINAUDIODS_DEVICE_THREAD_EXCPT(DirectSoundEnumerateW(&WinAudioDS::RenderDeviceEnumerationCallback, (void*)this), this, "WinAudioDS", "An error occurred whilst enumerating render devices.");
					WINAUDIODS_DEVICE_THREAD_EXCPT(DirectSoundCaptureEnumerateW(&WinAudioDS::CaptureDeviceEnumerationCallback, (void*)this), this, "WinAudioDS", "An error occurred whilst enumerating capture devices.");

					if (OnAudioDeviceAdded)
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
							deviceEventArgs.audioDevice = audioDevices.at(i);
							OnAudioDeviceAdded(&deviceEventArgs, nullptr);
						ADD_BREAK:;
						}
					}

					if (OnAudioDeviceRemoved)
					{
						for (size_t i = 0; i < oldDevices.size(); i++)
						{
							for (size_t j = 0; j < audioDevices.size(); j++)
							{
								if (oldDevices.at(i).id == audioDevices.at(j).id)
								{
									goto REMOVE_BREAK;
								}
							}
							deviceEventArgs.audioDevice = oldDevices.at(i);
							OnAudioDeviceRemoved(&deviceEventArgs, nullptr);
						REMOVE_BREAK:;
						}
					}

					StopWatch::Reset();
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
			HRESULT hres;

			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				notifyInfos[i].dwOffset = (i + 1) * dataBuffer.Size() - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[4].dwOffset = DSBPN_OFFSETSTOP;

			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)pDirectSoundNotify.GetAddressOf()), this, "WinAudioDS", "An error occurred whilst rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), this, "WinAudioDS", "An error occurred whilst rendering the samples.");
			WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING), this, "WinAudioDS", "An error occurred whilst rendering the samples.");

			while (!disposing && isRenderInitialized)
			{
				uint32_t retval = WaitForMultipleObjects(notificationCount, hEvents, FALSE, 2000);
				if (retval == WAIT_OBJECT_0 || retval == WAIT_OBJECT_0 + 1 || retval == WAIT_OBJECT_0 + 2 || retval == WAIT_OBJECT_0 + 3 || retval == WAIT_OBJECT_0 + 4)
				{
					Mix(dataBuffer, dataBuffer.FrameCount());

					WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Lock(0, dataBuffer.Size(), &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, DSBLOCK_FROMWRITECURSOR), this, "WinAudioDS", "An error occurred whilst rendering the samples.");

					memcpy(audioPtr1, dataBuffer.Begin(), audioBytes1);
					if (audioPtr2 != nullptr)
					{
						memcpy(audioPtr2, (uint8_t*)dataBuffer.Begin() + audioBytes1, audioBytes2);
					}
					WINAUDIODS_RENDER_THREAD_EXCPT(pDirectSoundBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), this, "WinAudioDS", "An error occurred whilst rendering the samples.");

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
			hres = pDirectSoundBuffer->Stop();
			if (FAILED(hres))
			{
				RAISE_AUDIO_EXCPT(this, AudioException(hres, "WinAudioDS", "An error occurred whilst rendering the samples."));
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
			HRESULT hres;

			for (size_t i = 0; i < notificationCount; i++)
			{
				hEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				notifyInfos[i].dwOffset = (i + 1) * dataBuffer.Size() - 1;
				notifyInfos[i].hEventNotify = hEvents[i];
			}
			notifyInfos[4].dwOffset = DSBPN_OFFSETSTOP;

			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)pDirectSoundNotify.GetAddressOf()), this, "WinAudioDS", "An error occurred whilst capturing the samples.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundNotify->SetNotificationPositions(notificationCount, notifyInfos), this, "WinAudioDS", "An error occurred whilst capturing the samples.");
			WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Start(DSCBSTART_LOOPING), this, "WinAudioDS", "An error occurred whilst capturing the samples.");

			while (!disposing && isCaptureInitialized)
			{
				if (!isCapturePaused && OnCapture)
				{
					uint32_t retval = WaitForMultipleObjects(notificationCount, hEvents, FALSE, 2000);
					if (retval == WAIT_OBJECT_0 || retval == WAIT_OBJECT_0 + 1 || retval == WAIT_OBJECT_0 + 2 || retval == WAIT_OBJECT_0 + 3 || retval == WAIT_OBJECT_0 + 4)
					{
						WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->GetCurrentPosition(&captureCursor, &readCursor), this, "WinAudioDS", "An error occurred whilst capturing the samples.");
						WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Lock(readCursor, dataBuffer.Size(), &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0), this, "WinAudioDS", "An error occurred whilst capturing the samples.");

						memcpy(dataBuffer.Begin(), audioPtr1, audioBytes1);
						if (audioPtr2 != nullptr)
						{
							memcpy((uint8_t*)dataBuffer.Begin() + audioBytes1, audioPtr2, audioBytes2);
						}

						WINAUDIODS_CAPTURE_THREAD_EXCPT(pDirectSoundCaptureBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2), this, "WinAudioDS", "An error occurred whilst capturing the samples.");

						AudioBuffer tempBuffer = dataBuffer;
						AudioProcessor::ConvertPcmToInnerFormat(tempBuffer);
						AudioCaptureEventArgs captureEventArgs = AudioCaptureEventArgs(this, tempBuffer);

						OnCapture(&captureEventArgs, nullptr);
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
			hres = pDirectSoundCaptureBuffer->Stop();
			if (FAILED(hres))
			{
				RAISE_AUDIO_EXCPT(this, AudioException(hres, "WinAudioDS", "An error occurred whilst capturing the samples."));
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

				HRESULT hres;
				WINAUDIODS_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, "WinAudioDS::WinAudioDS", "An error occurred whilst enumerating render devices.");

				AudioDevice device = AudioDevice();
				device.id = GuidToString(lpGuid);
				device.name = StringBuffer(lpcstrDescription);
				device.type = AudioDeviceType::Render;
				device.isDefault = device.id == GuidToString(&defaultDeviceId);
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

				HRESULT hres;
				WINAUDIODS_EXCPT(GetDeviceID(&src, &defaultDeviceId), wads, "WinAudioDS::WinAudioDS", "An error occurred whilst enumerating capture devices."); // Get default capture device id.

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
			guid.Data1 = StringBuffer::HexStringToUI32(str.SubString(0, 8));
			guid.Data2 = StringBuffer::HexStringToUI16(str.SubString(9, 4));
			guid.Data3 = StringBuffer::HexStringToUI16(str.SubString(14, 4));
			guid.Data4[0] = StringBuffer::HexStringToUI16(str.SubString(19, 2));
			guid.Data4[1] = StringBuffer::HexStringToUI16(str.SubString(21, 2));
			guid.Data4[2] = StringBuffer::HexStringToUI16(str.SubString(24, 2));
			guid.Data4[3] = StringBuffer::HexStringToUI16(str.SubString(26, 2));
			guid.Data4[4] = StringBuffer::HexStringToUI16(str.SubString(28, 2));
			guid.Data4[5] = StringBuffer::HexStringToUI16(str.SubString(30, 2));
			guid.Data4[6] = StringBuffer::HexStringToUI16(str.SubString(32, 2));
			guid.Data4[7] = StringBuffer::HexStringToUI16(str.SubString(34, 2));
			return guid;
		}
		LRESULT CALLBACK WinAudioDS::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			return TRUE;
		}
	}
}
#endif