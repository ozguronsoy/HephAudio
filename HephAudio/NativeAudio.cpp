#include "NativeAudio.h"
#include "AudioProcessor.h"
#include "AudioFormats.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"

namespace HephAudio
{
	namespace Native
	{
		NativeAudio::NativeAudio()
		{
			HEPHAUDIO_STOPWATCH_START;

			AudioFormatInfo defaultFormat = AudioFormatInfo(1, 2, 16, 48000);
			audioObjects = std::vector<std::shared_ptr<AudioObject>>(0);
			categories = Categories(0);
			mainThreadId = std::this_thread::get_id();
			renderDeviceId = "";
			captureDeviceId = "";
			renderFormat = defaultFormat;
			captureFormat = defaultFormat;
			disposing = false;
			isRenderInitialized = false;
			isCaptureInitialized = false;
			isCapturePaused = false;
			displayName = "";
			iconPath = "";
			audioExceptionEventArgs = AudioExceptionEventArgs();
		}
		std::shared_ptr<AudioObject> NativeAudio::Play(StringBuffer filePath)
		{
			return Play(filePath, 1u, false);
		}
		std::shared_ptr<AudioObject> NativeAudio::Play(StringBuffer filePath, bool isPaused)
		{
			return Play(filePath, 1u, isPaused);
		}
		std::shared_ptr<AudioObject> NativeAudio::Play(StringBuffer filePath, uint32_t loopCount)
		{
			return Play(filePath, loopCount, false);
		}
		std::shared_ptr<AudioObject> NativeAudio::Play(StringBuffer filePath, uint32_t loopCount, bool isPaused)
		{
			try
			{
				HEPHAUDIO_LOG_LINE("Playing \"" + AudioFile::GetFileName(filePath) + "\"", ConsoleLogger::info);

				AudioFile audioFile(filePath);
				Formats::IAudioFormat* format = audioFormats.GetAudioFormat(audioFile);

				if (format == nullptr)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "NativeAudio::Play", "File format '" + audioFile.Extension() + "' is not supported."));
					return nullptr;
				}

				std::shared_ptr<AudioObject> ao = std::shared_ptr<AudioObject>(new AudioObject());
				ao->filePath = filePath;
				ao->name = audioFile.Name();
				ao->buffer = format->ReadFile(audioFile);
				ao->loopCount = loopCount;
				ao->pause = isPaused;
				audioObjects.push_back(ao);

				return ao;
			}
			catch (AudioException ex)
			{
				RAISE_AUDIO_EXCPT(this, ex);
				return nullptr;
			}
		}
		std::vector<std::shared_ptr<AudioObject>> NativeAudio::Queue(StringBuffer queueName, double queueDelay, const std::vector<StringBuffer>& filePaths)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG_LINE("Adding files to the queue: " + queueName, ConsoleLogger::info);

			if (queueName.CompareContent(""))
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "NativeAudio::Queue", "Queue name must not be empty."));
				return std::vector<std::shared_ptr<AudioObject>>();
			}

			const size_t currentQueueSize = GetQueue(queueName).size();
			std::vector<std::shared_ptr<AudioObject>> queuedAudioObjects = std::vector<std::shared_ptr<AudioObject>>(currentQueueSize);
			size_t failedCount = 0;

			for (size_t i = 0; i < filePaths.size(); i++)
			{
				if (AudioFile::FileExists(filePaths.at(i)))
				{
					std::shared_ptr<AudioObject> ao = std::shared_ptr<AudioObject>(new AudioObject());
					ao->filePath = filePaths.at(i);
					ao->name = AudioFile::GetFileName(filePaths.at(i));
					ao->queueName = queueName;
					ao->queueIndex = i + currentQueueSize - failedCount;
					ao->queueDelay = queueDelay;

					if (ao->queueIndex == 0)
					{
						try
						{
							AudioFile audioFile(filePaths.at(i));
							Formats::IAudioFormat* format = audioFormats.GetAudioFormat(audioFile);
							if (format == nullptr)
							{
								throw AudioException(E_INVALIDARG, "NativeAudio::Play", "File format '" + audioFile.Extension() + "' is not supported.");
							}
							ao->buffer = format->ReadFile(audioFile);
						}
						catch (AudioException ex)
						{
							RAISE_AUDIO_EXCPT(this, ex);
							failedCount++;

							HEPHAUDIO_LOG_LINE("Failed to add the file \"" + ao->name + "\" to the queue: " + queueName, ConsoleLogger::warning);

							continue;
						}

					}

					audioObjects.push_back(ao);
					queuedAudioObjects.push_back(ao);

					HEPHAUDIO_LOG_LINE("The file \"" + ao->name + "\" is successfully added to the queue: " + queueName, ConsoleLogger::info);
				}
				else
				{
					failedCount++;

					HEPHAUDIO_LOG_LINE("Failed to add the file \"" + AudioFile::GetFileName(filePaths.at(i)) + "\" to the queue: " + queueName, ConsoleLogger::warning);
				}
			}

			return queuedAudioObjects;
		}
		std::shared_ptr<AudioObject> NativeAudio::Load(StringBuffer filePath)
		{
			std::shared_ptr<AudioObject> pao = Play(filePath, 1u, true);
			if (pao != nullptr && isRenderInitialized)
			{
				AudioProcessor::ConvertSampleRate(pao->buffer, renderFormat.sampleRate);
				AudioProcessor::ConvertChannels(pao->buffer, renderFormat.channelCount);
			}
			return pao;
		}
		std::shared_ptr<AudioObject> NativeAudio::CreateAO(StringBuffer name, size_t bufferFrameCount)
		{
			std::shared_ptr<AudioObject> ao = std::shared_ptr<AudioObject>(new AudioObject());
			ao->name = name;
			ao->buffer = AudioBuffer(bufferFrameCount > 0 ? bufferFrameCount : renderFormat.sampleRate, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, renderFormat.channelCount, sizeof(double) * 2, renderFormat.sampleRate));
			ao->constant = true;
			audioObjects.push_back(ao);
			return ao;
		}
		bool NativeAudio::DestroyAO(std::shared_ptr<AudioObject> audioObject)
		{
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObject.get() == audioObjects.at(i).get())
				{
					audioObjects.erase(audioObjects.begin() + i);
					return true;
				}
			}
			return false;
		}
		bool NativeAudio::AOExists(std::shared_ptr<AudioObject> audioObject) const
		{
			if (audioObject != nullptr)
			{
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					if (audioObject.get() == audioObjects.at(i).get())
					{
						return true;
					}
				}
			}
			return false;
		}
		void NativeAudio::SetAOPosition(std::shared_ptr<AudioObject> audioObject, double position)
		{
			if (position < 0.0 || position > 1.0)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "NativeAudio::SetAOPosition", "position must be between 0 and 1."));
				return;
			}

			if (AOExists(audioObject))
			{
				audioObject->frameIndex = audioObject->buffer.FrameCount() * position;
			}
		}
		double NativeAudio::GetAOPosition(std::shared_ptr<AudioObject> audioObject) const
		{
			if (AOExists(audioObject))
			{
				return (double)audioObject->frameIndex / (double)audioObject->buffer.FrameCount();
			}
			return -1.0;
		}
		std::shared_ptr<AudioObject> NativeAudio::GetAO(StringBuffer aoName) const
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
		std::shared_ptr<AudioObject> NativeAudio::GetAO(StringBuffer queueName, size_t index) const
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
		void NativeAudio::SetCategoryVolume(StringBuffer categoryName, double newVolume)
		{
			for (size_t i = 0; i < categories.size(); i++)
			{
				if (categories.at(i).name == categoryName)
				{
					categories.at(i).volume = newVolume;
					return;
				}
			}
		}
		double NativeAudio::GetCategoryVolume(StringBuffer categoryName) const
		{
			for (size_t i = 0; i < categories.size(); i++)
			{
				if (categories.at(i).name == categoryName)
				{
					return categories.at(i).volume;
				}
			}
			return 0.0;
		}
		void NativeAudio::RegisterCategory(Category category)
		{
			if (!CategoryExists(category.name))
			{
				categories.push_back(category);
			}
		}
		void NativeAudio::UnregisterCategory(StringBuffer categoryName)
		{
			for (size_t i = 0; i < categories.size(); i++)
			{
				if (categories.at(i).name == categoryName)
				{
					categories.erase(categories.begin() + i);
					return;
				}
			}
		}
		bool NativeAudio::CategoryExists(StringBuffer categoryName) const
		{
			for (size_t i = 0; i < categories.size(); i++)
			{
				if (categories.at(i).name == categoryName)
				{
					return true;
				}
			}
			return false;
		}
		void NativeAudio::Skip(StringBuffer queueName, bool applyDelay)
		{
			Skip(1, queueName, applyDelay);
		}
		void NativeAudio::Skip(size_t skipCount, StringBuffer queueName, bool applyDelay)
		{
			if (skipCount == 0) { return; }

			std::vector<std::shared_ptr<AudioObject>> queue = GetQueue(queueName);

			if (queue.size() <= skipCount) // Skip all queue.
			{
				for (size_t i = 0; i < queue.size(); i++)
				{
					HEPHAUDIO_LOG_LINE("Skipped the file: \"" + queue.at(i)->name + "\"", ConsoleLogger::info);

					DestroyAO(queue.at(i));
				}
			}
			else
			{
				uint32_t queueDelay = 0u;

				for (size_t i = 0; i < skipCount; i++)
				{
					HEPHAUDIO_LOG_LINE("Skipped the file \"" + queue.at(i)->name + "\"", ConsoleLogger::info);

					std::shared_ptr<AudioObject> qao = queue.at(i);

					if (qao->queueIndex == 0)
					{
						queueDelay = qao->queueDelay;
					}

					DestroyAO(queue.at(i));
				}

				if (applyDelay && queueDelay > 0)
				{
					queueThreads.push_back(std::thread(&NativeAudio::PlayNextInQueue, this, queueName, queueDelay, skipCount));
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
		AudioDevice NativeAudio::GetAudioDeviceById(StringBuffer deviceId) const
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
				renderDevice.name = renderDevice.description = "REMOVED";
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
				captureDevice.name = captureDevice.description = "REMOVED";
				captureDevice.type = AudioDeviceType::Capture;
			}
			return captureDevice;
		}
		bool NativeAudio::SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat)
		{
			try
			{
				if (buffer.FormatInfo() != targetFormat)
				{
					AudioProcessor::ConvertSampleRate(buffer, targetFormat.sampleRate);
					AudioProcessor::ConvertBPS(buffer, targetFormat.bitsPerSample);
					AudioProcessor::ConvertChannels(buffer, targetFormat.channelCount);
				}

				Formats::IAudioFormat* format = audioFormats.GetAudioFormat(filePath);

				if (format == nullptr)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "NativeAudio::SaveToFile", "File format '" + AudioFile::GetFileExtension(filePath) + L"' is not supported."));
					return false;
				}

				return format->SaveToFile(filePath, buffer, overwrite);
			}
			catch (AudioException ex)
			{
				RAISE_AUDIO_EXCPT(this, ex);
			}
			return false;
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
		AudioExceptionThread NativeAudio::GetCurrentThread() const
		{
			std::thread::id currentThreadId = std::this_thread::get_id();

			if (currentThreadId == mainThreadId)
			{
				return AudioExceptionThread::MainThread;
			}
			else if (currentThreadId == renderThread.get_id())
			{
				return AudioExceptionThread::RenderThread;
			}
			else if (currentThreadId == captureThread.get_id())
			{
				return AudioExceptionThread::CaptureThread;
			}

			for (size_t i = 0; i < queueThreads.size(); i++)
			{
				if (currentThreadId == queueThreads.at(i).get_id())
				{
					return AudioExceptionThread::QueueThread;
				}
			}

			return AudioExceptionThread::Other;
		}
		std::vector<std::shared_ptr<AudioObject>> NativeAudio::GetQueue(StringBuffer queueName) const
		{
			std::vector<std::shared_ptr<AudioObject>> queue = std::vector<std::shared_ptr<AudioObject>>(0);

			if (!queueName.CompareContent(""))
			{
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					std::shared_ptr<AudioObject> ao = audioObjects.at(i);
					if (ao != nullptr && ao->queueName == queueName)
					{
						queue.push_back(audioObjects.at(i));
					}
				}
			}

			return queue;
		}
		void NativeAudio::PlayNextInQueue(StringBuffer queueName, double queueDelay, uint32_t decreaseQueueIndex)
		{
			if (!queueName.CompareContent(""))
			{
				std::vector<std::shared_ptr<AudioObject>> queue = GetQueue(queueName);

				if (queue.size() > 0 && queueDelay > 0)
				{
					StopWatch::Reset();
					auto deltaTime = 0.0;

					while (!disposing && deltaTime < queueDelay)
					{
						deltaTime = StopWatch::DeltaTime(StopWatch::milli);
					}
				}

				if (!disposing)
				{
					for (size_t i = 0; i < queue.size(); i++)
					{
						std::shared_ptr<AudioObject> qao = queue.at(i);

						if (qao->queueIndex < decreaseQueueIndex)
						{
							RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "NativeAudio", "Queue is empty."));
							return;
						}

						if (qao->queueIndex == decreaseQueueIndex)
						{
							try
							{
								HEPHAUDIO_LOG_LINE("Playing the next file \"" + qao->name + "\" in queue: " + qao->queueName, ConsoleLogger::info);

								AudioFile audioFile(qao->filePath);
								Formats::IAudioFormat* format = audioFormats.GetAudioFormat(audioFile);

								if (format == nullptr)
								{
									RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, "NativeAudio", "File format '" + audioFile.Extension() + "' is not supported."));
									return;
								}

								qao->buffer = format->ReadFile(audioFile);
							}
							catch (AudioException ex)
							{
								RAISE_AUDIO_EXCPT(this, ex);
							}
						}

						qao->queueIndex -= decreaseQueueIndex;
					}
				}
			}
		}
		void NativeAudio::Mix(AudioBuffer& outputBuffer, uint32_t frameCount)
		{
			const double aoFactor = 1.0 / GetAOCountToMix();

			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				std::shared_ptr<AudioObject> audioObject = audioObjects.at(i);

				if (audioObject->IsPlaying())
				{
					const double volume = GetFinalAOVolume(audioObjects.at(i)) * aoFactor;

					AudioRenderEventArgs rArgs = AudioRenderEventArgs(this, audioObject.get(), frameCount);
					AudioRenderEventResult rResult;
					if (audioObject->OnRender)
					{
						audioObject->OnRender(&rArgs, &rResult);
					}

					for (size_t j = 0; j < frameCount && j < rResult.renderBuffer.FrameCount(); j++)
					{
						for (size_t k = 0; k < renderFormat.channelCount; k++)
						{
							outputBuffer.Set(outputBuffer.Get(j, k) + rResult.renderBuffer[j][k] * volume, j, k);
						}
					}

					if (!audioObject->constant)
					{
						if (rResult.isFinishedPlaying)
						{
							if (audioObject->loopCount == 1) // Finish playing.
							{
								StringBuffer queueName = audioObject->queueName;
								uint32_t queueDelay = audioObject->queueDelay;

								audioObjects.erase(audioObjects.begin() + i);
								i--;

								HEPHAUDIO_LOG_LINE("Finished playing the file \"" + audioObject->name + "\"", ConsoleLogger::info);

								if (!queueName.CompareContent(""))
								{
									if (queueDelay > 0)
									{
										queueThreads.push_back(std::thread(&NativeAudio::PlayNextInQueue, this, queueName, queueDelay, 1));
									}
									else
									{
										PlayNextInQueue(queueName, 0, 1);
									}
								}
							}
							else
							{
								if (audioObject->loopCount > 0) // 0 is infinite loop.
								{
									audioObject->loopCount--;
								}
								audioObject->frameIndex = 0;
							}
						}
					}
				}
			}
		}
		size_t NativeAudio::GetAOCountToMix() const
		{
			size_t result = 0;
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects.at(i)->IsPlaying())
				{
					result++;
				}
			}
			return result;
		}
		double NativeAudio::GetFinalAOVolume(std::shared_ptr<AudioObject> audioObject) const
		{
			if (audioObject == nullptr || audioObject->mute) { return 0.0; }

			double result = audioObject->volume;

			for (size_t i = 0; i < categories.size(); i++) // Calculate category volume.
			{
				for (size_t j = 0; j < audioObject->categories.size(); j++)
				{
					if (categories.at(i).name == audioObject->categories.at(j))
					{
						result *= categories.at(i).volume;
						break;
					}
				}
			}
			return result;
		}
		StringBuffer AudioExceptionThreadName(const AudioExceptionThread& t)
		{
			switch (t)
			{
			case AudioExceptionThread::MainThread:
				return "Main thread";
			case AudioExceptionThread::RenderThread:
				return "Render thread";
			case AudioExceptionThread::CaptureThread:
				return "Capture thread";
			case AudioExceptionThread::QueueThread:
				return "Queue thread";
			default:
				return "Unknown thread";
			}
		}
	}
}