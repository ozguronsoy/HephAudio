#include "NativeAudio.h"
#include "AudioProcessor.h"
#include "AudioFileFormatManager.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

namespace HephAudio
{
	namespace Native
	{
		NativeAudio::NativeAudio()
			: mainThreadId(std::this_thread::get_id()), renderDeviceId(""), captureDeviceId("")
			, renderFormat(AudioFormatInfo(1, 2, 16, 48000)), captureFormat(AudioFormatInfo(1, 2, 16, 48000)), disposing(false)
			, isRenderInitialized(false), isCaptureInitialized(false), isCapturePaused(false), displayName(""), iconPath(""), deviceEnumerationPeriod_ns(1e8)
		{
			HEPHAUDIO_STOPWATCH_START;
		}
		AudioObject* NativeAudio::Play(HephCommon::StringBuffer filePath)
		{
			return this->Play(filePath, 1u, false);
		}
		AudioObject* NativeAudio::Play(HephCommon::StringBuffer filePath, uint32_t loopCount)
		{
			return this->Play(filePath, loopCount, false);
		}
		AudioObject* NativeAudio::Play(HephCommon::StringBuffer filePath, uint32_t loopCount, bool isPaused)
		{
			try
			{
				HEPHAUDIO_LOG("Playing \"" + HephCommon::File::GetFileName(filePath) + "\"", HEPH_CL_INFO);

				std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);

				HephCommon::File audioFile(filePath, HephCommon::FileOpenMode::Read);
				FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);

				if (format == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::Play", "File format '" + audioFile.FileExtension() + "' is not supported."));
					return nullptr;
				}

				AudioObject audioObject;
				audioObject.filePath = filePath;
				audioObject.name = audioFile.FileName();
				audioObject.buffer = format->ReadFile(audioFile);
				audioObject.loopCount = loopCount;
				audioObject.pause = isPaused;
				audioObjects.push_back(std::move(audioObject));

				return &audioObjects[audioObjects.size() - 1];
			}
			catch (HephCommon::HephException ex)
			{
				RAISE_HEPH_EXCEPTION(this, ex);
				return nullptr;
			}
		}
		std::vector<AudioObject*> NativeAudio::Queue(HephCommon::StringBuffer queueName, heph_float queueDelay_ms, const std::vector<HephCommon::StringBuffer>& filePaths)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Adding files to the queue: " + queueName, HEPH_CL_INFO);

			std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);

			if (queueName.CompareContent(""))
			{
				RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::Queue", "Queue name must not be empty."));
				return std::vector<AudioObject*>();
			}

			const size_t currentQueueSize = GetQueue(queueName).size();
			std::vector<AudioObject*> queuedAudioObjects = std::vector<AudioObject*>(currentQueueSize);
			size_t failedCount = 0;

			for (size_t i = 0; i < filePaths.size(); i++)
			{
				if (HephCommon::File::FileExists(filePaths.at(i)))
				{
					AudioObject audioObject;
					audioObject.filePath = filePaths.at(i);
					audioObject.name = HephCommon::File::GetFileName(filePaths.at(i));
					audioObject.queueName = queueName;
					audioObject.queueIndex = i + currentQueueSize - failedCount;
					audioObject.queueDelay_ms = queueDelay_ms;

					if (audioObject.queueIndex == 0)
					{
						try
						{
							HephCommon::File audioFile(filePaths.at(i), HephCommon::FileOpenMode::Read);
							FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);
							if (format == nullptr)
							{
								RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::Play", "File format '" + audioFile.FileExtension() + "' is not supported."));
							}
							audioObject.buffer = format->ReadFile(audioFile);
						}
						catch (HephCommon::HephException ex)
						{
							RAISE_HEPH_EXCEPTION(this, ex);
							failedCount++;

							HEPHAUDIO_LOG("Failed to add the file \"" + audioObject.name + "\" to the queue: " + queueName, HEPH_CL_WARNING);

							continue;
						}

					}

					audioObjects.push_back(std::move(audioObject));
					queuedAudioObjects.push_back(&audioObjects[audioObjects.size() - 1]);

					HEPHAUDIO_LOG("The file \"" + audioObjects[audioObjects.size() - 1].name + "\" is successfully added to the queue: " + queueName, HEPH_CL_INFO);
				}
				else
				{
					failedCount++;

					HEPHAUDIO_LOG("Failed to add the file \"" + HephCommon::File::GetFileName(filePaths.at(i)) + "\" to the queue: " + queueName, HEPH_CL_WARNING);
				}
			}

			return queuedAudioObjects;
		}
		AudioObject* NativeAudio::Load(HephCommon::StringBuffer filePath)
		{
			AudioObject* pao = Play(filePath, 1u, true);
			if (pao != nullptr && isRenderInitialized)
			{
				AudioProcessor::ConvertSampleRate(pao->buffer, renderFormat.sampleRate);
				AudioProcessor::ConvertChannels(pao->buffer, renderFormat.channelCount);
			}
			return pao;
		}
		AudioObject* NativeAudio::CreateAO(HephCommon::StringBuffer name, size_t bufferFrameCount)
		{
			std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);
			AudioObject audioObject;
			audioObject.name = name;
			audioObject.buffer = AudioBuffer(bufferFrameCount, AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, renderFormat.channelCount, sizeof(heph_float) * 8, renderFormat.sampleRate));
			audioObjects.push_back(std::move(audioObject));
			return &audioObjects[audioObjects.size() - 1];
		}
		bool NativeAudio::DestroyAO(AudioObject* pAudioObject)
		{
			std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (pAudioObject == &audioObjects.at(i))
				{
					audioObjects.erase(audioObjects.begin() + i);
					return true;
				}
			}
			return false;
		}
		bool NativeAudio::AOExists(AudioObject* pAudioObject) const
		{
			if (pAudioObject != nullptr)
			{
				std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					if (pAudioObject == &audioObjects.at(i))
					{
						return true;
					}
				}
			}
			return false;
		}
		AudioObject* NativeAudio::GetAO(HephCommon::StringBuffer aoName)
		{
			std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects.at(i).name == aoName)
				{
					return &audioObjects.at(i);
				}
			}
			return nullptr;
		}
		AudioObject* NativeAudio::GetAO(HephCommon::StringBuffer queueName, size_t index)
		{
			std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);
			size_t count = 0;
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects.at(i).queueName == queueName)
				{
					if (count == index)
					{
						return &audioObjects.at(i);
					}
					count++;
				}
			}
			return nullptr;
		}
		void NativeAudio::PauseCapture(bool pause)
		{
			isCapturePaused = pause;
		}
		bool NativeAudio::IsCapturePaused() const noexcept
		{
			return isCapturePaused;
		}
		uint64_t NativeAudio::GetDeviceEnumerationPeriod() const noexcept
		{
			return this->deviceEnumerationPeriod_ns;
		}
		void NativeAudio::SetDeviceEnumerationPeriod(uint64_t deviceEnumerationPeriod_ns) noexcept
		{
			this->deviceEnumerationPeriod_ns = deviceEnumerationPeriod_ns;
		}
		void NativeAudio::Skip(HephCommon::StringBuffer queueName, bool applyDelay)
		{
			Skip(1, queueName, applyDelay);
		}
		void NativeAudio::Skip(size_t skipCount, HephCommon::StringBuffer queueName, bool applyDelay)
		{
			if (skipCount == 0) { return; }

			std::vector<AudioObject*> queue = GetQueue(queueName);

			if (queue.size() <= skipCount) // Skip all queue.
			{
				for (size_t i = 0; i < queue.size(); i++)
				{
					HEPHAUDIO_LOG("Skipped the file: \"" + queue.at(i)->name + "\"", HEPH_CL_INFO);

					DestroyAO(queue.at(i));
				}
			}
			else
			{
				uint32_t queueDelay_ms = 0u;

				for (size_t i = 0; i < skipCount; i++)
				{
					HEPHAUDIO_LOG("Skipped the file \"" + queue.at(i)->name + "\"", HEPH_CL_INFO);

					AudioObject* qao = queue.at(i);

					if (qao->queueIndex == 0)
					{
						queueDelay_ms = qao->queueDelay_ms;
					}

					DestroyAO(queue.at(i));
				}

				if (applyDelay && queueDelay_ms > 0)
				{
					queueThreads.push_back(std::thread(&NativeAudio::PlayNextInQueue, this, queueName, queueDelay_ms, skipCount));
				}
				else
				{
					PlayNextInQueue(queueName, 0, skipCount);
				}
			}
		}
		AudioFormatInfo NativeAudio::GetRenderFormat() const
		{
			return renderFormat;
		}
		AudioFormatInfo NativeAudio::GetCaptureFormat() const
		{
			return captureFormat;
		}
		AudioDevice NativeAudio::GetAudioDeviceById(HephCommon::StringBuffer deviceId) const
		{
			std::vector<AudioDevice> devices = GetAudioDevices(AudioDeviceType::All);
			for (size_t i = 0; i < devices.size(); i++)
			{
				if (devices.at(i).id == deviceId)
				{
					return devices.at(i);
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
				RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "WinAudioDS::GetDefaultAudioDevice", "DeviceType must be either Render or Capture."));
				return AudioDevice();
			}

			std::lock_guard<std::mutex> lockGuard(audioDevicesMutex);
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (audioDevices.at(i).isDefault && audioDevices.at(i).type == deviceType)
				{
					return audioDevices.at(i);
				}
			}

			return AudioDevice();
		}
		std::vector<AudioDevice> NativeAudio::GetAudioDevices(AudioDeviceType deviceType) const
		{
			std::vector<AudioDevice> result;

			if (deviceType == AudioDeviceType::Null)
			{
				RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "WinAudioDS::GetAudioDevices", "DeviceType must not be Null."));
				return result;
			}

			std::lock_guard<std::mutex> lockGuard(audioDevicesMutex);
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (deviceType == AudioDeviceType::All || audioDevices.at(i).type == deviceType)
				{
					result.push_back(audioDevices.at(i));
				}
			}

			return result;
		}
		bool NativeAudio::SaveToFile(HephCommon::StringBuffer filePath, bool overwrite, AudioBuffer& buffer)
		{
			try
			{
				FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(filePath);

				if (format == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::SaveToFile", "File format '" + HephCommon::File::GetFileExtension(filePath) + "' is not supported."));
					return false;
				}

				return format->SaveToFile(filePath, buffer, overwrite);
			}
			catch (HephCommon::HephException ex)
			{
				RAISE_HEPH_EXCEPTION(this, ex);
			}
			return false;
		}
		void NativeAudio::CheckAudioDevices()
		{
			AudioDeviceEventArgs deviceEventArgs = AudioDeviceEventArgs(this, AudioDevice());

			while (!disposing)
			{
				std::this_thread::sleep_for(std::chrono::nanoseconds(this->deviceEnumerationPeriod_ns));

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
		void NativeAudio::JoinQueueThreads()
		{
			for (size_t i = 0; i < queueThreads.size(); i++)
			{
				if (queueThreads.at(i).joinable())
				{
					queueThreads.at(i).join();
				}
			}
		}
		std::vector<AudioObject*> NativeAudio::GetQueue(HephCommon::StringBuffer queueName)
		{
			std::vector<AudioObject*> queue = std::vector<AudioObject*>(0);

			if (!queueName.CompareContent(""))
			{
				std::lock_guard<std::mutex> lockGuard(audioObjectsMutex);
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					AudioObject* pao = &audioObjects.at(i);
					if (pao != nullptr && pao->queueName == queueName)
					{
						queue.push_back(pao);
					}
				}
			}

			return queue;
		}
		void NativeAudio::PlayNextInQueue(HephCommon::StringBuffer queueName, heph_float queueDelay, uint32_t decreaseQueueIndex)
		{
			if (!queueName.CompareContent(""))
			{
				std::vector<AudioObject*> queue = GetQueue(queueName);

				if (queue.size() > 0 && queueDelay > 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds((uint64_t)HephCommon::Math::Ceil(queueDelay)));
				}

				if (!disposing)
				{
					for (size_t i = 0; i < queue.size(); i++)
					{
						AudioObject* qao = queue.at(i);

						if (qao->queueIndex < decreaseQueueIndex)
						{
							RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio", "Queue is empty."));
							return;
						}

						if (qao->queueIndex == decreaseQueueIndex)
						{
							try
							{
								HEPHAUDIO_LOG("Playing the next file \"" + qao->name + "\" in queue: " + qao->queueName, HEPH_CL_INFO);

								HephCommon::File audioFile(qao->filePath, HephCommon::FileOpenMode::Read);
								FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);

								if (format == nullptr)
								{
									RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio", "File format '" + audioFile.FileExtension() + "' is not supported."));
									return;
								}

								qao->buffer = format->ReadFile(audioFile);
							}
							catch (HephCommon::HephException ex)
							{
								RAISE_HEPH_EXCEPTION(this, ex);
							}
						}

						qao->queueIndex -= decreaseQueueIndex;
					}
				}
			}
		}
		void NativeAudio::Mix(AudioBuffer& outputBuffer, uint32_t frameCount)
		{
			const size_t mixedAOCount = GetAOCountToMix();
			AudioBuffer mixBuffer(frameCount, AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, renderFormat.channelCount, sizeof(heph_float) * 8, renderFormat.sampleRate));

			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				AudioObject* pAudioObject = &audioObjects.at(i);

				if (!pAudioObject->pause && (pAudioObject->queueName.CompareContent("") || pAudioObject->queueIndex == 0))
				{
					const heph_float volume = GetFinalAOVolume(pAudioObject) / mixedAOCount;

					AudioRenderEventArgs rArgs = AudioRenderEventArgs(this, pAudioObject, frameCount);
					AudioRenderEventResult rResult;
					pAudioObject->OnRender(&rArgs, &rResult);

					for (size_t j = 0; j < frameCount && j < rResult.renderBuffer.FrameCount(); j++)
					{
						for (size_t k = 0; k < renderFormat.channelCount; k++)
						{
							mixBuffer[j][k] += rResult.renderBuffer[j][k] * volume;
						}
					}

					if (rResult.isFinishedPlaying)
					{
						if (pAudioObject->loopCount == 1) // Finish playing.
						{
							AudioFinishedPlayingEventArgs ofpArgs = AudioFinishedPlayingEventArgs(this, pAudioObject, 0);
							pAudioObject->OnFinishedPlaying(&ofpArgs, nullptr);

							HephCommon::StringBuffer queueName = pAudioObject->queueName;
							uint32_t queueDelay_ms = pAudioObject->queueDelay_ms;

							HEPHAUDIO_LOG("Finished playing the file \"" + pAudioObject->name + "\"", HEPH_CL_INFO);

							audioObjects.erase(audioObjects.begin() + i);
							i--;

							if (!queueName.CompareContent(""))
							{
								if (queueDelay_ms > 0)
								{
									queueThreads.push_back(std::thread(&NativeAudio::PlayNextInQueue, this, queueName, queueDelay_ms, 1));
								}
								else
								{
									PlayNextInQueue(queueName, 0, 1);
								}
							}
						}
						else
						{
							pAudioObject->loopCount--;

							AudioFinishedPlayingEventArgs ofpArgs = AudioFinishedPlayingEventArgs(this, pAudioObject, pAudioObject->loopCount);
							pAudioObject->OnFinishedPlaying(&ofpArgs, nullptr);

							pAudioObject->frameIndex = 0;
						}
					}
				}
			}

			AudioProcessor::ConvertToTargetFormat(mixBuffer, renderFormat);
			outputBuffer = std::move(mixBuffer);
		}
		size_t NativeAudio::GetAOCountToMix() const
		{
			size_t result = 0;
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (!audioObjects.at(i).pause && (audioObjects.at(i).queueName.CompareContent("") || audioObjects.at(i).queueIndex == 0))
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