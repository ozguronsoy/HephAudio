#include "AudioEffects/Vibrato.h"
#include <cmath>

using namespace Heph;

namespace HephAudio
{
	Vibrato::Vibrato() : ModulationEffect(), extent(1.0) {}

	Vibrato::Vibrato(double depth, double extent, const Oscillator& lfo) : ModulationEffect(depth, lfo), extent(1.0)
	{
		this->SetExtent(extent);
	}

	std::string Vibrato::Name() const
	{
		return "Vibrato";
	}

	size_t Vibrato::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		if (this->extent < 0)
			return outputFrameCount;

		const double peakAmplitude = this->lfoBuffer.AbsMax();
		const double extent_sample = ceil(formatInfo.sampleRate * (pow(2, HephAudio::SemitoneToOctave(this->extent)) - 1.0));
		return outputFrameCount + peakAmplitude * extent_sample + 1;
	}

	void Vibrato::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		const AudioFormatInfo& formatInfo = buffer.FormatInfo();
		const size_t extent_sample = this->CalculatePastSamplesSize(buffer);

		if (this->pastSamples.FormatInfo().sampleRate != formatInfo.sampleRate)
		{
			this->pastSamples = AudioBuffer(extent_sample, formatInfo.channelLayout, formatInfo.sampleRate);
		}
		else if (this->pastSamples.FrameCount() != extent_sample)
		{
			if (extent_sample > this->pastSamples.FrameCount())
			{
				this->pastSamples.Resize(extent_sample);
				this->pastSamples >>= extent_sample - this->pastSamples.FrameCount();
			}
			else
			{
				this->pastSamples <<= this->pastSamples.FrameCount() - extent_sample;
				this->pastSamples.Resize(extent_sample);
			}
		}

		AudioBuffer tempBuffer = this->pastSamples;
		if (frameCount >= extent_sample)
		{
			const size_t startIndex = frameCount - extent_sample;
			for (size_t i = 0; i < extent_sample; ++i)
			{
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					tempBuffer[i][j] = buffer[i + startIndex][j];
				}
			}
		}
		else
		{
			const size_t startIndex = extent_sample - frameCount;
			tempBuffer <<= frameCount;
			for (size_t i = startIndex; i < extent_sample; ++i)
			{
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					tempBuffer[i][j] = buffer[i - startIndex][j];
				}
			}
		}

		ModulationEffect::Process(buffer, startIndex, frameCount);

		this->pastSamples = std::move(tempBuffer);
	}

	double Vibrato::GetExtent() const
	{
		return this->extent;
	}

	void Vibrato::SetExtent(double extent)
	{
		this->extent = extent;
	}

	void Vibrato::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const double extent_sample = formatInfo.sampleRate * (pow(2, HephAudio::SemitoneToOctave(this->extent)) - 1.0);

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			double resampleIndex = i + this->lfoBuffer[(i + this->lfoIndex) % this->lfoBuffer.Size()] * extent_sample;
			const double resampleFactor = fabs(resampleIndex - floor(resampleIndex));

			resampleIndex = floor(resampleIndex);

			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				heph_audio_sample_t wetSample = 0;
				if (resampleIndex >= 0)
				{
					if (resampleIndex + 1 < inputBuffer.FrameCount())
						wetSample = inputBuffer[resampleIndex][j] * (1.0 - resampleFactor) + inputBuffer[resampleIndex + 1.0][j] * resampleFactor;
				}
				else
				{
					wetSample = this->pastSamples[resampleIndex + this->pastSamples.FrameCount()][j] * (1.0 - resampleFactor);
					wetSample += (resampleIndex == -1)
						? (inputBuffer[0][j] * resampleFactor)
						: (this->pastSamples[resampleIndex + this->pastSamples.FrameCount() + 1.0][j] * resampleFactor);
				}

				outputBuffer[i][j] = wetSample * this->depth + inputBuffer[i][j] * (1.0 - this->depth);
			}
		}
	}

	size_t Vibrato::CalculatePastSamplesSize(const AudioBuffer& inputBuffer) const
	{
		return fabs(ceil(inputBuffer.FormatInfo().sampleRate * (pow(2, HephAudio::SemitoneToOctave(this->extent)) - 1.0)));
	}
}