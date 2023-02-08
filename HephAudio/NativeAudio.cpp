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
			AudioFormatInfo defaultFormat = AudioFormatInfo(1, 2, 16, 48000);
			audioObjects = std::vector<std::shared_ptr<AudioObject>>(0);
			categories = Categories(0);
			mainThreadId = std::this_thread::get_id();
			renderDeviceId = L"";
			captureDeviceId = L"";
			renderFormat = defaultFormat;
			captureFormat = defaultFormat;
			disposing = false;
			isRenderInitialized = false;
			isCaptureInitialized = false;
			isCapturePaused = false;
			displayName = L"";
			iconPath = L"";
			OnException = nullptr;
			OnDefaultAudioDeviceChange = nullptr;
			OnAudioDeviceAdded = nullptr;
			OnAudioDeviceRemoved = nullptr;
			OnCapture = nullptr;
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
				AudioFile audioFile(filePath);
				Formats::IAudioFormat* format = audioFormats.GetAudioFormat(audioFile);
				if (format == nullptr)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"NativeAudio::Play", L"File format '" + audioFile.Extension() + L"' not supported."));
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
		std::vector<std::shared_ptr<AudioObject>> NativeAudio::Queue(StringBuffer queueName, uint32_t queueDelay, std::vector<StringBuffer> filePaths)
		{
			if (queueName == L"")
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"NativeAudio::Queue", L"Queue name must not be empty."));
				return std::vector<std::shared_ptr<AudioObject>>(0);
			}
			std::vector<std::shared_ptr<AudioObject>> objects;
			const size_t currentQueueSize = GetQueue(queueName).size();
			size_t failedCount = 0;
			for (size_t i = 0; i < filePaths.size(); i++)
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
							throw AudioException(E_INVALIDARG, L"NativeAudio::Play", L"File format '" + audioFile.Extension() + L"' not supported.");
						}
						ao->buffer = format->ReadFile(audioFile);
					}
					catch (AudioException ex)
					{
						RAISE_AUDIO_EXCPT(this, ex);
						failedCount++;
						continue;
					}
				}
				audioObjects.push_back(ao);
				objects.push_back(ao);
			}
			return objects;
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
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"NativeAudio::SetAOPosition", L"position must be between 0 and 1."));
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
					DestroyAO(queue.at(i));
				}
			}
			else
			{
				uint32_t queueDelay = 0u;
				for (size_t i = 0; i < skipCount; i++)
				{
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
					PlayNextInQueue(queueName, queueDelay, skipCount);
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
			std::vector<AudioDevice> devices = GetAudioDevices(AudioDeviceType::All, true);
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
			return GetAudioDeviceById(renderDeviceId);
		}
		AudioDevice NativeAudio::GetCaptureDevice() const
		{
			return GetAudioDeviceById(captureDeviceId);
		}
		bool NativeAudio::SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat)
		{
			try
			{
				AudioProcessor::ConvertSampleRate(buffer, targetFormat.sampleRate);
				AudioProcessor::ConvertBPS(buffer, targetFormat.bitsPerSample);
				AudioProcessor::ConvertChannels(buffer, targetFormat.channelCount);
				Formats::IAudioFormat* format = audioFormats.GetAudioFormat(filePath);
				if (format == nullptr)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"NativeAudio::SaveToFile", L"File format '" + AudioFile::GetFileExtension(filePath) + L"' not supported."));
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
			if (queueName != L"")
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
		void NativeAudio::PlayNextInQueue(StringBuffer queueName, uint32_t queueDelay, uint32_t decreaseQueueIndex)
		{
			if (queueName != L"")
			{
				std::vector<std::shared_ptr<AudioObject>> queue = GetQueue(queueName);
				if (queue.size() > 0 && queueDelay > 0)
				{
					auto start = std::chrono::high_resolution_clock::now();
					auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - start);
					while (deltaTime < std::chrono::milliseconds(queueDelay))
					{
						if (disposing)
						{
							return;
						}
						deltaTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - start);
					}
				}
				for (size_t i = 0; i < queue.size(); i++)
				{
					std::shared_ptr<AudioObject> qao = queue.at(i);
					if (qao->queueIndex < decreaseQueueIndex)
					{
						RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"NativeAudio", L"Queue is empty."));
						return;
					}
					if (qao->queueIndex == decreaseQueueIndex)
					{
						try
						{
							AudioFile audioFile(qao->filePath);
							Formats::IAudioFormat* format = audioFormats.GetAudioFormat(audioFile);
							if (format == nullptr)
							{
								RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"NativeAudio", L"File format '" + audioFile.Extension() + L"' not supported."));
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
		void NativeAudio::Mix(AudioBuffer& outputBuffer, uint32_t frameCount)
		{
			const double aoFactor = 1.0 / GetAOCountToMix();
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				std::shared_ptr<AudioObject> audioObject = audioObjects.at(i);
				if (audioObject->IsPlaying())
				{
					const double volume = GetFinalAOVolume(audioObjects.at(i)) * aoFactor;
					const size_t nFramesToRead = ceil((double)frameCount * (double)audioObject->buffer.FormatInfo().sampleRate / (double)renderFormat.sampleRate);
					size_t frameIndex = 0;
					if (audioObject->GetSubBuffer == nullptr)
					{
						AudioException exception = AudioException(E_INVALIDARG, L"NativeAudio::Mix", L"AudioObject::GetSubBuffer method must not be null, if a custom method is not necessary use the default method.");
						RAISE_AUDIO_EXCPT(this, exception);
						throw exception;
					}
					AudioBuffer subBuffer = audioObject->GetSubBuffer(audioObject.get(), nFramesToRead, &frameIndex);
					if (audioObject->OnRender != nullptr)
					{
						audioObject->OnRender(audioObject.get(), subBuffer, frameIndex, frameCount);
					}
					for (size_t j = 0; j < frameCount && j < subBuffer.FrameCount(); j++)
					{
						for (size_t k = 0; k < renderFormat.channelCount; k++)
						{
							outputBuffer.Set(outputBuffer.Get(j, k) + subBuffer[j][k] * volume, j, k);
						}
					}
					if (!audioObject->constant)
					{
						audioObject->frameIndex += nFramesToRead;
						if (audioObject->IsFinishedPlaying == nullptr)
						{
							AudioException exception = AudioException(E_INVALIDARG, L"NativeAudio::Mix", L"AudioObject::IsFinishedPlaying method must not be null, if a custom method is not necessary use the default method.");
							RAISE_AUDIO_EXCPT(this, exception);
							throw exception;
						}
						if (audioObject->IsFinishedPlaying(audioObject.get()))
						{
							if (audioObject->loopCount == 1) // Finish playing.
							{
								StringBuffer queueName = audioObject->queueName;
								uint32_t queueDelay = audioObject->queueDelay;
								audioObjects.erase(audioObjects.begin() + i);
								i--;
								if (queueName != L"")
								{
									queueThreads.push_back(std::thread(&NativeAudio::PlayNextInQueue, this, queueName, queueDelay, 1));
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