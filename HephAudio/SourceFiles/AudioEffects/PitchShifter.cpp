#include "AudioEffects/PitchShifter.h"
#include "Fourier.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	PitchShifter::PitchShifter() : FrequencyDomainEffect(), pitchFactor(1) {}

	PitchShifter::PitchShifter(size_t hopSize, const Window& wnd) : PitchShifter(0, hopSize, wnd) {}

	PitchShifter::PitchShifter(double toneChange, size_t hopSize, const Window& wnd) : FrequencyDomainEffect(hopSize, wnd)
	{
		this->SetToneChange(toneChange);
	}

	std::string PitchShifter::Name() const
	{
		return "Pitch Shifter";
	}

	bool PitchShifter::HasRTSupport() const
	{
		return false;
	}

	double PitchShifter::GetToneChange() const
	{
		return 12 * log2(this->pitchFactor);
	}

	void PitchShifter::SetToneChange(double toneChange)
	{
		this->pitchFactor = pow(2, toneChange / 12);
	}

	void PitchShifter::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		constexpr double twopi = 2.0 * HEPH_MATH_PI;

		int64_t firstWindowStartIndex = (this->currentIndex < this->pastSamples.FrameCount())
			? (((int64_t)startIndex) - ((int64_t)this->currentIndex))
			: (((int64_t)startIndex) - ((int64_t)this->pastSamples.FrameCount()));

		firstWindowStartIndex = (firstWindowStartIndex - (firstWindowStartIndex % this->hopSize));

		const int64_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const size_t fftSize = this->wnd.Size();
		const size_t nyquistBin = fftSize / 2;
		const double overflowFactor = 1.0 / this->CalculateMaxNumberOfOverlaps();

		DoubleBuffer lastAnalysisPhases(nyquistBin);
		DoubleBuffer lastSynthesisPhases(nyquistBin);
		DoubleBuffer synthesisMagnitudes(nyquistBin);
		DoubleBuffer synthesisFrequencies(nyquistBin);

		for (int64_t i = firstWindowStartIndex; i < endIndex; i += this->hopSize)
		{
			synthesisMagnitudes.Reset();
			synthesisFrequencies.Reset();

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
				for (size_t k = 0; k < nyquistBin; k++)
				{
					const double phase = channel[k].Phase();
					double phaseRemainder = phase - lastAnalysisPhases[k] - twopi * k * this->hopSize / fftSize;
					phaseRemainder = phaseRemainder >= 0 ? (fmod(phaseRemainder + HEPH_MATH_PI, twopi) - HEPH_MATH_PI) : (fmod(phaseRemainder - HEPH_MATH_PI, -twopi) + HEPH_MATH_PI);

					const size_t newBin = floor(k * this->pitchFactor + 0.5);
					if (newBin < nyquistBin)
					{
						synthesisMagnitudes[newBin] += channel[k].Magnitude();
						synthesisFrequencies[newBin] = (k + phaseRemainder * fftSize / twopi / this->hopSize) * this->pitchFactor;
					}

					lastAnalysisPhases[k] = phase;
				}

				for (size_t k = 0; k < nyquistBin; k++)
				{
					double synthesisPhase = twopi * this->hopSize / fftSize * synthesisFrequencies[k] + lastSynthesisPhases[k];
					synthesisPhase = synthesisPhase >= 0 ? (fmod(synthesisPhase + HEPH_MATH_PI, twopi) - HEPH_MATH_PI) : (fmod(synthesisPhase - HEPH_MATH_PI, -twopi) + HEPH_MATH_PI);

					channel[k] = Complex(synthesisMagnitudes[k] * cos(synthesisPhase), synthesisMagnitudes[k] * sin(synthesisPhase));
					channel[fftSize - k - 1] = channel[k].Conjugate();

					lastSynthesisPhases[k] = synthesisPhase;
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