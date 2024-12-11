#include "NativeAudio/NativeAudio.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "FFmpeg/FFmpegAudioEncoder.h"
#include "AudioEffects/ChannelMapper.h"
#include "AudioEffects/Resampler.h"
#include "AudioEvents/AudioDeviceEventArgs.h"
#include "AudioEvents/AudioCaptureEventArgs.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "Stopwatch.h"
#include "ConsoleLogger.h"
#include "HephMath.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/NotFoundException.h"

using namespace Heph;

namespace HephAudio
{
	namespace Native
	{
		NativeAudio::NativeAudio()
			: pAudioDecoder(new FFmpegAudioDecoder()), pAudioEncoder(new FFmpegAudioEncoder()),
			mainThreadId(std::this_thread::get_id()), renderDeviceId(""), captureDeviceId(""),
			renderFormat(AudioFormatInfo(1, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000)), captureFormat(AudioFormatInfo(1, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000)),
			disposing(false), isRenderInitialized(false), isCaptureInitialized(false), isCapturePaused(false), deviceEnumerationPeriod_ms(100)
		{
			HEPH_SW_RESET;
		}

		std::shared_ptr<IAudioDecoder> NativeAudio::GetAudioDecoder() const
		{
			return this->pAudioDecoder;
		}

		void NativeAudio::SetAudioDecoder(std::shared_ptr<IAudioDecoder> pNewDecoder)
		{
			if (pNewDecoder == nullptr)
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Decoder cannot be null"));
			}
			this->pAudioDecoder = pNewDecoder;
		}

		std::shared_ptr<IAudioEncoder> NativeAudio::GetAudioEncoder() const
		{
			return this->pAudioEncoder;
		}

		void NativeAudio::SetAudioEncoder(std::shared_ptr<IAudioEncoder> pNewEncoder)
		{
			if (pNewEncoder == nullptr)
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Encoder cannot be null"));
			}
			this->pAudioEncoder = pNewEncoder;
		}

		AudioObject* NativeAudio::Play(const std::filesystem::path& filePath)
		{
			return this->Play(filePath, 1);
		}

		AudioObject* NativeAudio::Play(const std::filesystem::path& filePath, uint32_t playCount)
		{
			HEPHAUDIO_LOG("Playing \"" + filePath.filename().string() + "\"", HEPH_CL_INFO);

			if (!std::filesystem::exists(filePath))
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, NotFoundException(HEPH_FUNC, "file not found."));
			}

			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);

			AudioObject& audioObject = this->audioObjects.emplace_back();

			audioObject.filePath = filePath;
			audioObject.name = filePath.filename().string();

			this->pAudioDecoder->ChangeFile(filePath);
			audioObject.buffer = this->pAudioDecoder->Decode();

			audioObject.playCount = playCount;
			audioObject.isPaused = false;

			return &audioObject;
		}

		AudioObject* NativeAudio::Load(const std::filesystem::path& filePath)
		{
			return this->Load(filePath, 1);
		}

		AudioObject* NativeAudio::Load(const std::filesystem::path& filePath, uint32_t playCount)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			AudioObject* pAudioObject = this->Play(filePath, playCount);
			pAudioObject->isPaused = true;
			return pAudioObject;
		}

		AudioObject* NativeAudio::CreateAudioObject(const std::string& name, size_t bufferFrameCount, AudioChannelLayout channelLayout, uint32_t sampleRate)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);

			AudioObject& audioObject = this->audioObjects.emplace_back();
			audioObject.name = name;
			audioObject.buffer = AudioBuffer(bufferFrameCount, channelLayout, sampleRate);
			audioObject.isPaused = true;

			return &audioObject;
		}

		bool NativeAudio::DestroyAudioObject(AudioObject* pAudioObject)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (auto it = this->audioObjects.begin(); it != this->audioObjects.end(); ++it)
			{
				if (pAudioObject == &(*it))
				{
					this->audioObjects.erase(it);
					return true;
				}
			}
			return false;
		}

		bool NativeAudio::DestroyAudioObject(const Heph::Guid& audioObjectId)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (auto it = this->audioObjects.begin(); it != this->audioObjects.end(); ++it)
			{
				if (it->id == audioObjectId)
				{
					this->audioObjects.erase(it);
					return true;
				}
			}
			return false;
		}

		bool NativeAudio::AudioObjectExists(AudioObject* pAudioObject) const
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			if (pAudioObject != nullptr)
			{
				for (const AudioObject& ao : this->audioObjects)
				{
					if (pAudioObject == &ao)
					{
						return true;
					}
				}
			}
			return false;
		}

		bool NativeAudio::AudioObjectExists(const Guid& audioObjectId) const
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (const AudioObject& ao : this->audioObjects)
			{
				if (ao.id == audioObjectId)
				{
					return true;
				}
			}
			return false;
		}

		AudioObject* NativeAudio::GetAudioObject(size_t index)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);

			if (index >= this->audioObjects.size())
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Index out of range."));
			}

			auto it = this->audioObjects.begin();
			std::advance(it, index);
			return &(*it);
		}

		AudioObject* NativeAudio::GetAudioObject(const Heph::Guid& audioObjectId)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (AudioObject& ao : this->audioObjects)
			{
				if (ao.id == audioObjectId)
				{
					return &ao;
				}
			}
			return nullptr;
		}

		AudioObject* NativeAudio::GetAudioObject(const std::string& audioObjectName)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (AudioObject& ao : this->audioObjects)
			{
				if (ao.name == audioObjectName)
				{
					return &ao;
				}
			}
			return nullptr;
		}

		size_t NativeAudio::GetAudioObjectCount() const
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			return this->audioObjects.size();
		}

		void NativeAudio::ResumeCapture()
		{
			this->isCapturePaused = false;
		}

		void NativeAudio::PauseCapture()
		{
			this->isCapturePaused = true;
		}

		bool NativeAudio::IsCapturePaused() const
		{
			return this->isCapturePaused;
		}

		uint32_t NativeAudio::GetDeviceEnumerationPeriod() const
		{
			return this->deviceEnumerationPeriod_ms;
		}

		void NativeAudio::SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms)
		{
			this->deviceEnumerationPeriod_ms = deviceEnumerationPeriod_ms;
		}

		const AudioFormatInfo& NativeAudio::GetRenderFormat() const
		{
			return this->renderFormat;
		}

		const AudioFormatInfo& NativeAudio::GetCaptureFormat() const
		{
			return this->captureFormat;
		}

		void NativeAudio::InitializeRender()
		{
			this->InitializeRender(nullptr, HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000));
		}

		void NativeAudio::InitializeRender(AudioChannelLayout channelLayout, uint32_t sampleRate)
		{
			this->InitializeRender(nullptr, HEPHAUDIO_INTERNAL_FORMAT(channelLayout, sampleRate));
		}

		void NativeAudio::InitializeRender(AudioFormatInfo format)
		{
			this->InitializeRender(nullptr, format);
		}

		void NativeAudio::InitializeCapture()
		{
			this->InitializeCapture(nullptr, HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000));
		}

		void NativeAudio::InitializeCapture(AudioChannelLayout channelLayout, uint32_t sampleRate)
		{
			this->InitializeCapture(nullptr, HEPHAUDIO_INTERNAL_FORMAT(channelLayout, sampleRate));
		}

		void NativeAudio::InitializeCapture(AudioFormatInfo format)
		{
			this->InitializeCapture(nullptr, format);
		}

		AudioDevice NativeAudio::GetAudioDeviceById(const std::string& deviceId) const
		{
			std::vector<AudioDevice> devices = GetAudioDevices(AudioDeviceType::All);
			for (size_t i = 0; i < devices.size(); i++)
			{
				if (devices[i].id == deviceId)
				{
					return devices[i];
				}
			}
			return AudioDevice();
		}

		AudioDevice NativeAudio::GetRenderDevice() const
		{
			AudioDevice renderDevice = GetAudioDeviceById(renderDeviceId);
			if (renderDevice.id != renderDeviceId)
			{
				renderDevice.id = renderDeviceId;
				renderDevice.type = AudioDeviceType::Render;
			}
			return renderDevice;
		}

		AudioDevice NativeAudio::GetCaptureDevice() const
		{
			AudioDevice captureDevice = GetAudioDeviceById(captureDeviceId);
			if (captureDevice.id != captureDeviceId)
			{
				captureDevice.id = captureDeviceId;
				captureDevice.type = AudioDeviceType::Capture;
			}
			return captureDevice;
		}

		AudioDevice NativeAudio::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			if (deviceType == AudioDeviceType::All || deviceType == AudioDeviceType::Null)
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "DeviceType must be either Render or Capture."));
			}

			std::lock_guard<std::mutex> lockGuard(audioDevicesMutex);
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (audioDevices[i].isDefault && audioDevices[i].type == deviceType)
				{
					return audioDevices[i];
				}
			}

			return AudioDevice();
		}

		std::vector<AudioDevice> NativeAudio::GetAudioDevices(AudioDeviceType deviceType) const
		{
			std::vector<AudioDevice> result;

			if (deviceType == AudioDeviceType::Null)
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "DeviceType must not be Null."));
			}

			std::lock_guard<std::mutex> lockGuard(audioDevicesMutex);
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (deviceType == AudioDeviceType::All || audioDevices[i].type == deviceType)
				{
					result.push_back(audioDevices[i]);
				}
			}

			return result;
		}

		void NativeAudio::CheckAudioDevices()
		{
			AudioDeviceEventArgs deviceEventArgs = AudioDeviceEventArgs(this, AudioDevice());

			while (!disposing)
			{
				if (this->deviceEnumerationPeriod_ms > 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(this->deviceEnumerationPeriod_ms));
				}

				audioDevicesMutex.lock();
				std::vector<AudioDevice> oldDevices = audioDevices;
				audioDevices.clear();
				if (this->EnumerateAudioDevices() == NativeAudio::DEVICE_ENUMERATION_FAIL)
				{
					HEPHAUDIO_LOG("Device enumeration failed, terminating the device thread...", HEPH_CL_ERROR);
					audioDevices = oldDevices;
					audioDevicesMutex.unlock();
					return;
				}
				audioDevicesMutex.unlock();

				if (OnAudioDeviceAdded)
				{
					for (size_t i = 0; i < audioDevices.size(); i++)
					{
						for (size_t j = 0; j < oldDevices.size(); j++)
						{
							if (audioDevices[i].id == oldDevices.at(j).id)
							{
								goto ADD_BREAK;
							}
						}
						deviceEventArgs.audioDevice = audioDevices[i];
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
							if (oldDevices[i].id == audioDevices.at(j).id)
							{
								goto REMOVE_BREAK;
							}
						}
						deviceEventArgs.audioDevice = oldDevices[i];
						OnAudioDeviceRemoved(&deviceEventArgs, nullptr);
					REMOVE_BREAK:;
					}
				}
			}
		}

		void NativeAudio::JoinRenderThread()
		{
			if (renderThread.joinable())
			{
				renderThread.join();
			}
		}

		void NativeAudio::JoinCaptureThread()
		{
			if (captureThread.joinable())
			{
				captureThread.join();
			}
		}

		void NativeAudio::JoinDeviceThread()
		{
			if (deviceThread.joinable())
			{
				deviceThread.join();
			}
		}

		EncodedAudioBuffer NativeAudio::Mix(uint32_t frameCount)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);

			const size_t mixedAOCount = GetAOCountToMix();
			AudioBuffer mixBuffer(frameCount, this->renderFormat.channelLayout, this->renderFormat.sampleRate);

			size_t audioObjectCount = this->audioObjects.size();
			for (int32_t i = 0; i < audioObjectCount; ++i)
			{
				auto it = this->audioObjects.begin();
				std::advance(it, i);

				AudioObject* pAudioObject = &(*it);
				if (!pAudioObject->isPaused)
				{
					const double volume = this->GetFinalAOVolume(pAudioObject) / mixedAOCount;
					const Guid audioObjectId = pAudioObject->id;

					AudioRenderEventArgs rArgs(this, pAudioObject, frameCount);
					AudioRenderEventResult rResult;
					pAudioObject->OnRender(&rArgs, &rResult);

					for (size_t j = 0; j < frameCount && j < rResult.renderBuffer.FrameCount(); j++)
					{
						for (size_t k = 0; k < this->renderFormat.channelLayout.count; k++)
						{
							mixBuffer[j][k] += rResult.renderBuffer[j][k] * volume;
						}
					}

					if (rResult.isFinishedPlaying && this->AudioObjectExists(audioObjectId))
					{
						AudioFinishedPlayingEventArgs ofpArgs(this, pAudioObject);
						pAudioObject->OnFinishedPlaying(&ofpArgs, nullptr);
					}

					// audio object is destroyed during event handling
					// adjust the loop variables
					const size_t currentCount = this->audioObjects.size();
					if (audioObjectCount > currentCount)
					{
						audioObjectCount = currentCount;
						i--;
					}
				}
			}

			EncodedAudioBuffer encodedBuffer(this->renderFormat);
			this->pAudioEncoder->Encode(mixBuffer, encodedBuffer);
			return encodedBuffer;
		}

		size_t NativeAudio::GetAOCountToMix() const
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			size_t result = 0;
			for (const AudioObject& ao : this->audioObjects)
			{
				if (!ao.isPaused)
				{
					result++;
				}
			}
			return result;
		}

		double NativeAudio::GetFinalAOVolume(AudioObject* pAudioObject) const
		{
			return pAudioObject->volume;
		}
	}
}