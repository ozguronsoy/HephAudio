#include "AudioEffects/LowPassFilter.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Fourier.h"

using namespace Heph;

namespace HephAudio
{
	LowPassFilter::LowPassFilter() : FrequencyDomainEffect(), f(0) {}

	LowPassFilter::LowPassFilter(double f, size_t hopSize, const Window& wnd) : FrequencyDomainEffect(hopSize, wnd)
	{
		this->SetCutoffFreq(f);
	}

	std::string LowPassFilter::Name() const
	{
		return "Low Pass Filter";
	}

	double LowPassFilter::GetCutoffFreq() const
	{
		return this->f;
	}

	void LowPassFilter::SetCutoffFreq(double f)
	{
		if (f < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "cutoff frequency cannot be negative."));
		}

		this->f = f;
	}

	void LowPassFilter::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		int64_t firstWindowStartIndex = (this->currentIndex < this->pastSamples.FrameCount())
			? (((int64_t)startIndex) - ((int64_t)this->currentIndex))
			: (((int64_t)startIndex) - ((int64_t)this->pastSamples.FrameCount()));

		firstWindowStartIndex = (firstWindowStartIndex - (firstWindowStartIndex % this->hopSize));

		const int64_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const size_t fftSize = this->wnd.Size();
		const size_t nyquistBin = fftSize / 2;
		const size_t startBin = Fourier::BinFrequencyToIndex(formatInfo.sampleRate, fftSize, this->f);
		const double overflowFactor = 1.0 / this->CalculateMaxNumberOfOverlaps();

		for (int64_t i = firstWindowStartIndex; i < endIndex; i += this->hopSize)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				ComplexBuffer channel(fftSize);
				for (int64_t k = 0, l = i;
					(k < (int64_t)fftSize) && (l < (int64_t)inputBuffer.FrameCount());
					++k, ++l)
				{
					if (l < 0)
					{
						if ((-l) <= (int64_t)this->pastSamples.FrameCount())
						{
							channel[k].real = this->pastSamples[this->pastSamples.FrameCount() + l][j] * this->wnd[k];
						}
					}
					else
					{
						channel[k].real = inputBuffer[l][j] * this->wnd[k];
					}
				}

				Fourier::FFT(channel);
				for (size_t k = startBin; k < nyquistBin; ++k)
				{
					channel[k] = Complex();
					channel[fftSize - k - 1] = channel[k].Conjugate();
				}
				Fourier::IFFT(channel, false);

				for (int64_t k = 0, l = i;
					(k < (int64_t)fftSize) && (l < (int64_t)outputBuffer.FrameCount());
					++k, ++l)
				{
					if (l >= (int64_t)startIndex)
					{
						outputBuffer[l][j] += channel[k].real * overflowFactor * this->wnd[k] / fftSize;
					}
				}
			}
		}
	}
}