#include "AudioProcessor.h"
#include "AudioException.h"
#include "Fourier.h"
#include "AudioFile.h"
#include <thread>

namespace HephAudio
{
#pragma region Converts, Mix, Split/Merge Channels
	void AudioProcessor::ConvertBPS(AudioBuffer& buffer, uint16_t outputBps)
	{
		throw AudioException(E_NOTIMPL, "AudioProcessor::ConvertBPS", "Not implemented.");
	}
	void AudioProcessor::ConvertChannels(AudioBuffer& buffer, uint16_t outputChannelCount)
	{
		if (buffer.formatInfo.channelCount != outputChannelCount)
		{
			AudioBuffer resultBuffer(buffer.frameCount, AudioFormatInfo(buffer.formatInfo.formatTag, outputChannelCount, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate));

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				hephaudio_float averageValue = 0.0f;
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					averageValue += buffer[i][j];
				}
				averageValue /= buffer.formatInfo.channelCount;

				for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
				{
					resultBuffer[i][j] = averageValue;
				}
			}

			buffer = std::move(resultBuffer);
		}
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate)
	{
		const hephaudio_float srRatio = (hephaudio_float)outputSampleRate / (hephaudio_float)buffer.formatInfo.sampleRate;
		if (srRatio != 1.0)
		{
			const size_t targetFrameCount = ceil((hephaudio_float)buffer.frameCount * srRatio);
			AudioBuffer resultBuffer(targetFrameCount, AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, buffer.formatInfo.bitsPerSample, outputSampleRate));

			for (size_t i = 0; i < targetFrameCount; i++)
			{
				const hephaudio_float resampleIndex = i / srRatio;
				const hephaudio_float rho = resampleIndex - floor(resampleIndex);

				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					if (resampleIndex + 1.0 >= buffer.frameCount) break;
					resultBuffer[i][j] = buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho;
				}
			}

			buffer = std::move(resultBuffer);
		}
	}
	void AudioProcessor::ConvertSampleRateRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, uint32_t outputSampleRate, size_t outFrameCount)
	{
		const hephaudio_float srRatio = (hephaudio_float)outputSampleRate / (hephaudio_float)originalBuffer.formatInfo.sampleRate;
		if (srRatio != 1.0)
		{
			subBuffer.Resize(outFrameCount);
			subBuffer.formatInfo.sampleRate = outputSampleRate;

			for (size_t i = 0; i < outFrameCount; i++)
			{
				const hephaudio_float resampleIndex = i / srRatio + subBufferFrameIndex;
				const hephaudio_float rho = resampleIndex - floor(resampleIndex);

				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					if (resampleIndex + 1.0 >= originalBuffer.frameCount) break;
					subBuffer[i][j] = originalBuffer[resampleIndex][j] * (1.0 - rho) + originalBuffer[resampleIndex + 1.0][j] * rho;
				}
			}
		}
	}
	void AudioProcessor::Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers)
	{
		size_t outputBufferFrameCount = 0;
		for (int64_t i = 0; i < inputBuffers.size(); i++)
		{
			AudioBuffer& buffer = inputBuffers[i];
			if (buffer.frameCount == 0)
			{
				inputBuffers.erase(inputBuffers.begin() + i);
				i--;
				continue;
			}
			buffer.SetFormat(outputFormat);
			if (buffer.frameCount > outputBufferFrameCount)
			{
				outputBufferFrameCount = buffer.frameCount;
			}
		}
		outputBuffer = AudioBuffer(outputBufferFrameCount, outputFormat);
		for (size_t i = 0; i < inputBuffers.size(); i++)
		{
			AudioBuffer& buffer = inputBuffers[i];
			for (size_t j = 0; j < outputBuffer.frameCount; j++)
			{
				if (j >= buffer.frameCount) { break; }
				for (size_t k = 0; k < outputBuffer.formatInfo.channelCount; k++)
				{
					outputBuffer[j][k] += buffer[j][k] / (hephaudio_float)inputBuffers.size();
				}
			}
		}
	}
	std::vector<FloatBuffer> AudioProcessor::SplitChannels(const AudioBuffer& buffer)
	{
		std::vector<FloatBuffer> channels = std::vector<FloatBuffer>(buffer.formatInfo.channelCount, FloatBuffer(buffer.frameCount));
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				channels[j][i] = buffer[i][j];
			}
		}
		return channels;
	}
	AudioBuffer AudioProcessor::MergeChannels(const std::vector<FloatBuffer>& channels, uint32_t sampleRate)
	{
		if (channels.size() == 0) { return AudioBuffer(0, AudioFormatInfo()); }
		AudioBuffer resultBuffer(channels[0].frameCount, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, channels.size(), sizeof(hephaudio_float) * 8, sampleRate));
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = channels[j][i];
			}
		}
		return resultBuffer;
	}
	void AudioProcessor::ConvertPcmToInnerFormat(AudioBuffer& buffer, Endian pcmEndian)
	{
		if (buffer.formatInfo.formatTag == WAVE_FORMAT_PCM)
		{
			buffer = AudioProcessor::ConvertPcmToInnerFormat(buffer.Begin(), buffer.frameCount, buffer.formatInfo, pcmEndian);
		}
	}
	AudioBuffer AudioProcessor::ConvertPcmToInnerFormat(void* buffer, size_t frameCount, AudioFormatInfo formatInfo, Endian pcmEndian)
	{
		AudioBuffer resultBuffer = AudioBuffer(frameCount, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, formatInfo.channelCount, sizeof(hephaudio_float) * 8, formatInfo.sampleRate));

		if (AudioFile::GetSystemEndian() != pcmEndian)
		{
			switch (formatInfo.bitsPerSample)
			{
			case 8:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						resultBuffer[i][j] = (hephaudio_float)((uint8_t*)buffer)[i * formatInfo.channelCount + j] / (hephaudio_float)UINT8_MAX;
					}
				}
			}
			break;
			case 16:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						int16_t pcmSample = ((int16_t*)buffer)[i * formatInfo.channelCount + j];
						AudioFile::ChangeEndian((uint8_t*)&pcmSample, 2);

						resultBuffer[i][j] = (hephaudio_float)pcmSample / (hephaudio_float)(INT16_MAX + 1);
					}
				}
			}
			break;
			case 24:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						int24 pcmSample = ((int24*)buffer)[i * formatInfo.channelCount + j];
						AudioFile::ChangeEndian((uint8_t*)&pcmSample, 3);

						resultBuffer[i][j] = (hephaudio_float)pcmSample / (hephaudio_float)(INT24_MAX + 1);
					}
				}
			}
			break;
			case 32:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						int32_t pcmSample = ((int32_t*)buffer)[i * formatInfo.channelCount + j];
						AudioFile::ChangeEndian((uint8_t*)&pcmSample, 4);

						resultBuffer[i][j] = (hephaudio_float)pcmSample / (hephaudio_float)(INT32_MAX + 1ull);
					}
				}
			}
			break;
			default:
				throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
			}
		}
		else
		{
			switch (formatInfo.bitsPerSample)
			{
			case 8:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						resultBuffer[i][j] = (hephaudio_float)((uint8_t*)buffer)[i * formatInfo.channelCount + j] / (hephaudio_float)UINT8_MAX;
					}
				}
			}
			break;
			case 16:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						resultBuffer[i][j] = (hephaudio_float)((int16_t*)buffer)[i * formatInfo.channelCount + j] / (hephaudio_float)(INT16_MAX + 1);
					}
				}
			}
			break;
			case 24:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						resultBuffer[i][j] = (hephaudio_float)((int24*)buffer)[i * formatInfo.channelCount + j] / (hephaudio_float)(INT24_MAX + 1);
					}
				}
			}
			break;
			case 32:
			{
				for (size_t i = 0; i < frameCount; i++)
				{
					for (size_t j = 0; j < formatInfo.channelCount; j++)
					{
						resultBuffer[i][j] = (hephaudio_float)((int32_t*)buffer)[i * formatInfo.channelCount + j] / (hephaudio_float)(INT32_MAX + 1ull);
					}
				}
			}
			break;
			default:
				throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
			}
		}

		return resultBuffer;
	}
	void AudioProcessor::ConvertInnerToPcmFormat(AudioBuffer& buffer, size_t bps, Endian pcmEndian)
	{
		if (buffer.formatInfo.formatTag == WAVE_FORMAT_HEPHAUDIO)
		{
			AudioBuffer tempBuffer = AudioBuffer(buffer.frameCount, AudioFormatInfo(WAVE_FORMAT_PCM, buffer.formatInfo.channelCount, bps, buffer.formatInfo.sampleRate));

			if (AudioFile::GetSystemEndian() != pcmEndian)
			{
				switch (bps)
				{
				case 8:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							((uint8_t*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = (buffer[i][j] * 0.5 + 0.5) * UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							int16_t pcmSample = buffer[i][j] * (INT16_MAX + 1);
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 2);

							((int16_t*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							int24 pcmSample = buffer[i][j] * (INT24_MAX + 1);
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 3);

							((int24*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							int32_t pcmSample = buffer[i][j] * (INT32_MAX + 1ull);
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 4);

							((int32_t*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}
			else
			{
				switch (bps)
				{
				case 8:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							((uint8_t*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = (buffer[i][j] * 0.5 + 0.5) * UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							((int16_t*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = buffer[i][j] * (INT16_MAX + 1);
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							((int24*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = buffer[i][j] * (INT24_MAX + 1);
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
						{
							((int32_t*)tempBuffer.Begin())[i * buffer.formatInfo.channelCount + j] = buffer[i][j] * (INT32_MAX + 1ull);
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}

			buffer = std::move(tempBuffer);
		}
	}
#pragma endregion
#pragma region Sound Effects
	void AudioProcessor::Reverse(AudioBuffer& buffer)
	{
		const size_t hfc = buffer.frameCount * 0.5;
		for (size_t i = 0; i < hfc; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const hephaudio_float temp = buffer[i][j];
				buffer[i][j] = buffer[buffer.frameCount - i - 1][j];
				buffer[buffer.frameCount - i - 1][j] = temp;
			}
		}
	}
	void AudioProcessor::ReverseRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex)
	{
		const size_t frameCount = (subBufferFrameIndex + subBuffer.frameCount > originalBuffer.frameCount) ? (originalBuffer.frameCount - subBufferFrameIndex) : subBuffer.frameCount;
		const size_t hfc = frameCount * 0.5;
		const size_t reversedFrameIndex = originalBuffer.frameCount - subBufferFrameIndex - frameCount;

		for (size_t i = 0; i < hfc; i++)
		{
			for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
			{
				subBuffer[i][j] = originalBuffer[reversedFrameIndex + frameCount - i - 1][j];
				subBuffer[frameCount - i - 1][j] = originalBuffer[reversedFrameIndex + i][j];
			}
		}
	}
	void AudioProcessor::Echo(AudioBuffer& buffer, EchoInfo info)
	{
		const size_t delayFrameCount = buffer.formatInfo.sampleRate * info.reflectionDelay_s;
		const size_t echoStartFrame = buffer.frameCount * info.echoStartPosition;
		const AudioBuffer echoBuffer = buffer.GetSubBuffer(echoStartFrame, buffer.frameCount * info.echoEndPosition - echoStartFrame);
		hephaudio_float factor = info.decayFactor;
		size_t startFrameIndex = echoStartFrame + delayFrameCount;

		buffer.Resize(echoStartFrame + delayFrameCount * info.reflectionCount + echoBuffer.frameCount);

		for (size_t i = 0; i < info.reflectionCount; i++, factor *= info.decayFactor, startFrameIndex += delayFrameCount)
		{
			const size_t endFrameIndex = startFrameIndex + echoBuffer.frameCount;
			for (size_t j = startFrameIndex; j < endFrameIndex; j++)
			{
				for (size_t k = 0; k < buffer.formatInfo.channelCount; k++)
				{
					buffer[j][k] += echoBuffer[j - startFrameIndex][k] * factor;
				}
			}
		}
	}
	void AudioProcessor::EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info)
	{
		const size_t delayFrameCount = originalBuffer.formatInfo.sampleRate * info.reflectionDelay_s;
		const size_t echoStartFrame = originalBuffer.frameCount * info.echoStartPosition;
		const size_t echoFrameCount = originalBuffer.frameCount * info.echoEndPosition - echoStartFrame;
		const size_t subBufferEndFrameIndex = subBufferFrameIndex + subBuffer.frameCount;
		hephaudio_float factor = info.decayFactor;
		size_t startFrameIndex = echoStartFrame + delayFrameCount;

		for (size_t i = 0; i < info.reflectionCount; i++, factor *= info.decayFactor, startFrameIndex += delayFrameCount)
		{
			const size_t endFrameIndex = startFrameIndex + echoFrameCount;
			if (subBufferFrameIndex >= startFrameIndex)
			{
				for (size_t j = subBufferFrameIndex, jo = j - startFrameIndex; j < subBufferEndFrameIndex && jo < originalBuffer.frameCount; j++, jo++)
				{
					for (size_t k = 0; k < subBuffer.formatInfo.channelCount; k++)
					{
						subBuffer[j - subBufferFrameIndex][k] += originalBuffer[jo][k] * factor;
					}
				}
			}
		}
	}
	void AudioProcessor::LinearPanning(AudioBuffer& buffer, hephaudio_float panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const hephaudio_float rightVolume = panningFactor * 0.5 + 0.5;
			const hephaudio_float leftVolume = 1.0 - rightVolume;

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::SquareLawPanning(AudioBuffer& buffer, hephaudio_float panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const hephaudio_float volume = panningFactor * 0.5 + 0.5;
			const hephaudio_float rightVolume = sqrt(volume);
			const hephaudio_float leftVolume = sqrt(1.0 - volume);

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::SineLawPanning(AudioBuffer& buffer, hephaudio_float panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const hephaudio_float volume = panningFactor * 0.5 + 0.5;
			const hephaudio_float rightVolume = sin(volume * PI * 0.5);
			const hephaudio_float leftVolume = sin((1.0 - volume) * PI * 0.5);

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::Tremolo(AudioBuffer& buffer, hephaudio_float depth, const Oscillator& lfo)
	{
		const hephaudio_float wetFactor = depth;
		const hephaudio_float dryFactor = 1.0 - wetFactor;
		const FloatBuffer lfoPeriodBuffer = lfo.GenerateBuffer();

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float& lfoSample = lfoPeriodBuffer[i % lfoPeriodBuffer.frameCount];
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= wetFactor * lfoSample + dryFactor;
			}
		}
	}
	void AudioProcessor::TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float depth, const Oscillator& lfo)
	{
		const hephaudio_float wetFactor = depth;
		const hephaudio_float dryFactor = 1.0 - wetFactor;

		for (size_t i = 0; i < subBuffer.frameCount; i++)
		{
			const hephaudio_float lfoSample = lfo.Oscillate(subBufferFrameIndex + i);
			for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
			{
				subBuffer[i][j] *= wetFactor * lfoSample + dryFactor;
			}
		}
	}
	void AudioProcessor::Vibrato(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float extent_semitone, const Oscillator& lfo)
	{
		const hephaudio_float resampleDelta = buffer.formatInfo.sampleRate * (pow(2, extent_semitone / 12.0) - 1.0);
		const hephaudio_float wetFactor = depth;
		const hephaudio_float dryFactor = 1.0 - wetFactor;

		FloatBuffer lfoPeriodBuffer = FloatBuffer(ceil(lfo.sampleRate / lfo.frequency));
		for (size_t i = 0; i < lfoPeriodBuffer.frameCount; i++)
		{
			lfoPeriodBuffer[i] = lfo.Oscillate(i) * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer = AudioBuffer(buffer.frameCount, buffer.formatInfo);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float resampleIndex = i + lfoPeriodBuffer[i % lfoPeriodBuffer.frameCount] * resampleDelta;
			const hephaudio_float rho = resampleIndex - floor(resampleIndex);

			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				hephaudio_float wetSample = 0.0;
				if (resampleIndex + 1.0 < buffer.frameCount)
				{
					wetSample = buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho;
				}
				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::VibratoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float depth, hephaudio_float extent_semitone, const Oscillator& lfo)
	{
		const hephaudio_float resampleDelta = subBuffer.formatInfo.sampleRate * (pow(2, extent_semitone / 12.0) - 1.0);
		const hephaudio_float wetFactor = depth;
		const hephaudio_float dryFactor = 1.0 - wetFactor;

		for (size_t i = 0, t_sample = subBufferFrameIndex; i < subBuffer.frameCount && t_sample < originalBuffer.frameCount; i++, t_sample++)
		{
			const hephaudio_float resampleIndex = t_sample + (lfo.Oscillate(t_sample) * 0.5 + 0.5) * resampleDelta;
			const hephaudio_float rho = resampleIndex - floor(resampleIndex);

			for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
			{
				hephaudio_float wetSample = 0.0;
				if (resampleIndex + 1.0 < originalBuffer.frameCount)
				{
					wetSample = originalBuffer[resampleIndex][j] * (1.0 - rho) + originalBuffer[resampleIndex + 1.0][j] * rho;
				}
				subBuffer[i][j] = wetFactor * wetSample + dryFactor * originalBuffer[t_sample][j];
			}
		}
	}
	void AudioProcessor::Normalize(AudioBuffer& buffer, hephaudio_float peakAmplitude)
	{
		hephaudio_float maxSample = buffer.AbsMax();
		if (maxSample != 0.0 && maxSample != peakAmplitude)
		{
			buffer *= peakAmplitude / maxSample;
		}
	}
	void AudioProcessor::RmsNormalize(AudioBuffer& buffer, hephaudio_float desiredRms)
	{
		hephaudio_float sumOfSamplesSquared = 0.0;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				sumOfSamplesSquared += buffer[i][j] * buffer[i][j];
			}
		}

		if (sumOfSamplesSquared != 0.0)
		{
			buffer *= desiredRms * sqrt(buffer.frameCount * buffer.formatInfo.channelCount / sumOfSamplesSquared);
		}
	}
	void AudioProcessor::HardClipDistortion(AudioBuffer& buffer, hephaudio_float clippingLevel_dB)
	{
		const hephaudio_float clippingLevel = DecibelToGain(clippingLevel_dB);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				hephaudio_float& sample = buffer[i][j];
				if (sample > clippingLevel)
				{
					sample = clippingLevel;
				}
				else if (sample < -clippingLevel)
				{
					sample = -clippingLevel;
				}
			}
		}
	}
	void AudioProcessor::SoftClipDistortion(AudioBuffer& buffer, hephaudio_float alpha)
	{
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				hephaudio_float& sample = buffer[i][j];
				sample = 2.0 * atan(alpha * sample) / PI;
			}
		}
	}
	void AudioProcessor::Overdrive(AudioBuffer& buffer, hephaudio_float drive)
	{
		const hephaudio_float a = sin(drive * PI / 2.0);
		const hephaudio_float k = 2.0 * a / (1.0 - a);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				hephaudio_float& sample = buffer[i][j];
				sample = (1.0 + k) * sample / (1.0 + k * abs(sample));
			}
		}
	}
	void AudioProcessor::Fuzz(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float alpha)
	{
		const hephaudio_float& wetFactor = depth;
		const hephaudio_float dryFactor = 1.0 - wetFactor;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				hephaudio_float& sample = buffer[i][j];
				if (sample > 0)
				{
					sample = wetFactor * (1.0 - exp(alpha * sample)) + dryFactor * sample;
				}
				else
				{
					sample = wetFactor * (-1.0 + exp(alpha * sample)) + dryFactor * sample;
				}
			}
		}
	}
	void AudioProcessor::Flanger(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float feedbackGain, hephaudio_float baseDelay_ms, hephaudio_float delay_ms, const Oscillator& lfo)
	{
		const hephaudio_float baseDelay_sample = baseDelay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const hephaudio_float delay_sample = delay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const hephaudio_float wetFactor = depth;
		const hephaudio_float dryFactor = 1.0 - wetFactor;
		FloatBuffer feedbackBuffer = FloatBuffer(buffer.formatInfo.channelCount);
		FloatBuffer lfoPeriodBuffer = FloatBuffer(ceil(lfo.sampleRate / lfo.frequency));
		for (size_t i = 0; i < lfoPeriodBuffer.frameCount; i++)
		{
			lfoPeriodBuffer[i] = lfo.Oscillate(i) * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer = AudioBuffer(buffer.frameCount, buffer.formatInfo);
		memcpy(resultBuffer.pAudioData, buffer.pAudioData, round(delay_sample + baseDelay_sample) * buffer.formatInfo.FrameSize());

		for (size_t i = delay_sample + 1; i < buffer.frameCount; i++)
		{
			const size_t currentDelay_sample = round(lfoPeriodBuffer[i % lfoPeriodBuffer.frameCount] * delay_sample + baseDelay_sample);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const hephaudio_float wetSample = buffer[i - currentDelay_sample][j] + feedbackBuffer[j];
				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
				feedbackBuffer[j] = feedbackGain * wetSample;
			}
		}

		const hephaudio_float absMax = resultBuffer.AbsMax();
		if (absMax > 1.0)
		{
			resultBuffer /= absMax;
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::FlangerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex,
		hephaudio_float depth, hephaudio_float feedbackGain, hephaudio_float baseDelay_ms, hephaudio_float delay_ms, const Oscillator& lfo)
	{
		static FloatBuffer feedbackBuffer = FloatBuffer();
		if (subBufferFrameIndex == 0 || feedbackBuffer.frameCount != subBuffer.formatInfo.channelCount)
		{
			feedbackBuffer = FloatBuffer(subBuffer.formatInfo.channelCount);
		}

		const hephaudio_float baseDelay_sample = baseDelay_ms * 1e-3 * subBuffer.formatInfo.sampleRate;
		const hephaudio_float delay_sample = delay_ms * 1e-3 * subBuffer.formatInfo.sampleRate;

		if (subBufferFrameIndex > round(delay_sample + baseDelay_ms))
		{
			const hephaudio_float wetFactor = depth;
			const hephaudio_float dryFactor = 1.0 - wetFactor;

			for (size_t i = 0; i < subBuffer.frameCount && subBufferFrameIndex + i < originalBuffer.frameCount; i++)
			{
				const size_t currentDelay_sample = round((lfo.Oscillate(subBufferFrameIndex + i) * 0.5 + 0.5) * delay_sample + baseDelay_ms);
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const hephaudio_float wetSample = originalBuffer[subBufferFrameIndex + i - currentDelay_sample][j] + feedbackBuffer[j];
					subBuffer[i][j] = wetFactor * wetSample + dryFactor * originalBuffer[subBufferFrameIndex + i][j];
					feedbackBuffer[j] = feedbackGain * wetSample;
				}
			}
		}
	}
	void AudioProcessor::Wah(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float damping, hephaudio_float fcmin, hephaudio_float fcmax, const Oscillator& lfo)
	{
		if (buffer.frameCount > 0)
		{
			const hephaudio_float wetFactor = depth;
			const hephaudio_float dryFactor = 1.0 - wetFactor;
			const hephaudio_float fcdelta = fcmax - fcmin;
			const hephaudio_float Q = 2.0 * damping;

			FloatBuffer lfoPeriodBuffer = FloatBuffer(ceil(lfo.sampleRate / lfo.frequency));
			for (size_t i = 0; i < lfoPeriodBuffer.frameCount; i++)
			{
				lfoPeriodBuffer[i] = lfo.Oscillate(i) * 0.5 + 0.5;
			}

			hephaudio_float fc = fcdelta * lfoPeriodBuffer[0] + fcmin;
			hephaudio_float alpha = 2.0 * sin(PI * fc / lfo.sampleRate);
			AudioBuffer lowPassBuffer = AudioBuffer(buffer.frameCount, buffer.formatInfo);
			AudioBuffer bandPassBuffer = AudioBuffer(buffer.frameCount, buffer.formatInfo);
			AudioBuffer highPassBuffer = AudioBuffer(buffer.frameCount, buffer.formatInfo);

			for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
			{
				highPassBuffer[0][i] = buffer[0][i];
				bandPassBuffer[0][i] = alpha * buffer[0][i];
				lowPassBuffer[0][i] = alpha * bandPassBuffer[0][i];
			}

			for (size_t i = 1; i < buffer.frameCount; i++)
			{
				fc = fcdelta * lfoPeriodBuffer[i % lfoPeriodBuffer.frameCount] + fcmin;
				alpha = 2.0 * sin(PI * fc / lfo.sampleRate);

				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					highPassBuffer[i][j] = buffer[i][j] - lowPassBuffer[i - 1][j] - Q * bandPassBuffer[i - 1][j];
					bandPassBuffer[i][j] = alpha * highPassBuffer[i][j] + bandPassBuffer[i - 1][j];
					lowPassBuffer[i][j] = alpha * bandPassBuffer[i][j] + lowPassBuffer[i - 1][j];
				}
			}

			hephaudio_float absMax = INT32_MIN;
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					buffer[i][j] = wetFactor * bandPassBuffer[i][j] + dryFactor * buffer[i][j];
					if (abs(buffer[i][j]) > absMax)
					{
						absMax = abs(buffer[i][j]);
					}
				}
			}
			if (absMax > 1.0)
			{
				buffer /= absMax;
			}
		}
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos)
	{
		Equalizer(buffer, defaultHopSize, defaultFFTSize, infos);
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[j].GetSubBuffer(i, fftSize), fftSize);

				for (size_t k = 0; k < infos.size(); k++)
				{
					const EqualizerInfo& info = infos[k];
					uint64_t lowerFrequencyIndex, higherFrequencyIndex;
					if (info.f1 > info.f2)
					{
						higherFrequencyIndex = ceil(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f1));
						lowerFrequencyIndex = floor(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f2));
					}
					else
					{
						higherFrequencyIndex = ceil(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f2));
						lowerFrequencyIndex = floor(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f1));
					}
					const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;

					for (size_t l = lowerFrequencyIndex; l <= upperBound; l++)
					{
						complexBuffer[l] *= info.amplitudeFunction(Fourier::IndexToBinFrequency(buffer.formatInfo.sampleRate, fftSize, l));
						complexBuffer[fftSize - l - 1] = Complex(complexBuffer[l].real, -complexBuffer[l].imaginary);
					}
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real * hannWindow[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos)
	{
		AudioProcessor::EqualizerMT(buffer, defaultHopSize, defaultFFTSize, infos);
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos)
	{
		constexpr auto applyEqualizer = [](AudioBuffer* buffer, const FloatBuffer* window, uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, const std::vector<EqualizerInfo>* const infos)
		{
			FloatBuffer channel = FloatBuffer(buffer->frameCount);
			for (size_t i = 0; i < buffer->frameCount; i++)
			{
				channel[i] = (*buffer)[i][channelIndex];
				(*buffer)[i][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);

				for (size_t j = 0; j < infos->size(); j++)
				{
					const EqualizerInfo& info = infos->at(j);
					uint64_t lowerFrequencyIndex, higherFrequencyIndex;
					if (info.f1 > info.f2)
					{
						higherFrequencyIndex = ceil(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f1));
						lowerFrequencyIndex = floor(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f2));
					}
					else
					{
						higherFrequencyIndex = ceil(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f2));
						lowerFrequencyIndex = floor(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f1));
					}
					const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;

					for (size_t l = lowerFrequencyIndex; l <= upperBound; l++)
					{
						complexBuffer[l] *= info.amplitudeFunction(Fourier::IndexToBinFrequency(buffer->formatInfo.sampleRate, fftSize, l));
						complexBuffer[fftSize - l - 1] = Complex(complexBuffer[l].real, -complexBuffer[l].imaginary);
					}
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0, k = i; j < fftSize && k < channel.frameCount; j++, k++)
				{
					(*buffer)[k][channelIndex] += complexBuffer[j].real * (*window)[j] / fftSize;
				}
			}
		};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);
		std::vector<std::thread> threads = std::vector<std::thread>(buffer.formatInfo.channelCount);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads[i] = std::thread(applyEqualizer, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, &infos);
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos)
	{
		EqualizerRT(originalBuffer, subBuffer, subBufferFrameIndex, 512, 1024, infos);
	}
	void AudioProcessor::EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;

		const auto applyEqualizer = [&hopSize, &fftSize, &infos](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<FloatBuffer> channels = SplitChannels(subBuffer);

			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[i], fftSize);

				for (size_t j = 0; j < infos.size(); j++)
				{
					const EqualizerInfo& info = infos[j];
					uint64_t lowerFrequencyIndex, higherFrequencyIndex;
					if (info.f1 > info.f2)
					{
						higherFrequencyIndex = ceil(Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f1));
						lowerFrequencyIndex = floor(Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f2));
					}
					else
					{
						higherFrequencyIndex = ceil(Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f2));
						lowerFrequencyIndex = floor(Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f1));
					}
					const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;

					for (size_t k = lowerFrequencyIndex; k <= upperBound; k++)
					{
						complexBuffer[k] *= info.amplitudeFunction(Fourier::IndexToBinFrequency(subBuffer.formatInfo.sampleRate, fftSize, k));
						complexBuffer[fftSize - k - 1].real = complexBuffer[k].real;
						complexBuffer[fftSize - k - 1].imaginary = -complexBuffer[k].imaginary;
					}
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = complexBuffer[j].real / fftSize;
				}
			}
		};

		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex > fftSize)
		{
			fStart = subBufferFrameIndex - fftSize;
			fStart = fStart + hopSize - (fStart % hopSize);
		}
		size_t fEnd = fTarget + fftSize;
		fEnd = fEnd - (fEnd % hopSize);

		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);

		size_t isb, itb;
		const hephaudio_float piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i, hopSize);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i, hopSize);
				applyEqualizer(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}

			if (i > subBufferFrameIndex)
			{
				isb = i - subBufferFrameIndex;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = subBufferFrameIndex - i;
			}

			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				const hephaudio_float s = sin(itb * piOverN);
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float speed)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t targetSize = Fourier::CalculateFFTSize(fftSize / speed);
		const size_t targetNyquistFrequency = targetSize * 0.5;
		const size_t tsm1 = targetSize - 1;
		const FloatBuffer window = speed >= 2.0 ? GenerateWelchWindow(targetSize) : GenerateBlackmanWindow(targetSize);
		const hephaudio_float cursorRatio = 1.0 / tsm1 * (fftSize - 1);
		const size_t oldFrameCount = buffer.frameCount;
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

		buffer.Reset();
		buffer.Resize(buffer.frameCount / speed);

		for (size_t i = 0; i < oldFrameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[j].GetSubBuffer(i, fftSize), fftSize);
				ComplexBuffer targetComplexBuffer = ComplexBuffer(targetSize);
				hephaudio_float cursor = 0.0;

				for (size_t k = 0; k < targetNyquistFrequency; k++, cursor += cursorRatio)
				{
					const hephaudio_float fc = floor(cursor);
					const hephaudio_float factor = cursor - fc;
					targetComplexBuffer[k] = complexBuffer[fc] * (1.0 - factor) + complexBuffer[fc + 1.0] * factor;
					targetComplexBuffer[targetSize - k - 1] = Complex(targetComplexBuffer[k].real, -targetComplexBuffer[k].imaginary);
				}
				complexBuffer.~ComplexBuffer();

				Fourier::FFT_Inverse(targetComplexBuffer, false);
				for (size_t k = 0, l = i / speed; k < targetSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += targetComplexBuffer[k].real * window[k] / targetSize;
				}
			}
		}
	}
	void AudioProcessor::ChangeSpeedTD(AudioBuffer& buffer, size_t hopSize, size_t windowSize, hephaudio_float speed)
	{
		const hephaudio_float frameCountRatio = 1.0 / speed;
		const FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(windowSize);
		AudioBuffer tempBuffer = AudioBuffer(buffer.frameCount * frameCountRatio, buffer.formatInfo);

		if (speed <= 1.0)
		{
			for (size_t i = 0; i < buffer.frameCount; i += hopSize)
			{
				AudioBuffer subBuffer = buffer.GetSubBuffer(i, windowSize);
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					for (size_t k = 0; k < frameCountRatio; k++)
					{
						for (size_t l = 0, m = i * frameCountRatio + k * hopSize; l < windowSize && m < tempBuffer.frameCount; l++, m++)
						{
							tempBuffer[m][j] += subBuffer[l][j] * hannWindow[l];
						}
					}
				}
			}
		}
		else
		{
			for (size_t i = 0; i < buffer.frameCount; i += hopSize)
			{
				AudioBuffer subBuffer = buffer.GetSubBuffer(i, windowSize);
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					for (size_t k = 0, l = i * frameCountRatio; k < windowSize && l < tempBuffer.frameCount; k++, l++)
					{
						tempBuffer[l][j] += subBuffer[k][j] * hannWindow[k];
					}
				}
			}
		}

		const hephaudio_float absMax = tempBuffer.AbsMax();
		if (absMax > 1.0)
		{
			tempBuffer /= absMax;
		}

		buffer = std::move(tempBuffer);
	}
	void AudioProcessor::PitchShift(AudioBuffer& buffer, hephaudio_float shiftFactor)
	{
		PitchShift(buffer, 512, 4096, shiftFactor);
	}
	void AudioProcessor::PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float shiftFactor)
	{
		constexpr hephaudio_float twopi = 2.0 * PI;
		constexpr size_t hephaudio_floatSizeInBits = sizeof(hephaudio_float) * 8;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);
		AudioBuffer lastAnalysisPhases = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));
		AudioBuffer lastSynthesisPhases = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));
		AudioBuffer synthesisMagnitudes = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));
		AudioBuffer synthesisFrequencies = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			synthesisMagnitudes.Reset();
			synthesisFrequencies.Reset();

			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const FloatBuffer subBuffer = channels[j].GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = ComplexBuffer(fftSize);
				for (size_t k = 0; k < fftSize; k++)
				{
					complexBuffer[k].real = subBuffer[k] * hannWindow[k];
				}
				Fourier::FFT_Forward(complexBuffer, fftSize);

				for (size_t k = 0; k < nyquistFrequency; k++)
				{
					const hephaudio_float phase = complexBuffer[k].Phase();
					hephaudio_float phaseRemainder = phase - lastAnalysisPhases[k][j] - twopi * k * hopSize / fftSize;
					phaseRemainder = phaseRemainder >= 0 ? (fmod(phaseRemainder + PI, twopi) - PI) : (fmod(phaseRemainder - PI, -twopi) + PI);

					const size_t newBin = floor(k * shiftFactor + 0.5);
					if (newBin < nyquistFrequency)
					{
						synthesisMagnitudes[newBin][j] += complexBuffer[k].Magnitude();
						synthesisFrequencies[newBin][j] = (k + phaseRemainder * fftSize / twopi / hopSize) * shiftFactor;
					}

					lastAnalysisPhases[k][j] = phase;
				}

				for (size_t k = 0; k < nyquistFrequency; k++)
				{
					hephaudio_float synthesisPhase = twopi * hopSize / fftSize * synthesisFrequencies[k][j] + lastSynthesisPhases[k][j];
					synthesisPhase = synthesisPhase >= 0 ? (fmod(synthesisPhase + PI, twopi) - PI) : (fmod(synthesisPhase - PI, -twopi) + PI);

					complexBuffer[k] = Complex(synthesisMagnitudes[k][j] * cos(synthesisPhase), synthesisMagnitudes[k][j] * sin(synthesisPhase));
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();

					lastSynthesisPhases[k][j] = synthesisPhase;
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real * hannWindow[k] / fftSize;
				}
			}
		}
	}
#pragma endregion
#pragma region Filters
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, hephaudio_float cutoffFreq)
	{
		LowPassFilter(buffer, defaultHopSize, defaultFFTSize, cutoffFreq);
	}
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[j].GetSubBuffer(i, fftSize), fftSize);

				for (size_t k = startIndex; k < nyquistFrequency; k++)
				{
					complexBuffer[k] = Complex(0, 0);
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real * hannWindow[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, hephaudio_float cutoffFreq)
	{
		LowPassFilterMT(buffer, defaultHopSize, defaultFFTSize, cutoffFreq);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq)
	{
		constexpr auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* window, size_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex)
		{
			FloatBuffer channel = FloatBuffer(buffer->frameCount);
			for (size_t i = 0; i < buffer->frameCount; i++)
			{
				channel[i] = (*buffer)[i][channelIndex];
				(*buffer)[i][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);

				for (int64_t j = startIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0, k = i; j < fftSize && k < channel.frameCount; j++, k++)
				{
					(*buffer)[k][channelIndex] += complexBuffer[j].real * (*window)[j] / fftSize;
				}
			}
		};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);
		std::vector<std::thread> threads = std::vector<std::thread>(buffer.formatInfo.channelCount);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads[i] = std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, startIndex);
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float cutoffFreq)
	{
		LowPassFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, 512, 1024, cutoffFreq);
	}
	void AudioProcessor::LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;

		const auto applyFilter = [&fftSize, &cutoffFreq](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const uint64_t startIndex = Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<FloatBuffer> channels = SplitChannels(subBuffer);

			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[i], fftSize);

				for (size_t j = startIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = complexBuffer[j].real / fftSize;
				}
			}
		};

		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex > fftSize)
		{
			fStart = subBufferFrameIndex - fftSize;
			fStart = fStart + hopSize - (fStart % hopSize);
		}
		size_t fEnd = fTarget + fftSize;
		fEnd = fEnd - (fEnd % hopSize);

		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);

		size_t isb, itb;
		const hephaudio_float piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i, hopSize);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i, hopSize);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}

			if (i > subBufferFrameIndex)
			{
				isb = i - subBufferFrameIndex;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = subBufferFrameIndex - i;
			}

			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				const hephaudio_float s = sin(itb * piOverN);
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, hephaudio_float cutoffFreq)
	{
		HighPassFilter(buffer, defaultHopSize, defaultFFTSize, cutoffFreq);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[j].GetSubBuffer(i, fftSize), fftSize);

				for (int64_t k = stopIndex; k >= 0; k--)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real * hannWindow[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, hephaudio_float cutoffFreq)
	{
		HighPassFilterMT(buffer, defaultHopSize, defaultFFTSize, cutoffFreq);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq)
	{
		constexpr auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* window, const uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t stopIndex)
		{
			FloatBuffer channel = FloatBuffer(buffer->frameCount);
			for (size_t i = 0; i < buffer->frameCount; i++)
			{
				channel[i] = (*buffer)[i][channelIndex];
				(*buffer)[i][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);

				for (int64_t j = stopIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0, k = i; j < fftSize && k < channel.frameCount; j++, k++)
				{
					(*buffer)[k][channelIndex] += complexBuffer[j].real * (*window)[j] / (hephaudio_float)fftSize;
				}
			}
		};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);
		std::vector<std::thread> threads = std::vector<std::thread>(buffer.formatInfo.channelCount);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads[i] = std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, stopIndex);
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float cutoffFreq)
	{
		HighPassFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, 512, 1024, cutoffFreq);
	}
	void AudioProcessor::HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;

		const auto applyFilter = [&fftSize, &cutoffFreq](AudioBuffer& subBuffer) -> void
		{
			const uint64_t stopIndex = Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<FloatBuffer> channels = SplitChannels(subBuffer);

			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[i], fftSize);

				for (int64_t j = stopIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = complexBuffer[j].real / fftSize;
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex > fftSize)
		{
			fStart = subBufferFrameIndex - fftSize;
			fStart = fStart + hopSize - (fStart % hopSize);
		}
		size_t fEnd = fTarget + fftSize;
		fEnd = fEnd - (fEnd % hopSize);

		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);

		size_t isb, itb;
		const hephaudio_float piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i, hopSize);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i, hopSize);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}

			if (i > subBufferFrameIndex)
			{
				isb = i - subBufferFrameIndex;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = subBufferFrameIndex - i;
			}

			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				const hephaudio_float s = sin(itb * piOverN);
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		BandPassFilter(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[j].GetSubBuffer(i, fftSize), fftSize);

				for (int64_t k = startIndex; k >= 0; k--)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}

				for (size_t k = stopIndex; k < nyquistFrequency; k++)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real * hannWindow[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		BandPassFilterMT(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		constexpr auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* window, uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex, size_t stopIndex)
		{
			FloatBuffer channel = FloatBuffer(buffer->frameCount);
			for (size_t i = 0; i < buffer->frameCount; i++)
			{
				channel[i] = (*buffer)[i][channelIndex];
				(*buffer)[i][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);

				for (int64_t j = startIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				for (size_t j = stopIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0, k = i; j < fftSize && k < channel.frameCount; j++, k++)
				{
					(*buffer)[k][channelIndex] += complexBuffer[j].real * (*window)[j] / fftSize;
				}
			}
		};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);
		std::vector<std::thread> threads = std::vector<std::thread>(buffer.formatInfo.channelCount);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads[i] = std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, startIndex, stopIndex);
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		BandPassFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, 512, 1024, lowCutoffFreq, highCutoffFreq);
	}
	void AudioProcessor::BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;

		const auto applyFilter = [&fftSize, &lowCutoffFreq, &highCutoffFreq](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const uint64_t startIndex = Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
			const uint64_t stopIndex = Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<FloatBuffer> channels = SplitChannels(subBuffer);

			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[i], fftSize);

				for (int64_t j = startIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				for (size_t j = stopIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = complexBuffer[j].real / fftSize;
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex > fftSize)
		{
			fStart = subBufferFrameIndex - fftSize;
			fStart = fStart + hopSize - (fStart % hopSize);
		}
		size_t fEnd = fTarget + fftSize;
		fEnd = fEnd - (fEnd % hopSize);

		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);

		size_t isb, itb;
		const hephaudio_float piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i, hopSize);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i, hopSize);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}

			if (i > subBufferFrameIndex)
			{
				isb = i - subBufferFrameIndex;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = subBufferFrameIndex - i;
			}

			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				const hephaudio_float s = sin(itb * piOverN);
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		BandCutFilter(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[j].GetSubBuffer(i, fftSize), fftSize);
				for (size_t k = startIndex; k <= stopIndex; k++)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}
				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real * hannWindow[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		BandCutFilterMT(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		constexpr auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* window, uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex, size_t stopIndex)
		{
			FloatBuffer channel = FloatBuffer(buffer->frameCount);
			for (size_t i = 0; i < buffer->frameCount; i++)
			{
				channel[i] = (*buffer)[i][channelIndex];
				(*buffer)[i][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);

				for (size_t j = startIndex; j <= stopIndex; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0, k = i; j < fftSize && k < channel.frameCount; j++, k++)
				{
					(*buffer)[k][channelIndex] += complexBuffer[j].real * (*window)[j] / fftSize;
				}
			}
		};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);
		std::vector<std::thread> threads = std::vector<std::thread>(buffer.formatInfo.channelCount);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads[i] = std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, startIndex, stopIndex);
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		BandCutFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, 512, 1024, lowCutoffFreq, highCutoffFreq);
	}
	void AudioProcessor::BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;

		const auto applyFilter = [&fftSize, &lowCutoffFreq, &highCutoffFreq](AudioBuffer& subBuffer) -> void
		{
			const uint64_t startIndex = Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
			const uint64_t stopIndex = Fourier::BinFrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<FloatBuffer> channels = SplitChannels(subBuffer);

			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels[i], fftSize);

				for (size_t j = startIndex; j <= stopIndex; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = complexBuffer[j].real / fftSize;
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex > fftSize)
		{
			fStart = subBufferFrameIndex - fftSize;
			fStart = fStart + hopSize - (fStart % hopSize);
		}
		size_t fEnd = fTarget + fftSize;
		fEnd = fEnd - (fEnd % hopSize);

		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);

		size_t isb, itb;
		const hephaudio_float piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i, hopSize);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i, hopSize);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}

			if (i > subBufferFrameIndex)
			{
				isb = i - subBufferFrameIndex;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = subBufferFrameIndex - i;
			}

			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				const hephaudio_float s = sin(itb * piOverN);
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
#pragma endregion
#pragma region Windows
	void AudioProcessor::ApplyTriangleWindow(AudioBuffer& buffer)
	{
		const hephaudio_float hN = 0.5 * (buffer.frameCount - 1);
		const hephaudio_float hL = hN + 1.0;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 1.0 - fabs((i - hN) / hL);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateTriangleWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float hN = 0.5 * (frameCount - 1);
		const hephaudio_float hL = hN + 1.0;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 1.0 - fabs((i - hN) / hL);
		}

		return window;
	}
	void AudioProcessor::ApplyParzenWindow(AudioBuffer& buffer)
	{
		const hephaudio_float hN = 0.5 * (buffer.frameCount - 1);
		const hephaudio_float hL = hN + 0.5;
		const hephaudio_float qL = hL * 0.5;
		hephaudio_float factor = 1.0;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float n = i - hN;
			const hephaudio_float absN = fabs(n);

			if (absN >= 0 && absN <= qL)
			{
				factor = 1.0 - 6.0 * pow(n / hL, 2) * (1.0 - absN / hL);
			}
			else
			{
				factor = 2.0 * pow(1.0 - absN / hL, 3);
			}

			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateParzenWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float hN = 0.5 * (frameCount - 1);
		const hephaudio_float hL = hN + 0.5;
		const hephaudio_float qL = hL * 0.5;
		hephaudio_float n = 0.0;
		hephaudio_float absN = 0.0;

		for (size_t i = 0; i < frameCount; i++)
		{
			n = i - hN;
			absN = fabs(n);

			if (absN >= 0 && absN <= qL)
			{
				window[i] = 1.0 - 6.0 * pow(n / hL, 2) * (1.0 - absN / hL);
			}
			else
			{
				window[i] = 2.0 * pow(1.0 - absN / hL, 3);
			}
		}

		return window;
	}
	void AudioProcessor::ApplyWelchWindow(AudioBuffer& buffer)
	{
		const hephaudio_float hN = 0.5 * (buffer.frameCount - 1);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 1.0 - pow((i - hN) / hN, 2);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateWelchWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float hN = 0.5 * (frameCount - 1);

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 1.0 - pow((i - hN) / hN, 2);
		}

		return window;
	}
	void AudioProcessor::ApplySineWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = sin(PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateSineWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = sin(PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyHannWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = pow(sin(PI * i / N), 2);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateHannWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = pow(sin(PI * i / N), 2);
		}

		return window;
	}
	void AudioProcessor::ApplyHammingWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.54 - 0.46 * cos(2.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateHammingWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.54 - 0.46 * cos(2.0 * PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyBlackmanWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.42 - 0.5 * cos(2.0 * PI * i / N) + 0.08 * cos(4.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateBlackmanWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.42 - 0.5 * cos(2.0 * PI * i / N) + 0.08 * cos(4.0 * PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyExactBlackmanWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.42659 - 0.49656 * cos(2.0 * PI * i / N) + 0.076849 * cos(4.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateExactBlackmanWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.42659 - 0.49656 * cos(2.0 * PI * i / N) + 0.076849 * cos(4.0 * PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyNuttallWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.355768 - 0.487396 * cos(2.0 * PI * i / N) + 0.144232 * cos(4.0 * PI * i / N) - 0.012604 * cos(6.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateNuttallWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.355768 - 0.487396 * cos(2.0 * PI * i / N) + 0.144232 * cos(4.0 * PI * i / N) - 0.012604 * cos(6.0 * PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyBlackmanNuttallWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.3635819 - 0.4891775 * cos(2.0 * PI * i / N) + 0.1365995 * cos(4.0 * PI * i / N) - 0.0106411 * cos(6.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateBlackmanNuttallWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.3635819 - 0.4891775 * cos(2.0 * PI * i / N) + 0.1365995 * cos(4.0 * PI * i / N) - 0.0106411 * cos(6.0 * PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyBlackmanHarrisWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.35875 - 0.48829 * cos(2.0 * PI * i / N) + 0.14128 * cos(4.0 * PI * i / N) - 0.01168 * cos(6.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateBlackmanHarrisWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.35875 - 0.48829 * cos(2.0 * PI * i / N) + 0.14128 * cos(4.0 * PI * i / N) - 0.01168 * cos(6.0 * PI * i / N);
		}
		return window;
	}
	void AudioProcessor::ApplyFlatTopWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.21557895 - 0.41663158 * cos(2.0 * PI * i / N) + 0.277263158 * cos(4.0 * PI * i / N) - 0.083578947 * cos(6.0 * PI * i / N) + 0.006947368 * cos(8.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateFlatTopWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.21557895 - 0.41663158 * cos(2.0 * PI * i / N) + 0.277263158 * cos(4.0 * PI * i / N) - 0.083578947 * cos(6.0 * PI * i / N) + 0.006947368 * cos(8.0 * PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyGaussianWindow(AudioBuffer& buffer, hephaudio_float sigma)
	{
		const hephaudio_float hN = 0.5 * (buffer.frameCount - 1);
		const hephaudio_float shN = sigma * hN;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = exp(-0.5 * pow((i - hN) / shN, 2));
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateGaussianWindow(size_t frameCount, hephaudio_float sigma)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float hN = 0.5 * (frameCount - 1);
		const hephaudio_float shN = sigma * hN;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = exp(-0.5 * pow((i - hN) / shN, 2));
		}

		return window;
	}
	void AudioProcessor::ApplyTukeyWindow(AudioBuffer& buffer, hephaudio_float alpha)
	{
		const hephaudio_float N = (buffer.frameCount - 1);
		const hephaudio_float hN = 0.5 * N;
		const hephaudio_float aN = alpha * N;
		const hephaudio_float haN = 0.5 * aN;
		hephaudio_float factor = 1.0;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			if (i < haN)
			{
				factor = 0.5 * (1.0 - cos(2.0 * PI * i / aN));
			}
			else if (i >= haN && i <= hN)
			{
				factor = 1.0;
			}
			else
			{
				factor = 0.5 * (1.0 - cos(2.0 * PI * (N - i) / aN));
			}

			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateTukeyWindow(size_t frameCount, hephaudio_float alpha)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = (frameCount - 1);
		const hephaudio_float hN = 0.5 * N;
		const hephaudio_float aN = alpha * N;
		const hephaudio_float haN = 0.5 * aN;

		for (size_t i = 0; i < frameCount; i++)
		{
			if (i < haN)
			{
				window[i] = 0.5 * (1.0 - cos(2.0 * PI * i / aN));
			}
			else if (i >= haN && i <= hN)
			{
				window[i] = 1.0;
			}
			else
			{
				window[i] = 0.5 * (1.0 - cos(2.0 * PI * (N - i) / aN));
			}
		}
		return window;
	}
	void AudioProcessor::ApplyBartlettHannWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.62 - 0.48 * fabs(i / N - 0.5) - 0.38 * cos(2.0 * PI * i / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateBartlettHannWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.62 - 0.48 * fabs(i / N - 0.5) - 0.38 * cos(2.0 * PI * i / N);
		}

		return window;
	}
	void AudioProcessor::ApplyHannPoissonWindow(AudioBuffer& buffer, hephaudio_float alpha)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float factor = 0.5 * (1.0 - cos(2.0 * PI * i / N)) * exp(-alpha * fabs(N - 2.0 * i) / N);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateHannPoissonWindow(size_t frameCount, hephaudio_float alpha)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			window[i] = 0.5 * (1.0 - cos(2.0 * PI * i / N)) * exp(-alpha * fabs(N - 2.0 * i) / N);
		}

		return window;
	}
	void AudioProcessor::ApplyLanczosWindow(AudioBuffer& buffer)
	{
		const hephaudio_float N = buffer.frameCount - 1;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const hephaudio_float pix = PI * (2.0 * i / N - 1.0);
			const hephaudio_float factor = sin(pix) / pix;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	FloatBuffer AudioProcessor::GenerateLanczosWindow(size_t frameCount)
	{
		FloatBuffer window = FloatBuffer(frameCount);
		const hephaudio_float N = frameCount - 1;

		for (size_t i = 0; i < frameCount; i++)
		{
			const hephaudio_float pix = PI * (2.0 * i / N - 1.0);
			window[i] = sin(pix) / pix;
		}
		return window;
	}
#pragma endregion
#pragma region Processed Buffer
	AudioProcessor::ProcessedBuffer::ProcessedBuffer(const AudioBuffer* pOriginalBuffer, const AudioBuffer& audioBuffer, const size_t& fStart, const size_t& hopSize)
	{
		this->pOriginalBuffer = pOriginalBuffer;
		this->audioBuffer = audioBuffer;
		this->fStart = fStart;
		this->hopSize = hopSize;
	}
	AudioProcessor::ProcessedBuffer* AudioProcessor::GetProcessedBuffer(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart, const size_t& hopSize)
	{
		for (size_t i = 0; i < processedBuffers.size(); i++)
		{
			ProcessedBuffer*& processedBuffer = processedBuffers[i];
			if (processedBuffer->fStart == fStart && hopSize == processedBuffer->hopSize && processedBuffer->pOriginalBuffer == pOriginalBuffer)
			{
				return processedBuffer;
			}
		}
		return nullptr;
	}
	void AudioProcessor::RemoveOldProcessedBuffers(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart)
	{
		for (int64_t i = 0; i < processedBuffers.size(); i++)
		{
			ProcessedBuffer*& processedBuffer = processedBuffers[i];
			if (processedBuffer->fStart < fStart && processedBuffer->pOriginalBuffer == pOriginalBuffer)
			{
				delete processedBuffer;
				processedBuffers.erase(processedBuffers.begin() + i);
				i--;
			}
		}
	}
#pragma endregion
}