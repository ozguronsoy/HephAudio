#include "AudioEffects/Flanger.h"
#include "Exceptions/InvalidArgumentException.h"
#include <cmath>

using namespace Heph;

namespace HephAudio
{
	Flanger::Flanger() : ModulationEffect(), constantDelay(0), variableDelay(0) {}

	Flanger::Flanger(double depth, double constantDelay, double variableDelay, const Oscillator& lfo)
		: ModulationEffect(depth, lfo), constantDelay(constantDelay), variableDelay(variableDelay) {}

	std::string Flanger::Name() const
	{
		return "Flanger";
	}

	void Flanger::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		const AudioFormatInfo& formatInfo = buffer.FormatInfo();
		const size_t maxDelay_sample = ceil((this->constantDelay + this->variableDelay) * 1e-3 * formatInfo.sampleRate);

		if (this->oldSamples.FormatInfo().sampleRate != formatInfo.sampleRate)
		{
			this->oldSamples = AudioBuffer(maxDelay_sample, formatInfo.channelLayout, formatInfo.sampleRate);
		}

		AudioBuffer tempBuffer = this->oldSamples;
		if (buffer.FrameCount() >= maxDelay_sample)
		{
			const size_t startIndex = buffer.FrameCount() - maxDelay_sample;
			for (size_t i = 0; i < maxDelay_sample; ++i)
			{
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					tempBuffer[i][j] = buffer[i + startIndex][j];
				}
			}
		}
		else
		{
			const size_t startIndex = maxDelay_sample - buffer.FrameCount();
			tempBuffer <<= buffer.FrameCount();
			for (size_t i = startIndex; i < maxDelay_sample; ++i)
			{
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					tempBuffer[i][j] = buffer[i - startIndex][j];
				}
			}
		}

		ModulationEffect::Process(buffer, startIndex, frameCount);

		this->oldSamples = std::move(tempBuffer);
	}

	double Flanger::GetConstantDelay() const
	{
		return this->constantDelay;
	}

	void Flanger::SetConstantDelay(double constantDelay)
	{
		if (constantDelay < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "delay must be greater than or equal to zero."));
		}

		this->constantDelay = constantDelay;
		this->SetOldSamplesSize();
	}

	double Flanger::GetVariableDelay() const
	{
		return this->variableDelay;
	}

	void Flanger::SetVariableDelay(double variableDelay)
	{
		if (variableDelay < 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "delay must be greater than or equal to zero."));
		}

		this->variableDelay = variableDelay;
		this->SetOldSamplesSize();
	}

	void Flanger::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const double constantDelay_sample = ceil(this->constantDelay * 1e-3 * formatInfo.sampleRate);
		const double variableDelay_sample = ceil(this->variableDelay * 1e-3 * formatInfo.sampleRate);

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			const double lfoSample = this->lfoBuffer[(i + this->lfoIndex) % this->lfoBuffer.Size()];
			const double delaySampleIndex = i - round(lfoSample * variableDelay_sample + constantDelay_sample);

			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				heph_audio_sample_t wetSample = 0;
				if (delaySampleIndex < 0)
					wetSample = this->oldSamples[this->oldSamples.FrameCount() + delaySampleIndex][j];
				else
					wetSample = inputBuffer[delaySampleIndex][j];

				outputBuffer[i][j] = wetSample * this->depth + inputBuffer[i][j] * (1.0 - this->depth);
			}
		}
	}

	void Flanger::SetOldSamplesSize()
	{
		const size_t maxDelay_sample = ceil((this->constantDelay + this->variableDelay) * 1e-3 * this->oldSamples.FormatInfo().sampleRate);
		const size_t frameCount = this->oldSamples.FrameCount();

		if (maxDelay_sample != frameCount)
		{
			if (maxDelay_sample > frameCount)
			{
				this->oldSamples.Resize(maxDelay_sample);
				this->oldSamples >>= maxDelay_sample - frameCount;
			}
			else
			{
				this->oldSamples <<= frameCount - maxDelay_sample;
				this->oldSamples.Resize(maxDelay_sample);
			}
		}
	}
}