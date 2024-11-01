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
		if (threadCount == 0)
		{
			HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "threadCount cannot be 0, using the default value..."));
			this->threadCount = 1;
			return;
		}

		this->threadCount = threadCount;
	}

	void AudioEffect::Process(AudioBuffer& buffer)
	{
		this->Process(buffer, 0, buffer.FrameCount());
	}

	void AudioEffect::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
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