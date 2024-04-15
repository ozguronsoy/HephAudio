#include "NativeAudio/NativeAudio.h"
#include "AudioProcessor.h"
#include "AudioFormats/AudioFileFormatManager.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		NativeAudio::NativeAudio()
			: mainThreadId(std::this_thread::get_id()), renderDeviceId(""), captureDeviceId("")
			, renderFormat(AudioFormatInfo(1, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000)), captureFormat(AudioFormatInfo(1, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000)), disposing(false)
			, isRenderInitialized(false), isCaptureInitialized(false), isCapturePaused(false), deviceEnumerationPeriod_ms(100)
		{
			HEPHAUDIO_STOPWATCH_START;
		}
		AudioObject* NativeAudio::Play(const std::string& filePath)
		{
			return this->Play(filePath, 1u, false);
		}
		AudioObject* NativeAudio::Play(const std::string& filePath, uint32_t playCount)
		{
			return this->Play(filePath, playCount, false);
		}
		AudioObject* NativeAudio::Play(const std::string& filePath, uint32_t playCount, bool isPaused)
		{
			try
			{
				HEPHAUDIO_LOG("Playing \"" + File::GetFileName(filePath) + "\"", HEPH_CL_INFO);

				std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);

				File audioFile(filePath, FileOpenMode::Read);
				FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);

				if (format == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "NativeAudio::Play", "File format '" + audioFile.FileExtension() + "' is not supported."));
					return nullptr;
				}

#if CPP_VERSION >= CPP_VERSION_17
				AudioObject& audioObject = audioObjects.emplace_back();
#else
				audioObjects.emplace_back();
				AudioObject& audioObject = audioObjects[audioObjects.size() - 1];
#endif

				audioObject.filePath = filePath;
				audioObject.name = audioFile.FileName();
				audioObject.buffer = format->ReadFile(audioFile);
				audioObject.playCount = playCount;
				audioObject.isPaused = isPaused;

				return &audioObject;
			}
			catch (HephException ex)
			{
				return nullptr;
			}
		}
		AudioObject* NativeAudio::Load(const std::string& filePath)
		{
			return this->Load(filePath, 1, true);
		}
		AudioObject* NativeAudio::Load(const std::string& filePath, uint32_t playCount)
		{
			return this->Load(filePath, playCount, true);
		}
		AudioObject* NativeAudio::Load(const std::string& filePath, uint32_t playCount, bool isPaused)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			AudioObject* pao = this->Play(filePath, playCount, isPaused);
			if (pao != nullptr && this->isRenderInitialized)
			{
				AudioProcessor::ChangeSampleRate(pao->buffer, this->renderFormat.sampleRate);
				AudioProcessor::ChangeChannelLayout(pao->buffer, this->renderFormat.channelLayout);
			}
			return pao;
		}
		AudioObject* NativeAudio::CreateAudioObject(const std::string& name, size_t bufferFrameCount, AudioFormatInfo bufferFormatInfo)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);

#if CPP_VERSION >= CPP_VERSION_17
			AudioObject& audioObject = audioObjects.emplace_back();
#else
			audioObjects.emplace_back();
			AudioObject& audioObject = audioObjects[audioObjects.size() - 1];
#endif

			audioObject.name = name;
			audioObject.buffer = AudioBuffer(bufferFrameCount, bufferFormatInfo);
			return &audioObject;
		}
		bool NativeAudio::DestroyAudioObject(AudioObject* pAudioObject)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (pAudioObject == &audioObjects[i])
				{
					audioObjects.erase(audioObjects.begin() + i);
					return true;
				}
			}
			return false;
		}
		bool NativeAudio::DestroyAudioObject(const HephCommon::Guid& audioObjectId)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects[i].id == audioObjectId)
				{
					audioObjects.erase(audioObjects.begin() + i);
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
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					if (pAudioObject == &audioObjects[i])
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
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects[i].id == audioObjectId)
				{
					return true;
				}
			}
			return false;
		}
		AudioObject* NativeAudio::GetAudioObject(size_t index)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			if (index >= audioObjects.size())
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "NativeAudio::GetAudioObject", "Index out of range."));
				return nullptr;
			}
			return &audioObjects[index];
		}
		AudioObject* NativeAudio::GetAudioObject(const HephCommon::Guid& audioObjectId)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects[i].id == audioObjectId)
				{
					return &audioObjects[i];
				}
			}
			return nullptr;
		}
		AudioObject* NativeAudio::GetAudioObject(const std::string& audioObjectName)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects[i].name == audioObjectName)
				{
					return &audioObjects[i];
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
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "WinAudioDS::GetDefaultAudioDevice", "DeviceType must be either Render or Capture."));
				return AudioDevice();
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
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "WinAudioDS::GetAudioDevices", "DeviceType must not be Null."));
				return result;
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
		bool NativeAudio::SaveToFile(AudioBuffer& buffer, const std::string& filePath, bool overwrite)
		{
			try
			{
				FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(filePath);

				if (format == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "NativeAudio::SaveToFile", "File format '" + File::GetFileExtension(filePath) + "' is not supported."));
					return false;
				}

				return format->SaveToFile(filePath, buffer, overwrite);
			}
			catch (HephException ex)
			{
				return false;
			}
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
		void NativeAudio::Mix(AudioBuffer& outputBuffer, uint32_t frameCount)
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);

			const size_t mixedAOCount = GetAOCountToMix();
			AudioBuffer mixBuffer(frameCount, HEPHAUDIO_INTERNAL_FORMAT(renderFormat.channelLayout, renderFormat.sampleRate));

			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				AudioObject* pAudioObject = &audioObjects[i];

				if (!pAudioObject->isPaused)
				{
					const heph_float volume = GetFinalAOVolume(pAudioObject) / mixedAOCount;
					const Guid audioObjectId = pAudioObject->id;

					AudioRenderEventArgs rArgs(pAudioObject, this, frameCount);
					AudioRenderEventResult rResult;
					pAudioObject->OnRender(&rArgs, &rResult);

					for (size_t j = 0; j < frameCount && j < rResult.renderBuffer.FrameCount(); j++)
					{
						for (size_t k = 0; k < renderFormat.channelLayout.count; k++)
						{
							mixBuffer[j][k] += rResult.renderBuffer[j][k] * volume;
						}
					}

					if (rResult.isFinishedPlaying && AudioObjectExists(audioObjectId))
					{
						if (pAudioObject->playCount == 1) // finish playing.
						{
							const std::string audioObjectName = pAudioObject->name;

							AudioFinishedPlayingEventArgs ofpArgs(pAudioObject, this, 0);
							pAudioObject->OnFinishedPlaying(&ofpArgs, nullptr);

							HEPHAUDIO_LOG("Finished playing the file \"" + audioObjectName + "\"", HEPH_CL_INFO);

							if (AudioObjectExists(audioObjectId))
							{
								audioObjects.erase(audioObjects.begin() + i);
								i--;
							}
						}
						else
						{
							pAudioObject->playCount--;

							AudioFinishedPlayingEventArgs ofpArgs(pAudioObject, this, pAudioObject->playCount);
							pAudioObject->OnFinishedPlaying(&ofpArgs, nullptr);

							if (AudioObjectExists(audioObjectId))
							{
								pAudioObject->frameIndex = 0;
							}
						}
					}
				}
			}

			AudioProcessor::ConvertToTargetFormat(mixBuffer, renderFormat);
			outputBuffer = std::move(mixBuffer);
		}
		size_t NativeAudio::GetAOCountToMix() const
		{
			std::lock_guard<std::recursive_mutex> lockGuard(this->audioObjectsMutex);
			size_t result = 0;
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (!audioObjects[i].isPaused)
				{
					result++;
				}
			}
			return result;
		}
		heph_float NativeAudio::GetFinalAOVolume(AudioObject* pAudioObject) const
		{
			return pAudioObject->volume;
		}
	}
}