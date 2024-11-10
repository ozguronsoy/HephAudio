#include "AudioEffects/RmsNormalizer.h"
#include "Exceptions/InvalidArgumentException.h"
#include <cmath>

#define DEFAULT_RMS (0.1 * (HEPH_AUDIO_SAMPLE_MAX))

using namespace Heph;

namespace HephAudio
{
	RmsNormalizer::RmsNormalizer() : RmsNormalizer(DEFAULT_RMS) {}

	RmsNormalizer::RmsNormalizer(heph_audio_sample_t targetRms) : RmsNormalizer(targetRms, 0.99) {}

	RmsNormalizer::RmsNormalizer(heph_audio_sample_t targetRms, double smoothingFactor)
		: AudioEffect(), currentRms(0), threadsCompleted(0),
		lastGain(1.0), smoothingFactor(smoothingFactor)
	{
		this->SetTargetRms(targetRms);
	}

	std::string RmsNormalizer::Name() const
	{
		return "RMS Normalizer";
	}

	double RmsNormalizer::GetTargetRms() const
	{
		return this->targetRms;
	}

	void RmsNormalizer::SetTargetRms(double targetRms)
	{
		if (targetRms < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "targetRms cannot be negative."));
		}

		this->targetRms = targetRms;
	}

	double RmsNormalizer::GetSmoothingFactor() const
	{
		return this->smoothingFactor;
	}

	void RmsNormalizer::SetSmoothingFactor(double smoothingFactor)
	{
		if (smoothingFactor < 0 || smoothingFactor >= 1)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "smoothingFactor must be in range of [0, 1)."));
		}

		this->smoothingFactor = smoothingFactor;
	}

	void RmsNormalizer::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();
		const double sampleCount = outputBuffer.FrameCount();
		double currentGain = this->lastGain;

		double meanSquared = 0;
		for (size_t i = startIndex; i < endIndex; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				meanSquared += outputBuffer[i][j] * outputBuffer[i][j] / sampleCount;
			}
		}

		{
			std::unique_lock<std::mutex> lk(this->mutex);
			this->currentRms += meanSquared;
			this->threadsCompleted++;
			if (this->threadsCompleted == this->threadCount)
			{
				this->currentRms = sqrt(this->currentRms);
				this->threadsCompleted = 0;
				lk.unlock();
				this->cv.notify_all();
			}
			else
			{
				this->cv.wait(lk, [this] { return this->threadsCompleted == 0; });
				lk.unlock();
			}
		}

		if (this->currentRms != 0)
		{
			const double targetGain = this->targetRms / this->currentRms;

			for (size_t i = 0; i < startIndex; ++i)
			{
				currentGain = currentGain * this->smoothingFactor + targetGain * (1.0 - this->smoothingFactor);
			}

			for (size_t i = startIndex; i < endIndex; ++i)
			{
				currentGain = currentGain * this->smoothingFactor + targetGain * (1.0 - this->smoothingFactor);
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					outputBuffer[i][j] *= currentGain;
				}
			}

			if (endIndex == outputBuffer.FrameCount())
			{
				this->lastGain = currentGain;
			}
		}

		std::lock_guard<std::mutex> lk(this->mutex);
		this->threadsCompleted++;
		if (this->threadsCompleted == this->threadCount)
		{
			this->threadsCompleted = 0;
			this->currentRms = 0;
		}
	}
}