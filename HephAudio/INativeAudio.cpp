#include "INativeAudio.h"
#include "AudioProcessor.h"
#include "AudioFormats.h"

namespace HephAudio
{
	namespace Native
	{
		INativeAudio::INativeAudio()
		{
			AudioFormatInfo defaultFormat = AudioFormatInfo(1, 2, 16, 48000);
			audioObjects = std::vector<std::shared_ptr<IAudioObject>>(0);
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
		std::shared_ptr<IAudioObject> INativeAudio::Play(std::wstring filePath)
		{
			return Play(filePath, 1u, false);
		}
		std::shared_ptr<IAudioObject> INativeAudio::Play(std::wstring filePath, bool isPaused)
		{
			return Play(filePath, 1u, isPaused);
		}
		std::shared_ptr<IAudioObject> INativeAudio::Play(std::wstring filePath, uint32_t loopCount)
		{
			return Play(filePath, loopCount, false);
		}
		std::shared_ptr<IAudioObject> INativeAudio::Play(std::wstring filePath, uint32_t loopCount, bool isPaused)
		{
			try
			{
				AudioFile audioFile(filePath);
				Formats::IAudioFormat* format = audioFormats.GetAudioFormat(audioFile);
				if (format == nullptr)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"INativeAudio::Play", (L"File format '" + audioFile.Extension() + L"' not supported.").c_str()));
					return nullptr;
				}
				std::shared_ptr<IAudioObject> ao = std::shared_ptr<IAudioObject>(new IAudioObject());
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
		std::vector<std::shared_ptr<IAudioObject>> INativeAudio::Queue(std::wstring queueName, uint32_t queueDelay, std::vector<std::wstring> filePaths)
		{
			if (queueName == L"")
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"INativeAudio::Queue", L"Queue name must not be empty."));
				return std::vector<std::shared_ptr<IAudioObject>>(0);
			}
			std::vector<std::shared_ptr<IAudioObject>> objects;
			const size_t currentQueueSize = GetQueue(queueName).size();
			size_t failedCount = 0;
			for (size_t i = 0; i < filePaths.size(); i++)
			{
				std::shared_ptr<IAudioObject> ao = std::shared_ptr<IAudioObject>(new IAudioObject());
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
							throw AudioException(E_INVALIDARG, L"INativeAudio::Play", (L"File format '" + audioFile.Extension() + L"' not supported.").c_str());
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
		std::shared_ptr<IAudioObject> INativeAudio::Load(std::wstring filePath)
		{
			std::shared_ptr<IAudioObject> pao = Play(filePath, 1u, true);
			if (pao != nullptr && isRenderInitialized)
			{
				AudioProcessor::ConvertSampleRate(pao->buffer, renderFormat.sampleRate);
				AudioProcessor::ConvertChannels(pao->buffer, renderFormat.channelCount);
			}
			return pao;
		}
		std::shared_ptr<IAudioObject> INativeAudio::CreateAO(std::wstring name, size_t bufferFrameCount)
		{
			std::shared_ptr<IAudioObject> ao = std::shared_ptr<IAudioObject>(new IAudioObject());
			ao->name = name;
			ao->buffer = AudioBuffer(bufferFrameCount > 0 ? bufferFrameCount : renderFormat.sampleRate, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, renderFormat.channelCount, sizeof(double) * 2, renderFormat.sampleRate));
			ao->constant = true;
			audioObjects.push_back(ao);
			return ao;
		}
		bool INativeAudio::DestroyAO(std::shared_ptr<IAudioObject> audioObject)
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
		bool INativeAudio::AOExists(std::shared_ptr<IAudioObject> audioObject) const
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
		void INativeAudio::SetAOPosition(std::shared_ptr<IAudioObject> audioObject, double position)
		{
			if (position < 0.0 || position > 1.0)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"INativeAudio::SetAOPosition", L"position must be between 0 and 1."));
				return;
			}
			if (AOExists(audioObject))
			{
				audioObject->frameIndex = audioObject->buffer.FrameCount() * position;
			}
		}
		double INativeAudio::GetAOPosition(std::shared_ptr<IAudioObject> audioObject) const
		{
			if (AOExists(audioObject))
			{
				return (double)audioObject->frameIndex / (double)audioObject->buffer.FrameCount();
			}
			return -1.0;
		}
		void INativeAudio::PauseCapture(bool pause)
		{
			isCapturePaused = pause;
		}
		bool INativeAudio::IsCapturePaused() const noexcept
		{
			return isCapturePaused;
		}
		void INativeAudio::SetCategoryVolume(std::wstring categoryName, double newVolume)
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
		double INativeAudio::GetCategoryVolume(std::wstring categoryName) const
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
		void INativeAudio::RegisterCategory(Category category)
		{
			if (!CategoryExists(category.name))
			{
				categories.push_back(category);
			}
		}
		void INativeAudio::UnregisterCategory(std::wstring categoryName)
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
		bool INativeAudio::CategoryExists(std::wstring categoryName) const
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
		void INativeAudio::Skip(std::wstring queueName, bool applyDelay)
		{
			Skip(1, queueName, applyDelay);
		}
		void INativeAudio::Skip(size_t skipCount, std::wstring queueName, bool applyDelay)
		{
			if (skipCount == 0) { return; }
			std::vector<std::shared_ptr<IAudioObject>> queue = GetQueue(queueName);
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
					std::shared_ptr<IAudioObject> qao = queue.at(i);
					if (qao->queueIndex == 0)
					{
						queueDelay = qao->queueDelay;
					}
					DestroyAO(queue.at(i));
				}
				if (applyDelay && queueDelay > 0)
				{
					queueThreads.push_back(std::thread(&INativeAudio::PlayNextInQueue, this, queueName, queueDelay, skipCount));
				}
				else
				{
					PlayNextInQueue(queueName, queueDelay, skipCount);
				}
			}
		}
		AudioFormatInfo INativeAudio::GetRenderFormat() const
		{
			return renderFormat;
		}
		AudioFormatInfo INativeAudio::GetCaptureFormat() const
		{
			return captureFormat;
		}
		AudioDevice INativeAudio::GetAudioDeviceById(std::wstring deviceId) const
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
		AudioDevice INativeAudio::GetRenderDevice() const
		{
			return GetAudioDeviceById(renderDeviceId);
		}
		AudioDevice INativeAudio::GetCaptureDevice() const
		{
			return GetAudioDeviceById(captureDeviceId);
		}
		bool INativeAudio::SaveToFile(std::wstring filePath, bool overwrite, AudioBuffer& buffer, AudioFormatInfo targetFormat)
		{
			try
			{
				AudioProcessor::ConvertSampleRate(buffer, targetFormat.sampleRate);
				AudioProcessor::ConvertBPS(buffer, targetFormat.bitsPerSample);
				AudioProcessor::ConvertChannels(buffer, targetFormat.channelCount);
				Formats::IAudioFormat* format = audioFormats.GetAudioFormat(filePath);
				if (format == nullptr)
				{
					RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"INativeAudio::SaveToFile", (L"File format '" + AudioFile::GetFileExtension(filePath) + L"' not supported.").c_str()));
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
		void INativeAudio::JoinRenderThread()
		{
			if (renderThread.joinable())
			{
				renderThread.join();
			}
		}
		void INativeAudio::JoinCaptureThread()
		{
			if (captureThread.joinable())
			{
				captureThread.join();
			}
		}
		void INativeAudio::JoinQueueThreads()
		{
			for (size_t i = 0; i < queueThreads.size(); i++)
			{
				if (queueThreads.at(i).joinable())
				{
					queueThreads.at(i).join();
				}
			}
		}
		AudioExceptionThread INativeAudio::GetCurrentThread() const
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
		std::vector<std::shared_ptr<IAudioObject>> INativeAudio::GetQueue(std::wstring queueName) const
		{
			std::vector<std::shared_ptr<IAudioObject>> queue = std::vector<std::shared_ptr<IAudioObject>>(0);
			if (queueName != L"")
			{
				for (size_t i = 0; i < audioObjects.size(); i++)
				{
					std::shared_ptr<IAudioObject> ao = audioObjects.at(i);
					if (ao != nullptr && ao->queueName == queueName)
					{
						queue.push_back(audioObjects.at(i));
					}
				}
			}
			return queue;
		}
		void INativeAudio::PlayNextInQueue(std::wstring queueName, uint32_t queueDelay, uint32_t decreaseQueueIndex)
		{
			if (queueName != L"")
			{
				std::vector<std::shared_ptr<IAudioObject>> queue = GetQueue(queueName);
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
					std::shared_ptr<IAudioObject> qao = queue.at(i);
					if (qao->queueIndex < decreaseQueueIndex)
					{
						RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"INativeAudio", L"Queue is empty."));
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
								RAISE_AUDIO_EXCPT(this, AudioException(E_INVALIDARG, L"INativeAudio", (L"File format '" + audioFile.Extension() + L"' not supported.").c_str()));
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
		void INativeAudio::Mix(AudioBuffer& outputBuffer, uint32_t frameCount)
		{
			const double aoFactor = 1.0 / GetAOCountToMix();
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				std::shared_ptr<IAudioObject> audioObject = audioObjects.at(i);
				if (audioObject->IsPlaying())
				{
					const double volume = GetFinalAOVolume(audioObjects.at(i)) * aoFactor;
					const size_t nFramesToRead = ceil((double)frameCount * (double)audioObject->buffer.FormatInfo().sampleRate / (double)renderFormat.sampleRate);
					size_t frameIndex = 0;
					if (audioObject->GetSubBuffer == nullptr)
					{
						AudioException exception = AudioException(E_INVALIDARG, L"INativeAudio::Mix", L"IAudioObject::GetSubBuffer method must not be null, if a custom method is not necessary use the default method.");
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
							AudioException exception = AudioException(E_INVALIDARG, L"INativeAudio::Mix", L"IAudioObject::IsFinishedPlaying method must not be null, if a custom method is not necessary use the default method.");
							RAISE_AUDIO_EXCPT(this, exception);
							throw exception;
						}
						if (audioObject->IsFinishedPlaying(audioObject.get()))
						{
							if (audioObject->loopCount == 1) // Finish playing.
							{
								std::wstring queueName = audioObject->queueName;
								uint32_t queueDelay = audioObject->queueDelay;
								audioObjects.erase(audioObjects.begin() + i);
								i--;
								if (queueName != L"")
								{
									queueThreads.push_back(std::thread(&INativeAudio::PlayNextInQueue, this, queueName, queueDelay, 1));
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
		size_t INativeAudio::GetAOCountToMix() const
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
		double INativeAudio::GetFinalAOVolume(std::shared_ptr<IAudioObject> audioObject) const
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
	}
}