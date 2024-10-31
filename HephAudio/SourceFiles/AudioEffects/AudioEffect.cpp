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
		if (buffer.FormatInfo().channelLayout != HEPHAUDIO_CH_LAYOUT_STEREO)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "buffer must be stereo."));
		}

		if (this->threadCount == 1)
			this->ProcessST(buffer, 0, buffer.FrameCount());
		else
			this->ProcessMT(buffer);
	}

	void AudioEffect::ProcessMT(AudioBuffer& buffer)
	{
		const size_t framesPerThread = buffer.FrameCount() / this->threadCount;
		const size_t remainingFrameCount = buffer.FrameCount() % this->threadCount;
		std::vector<std::thread> threads(this->threadCount);

		size_t startIndex = 0;
		for (size_t i = 0; i < threads.size(); ++i)
		{
			const size_t endIndex = startIndex + framesPerThread;
			threads[i] = std::thread(&AudioEffect::ProcessST, this, std::ref(buffer), startIndex, (i == threads.size() - 1) ? (endIndex + remainingFrameCount) : (endIndex));
			startIndex = endIndex;
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