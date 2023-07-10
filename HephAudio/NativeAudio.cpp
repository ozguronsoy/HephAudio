#include "NativeAudio.h"
#include "AudioProcessor.h"
#include "AudioFileFormatManager.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"
#include "HephMath.h"

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
		std::shared_ptr<AudioObject> NativeAudio::Play(HephCommon::StringBuffer filePath)
		{
			return this->Play(filePath, 1u, false);
		}
		std::shared_ptr<AudioObject> NativeAudio::Play(HephCommon::StringBuffer filePath, uint32_t loopCount)
		{
			return this->Play(filePath, loopCount, false);
		}
		std::shared_ptr<AudioObject> NativeAudio::Play(HephCommon::StringBuffer filePath, uint32_t loopCount, bool isPaused)
		{
			try
			{
				HEPHAUDIO_LOG("Playing \"" + HephCommon::File::GetFileName(filePath) + "\"", HephCommon::ConsoleLogger::info);

				HephCommon::File audioFile(filePath, HephCommon::FileOpenMode::Read);
				FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);

				if (format == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::Play", "File format '" + audioFile.FileExtension() + "' is not supported."));
					return nullptr;
				}

				std::shared_ptr<AudioObject> pao = std::shared_ptr<AudioObject>(new AudioObject());
				pao->filePath = filePath;
				pao->name = audioFile.FileName();
				pao->buffer = format->ReadFile(&audioFile);
				pao->loopCount = loopCount;
				pao->pause = isPaused;
				audioObjects.push_back(pao);

				return pao;
			}
			catch (HephCommon::HephException ex)
			{
				RAISE_HEPH_EXCEPTION(this, ex);
				return nullptr;
			}
		}
		std::vector<std::shared_ptr<AudioObject>> NativeAudio::Queue(HephCommon::StringBuffer queueName, heph_float queueDelay_ms, const std::vector<HephCommon::StringBuffer>& filePaths)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Adding files to the queue: " + queueName, HephCommon::ConsoleLogger::info);

			if (queueName.CompareContent(""))
			{
				RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::Queue", "Queue name must not be empty."));
				return std::vector<std::shared_ptr<AudioObject>>();
			}

			const size_t currentQueueSize = GetQueue(queueName).size();
			std::vector<std::shared_ptr<AudioObject>> queuedAudioObjects = std::vector<std::shared_ptr<AudioObject>>(currentQueueSize);
			size_t failedCount = 0;

			for (size_t i = 0; i < filePaths.size(); i++)
			{
				if (HephCommon::File::FileExists(filePaths.at(i)))
				{
					std::shared_ptr<AudioObject> pao = std::shared_ptr<AudioObject>(new AudioObject());
					pao->filePath = filePaths.at(i);
					pao->name = HephCommon::File::GetFileName(filePaths.at(i));
					pao->queueName = queueName;
					pao->queueIndex = i + currentQueueSize - failedCount;
					pao->queueDelay_ms = queueDelay_ms;

					if (pao->queueIndex == 0)
					{
						try
						{
							HephCommon::File audioFile(filePaths.at(i), HephCommon::FileOpenMode::Read);
							FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);
							if (format == nullptr)
							{
								RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::Play", "File format '" + audioFile.FileExtension() + "' is not supported."));
							}
							pao->buffer = format->ReadFile(&audioFile);
						}
						catch (HephCommon::HephException ex)
						{
							RAISE_HEPH_EXCEPTION(this, ex);
							failedCount++;

							HEPHAUDIO_LOG("Failed to add the file \"" + pao->name + "\" to the queue: " + queueName, HephCommon::ConsoleLogger::warning);

							continue;
						}

					}

					audioObjects.push_back(pao);
					queuedAudioObjects.push_back(pao);

					HEPHAUDIO_LOG("The file \"" + pao->name + "\" is successfully added to the queue: " + queueName, HephCommon::ConsoleLogger::info);
				}
				else
				{
					failedCount++;

					HEPHAUDIO_LOG("Failed to add the file \"" + HephCommon::File::GetFileName(filePaths.at(i)) + "\" to the queue: " + queueName, HephCommon::ConsoleLogger::warning);
				}
			}

			return queuedAudioObjects;
		}
		std::shared_ptr<AudioObject> NativeAudio::Load(HephCommon::StringBuffer filePath)
		{
			std::shared_ptr<AudioObject> pao = Play(filePath, 1u, true);
			if (pao != nullptr && isRenderInitialized)
			{
				AudioProcessor::ConvertSampleRate(pao->buffer, renderFormat.sampleRate);
				AudioProcessor::ConvertChannels(pao->buffer, renderFormat.channelCount);
			}
			return pao;
		}
		std::shared_ptr<AudioObject> NativeAudio::CreateAO(HephCommon::StringBuffer name, size_t bufferFrameCount)
		{
			std::shared_ptr<AudioObject> pao = std::shared_ptr<AudioObject>(new AudioObject());
			pao->name = name;
			if (bufferFrameCount > 0)
			{
				pao->buffer = AudioBuffer(bufferFrameCount, AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, renderFormat.channelCount, sizeof(heph_float) * 8, renderFormat.sampleRate));
			}
			audioObjects.push_back(pao);
			return pao;
		}
		bool NativeAudio::DestroyAO(std::shared_ptr<AudioObject> pAudioObject)
		{
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (pAudioObject.get() == audioObjects.at(i).get())
				{
					audioObjects.erase(audioObjects.begin() + i);
					return true;
				}
			}
			return false;
		}
		bool NativeAudio::AOExists(std::shared_ptr<AudioObject> pAudioObject) const
		{
			if (pAudioObject != nullptr)
			{
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					if (pAudioObject.get() == audioObjects.at(i).get())
					{
						return true;
					}
				}
			}
			return false;
		}
		void NativeAudio::SetAOPosition(std::shared_ptr<AudioObject> pAudioObject, heph_float position)
		{
			if (position < 0.0 || position > 1.0)
			{
				RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio::SetAOPosition", "position must be between 0 and 1."));
				return;
			}

			if (AOExists(pAudioObject))
			{
				pAudioObject->frameIndex = pAudioObject->buffer.FrameCount() * position;
			}
		}
		heph_float NativeAudio::GetAOPosition(std::shared_ptr<AudioObject> pAudioObject) const
		{
			if (AOExists(pAudioObject))
			{
				return (heph_float)pAudioObject->frameIndex / (heph_float)pAudioObject->buffer.FrameCount();
			}
			return -1.0;
		}
		std::shared_ptr<AudioObject> NativeAudio::GetAO(HephCommon::StringBuffer aoName) const
		{
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects.at(i)->name == aoName)
				{
					return audioObjects.at(i);
				}
			}
			return nullptr;
		}
		std::shared_ptr<AudioObject> NativeAudio::GetAO(HephCommon::StringBuffer queueName, size_t index) const
		{
			size_t count = 0;
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects.at(i)->queueName == queueName)
				{
					if (count == index)
					{
						return audioObjects.at(i);
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

			std::vector<std::shared_ptr<AudioObject>> queue = GetQueue(queueName);

			if (queue.size() <= skipCount) // Skip all queue.
			{
				for (size_t i = 0; i < queue.size(); i++)
				{
					HEPHAUDIO_LOG("Skipped the file: \"" + queue.at(i)->name + "\"", HephCommon::ConsoleLogger::info);

					DestroyAO(queue.at(i));
				}
			}
			else
			{
				uint32_t queueDelay_ms = 0u;

				for (size_t i = 0; i < skipCount; i++)
				{
					HEPHAUDIO_LOG("Skipped the file \"" + queue.at(i)->name + "\"", HephCommon::ConsoleLogger::info);

					std::shared_ptr<AudioObject> qao = queue.at(i);

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

			deviceMutex.lock();
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (audioDevices.at(i).isDefault && audioDevices.at(i).type == deviceType)
				{
					deviceMutex.unlock();
					return audioDevices.at(i);
				}
			}
			deviceMutex.unlock();

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

			deviceMutex.lock();
			for (size_t i = 0; i < audioDevices.size(); i++)
			{
				if (deviceType == AudioDeviceType::All || audioDevices.at(i).type == deviceType)
				{
					result.push_back(audioDevices.at(i));
				}
			}
			deviceMutex.unlock();

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

				deviceMutex.lock();
				std::vector<AudioDevice> oldDevices = audioDevices;
				audioDevices.clear();
				if (this->EnumerateAudioDevices() == NativeAudio::DEVICE_ENUMERATION_FAIL)
				{
					HEPHAUDIO_LOG("Device enumeration failed, terminating the device thread...", HephCommon::ConsoleLogger::error);
					audioDevices = oldDevices;
					deviceMutex.unlock();
					return;
				}
				deviceMutex.unlock();

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
		std::vector<std::shared_ptr<AudioObject>> NativeAudio::GetQueue(HephCommon::StringBuffer queueName) const
		{
			std::vector<std::shared_ptr<AudioObject>> queue = std::vector<std::shared_ptr<AudioObject>>(0);

			if (!queueName.CompareContent(""))
			{
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					std::shared_ptr<AudioObject> pao = audioObjects.at(i);
					if (pao != nullptr && pao->queueName == queueName)
					{
						queue.push_back(audioObjects.at(i));
					}
				}
			}

			return queue;
		}
		void NativeAudio::PlayNextInQueue(HephCommon::StringBuffer queueName, heph_float queueDelay, uint32_t decreaseQueueIndex)
		{
			if (!queueName.CompareContent(""))
			{
				std::vector<std::shared_ptr<AudioObject>> queue = GetQueue(queueName);

				if (queue.size() > 0 && queueDelay > 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds((uint64_t)HephCommon::Math::Ceil(queueDelay)));
				}

				if (!disposing)
				{
					for (size_t i = 0; i < queue.size(); i++)
					{
						std::shared_ptr<AudioObject> qao = queue.at(i);

						if (qao->queueIndex < decreaseQueueIndex)
						{
							RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio", "Queue is empty."));
							return;
						}

						if (qao->queueIndex == decreaseQueueIndex)
						{
							try
							{
								HEPHAUDIO_LOG("Playing the next file \"" + qao->name + "\" in queue: " + qao->queueName, HephCommon::ConsoleLogger::info);

								HephCommon::File audioFile(qao->filePath, HephCommon::FileOpenMode::Read);
								FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);

								if (format == nullptr)
								{
									RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "NativeAudio", "File format '" + audioFile.FileExtension() + "' is not supported."));
									return;
								}

								qao->buffer = format->ReadFile(&audioFile);
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
			AudioBuffer mixBuffer = AudioBuffer(frameCount, AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, renderFormat.channelCount, sizeof(heph_float) * 8, renderFormat.sampleRate));

			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				std::shared_ptr<AudioObject> pAudioObject = audioObjects.at(i);

				if (!pAudioObject->pause && (pAudioObject->queueName.CompareContent("") || pAudioObject->queueIndex == 0))
				{
					const heph_float volume = GetFinalAOVolume(audioObjects.at(i)) / mixedAOCount;

					AudioRenderEventArgs rArgs = AudioRenderEventArgs(this, pAudioObject.get(), frameCount);
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
							AudioFinishedPlayingEventArgs ofpArgs = AudioFinishedPlayingEventArgs(this, pAudioObject.get(), 0);
							pAudioObject->OnFinishedPlaying(&ofpArgs, nullptr);

							HephCommon::StringBuffer queueName = pAudioObject->queueName;
							uint32_t queueDelay_ms = pAudioObject->queueDelay_ms;

							audioObjects.erase(audioObjects.begin() + i);
							i--;

							HEPHAUDIO_LOG("Finished playing the file \"" + pAudioObject->name + "\"", HephCommon::ConsoleLogger::info);

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

							AudioFinishedPlayingEventArgs ofpArgs = AudioFinishedPlayingEventArgs(this, pAudioObject.get(), pAudioObject->loopCount);
							pAudioObject->OnFinishedPlaying(&ofpArgs, nullptr);

							pAudioObject->frameIndex = 0;
						}
					}
				}
			}

			AudioProcessor::ConvertInnerToPcmFormat(mixBuffer, renderFormat.bitsPerSample, HephCommon::File::GetSystemEndian());
			outputBuffer = std::move(mixBuffer);
		}
		size_t NativeAudio::GetAOCountToMix() const
		{
			size_t result = 0;
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (!audioObjects.at(i)->pause && (audioObjects.at(i)->queueName.CompareContent("") || audioObjects.at(i)->queueIndex == 0))
				{
					result++;
				}
			}
			return result;
		}
		heph_float NativeAudio::GetFinalAOVolume(std::shared_ptr<AudioObject> pAudioObject) const
		{
			return pAudioObject->volume;
		}
	}
}