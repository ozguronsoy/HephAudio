#include "AudioProcessor.h"
#include "AudioException.h"
#include "Fourier.h"
#include <thread>

namespace HephAudio
{
#pragma region Converts, Mix, Split/Merge Channels
	void AudioProcessor::ConvertBPS(AudioBuffer& buffer, uint16_t outputBps)
	{
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, outputBps, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (uint8_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				hephaudio_float sample = buffer.Get(i, j);
				if (outputBps == 8)
				{
					sample += 1.0;
					sample *= 0.5;
				}
				else if (buffer.formatInfo.bitsPerSample == 8)
				{
					sample *= 2.0;
					sample -= 1.0;
				}
				resultBuffer.Set(sample, i, j);
			}
		}
		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::ConvertChannels(AudioBuffer& buffer, uint16_t outputChannelCount)
	{
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, outputChannelCount, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
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
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate)
	{
		ConvertSampleRate(buffer, outputSampleRate, 0u);
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate, size_t outFrameCount)
	{
		const hephaudio_float srRatio = (hephaudio_float)outputSampleRate / (hephaudio_float)buffer.formatInfo.sampleRate;
		const size_t currentFrameCount = buffer.frameCount;
		size_t targetFrameCount = outFrameCount;
		if (targetFrameCount == 0)
		{
			targetFrameCount = ceil((hephaudio_float)currentFrameCount * srRatio);
		}
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, buffer.formatInfo.bitsPerSample, outputSampleRate);
		AudioBuffer resultBuffer(targetFrameCount, resultFormat);
		const hephaudio_float dc = 1.0 / srRatio;
		hephaudio_float c = 0.0;
		for (size_t i = 0; i < targetFrameCount; i++, c += dc)
		{
			const hephaudio_float fc = floor(c);
			const hephaudio_float factor = c - fc;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = buffer[fc][j] * (1.0 - factor) + buffer[fc + 1.0][j] * factor;
			}
		}
		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers)
	{
		size_t outputBufferFrameCount = 0;
		for (int64_t i = 0; i < inputBuffers.size(); i++)
		{
			AudioBuffer& buffer = inputBuffers.at(i);
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
			AudioBuffer& buffer = inputBuffers.at(i);
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
	std::vector<AudioBuffer> AudioProcessor::SplitChannels(const AudioBuffer& buffer)
	{
		std::vector<AudioBuffer> channels(buffer.formatInfo.channelCount, AudioBuffer(buffer.frameCount, AudioFormatInfo(buffer.formatInfo.formatTag, 1, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate)));
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				channels.at(j)[i][0] = buffer[i][j];
			}
		}
		return channels;
	}
	AudioBuffer AudioProcessor::MergeChannels(const std::vector<AudioBuffer>& channels)
	{
		if (channels.size() == 0) { return AudioBuffer(0, AudioFormatInfo()); }
		AudioBuffer resultBuffer(channels.at(0).frameCount, AudioFormatInfo(channels.at(0).formatInfo.formatTag, channels.size(), channels.at(0).formatInfo.bitsPerSample, channels.at(0).formatInfo.sampleRate));
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = channels.at(j)[i][0];
			}
		}
		return resultBuffer;
	}
	void AudioProcessor::ConvertPcmToInnerFormat(AudioBuffer& buffer)
	{
		if (buffer.formatInfo.formatTag == WAVE_FORMAT_PCM)
		{
			AudioBuffer tempBuffer = AudioBuffer(buffer.frameCount, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, sizeof(hephaudio_float) * 8, buffer.formatInfo.sampleRate));
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					tempBuffer[i][j] = buffer.Get(i, j);
				}
			}
			buffer = std::move(tempBuffer);
		}
	}
	void AudioProcessor::ConvertInnerToPcmFormat(AudioBuffer& buffer, size_t bps)
	{
		if (buffer.formatInfo.formatTag == WAVE_FORMAT_HEPHAUDIO)
		{
			AudioBuffer tempBuffer = AudioBuffer(buffer.frameCount, AudioFormatInfo(WAVE_FORMAT_PCM, buffer.formatInfo.channelCount, bps, buffer.formatInfo.sampleRate));
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					tempBuffer.Set(buffer[i][j], i, j);
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
		buffer.Resize(echoStartFrame + delayFrameCount * info.reflectionCount + echoBuffer.frameCount);
		hephaudio_float factor = info.decayFactor;
		size_t startFrameIndex = echoStartFrame + delayFrameCount;
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
			constexpr hephaudio_float piOver2 = PI * 0.5;
			const hephaudio_float volume = panningFactor * 0.5 + 0.5;
			const hephaudio_float rightVolume = sin(volume * piOver2);
			const hephaudio_float leftVolume = sin((1.0 - volume) * piOver2);
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::Tremolo(AudioBuffer& buffer, hephaudio_float depth, const OscillatorBase& lfo)
	{
		AudioProcessor::TremoloRT(buffer, 0, depth, lfo);
	}
	void AudioProcessor::TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float depth, const OscillatorBase& lfo)
	{
		const hephaudio_float wetFactor = depth * 0.5;
		const hephaudio_float dryFactor = 1.0 - wetFactor;

		for (size_t i = 0, t_sample = subBufferFrameIndex; i < subBuffer.frameCount; i++, t_sample++)
		{
			const hephaudio_float lfoSample = lfo.Oscillate(t_sample);
			for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
			{
				subBuffer[i][j] *= wetFactor * lfoSample + dryFactor;
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
		constexpr hephaudio_float twoOverPi = 2.0 / PI;
		alpha = max(alpha, 1.0);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				hephaudio_float& sample = buffer[i][j];
				sample = twoOverPi * atan(alpha * sample);
			}
		}
	}
	void AudioProcessor::Overdrive(AudioBuffer& buffer, hephaudio_float drive)
	{
		constexpr hephaudio_float piOverTwo = PI / 2.0;
		const hephaudio_float a = sin(drive * piOverTwo);
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
	void AudioProcessor::Flanger(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float delay_ms, const OscillatorBase& lfo)
	{
		constexpr hephaudio_float twopi = 2.0 * PI;
		const size_t maxSampleDelay = round(delay_ms * 1e-3 * buffer.formatInfo.sampleRate);
		const hephaudio_float wetFactor = depth * 0.5;
		const hephaudio_float dryFactor = 1.0 - wetFactor;

		AudioBuffer resultBuffer = AudioBuffer(buffer.frameCount, buffer.formatInfo);
		memcpy(resultBuffer.pAudioData, buffer.pAudioData, maxSampleDelay * buffer.formatInfo.FrameSize());

		for (size_t i = maxSampleDelay + 1; i < buffer.frameCount; i++)
		{
			const size_t currentDelay = round(abs(lfo.Oscillate(i)) * maxSampleDelay);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = wetFactor * buffer[i - currentDelay][j] + dryFactor * buffer[i][j];
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::Wah(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float damping, hephaudio_float fcmin, hephaudio_float fcmax, const OscillatorBase& lfo)
	{
		if (buffer.frameCount > 0)
		{
			const hephaudio_float wetFactor = depth;
			const hephaudio_float dryFactor = 1.0 - wetFactor;
			const hephaudio_float fcdelta = fcmax - fcmin;
			const hephaudio_float Q = 2.0 * damping;
			hephaudio_float fc = fcdelta * abs(lfo.Oscillate(0)) + fcmin;
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
				fc = fcdelta * abs(lfo.Oscillate(i)) + fcmin;
				alpha = 2.0 * sin(PI * fc / lfo.sampleRate);

				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					highPassBuffer[i][j] = buffer[i][j] - lowPassBuffer[i - 1][j] - Q * bandPassBuffer[i - 1][j];
					bandPassBuffer[i][j] = alpha * highPassBuffer[i][j] + bandPassBuffer[i - 1][j];
					lowPassBuffer[i][j] = alpha * bandPassBuffer[i][j] + lowPassBuffer[i - 1][j];
				}
			}

			hephaudio_float maxSample = 0.0;
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					buffer[i][j] = wetFactor * bandPassBuffer[i][j] + dryFactor * buffer[i][j];
					if (abs(buffer[i][j]) > maxSample)
					{
						maxSample = abs(buffer[i][j]);
					}
				}
			}
			if (maxSample > 1.0)
			{
				buffer /= maxSample;
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
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				AudioBuffer subBuffer = channels.at(j).GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(subBuffer, fftSize);
				for (size_t k = 0; k < infos.size(); k++)
				{
					const EqualizerInfo& info = infos.at(k);
					uint64_t lowerFrequencyIndex, higherFrequencyIndex;
					if (info.f1 > info.f2)
					{
						higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f1));
						lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f2));
					}
					else
					{
						higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f2));
						lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f1));
					}
					const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;
					for (size_t l = lowerFrequencyIndex; l <= upperBound; l++)
					{
						complexBuffer[l] *= info.amplitudeFunction(Fourier::IndexToFrequency(buffer.formatInfo.sampleRate, fftSize, l));
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
		constexpr auto applyEqualizer = [](AudioBuffer* buffer, FloatBuffer* window, uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, const std::vector<EqualizerInfo>* const infos)
		{
			AudioBuffer channel = AudioBuffer(buffer->frameCount, AudioFormatInfo(buffer->formatInfo.formatTag, 1, buffer->formatInfo.bitsPerSample, buffer->formatInfo.sampleRate));
			for (size_t j = 0; j < buffer->frameCount; j++)
			{
				channel[j][0] = (*buffer)[j][channelIndex];
				(*buffer)[j][channelIndex] = 0;
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
						higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(channel.formatInfo.sampleRate, fftSize, info.f1));
						lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(channel.formatInfo.sampleRate, fftSize, info.f2));
					}
					else
					{
						higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(channel.formatInfo.sampleRate, fftSize, info.f2));
						lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(channel.formatInfo.sampleRate, fftSize, info.f1));
					}
					const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;

					for (size_t l = lowerFrequencyIndex; l <= upperBound; l++)
					{
						complexBuffer[l] *= info.amplitudeFunction(Fourier::IndexToFrequency(channel.formatInfo.sampleRate, fftSize, l));
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
		std::vector<std::thread> threads;
		FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads.push_back(std::thread(applyEqualizer, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, &infos));
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads.at(i).joinable())
			{
				threads.at(i).join();
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
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(i), fftSize);
				for (size_t j = 0; j < infos.size(); j++)
				{
					const EqualizerInfo& info = infos.at(j);
					uint64_t lowerFrequencyIndex, higherFrequencyIndex;
					if (info.f1 > info.f2)
					{
						higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f1));
						lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f2));
					}
					else
					{
						higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f2));
						lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, info.f1));
					}
					const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;
					for (size_t k = lowerFrequencyIndex; k <= upperBound; k++)
					{
						complexBuffer[k] *= info.amplitudeFunction(Fourier::IndexToFrequency(subBuffer.formatInfo.sampleRate, fftSize, k));
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
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const hephaudio_float s = sin(itb * piOverN);
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
		FloatBuffer window;
		if (speed >= 2.0)
		{
			window = GenerateWelchWindow(targetSize);
		}
		else
		{
			window = GenerateBlackmanWindow(targetSize);
		}
		const hephaudio_float cursorRatio = 1.0 / tsm1 * (fftSize - 1);
		const size_t oldFrameCount = buffer.frameCount;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		buffer.Resize(buffer.frameCount / speed);
		for (size_t i = 0; i < oldFrameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
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
		std::vector<AudioBuffer> channels = AudioProcessor::SplitChannels(buffer);
		buffer.Reset();
		FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		AudioBuffer lastAnalysisPhases = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));
		AudioBuffer lastSynthesisPhases = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));
		AudioBuffer synthesisMagnitudes = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));
		AudioBuffer synthesisFrequencies = AudioBuffer(nyquistFrequency, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, hephaudio_floatSizeInBits, buffer.formatInfo.sampleRate));
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			synthesisMagnitudes.Reset();
			synthesisFrequencies.Reset();
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const AudioBuffer subBuffer = channels.at(j).GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = ComplexBuffer(fftSize);
				for (size_t k = 0; k < fftSize; k++)
				{
					complexBuffer[k].real = subBuffer[k][0] * hannWindow[k];
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
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
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
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (size_t k = startIndex; k < nyquistFrequency; k++)
				{
					complexBuffer[k] = Complex(0, 0);
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
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
		constexpr auto applyFilter = [](AudioBuffer* buffer, FloatBuffer* window, size_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex)
		{
			AudioBuffer channel = AudioBuffer(buffer->frameCount, AudioFormatInfo(buffer->formatInfo.formatTag, 1, buffer->formatInfo.bitsPerSample, buffer->formatInfo.sampleRate));
			for (size_t j = 0; j < buffer->frameCount; j++)
			{
				channel[j][0] = (*buffer)[j][channelIndex];
				(*buffer)[j][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);
				for (int64_t j = startIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex();
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
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		std::vector<std::thread> threads;
		FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads.push_back(std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, startIndex));
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads.at(i).joinable())
			{
				threads.at(i).join();
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
			const uint64_t startIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(i), fftSize);
				for (size_t j = startIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
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
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const hephaudio_float s = sin(itb * piOverN);
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
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (int64_t k = stopIndex; k >= 0; k--)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
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
		constexpr auto applyFilter = [](AudioBuffer* buffer, FloatBuffer* window, const uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t stopIndex)
		{
			AudioBuffer channel = AudioBuffer(buffer->frameCount, AudioFormatInfo(buffer->formatInfo.formatTag, 1, buffer->formatInfo.bitsPerSample, buffer->formatInfo.sampleRate));
			for (size_t i = 0; i < buffer->frameCount; i++)
			{
				channel[i][0] = (*buffer)[i][channelIndex];
				(*buffer)[i][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);
				for (int64_t j = stopIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex();
				}

				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t j = 0, k = i; j < fftSize && k < channel.frameCount; j++, k++)
				{
					(*buffer)[k][channelIndex] += complexBuffer[j].real * (*window)[j] / (hephaudio_float)fftSize;
				}
			}
		};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		std::vector<std::thread> threads;
		FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads.push_back(std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, stopIndex));
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads.at(i).joinable())
			{
				threads.at(i).join();
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
			const uint64_t stopIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(i), fftSize);
				for (int64_t j = stopIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
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
		AudioBuffer tempBuffer;
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
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const hephaudio_float s = sin(itb * piOverN);
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
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (int64_t k = startIndex; k >= 0; k--)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
				}
				for (size_t k = stopIndex; k < nyquistFrequency; k++)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
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
		constexpr auto applyFilter = [](AudioBuffer* buffer, FloatBuffer* window, uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex, size_t stopIndex)
		{
			AudioBuffer channel = AudioBuffer(buffer->frameCount, AudioFormatInfo(buffer->formatInfo.formatTag, 1, buffer->formatInfo.bitsPerSample, buffer->formatInfo.sampleRate));
			for (size_t j = 0; j < buffer->frameCount; j++)
			{
				channel[j][0] = (*buffer)[j][channelIndex];
				(*buffer)[j][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);

				for (int64_t j = startIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
				}

				for (size_t j = stopIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
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
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		std::vector<std::thread> threads;
		FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads.push_back(std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, startIndex, stopIndex));
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads.at(i).joinable())
			{
				threads.at(i).join();
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
			const uint64_t startIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
			const uint64_t stopIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(i), fftSize);
				for (int64_t j = startIndex; j >= 0; j--)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
				}
				for (size_t j = stopIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
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
		AudioBuffer tempBuffer;
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
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const hephaudio_float s = sin(itb * piOverN);
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
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		FloatBuffer hannWindow = GenerateHannWindow(fftSize);
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (size_t k = startIndex; k <= stopIndex; k++)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
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
		constexpr auto applyFilter = [](AudioBuffer* buffer, FloatBuffer* window, uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex, size_t stopIndex)
		{
			AudioBuffer channel = AudioBuffer(buffer->frameCount, AudioFormatInfo(buffer->formatInfo.formatTag, 1, buffer->formatInfo.bitsPerSample, buffer->formatInfo.sampleRate));
			for (size_t j = 0; j < buffer->frameCount; j++)
			{
				channel[j][0] = (*buffer)[j][channelIndex];
				(*buffer)[j][channelIndex] = 0;
			}

			for (size_t i = 0; i < channel.frameCount; i += hopSize)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);
				for (size_t j = startIndex; j <= stopIndex; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
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
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		std::vector<std::thread> threads;
		FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads.push_back(std::thread(applyFilter, &buffer, &hannWindow, i, hopSize, fftSize, nyquistFrequency, startIndex, stopIndex));
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads.at(i).joinable())
			{
				threads.at(i).join();
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
			const uint64_t startIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
			const uint64_t stopIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(i), fftSize);
				for (size_t j = startIndex; j <= stopIndex; j++)
				{
					complexBuffer[j] = Complex();
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
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
		AudioBuffer tempBuffer;
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
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const hephaudio_float s = sin(itb * piOverN);
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
			ProcessedBuffer*& processedBuffer = processedBuffers.at(i);
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
			ProcessedBuffer*& processedBuffer = processedBuffers.at(i);
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