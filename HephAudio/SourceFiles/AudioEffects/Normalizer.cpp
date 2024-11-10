#include "AudioEffects/Normalizer.h"
#include "Exceptions/InvalidArgumentException.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	Normalizer::Normalizer() : Normalizer(HEPH_AUDIO_SAMPLE_MAX) {}

	Normalizer::Normalizer(heph_audio_sample_t peakAmplitude) : Normalizer(peakAmplitude, 0.99) {}

	Normalizer::Normalizer(heph_audio_sample_t peakAmplitude, double smoothingFactor)
		: AudioEffect(), globalMaxSample(HEPH_AUDIO_SAMPLE_MIN), threadsCompleted(0),
		lastGain(1.0), smoothingFactor(smoothingFactor)
	{
		this->SetPeakAmplitude(peakAmplitude);
	}

	std::string Normalizer::Name() const
	{
		return "Normalizer";
	}

	heph_audio_sample_t Normalizer::GetPeakAmplitude() const
	{
		return this->peakAmplitude;
	}

	void Normalizer::SetPeakAmplitude(heph_audio_sample_t peakAmplitude)
	{
		if (peakAmplitude < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "peakAmplitude cannot be negative."));
		}

		this->peakAmplitude = peakAmplitude;
	}

	double Normalizer::GetSmoothingFactor() const
	{
		return this->smoothingFactor;
	}

	void Normalizer::SetSmoothingFactor(double smoothingFactor)
	{
		if (smoothingFactor < 0 || smoothingFactor >= 1)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "smoothingFactor must be in range of [0, 1)."));
		}

		this->smoothingFactor = smoothingFactor;
	}

	void Normalizer::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = outputBuffer.FormatInfo();
		double currentGain = this->lastGain;

		heph_audio_sample_t maxSample = HEPH_AUDIO_SAMPLE_MIN;
		for (size_t i = startIndex; i < endIndex; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				const heph_audio_sample_t absSample = abs(outputBuffer[i][j]);
				maxSample = HEPH_MATH_MAX(absSample, maxSample);
			}
		}

		{
			std::unique_lock<std::mutex> lk(this->mutex);
			this->globalMaxSample = HEPH_MATH_MAX(maxSample, this->globalMaxSample);
			this->threadsCompleted++;
			if (this->threadsCompleted == this->threadCount)
			{
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

		if (this->globalMaxSample != 0)
		{
			const double targetGain = (double)this->peakAmplitude / (double)this->globalMaxSample;

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
	}
}