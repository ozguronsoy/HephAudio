#include "AudioEffects/Equalizer.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Fourier.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	Equalizer::FrequencyRange::FrequencyRange(double f1, double f2, double volume) : f1(f1), f2(f2), volume(volume) {}

	Equalizer::Equalizer() : FrequencyDomainEffect() {}

	Equalizer::Equalizer(size_t hopSize, const Window& wnd) : FrequencyDomainEffect(hopSize, wnd) {}

	Equalizer::Equalizer(size_t hopSize, const Window& wnd, const std::initializer_list<Equalizer::FrequencyRange>& frequencyRanges) 
		: FrequencyDomainEffect(hopSize, wnd)
	{
		for (const Equalizer::FrequencyRange& range : frequencyRanges)
		{
			this->AddFrequencyRange(range);
		}
	}

	Equalizer::Equalizer(size_t hopSize, const Window& wnd, const std::vector<Equalizer::FrequencyRange>& frequencyRanges) 
		: FrequencyDomainEffect(hopSize, wnd)
	{
		for (const Equalizer::FrequencyRange& range : frequencyRanges)
		{
			this->AddFrequencyRange(range);
		}
	}

	std::string Equalizer::Name() const
	{
		return "Equalizer";
	}

	const std::vector<Equalizer::FrequencyRange>& Equalizer::GetFrequencyRanges() const
	{
		return this->frequencyRanges;
	}

	void Equalizer::AddFrequencyRange(double f1, double f2, double volume)
	{
		this->AddFrequencyRange(Equalizer::FrequencyRange(f1, f2, volume));
	}

	void Equalizer::AddFrequencyRange(const Equalizer::FrequencyRange& range)
	{
		if (range.f1 < 0 || range.f2 < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "frequency cannot be negative."));
		}

		this->frequencyRanges.push_back(range);
	}

	void Equalizer::RemoveFrequencyRange(size_t index)
	{
		if (index >= this->frequencyRanges.size())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "index out of bounds."));
		}

		this->frequencyRanges.erase(this->frequencyRanges.begin() + index);
	}

	void Equalizer::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		int64_t firstWindowStartIndex = (this->currentIndex < this->pastSamples.FrameCount())
			? (((int64_t)startIndex) - ((int64_t)this->currentIndex))
			: (((int64_t)startIndex) - ((int64_t)this->pastSamples.FrameCount()));

		firstWindowStartIndex = (firstWindowStartIndex - (firstWindowStartIndex % this->hopSize));

		const int64_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const size_t fftSize = this->wnd.Size();
		const size_t nyquistBin = fftSize / 2;
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
				for (const Equalizer::FrequencyRange& range : this->frequencyRanges)
				{
					size_t startBin, endBin;
					if (range.f2 > range.f1)
					{
						startBin = Fourier::BinFrequencyToIndex(formatInfo.sampleRate, fftSize, range.f1);
						endBin = Fourier::BinFrequencyToIndex(formatInfo.sampleRate, fftSize, range.f2);
					}
					else
					{
						startBin = Fourier::BinFrequencyToIndex(formatInfo.sampleRate, fftSize, range.f2);
						endBin = Fourier::BinFrequencyToIndex(formatInfo.sampleRate, fftSize, range.f1);
					}
					endBin = HEPH_MATH_MIN(endBin, nyquistBin - 1);

					for (size_t k = startBin; k < endBin; ++k)
					{
						channel[k] *= range.volume;
						channel[fftSize - k - 1] = channel[k].Conjugate();
					}
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