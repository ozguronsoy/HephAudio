#include "AudioEffects/AudioEffect.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/NotSupportedException.h"
#include <vector>
#include <thread>

using namespace Heph;

namespace HephAudio
{
	AudioEffect::AudioEffect() : threadCount(1) {}

	bool AudioEffect::HasMTSupport() const
	{
		return true;
	}

	bool AudioEffect::HasRTSupport() const
	{
		return true;
	}

	size_t AudioEffect::GetThreadCount() const
	{
		return this->threadCount;
	}

	void AudioEffect::SetThreadCount(size_t threadCount)
	{
		if (!this->HasMTSupport())
			HEPH_RAISE_AND_THROW_EXCEPTION(this, NotSupportedException(HEPH_FUNC, "this effect does not support multithreaded processing."));

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

	size_t AudioEffect::CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return inputFrameCount;
	}

	size_t AudioEffect::CalculateAdvanceSize(size_t renderFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return renderFrameCount;
	}

	void AudioEffect::ResetInternalState()
	{
		return;
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
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "endIndex exceeds the buffer's frame count."));
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
		std::vector<std::thread> threads(this->threadCount - 1);

		for (std::thread& t : threads)
		{
			t = std::thread(
				&AudioEffect::ProcessST,
				this,
				std::cref(inputBuffer),
				std::ref(outputBuffer),
				startIndex,
				framesPerThread
			);
			startIndex += framesPerThread;
		}
		this->ProcessST(inputBuffer, outputBuffer, startIndex, framesPerThread + remainingFrameCount);

		for (std::thread& t : threads)
		{
			if (t.joinable())
			{
				t.join();
			}
		}
	}
}