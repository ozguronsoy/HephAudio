#include "NativeAudio.h"
#include "AudioProcessor.h"
#include "AudioFileFormatManager.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		NativeAudio::NativeAudio()
			: mainThreadId(std::this_thread::get_id()), renderDeviceId(""), captureDeviceId("")
			, renderFormat(AudioFormatInfo(1, 2, 16, 48000)), captureFormat(AudioFormatInfo(1, 2, 16, 48000)), disposing(false)
			, isRenderInitialized(false), isCaptureInitialized(false), isCapturePaused(false), displayName(""), iconPath(""), deviceEnumerationPeriod_ms(100)
		{
			HEPHAUDIO_STOPWATCH_START;
		}
		AudioObject* NativeAudio::Play(StringBuffer filePath)
		{
			return this->Play(filePath, 1u, false);
		}
		AudioObject* NativeAudio::Play(StringBuffer filePath, uint32_t loopCount)
		{
			return this->Play(filePath, loopCount, false);
		}
		AudioObject* NativeAudio::Play(StringBuffer filePath, uint32_t loopCount, bool isPaused)
		{
			try
			{
				HEPHAUDIO_LOG("Playing \"" + File::GetFileName(filePath) + "\"", HEPH_CL_INFO);

				File audioFile(filePath, FileOpenMode::Read);
				FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(audioFile);

				if (format == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "NativeAudio::Play", "File format '" + audioFile.FileExtension() + "' is not supported."));
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
				audioObject.loopCount = loopCount;
				audioObject.isPaused = isPaused;

				return &audioObject;
			}
			catch (HephException ex)
			{
				RAISE_HEPH_EXCEPTION(this, ex);
				return nullptr;
			}
		}
		AudioObject* NativeAudio::Load(StringBuffer filePath)
		{
			AudioObject* pao = Play(filePath, 1u, true);
			if (pao != nullptr && isRenderInitialized)
			{
				AudioProcessor::ConvertSampleRate(pao->buffer, renderFormat.sampleRate);
				AudioProcessor::ConvertChannels(pao->buffer, renderFormat.channelCount);
			}
			return pao;
		}
		AudioObject* NativeAudio::CreateAO(StringBuffer name, size_t bufferFrameCount)
		{
#if CPP_VERSION >= CPP_VERSION_17
			AudioObject& audioObject = audioObjects.emplace_back();
#else
			audioObjects.emplace_back();
			AudioObject& audioObject = audioObjects[audioObjects.size() - 1];
#endif

			audioObject.name = name;
			audioObject.buffer = AudioBuffer(bufferFrameCount, HEPHAUDIO_INTERNAL_FORMAT(renderFormat.channelCount, renderFormat.sampleRate));
			return &audioObject;
		}
		bool NativeAudio::DestroyAO(AudioObject* pAudioObject)
		{
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
		AudioObject* NativeAudio::GetAO(size_t index)
		{
			if (index >= audioObjects.size())
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "NativeAudio::GetAO", "Index out of range."));
				return nullptr;
			}
			return &audioObjects[index];
		}
		AudioObject* NativeAudio::GetAO(StringBuffer aoName)
		{
			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				if (audioObjects.at(i).name == aoName)
				{
					return &audioObjects.at(i);
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
		uint32_t NativeAudio::GetDeviceEnumerationPeriod() const noexcept
		{
			return this->deviceEnumerationPeriod_ms;
		}
		void NativeAudio::SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms) noexcept
		{
			this->deviceEnumerationPeriod_ms = deviceEnumerationPeriod_ms;
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
				RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "WinAudioDS::GetDefaultAudioDevice", "DeviceType must be either Render or Capture."));
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
				RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "WinAudioDS::GetAudioDevices", "DeviceType must not be Null."));
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
		bool NativeAudio::SaveToFile(StringBuffer filePath, bool overwrite, AudioBuffer& buffer)
		{
			try
			{
				FileFormats::IAudioFileFormat* format = FileFormats::AudioFileFormatManager::FindFileFormat(filePath);

				if (format == nullptr)
				{
					RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "NativeAudio::SaveToFile", "File format '" + File::GetFileExtension(filePath) + "' is not supported."));
					return false;
				}

				return format->SaveToFile(filePath, buffer, overwrite);
			}
			catch (HephException ex)
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
		void NativeAudio::Mix(AudioBuffer& outputBuffer, uint32_t frameCount)
		{
			const size_t mixedAOCount = GetAOCountToMix();
			AudioBuffer mixBuffer(frameCount, HEPHAUDIO_INTERNAL_FORMAT(renderFormat.channelCount, renderFormat.sampleRate));

			for (size_t i = 0; i < audioObjects.size(); i++)
			{
				AudioObject* pAudioObject = &audioObjects.at(i);

				if (!pAudioObject->isPaused)
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

							HEPHAUDIO_LOG("Finished playing the file \"" + pAudioObject->name + "\"", HEPH_CL_INFO);

							audioObjects.erase(audioObjects.begin() + i);
							i--;
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
				if (!audioObjects.at(i).isPaused)
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