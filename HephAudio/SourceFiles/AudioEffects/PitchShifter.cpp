#include "AudioEffects/PitchShifter.h"
#include "Fourier.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	PitchShifter::PitchShifter() : OlaEffect(), pitchFactor(1) {}

	PitchShifter::PitchShifter(size_t hopSize, const Window& wnd) : PitchShifter(0, hopSize, wnd) {}

	PitchShifter::PitchShifter(double toneChange, size_t hopSize, const Window& wnd) : OlaEffect(hopSize, wnd)
	{
		this->SetToneChange(toneChange);
	}

	std::string PitchShifter::Name() const
	{
		return "Pitch Shifter";
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
		startIndex += this->currentIndex;
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const int64_t wndSize = this->wnd.Size();
		const int64_t ioFrameCount = outputBuffer.FrameCount() + this->currentIndex;
		const size_t maxNumberOfOverlaps = this->CalculateMaxNumberOfOverlaps();
		const double overflowFactor = 1.0 / maxNumberOfOverlaps;

		// in multithreaded use startIndex instead
		// since it causes distortion at the edge samples between threads.
		const size_t firstWindowStartIndex = (this->threadCount != 1)
			? (startIndex)
			: ((startIndex >= (this->hopSize * maxNumberOfOverlaps))
				? (startIndex - (startIndex % this->hopSize) - (this->hopSize * (maxNumberOfOverlaps - 1)))
				: (0));

		for (size_t i = firstWindowStartIndex; i < endIndex; i += this->hopSize)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				double m = i;
				for (size_t k = 0, l = i;
					(k < wndSize) && (l < ioFrameCount) && (m < ioFrameCount);
					++k, ++l, m += this->pitchFactor
					)
				{
					if (l >= this->currentIndex)
					{
						if (m < this->currentIndex)
						{
							const size_t n = this->currentIndex - floor(m);
							if (n <= this->pastSamples.FrameCount())
							{
								outputBuffer[l - this->currentIndex][j] += this->pastSamples[this->pastSamples.FrameCount() - n][j] * overflowFactor * this->wnd[k];
							}
						}
						else
						{
							outputBuffer[l - this->currentIndex][j] += inputBuffer[m - this->currentIndex][j] * overflowFactor * this->wnd[k];
						}
					}
				}
			}
		}
	}
}