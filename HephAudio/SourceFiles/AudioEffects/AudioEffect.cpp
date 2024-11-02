#include "AudioEffects/AudioEffect.h"
#include "Exceptions/InvalidArgumentException.h"
#include <vector>
#include <thread>

using namespace Heph;

namespace HephAudio
{
	AudioEffect::AudioEffect() : threadCount(1) {}
	
	AudioEffect::AudioEffect(size_t threadCount) : AudioEffect()
	{
		this->SetThreadCount(threadCount);
	}

	size_t AudioEffect::GetThreadCount() const
	{
		return this->threadCount;
	}

	void AudioEffect::SetThreadCount(size_t threadCount)
	{
		this->threadCount = (threadCount == 0) ? (std::thread::hardware_concurrency()) : (threadCount);
		if (this->threadCount == 0)
		{
			this->threadCount = 1;
		}
	}

	size_t AudioEffect::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return outputFrameCount;
	}

	void AudioEffect::Process(AudioBuffer& buffer)
	{
		this->Process(buffer, 0, buffer.FrameCount());
	}

	void AudioEffect::Process(AudioBuffer& buffer, size_t startIndex)
	{
		if (startIndex > buffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "startIndex out of bounds."));
		}

		this->Process(buffer, startIndex, buffer.FrameCount() - startIndex);
	}

	void AudioEffect::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		if (startIndex > buffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "startIndex out of bounds."));
		}

		if (startIndex + frameCount > buffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "(startIndex + frameCount) exceeds the buffer's frame count."));
		}

		if (this->threadCount == 1)
			this->ProcessST(buffer, buffer, startIndex, frameCount);
		else
			this->ProcessMT(buffer, buffer, startIndex, frameCount);
	}

	void AudioEffect::ProcessMT(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t framesPerThread = frameCount / this->threadCount;
		const size_t remainingFrameCount = frameCount % this->threadCount;
		std::vector<std::thread> threads(this->threadCount);

		for (size_t i = 0; i < threads.size(); ++i)
		{
			threads[i] = std::thread(
				&AudioEffect::ProcessST, 
				this, 
				std::cref(inputBuffer), 
				std::ref(outputBuffer), 
				startIndex, 
				(i == threads.size() - 1) ? (framesPerThread + remainingFrameCount) : (framesPerThread)
			);
			startIndex += framesPerThread;
		}

		for (std::thread& t : threads)
		{
			if (t.joinable())
			{
				t.join();
			}
		}
	}
}