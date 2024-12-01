#include "AudioEffects/Flanger.h"
#include "Exceptions/InvalidArgumentException.h"
#include <cmath>

using namespace Heph;

namespace HephAudio
{
	Flanger::Flanger() : ModulationEffect(), constantDelay(0), variableDelay(0) {}

	Flanger::Flanger(double depth, double constantDelay, double variableDelay, const Oscillator& lfo)
		: ModulationEffect(depth, lfo), constantDelay(0), variableDelay(0) 
	{
		this->SetConstantDelay(constantDelay);
		this->SetVariableDelay(variableDelay);
	}

	std::string Flanger::Name() const
	{
		return "Flanger";
	}

	void Flanger::ResetInternalState()
	{
		ModulationEffect::ResetInternalState();
		this->pastSamples.Release();
	}

	void Flanger::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		const AudioFormatInfo& formatInfo = buffer.FormatInfo();
		const size_t maxDelay_sample = this->CalculatePastSamplesSize(buffer);

		if (this->pastSamples.FormatInfo().sampleRate != formatInfo.sampleRate)
		{
			this->pastSamples = AudioBuffer(maxDelay_sample, formatInfo.channelLayout, formatInfo.sampleRate);
		}
		else if (this->pastSamples.FrameCount() != maxDelay_sample)
		{
			if (maxDelay_sample > this->pastSamples.FrameCount())
			{
				this->pastSamples.Resize(maxDelay_sample);
				this->pastSamples >>= maxDelay_sample - this->pastSamples.FrameCount();
			}
			else
			{
				this->pastSamples <<= this->pastSamples.FrameCount() - maxDelay_sample;
				this->pastSamples.Resize(maxDelay_sample);
			}
		}

		AudioBuffer tempBuffer = this->pastSamples;
		if (frameCount >= maxDelay_sample)
		{
			const size_t startIndex = frameCount - maxDelay_sample;
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
			const size_t startIndex = maxDelay_sample - frameCount;
			tempBuffer <<= frameCount;
			for (size_t i = startIndex; i < maxDelay_sample; ++i)
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
	}

	void Flanger::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const size_t constantDelay_sample = ceil(this->constantDelay * 1e-3 * formatInfo.sampleRate);
		const size_t variableDelay_sample = ceil(this->variableDelay * 1e-3 * formatInfo.sampleRate);

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			const double lfoSample = this->lfoBuffer[(i + this->lfoIndex) % this->lfoBuffer.Size()];
			const double delaySampleIndex = i - round(lfoSample * variableDelay_sample + constantDelay_sample);

			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				heph_audio_sample_t wetSample = 0;
				if (delaySampleIndex < 0)
					wetSample = this->pastSamples[this->pastSamples.FrameCount() + delaySampleIndex][j];
				else
					wetSample = inputBuffer[delaySampleIndex][j];

				outputBuffer[i][j] = wetSample * this->depth + inputBuffer[i][j] * (1.0 - this->depth);
			}
		}
	}

	size_t Flanger::CalculatePastSamplesSize(const AudioBuffer& inputBuffer) const
	{
		return ceil((this->constantDelay + this->variableDelay) * 1e-3 * inputBuffer.FormatInfo().sampleRate);
	}
}