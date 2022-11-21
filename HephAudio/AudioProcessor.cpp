#include "AudioProcessor.h"
#include "AudioException.h"
#include "Fourier.h"
#include <functional>

#pragma region Helper Methods
constexpr int sgn(double x)
{
	if (x > 0)
	{
		return 1.0;
	}
	if (x < 0)
	{
		return -1.0;
	}
	return 0.0;
}
inline double DecibelToGain(double decibel)
{
	return pow(10.0, decibel * 0.05);
}
inline double GainToDecibel(double gain)
{
	return gain == 0 ? -120.0 : 20.0 * log10(abs(gain));
}
#pragma endregion

namespace HephAudio
{
#pragma region Converts, Mix, Split/Merge Channels
	void AudioProcessor::ConvertBPS(AudioBuffer& buffer, uint16_t outputBps)
	{
		if (buffer.formatInfo.bitsPerSample == outputBps) { return; }
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, outputBps, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (uint8_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				double sample = buffer.Get(i, j);
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
				resultBuffer.Set(sample, i, j); // Get the normailzed data from current buffer and set it in the result buffer. (AudioBuffer will do the rest)
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::ConvertChannels(AudioBuffer& buffer, uint16_t outputChannelCount)
	{
		if (buffer.formatInfo.channelCount == outputChannelCount) { return; }
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, outputChannelCount, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
		for (size_t i = 0; i < buffer.frameCount; i++) // For each frame, find the average value and then set all the result channels to it.
		{
			double averageValue = 0.0f;
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
		buffer = resultBuffer;
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate)
	{
		ConvertSampleRate(buffer, outputSampleRate, 0u);
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate, size_t outFrameCount)
	{
		if (buffer.formatInfo.sampleRate == outputSampleRate) { return; }
		const double srRatio = (double)outputSampleRate / (double)buffer.formatInfo.sampleRate;
		const size_t currentFrameCount = buffer.frameCount;
		size_t targetFrameCount = outFrameCount;
		if (targetFrameCount == 0)
		{
			targetFrameCount = ceil((double)currentFrameCount * srRatio);
		}
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, buffer.formatInfo.bitsPerSample, outputSampleRate);
		AudioBuffer resultBuffer(targetFrameCount, resultFormat);
		const double cursorRatio = (1.0 / (targetFrameCount - 1)) * (currentFrameCount - 1);
		double cursor = 0.0;
		for (size_t i = 0; i < targetFrameCount; i++)
		{
			const double fc = floor(cursor);
			const double factor = cursor - fc;
			const size_t fc2 = fc + 1 < currentFrameCount ? fc + 1 : currentFrameCount - 1;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = buffer[fc][j] * (1.0 - factor) + buffer[fc + 1.0][j] * factor;
			}
			cursor += cursorRatio;
		}
		buffer = resultBuffer;
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
					outputBuffer[j][k] += buffer[j][k] / (double)inputBuffers.size();
				}
			}
		}
	}
	std::vector<AudioBuffer> AudioProcessor::SplitChannels(const AudioBuffer& buffer)
	{
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, 1, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate);
		std::vector<AudioBuffer> channels(buffer.formatInfo.channelCount, AudioBuffer(buffer.frameCount, resultFormat));
		const size_t sampleSize = buffer.formatInfo.bitsPerSample * 0.125;
		const size_t frameSize = buffer.formatInfo.FrameSize();
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				memcpy((uint8_t*)channels.at(j).pAudioData + i * sampleSize, (uint8_t*)buffer.pAudioData + i * frameSize + j * sampleSize, sampleSize);
			}
		}
		return channels;
	}
	AudioBuffer AudioProcessor::MergeChannels(const std::vector<AudioBuffer>& channels)
	{
		if (channels.size() == 0) { return AudioBuffer(0, AudioFormatInfo()); }
		for (size_t i = 0; i < channels.size() - 1; i++)
		{
			if (channels.at(i).formatInfo != channels.at(i + 1).formatInfo)
			{
				throw AudioException(E_FAIL, L"AudioProcessor::MergeChannels", L"All channels must have the same audio format.");
			}
		}
		AudioBuffer resultBuffer(channels.at(0).frameCount, AudioFormatInfo(channels.at(0).formatInfo.formatTag, channels.size(), channels.at(0).formatInfo.bitsPerSample, channels.at(0).formatInfo.sampleRate));
		const size_t sampleSize = resultBuffer.formatInfo.bitsPerSample * 0.125;
		const size_t frameSize = resultBuffer.formatInfo.FrameSize();
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				memcpy((uint8_t*)resultBuffer.pAudioData + i * frameSize + j * sampleSize, (uint8_t*)channels.at(j).pAudioData + i * sampleSize, sampleSize);
			}
		}
		return resultBuffer;
	}
	void AudioProcessor::ConvertPcmToInnerFormat(AudioBuffer& buffer)
	{
		if (buffer.formatInfo.formatTag != WAVE_FORMAT_PCM)
		{
			throw AudioException(E_FAIL, L"AudioProcessor::ConvertPcmToInnerFormat", L"Invalid format. buffer format must be raw pcm.");
		}
		AudioBuffer tempBuffer = AudioBuffer(buffer.frameCount, AudioFormatInfo(WAV_FORMAT_HEPHAUDIO, buffer.formatInfo.channelCount, sizeof(double) * 8, buffer.formatInfo.sampleRate));
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				tempBuffer[i][j] = buffer.Get(i, j);
			}
		}
		buffer = tempBuffer;
	}
	void AudioProcessor::ConvertInnerToPcmFormat(AudioBuffer& buffer, size_t bps)
	{
		if (buffer.formatInfo.formatTag != WAV_FORMAT_HEPHAUDIO)
		{
			throw AudioException(E_FAIL, L"AudioProcessor::ConvertInnerToPcmFormat", L"Invalid format. buffer format must be the inner format of the HephAudio library.");
		}
		AudioBuffer tempBuffer = AudioBuffer(buffer.frameCount, AudioFormatInfo(WAVE_FORMAT_PCM, buffer.formatInfo.channelCount, bps, buffer.formatInfo.sampleRate));
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				tempBuffer.Set(buffer[i][j], i, j);
			}
		}
		buffer = tempBuffer;
	}
#pragma endregion
#pragma region Sound Effects
	void AudioProcessor::Reverse(AudioBuffer& buffer)
	{
		for (size_t i = 0; i < buffer.frameCount * 0.5; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const double temp = buffer[i][j];
				buffer[i][j] = buffer[buffer.frameCount - i - 1][j];
				buffer[buffer.frameCount - i - 1][j] = temp;
			}
		}
	}
	void AudioProcessor::Echo(AudioBuffer& buffer, EchoInfo info)
	{
		if (info.reflectionCount == 0 || info.volumeFactor == 0.0 || info.echoStartPosition < 0 || info.echoStartPosition >= 1.0 || info.reflectionDelay < 0) { return; }
		struct EchoKeyPoints
		{
			size_t startFrameIndex;
			size_t endFrameIndex;
			double factor;
		};
		const size_t delayFrameCount = buffer.formatInfo.sampleRate * info.reflectionDelay;
		const size_t echoStartFrame = buffer.frameCount * info.echoStartPosition;
		const double echoEndPosition = info.echoEndPosition > info.echoStartPosition ? info.echoEndPosition : 1.0;
		const AudioBuffer echoBuffer = buffer.GetSubBuffer(echoStartFrame, buffer.frameCount * echoEndPosition - echoStartFrame);
		size_t resultBufferFrameCount = buffer.frameCount;
		std::vector<EchoKeyPoints> keyPoints(info.reflectionCount);
		for (size_t i = 0; i < keyPoints.size(); i++) // Find echo key points.
		{
			keyPoints.at(i).startFrameIndex = echoStartFrame + delayFrameCount * (i + 1);
			keyPoints.at(i).endFrameIndex = keyPoints.at(i).startFrameIndex + echoBuffer.frameCount - 1;
			keyPoints.at(i).factor = pow(info.volumeFactor, i + 1.0);
			if (keyPoints.at(i).endFrameIndex >= resultBufferFrameCount)
			{
				resultBufferFrameCount = keyPoints.at(i).endFrameIndex + 1;
			}
		}
		buffer.Resize(resultBufferFrameCount);
		for (size_t i = keyPoints.at(0).startFrameIndex; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				for (size_t k = 0; k < keyPoints.size(); k++)
				{
					if (i >= keyPoints.at(k).startFrameIndex && i <= keyPoints.at(k).endFrameIndex)
					{
						buffer[i][j] += echoBuffer[i - keyPoints.at(k).startFrameIndex][j] * keyPoints.at(k).factor;
					}
				}
			}
		}
	}
	void AudioProcessor::EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info)
	{
		if (subBuffer.frameCount == 0 || info.reflectionCount == 0 || info.volumeFactor == 0.0 || info.echoStartPosition < 0 || info.echoStartPosition >= 1.0 || info.reflectionDelay < 0) { return; }
		struct EchoKeyPoints
		{
			size_t startFrameIndex;
			size_t endFrameIndex;
			double factor;
		};
		const size_t delayFrameCount = originalBuffer.formatInfo.sampleRate * info.reflectionDelay;
		const size_t echoStartFrame = originalBuffer.frameCount * info.echoStartPosition;
		const double echoEndPosition = info.echoEndPosition > info.echoStartPosition ? info.echoEndPosition : 1.0;
		const size_t echoFrameCount = originalBuffer.frameCount * echoEndPosition - echoStartFrame;
		std::vector<EchoKeyPoints> keyPoints(info.reflectionCount);
		size_t erasedKeyPointsCount = 0;
		for (int64_t i = 0; i < keyPoints.size(); i++) // Find echo key points.
		{
			keyPoints.at(i).startFrameIndex = echoStartFrame + delayFrameCount * (i + erasedKeyPointsCount + 1);
			keyPoints.at(i).endFrameIndex = keyPoints.at(i).startFrameIndex + echoFrameCount - 1;
			keyPoints.at(i).factor = pow(info.volumeFactor, i + erasedKeyPointsCount + 1.0);
			if (keyPoints.at(i).startFrameIndex > subBufferFrameIndex + subBuffer.frameCount || keyPoints.at(i).endFrameIndex < subBufferFrameIndex) // Remove keyPoints if not needed to reduce the total loop count.
			{
				keyPoints.erase(keyPoints.begin() + i);
				i--;
				erasedKeyPointsCount++;
			}
		}
		if (keyPoints.size() > 0)
		{
			const AudioBuffer echoBuffer = originalBuffer.GetSubBuffer(echoStartFrame, subBuffer.frameCount + subBufferFrameIndex - keyPoints.at(0).startFrameIndex);
			size_t cursor = subBufferFrameIndex;
			for (size_t i = 0; i < subBuffer.frameCount; i++)
			{
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					for (size_t k = 0; k < keyPoints.size(); k++)
					{
						if (cursor >= keyPoints.at(k).startFrameIndex && cursor <= keyPoints.at(k).endFrameIndex)
						{
							subBuffer[i][j] += echoBuffer[cursor - keyPoints.at(k).startFrameIndex][j] * keyPoints.at(k).factor;
						}
					}
				}
				cursor++;
			}
		}
	}
	void AudioProcessor::LinearPanning(AudioBuffer& buffer, double panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const double rightVolume = panningFactor * 0.5 + 0.5;
			const double leftVolume = 1.0 - rightVolume;
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::SquareLawPanning(AudioBuffer& buffer, double panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const double volume = panningFactor * 0.5 + 0.5;
			const double rightVolume = sqrt(volume);
			const double leftVolume = sqrt(1.0 - volume);
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::SineLawPanning(AudioBuffer& buffer, double panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			constexpr double pi2 = PI * 0.5;
			const double volume = panningFactor * 0.5 + 0.5;
			const double rightVolume = sin(volume * pi2);
			const double leftVolume = sin((1.0 - volume) * pi2);
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::Tremolo(AudioBuffer& buffer, double frequency, double depth, double phase, uint8_t waveType)
	{
		TremoloRT(buffer, 0u, frequency, depth, phase, waveType);
	}
	void AudioProcessor::TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, double frequency, double depth, double phase, uint8_t waveType)
	{
		constexpr double pi2 = PI * 2.0;
		const double w = pi2 * frequency;
		const double dt = 1.0 / subBuffer.formatInfo.sampleRate;
		const double wetFactor = depth * 0.5;
		const double dryFactor = 1.0 - wetFactor;
		double t = subBufferFrameIndex * dt;
		double (*waveFunction)(const double& frequency, const double& phase, const double& w, const double& t, const double& wetFactor, const double& dryFactor) = nullptr;
		switch (waveType)
		{
		case TREMOLO_SINE_WAVE:
		{
			waveFunction = [](const double& frequency, const double& phase, const double& w, const double& t, const double& wetFactor, const double& dryFactor) -> double
			{
				return wetFactor * sin(w * t + phase) + dryFactor;
			};
		}
		break;
		case TREMOLO_SQUARE_WAVE:
		{
			waveFunction = [](const double& frequency, const double& phase, const double& w, const double& t, const double& wetFactor, const double& dryFactor) -> double
			{
				return wetFactor * sgn(sin(w * t + phase)) + dryFactor;
			};
		}
		break;
		case TREMOLO_TRIANGLE_WAVE:
		{
			waveFunction = [](const double& frequency, const double& phase, const double& w, const double& t, const double& wetFactor, const double& dryFactor) -> double
			{
				constexpr double topi = 2.0 / PI;
				return wetFactor * (topi * asin(sin(w * t + phase))) + dryFactor;
			};
		}
		break;
		case TREMOLO_SAWTOOTH_WAVE:
		{
			waveFunction = [](const double& frequency, const double& phase, const double& w, const double& t, const double& wetFactor, const double& dryFactor) -> double
			{
				const double x = t * frequency + phase;
				return wetFactor * (2.0 * (x - floor(x + 0.5))) + dryFactor;
			};
		}
		break;
		default:
			throw AudioException(E_FAIL, L"AudioProcessor::Tremolo", L"Invalid wave type.");
		}
		for (size_t i = 0; i < subBuffer.frameCount; i++, t += dt)
		{
			for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
			{
				subBuffer[i][j] *= waveFunction(frequency, phase, w, t, wetFactor, dryFactor);
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
		const double piOverN = PI / (fftSize - 1);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
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
						complexBuffer[l] *= info.volumeFunction(Fourier::IndexToFrequency(buffer.formatInfo.sampleRate, fftSize, l));
						complexBuffer[fftSize - l - 1] = Complex(complexBuffer[l].real, -complexBuffer[l].imaginary);
					}
				}
				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					const double s = sin(k * piOverN);
					buffer[l][j] += complexBuffer[k].real * s * s / fftSize;
				}
			}
		}
	}
	void AudioProcessor::EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos)
	{
		EqualizerRT(originalBuffer, subBuffer, subBufferFrameIndex, subBuffer.frameCount, subBuffer.frameCount * 2, infos);
	}
	void AudioProcessor::EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const auto applyEqualizer = [&hopSize, &fftSize, &infos](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				AudioBuffer& channel = channels.at(i);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel, fftSize);
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
						complexBuffer[k] *= info.volumeFunction(Fourier::IndexToFrequency(subBuffer.formatInfo.sampleRate, fftSize, k));
						complexBuffer[fftSize - k - 1].real = complexBuffer[k].real;
						complexBuffer[fftSize - k - 1].imaginary = -complexBuffer[k].imaginary;
					}
				}
				Fourier::FFT_Inverse(channel, complexBuffer);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = channel[j][i];
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex >= fftSize)
		{
			fStart = fTarget;
			do
			{
				fStart -= hopSize;
			} while (fStart + fftSize > fTarget);
		}
		size_t fEnd = fStart;
		while (fEnd + hopSize < fTarget + fftSize)
		{
			fEnd += hopSize;
		}
		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);
		size_t isb, itb;
		const double piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i);
				applyEqualizer(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}
			if (i > fTarget)
			{
				isb = (int64_t)i - fTarget;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = (int64_t)fTarget - i;
			}
			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const double s = sin(itb * piOverN);
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double speed)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t targetSize = Fourier::CalculateFFTSize(fftSize / speed);
		const size_t targetNyquistFrequency = targetSize * 0.5;
		const size_t tsm1 = targetSize - 1;
		std::function<double(const size_t& i)> window;
		if (speed >= 2.0)
		{
			window = [&tsm1](const size_t& i) -> double { const double s = (i - 0.5 * tsm1) / (0.5 * tsm1); return (1.0 - s * s) * 0.25; };
		}
		else
		{
			window = [&tsm1](const size_t& i) -> double { return 0.42 - 0.5 * cos(2.0 * PI * i / tsm1) + 0.08 * cos(4.0 * PI * i / tsm1); };
		}
		const double cursorRatio = 1.0 / tsm1 * (fftSize - 1);
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
				double cursor = 0.0;
				for (size_t k = 0; k < targetNyquistFrequency; k++, cursor += cursorRatio)
				{
					const double fc = floor(cursor);
					const double factor = cursor - fc;
					targetComplexBuffer[k] = complexBuffer[fc] * (1.0 - factor) + complexBuffer[fc + 1.0] * factor;
					targetComplexBuffer[targetSize - k - 1] = Complex(targetComplexBuffer[k].real, -targetComplexBuffer[k].imaginary);
				}
				complexBuffer.~ComplexBuffer();
				Fourier::FFT_Inverse(targetComplexBuffer, false);
				for (size_t k = 0, l = i / speed; k < targetSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += targetComplexBuffer[k].real * window(k) / targetSize;
				}
			}
		}
	}
#pragma endregion
#pragma region Filters
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		LowPassFilter(buffer, defaultHopSize, defaultFFTSize, cutoffFreq, volumeFunction);
	}
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const double piOverN = PI / (fftSize - 1);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (size_t k = startIndex; k < nyquistFrequency; k++)
				{
					complexBuffer[k] *= volumeFunction(Fourier::IndexToFrequency(buffer.formatInfo.sampleRate, fftSize, k));
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
				}
				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					const double s = sin(k * piOverN);
					buffer[l][j] += complexBuffer[k].real * s * s / fftSize;
				}
			}
		}
	}
	void AudioProcessor::LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		LowPassFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, subBuffer.frameCount, subBuffer.frameCount * 2, cutoffFreq, volumeFunction);
	}
	void AudioProcessor::LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const auto applyFilter = [&hopSize, &fftSize, &cutoffFreq, &volumeFunction](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const uint64_t startIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				AudioBuffer& channel = channels.at(i);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel, fftSize);
				for (size_t j = startIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] *= volumeFunction(Fourier::IndexToFrequency(subBuffer.formatInfo.sampleRate, fftSize, j));
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
				}
				Fourier::FFT_Inverse(channel, complexBuffer);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = channel[j][i];
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex >= fftSize)
		{
			fStart = fTarget;
			do
			{
				fStart -= hopSize;
			} while (fStart + fftSize > fTarget);
		}
		size_t fEnd = fStart;
		while (fEnd + hopSize < fTarget + fftSize)
		{
			fEnd += hopSize;
		}
		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);
		size_t isb, itb;
		const double piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}
			if (i > fTarget)
			{
				isb = (int64_t)i - fTarget;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = (int64_t)fTarget - i;
			}
			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const double s = sin(itb * piOverN);
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		HighPassFilter(buffer, defaultHopSize, defaultFFTSize, cutoffFreq, volumeFunction);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const double piOverN = PI / (fftSize - 1);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (int64_t k = stopIndex; k >= 0; k--)
				{
					complexBuffer[k] *= volumeFunction(Fourier::IndexToFrequency(buffer.formatInfo.sampleRate, fftSize, k));
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
				}
				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					const double s = sin(k * piOverN);
					buffer[l][j] += complexBuffer[k].real * s * s / fftSize;
				}
			}
		}
	}
	void AudioProcessor::HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		HighPassFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, subBuffer.frameCount, subBuffer.frameCount * 2, cutoffFreq, volumeFunction);
	}
	void AudioProcessor::HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const auto applyFilter = [&hopSize, &fftSize, &cutoffFreq, &volumeFunction](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const uint64_t stopIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				AudioBuffer& channel = channels.at(i);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel, fftSize);
				for (int64_t j = stopIndex; j >= 0; j--)
				{
					complexBuffer[j] *= volumeFunction(Fourier::IndexToFrequency(subBuffer.formatInfo.sampleRate, fftSize, j));
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
				}
				Fourier::FFT_Inverse(channel, complexBuffer);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = channel[j][i];
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex >= fftSize)
		{
			fStart = fTarget;
			do
			{
				fStart -= hopSize;
			} while (fStart + fftSize > fTarget);
		}
		size_t fEnd = fStart;
		while (fEnd + hopSize < fTarget + fftSize)
		{
			fEnd += hopSize;
		}
		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);
		AudioBuffer tempBuffer;
		size_t isb, itb;
		const double piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}
			if (i > fTarget)
			{
				isb = (int64_t)i - fTarget;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = (int64_t)fTarget - i;
			}
			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const double s = sin(itb * piOverN);
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		BandPassFilter(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq, volumeFunction);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const double piOverN = PI / (fftSize - 1);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (int64_t k = startIndex; k >= 0; k--)
				{
					complexBuffer[k] *= volumeFunction(Fourier::IndexToFrequency(buffer.formatInfo.sampleRate, fftSize, k));
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
				}
				for (size_t k = stopIndex; k < nyquistFrequency; k++)
				{
					complexBuffer[k] *= volumeFunction(Fourier::IndexToFrequency(buffer.formatInfo.sampleRate, fftSize, k));
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
				}
				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					const double s = sin(k * piOverN);
					buffer[l][j] += complexBuffer[k].real * s * s / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		BandPassFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, subBuffer.frameCount, subBuffer.frameCount * 2, lowCutoffFreq, highCutoffFreq, volumeFunction);
	}
	void AudioProcessor::BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const auto applyFilter = [&hopSize, &fftSize, &lowCutoffFreq, &highCutoffFreq, &volumeFunction](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const uint64_t startIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
			const uint64_t stopIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				AudioBuffer& channel = channels.at(i);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel, fftSize);
				for (int64_t j = startIndex; j >= 0; j--)
				{
					complexBuffer[j] *= volumeFunction(Fourier::IndexToFrequency(subBuffer.formatInfo.sampleRate, fftSize, j));
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
				}
				for (size_t j = stopIndex; j < nyquistFrequency; j++)
				{
					complexBuffer[j] *= volumeFunction(Fourier::IndexToFrequency(subBuffer.formatInfo.sampleRate, fftSize, j));
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
				}
				Fourier::FFT_Inverse(channel, complexBuffer);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = channel[j][i];
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex >= fftSize)
		{
			fStart = fTarget;
			do
			{
				fStart -= hopSize;
			} while (fStart + fftSize > fTarget);
		}
		size_t fEnd = fStart;
		while (fEnd + hopSize < fTarget + fftSize)
		{
			fEnd += hopSize;
		}
		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);
		AudioBuffer tempBuffer;
		size_t isb, itb;
		const double piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}
			if (i > fTarget)
			{
				isb = (int64_t)i - fTarget;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = (int64_t)fTarget - i;
			}
			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const double s = sin(itb * piOverN);
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		BandCutFilter(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq, volumeFunction);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const double piOverN = PI / (fftSize - 1);
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.Reset();
		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channels.at(j).GetSubBuffer(i, fftSize), fftSize);
				for (size_t k = startIndex; k <= stopIndex; k++)
				{
					complexBuffer[k] *= volumeFunction(Fourier::IndexToFrequency(buffer.formatInfo.sampleRate, fftSize, k));
					complexBuffer[fftSize - k - 1] = Complex(complexBuffer[k].real, -complexBuffer[k].imaginary);
				}
				Fourier::FFT_Inverse(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					const double s = sin(k * piOverN);
					buffer[l][j] += complexBuffer[k].real * s * s / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		BandCutFilterRT(originalBuffer, subBuffer, subBufferFrameIndex, subBuffer.frameCount, subBuffer.frameCount * 2, lowCutoffFreq, highCutoffFreq, volumeFunction);
	}
	void AudioProcessor::BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction)
	{
		static std::vector<ProcessedBuffer*> processedBuffers;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const auto applyFilter = [&hopSize, &fftSize, &lowCutoffFreq, &highCutoffFreq, &volumeFunction](AudioBuffer& subBuffer) -> void
		{
			const size_t nyquistFrequency = fftSize * 0.5;
			const uint64_t startIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
			const uint64_t stopIndex = Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
			const size_t sampleSize = subBuffer.formatInfo.bitsPerSample * 0.125;
			const size_t frameSize = subBuffer.formatInfo.FrameSize();
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < subBuffer.formatInfo.channelCount; i++)
			{
				AudioBuffer& channel = channels.at(i);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel, fftSize);
				for (size_t j = startIndex; j <= stopIndex; j++)
				{
					complexBuffer[j] *= volumeFunction(Fourier::IndexToFrequency(subBuffer.formatInfo.sampleRate, fftSize, j));
					complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
				}
				Fourier::FFT_Inverse(channel, complexBuffer);
				for (size_t j = 0; j < fftSize; j++)
				{
					subBuffer[j][i] = channel[j][i];
				}
			}
		};
		const size_t fTarget = subBufferFrameIndex - (subBufferFrameIndex % hopSize);
		size_t fStart = 0;
		if (subBufferFrameIndex >= fftSize)
		{
			fStart = fTarget;
			do
			{
				fStart -= hopSize;
			} while (fStart + fftSize > fTarget);
		}
		size_t fEnd = fStart;
		while (fEnd + hopSize < fTarget + fftSize)
		{
			fEnd += hopSize;
		}
		subBuffer.Reset();
		RemoveOldProcessedBuffers(processedBuffers, &originalBuffer, fStart);
		AudioBuffer tempBuffer;
		size_t isb, itb;
		const double piOverN = PI / (fftSize - 1);
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			ProcessedBuffer* processedBuffer = GetProcessedBuffer(processedBuffers, &originalBuffer, i);
			if (processedBuffer == nullptr)
			{
				processedBuffer = new ProcessedBuffer(&originalBuffer, originalBuffer.GetSubBuffer(i, fftSize), i);
				applyFilter(processedBuffer->audioBuffer);
				processedBuffers.push_back(processedBuffer);
			}
			if (i > fTarget)
			{
				isb = (int64_t)i - fTarget;
				itb = 0;
			}
			else
			{
				isb = 0;
				itb = (int64_t)fTarget - i;
			}
			for (; isb < subBuffer.frameCount && itb < fftSize; isb++, itb++)
			{
				for (size_t j = 0; j < subBuffer.formatInfo.channelCount; j++)
				{
					const double s = sin(itb * piOverN);
					subBuffer[isb][j] += processedBuffer->audioBuffer[itb][j] * s * s;
				}
			}
		}
	}
#pragma endregion
#pragma region Windows
	void AudioProcessor::TriangleWindow(AudioBuffer& buffer)
	{
		const double hN = 0.5 * (buffer.frameCount - 1);
		const double hL = hN + 1.0;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 1.0 - fabs((i - hN) / hL);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::ParzenWindow(AudioBuffer& buffer)
	{
		const double hN = 0.5 * (buffer.frameCount - 1);
		const double hL = hN + 0.5;
		const double qL = hL * 0.5;
		double n = 0.0;
		double absN = 0.0;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			n = i - hN;
			absN = fabs(n);
			if (absN >= 0 && absN <= qL)
			{
				factor = 1.0 - 6.0 * pow(n / hL, 2) * (1.0 - absN / hL);
			}
			else
			{
				factor = 2.0 * pow(1.0 - absN / hL, 3);
			}
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::WelchWindow(AudioBuffer& buffer)
	{
		const double hN = 0.5 * (buffer.frameCount - 1);
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 1.0 - pow((i - hN) / hN, 2);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::SineWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = sin(PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::HannWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = pow(sin(PI * i / N), 2);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::HammingWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.54 - 0.46 * cos(2.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::BlackmanWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.42 - 0.5 * cos(2.0 * PI * i / N) + 0.08 * cos(4.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::ExactBlackmanWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.42659 - 0.49656 * cos(2.0 * PI * i / N) + 0.076849 * cos(4.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::NuttallWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.355768 - 0.487396 * cos(2.0 * PI * i / N) + 0.144232 * cos(4.0 * PI * i / N) - 0.012604 * cos(6.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::BlackmanNuttallWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.3635819 - 0.4891775 * cos(2.0 * PI * i / N) + 0.1365995 * cos(4.0 * PI * i / N) - 0.0106411 * cos(6.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::BlackmanHarrisWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.35875 - 0.48829 * cos(2.0 * PI * i / N) + 0.14128 * cos(4.0 * PI * i / N) - 0.01168 * cos(6.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::FlatTopWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.21557895 - 0.41663158 * cos(2.0 * PI * i / N) + 0.277263158 * cos(4.0 * PI * i / N) - 0.083578947 * cos(6.0 * PI * i / N) + 0.006947368 * cos(8.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::GaussianWindow(AudioBuffer& buffer, double sigma)
	{
		const double hN = 0.5 * (buffer.frameCount - 1);
		const double shN = sigma * hN;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = exp(-0.5 * pow((i - hN) / shN, 2));
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::TukeyWindow(AudioBuffer& buffer, double alpha)
	{
		const double N = (buffer.frameCount - 1);
		const double hN = 0.5 * N;
		const double aN = alpha * N;
		const double haN = 0.5 * aN;
		double factor = 1.0;
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
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::BartlettHannWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.62 - 0.48 * fabs(i / N - 0.5) - 0.38 * cos(2.0 * PI * i / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::HannPoissonWindow(AudioBuffer& buffer, double alpha)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			factor = 0.5 * (1.0 - cos(2.0 * PI * i / N)) * exp(-alpha * fabs(N - 2.0 * i) / N);
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::LanczosWindow(AudioBuffer& buffer)
	{
		const double N = buffer.frameCount - 1;
		double factor = 1.0;
		double pix = 1.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			pix = PI * (2.0 * i / N - 1.0);
			factor = sin(pix) / pix;
			for (size_t j = 0; j < buffer.GetFormat().channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
#pragma endregion
#pragma region Maximize Volume
	double AudioProcessor::FindMaxVolume(const AudioBuffer& buffer)
	{
		const size_t channelCount = buffer.formatInfo.channelCount;
		double maxSample = 0.0;
		double currentSample = 0.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < channelCount; j++)
			{
				currentSample = abs(buffer[i][j]);
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
					if (maxSample == 1.0)
					{
						return 1.0;
					}
				}
			}
		}
		return maxSample != 0.0 ? (1.0 / maxSample) : 1.0;
	}
	void AudioProcessor::MaximizeVolume(AudioBuffer& buffer)
	{
		const size_t channelCount = buffer.formatInfo.channelCount;
		double maxSample = 0.0;
		double currentSample = 0.0;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < channelCount; j++)
			{
				currentSample = abs(buffer[i][j]);
				if (currentSample > maxSample)
				{
					maxSample = currentSample;
					if (maxSample == 1.0)
					{
						return;
					}
				}
			}
		}
		if (maxSample != 0.0)
		{
			buffer /= maxSample;
		}
	}
#pragma endregion
#pragma region Processed Buffer
	AudioProcessor::ProcessedBuffer::ProcessedBuffer() : ProcessedBuffer(nullptr, AudioBuffer(), 0u) {}
	AudioProcessor::ProcessedBuffer::ProcessedBuffer(const AudioBuffer* pOriginalBuffer, const AudioBuffer& audioBuffer, size_t fStart)
	{
		this->pOriginalBuffer = pOriginalBuffer;
		this->audioBuffer = audioBuffer;
		this->fStart = fStart;
	}
	AudioProcessor::ProcessedBuffer* AudioProcessor::GetProcessedBuffer(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart)
	{
		for (size_t i = 0; i < processedBuffers.size(); i++)
		{
			if (fStart == processedBuffers.at(i)->fStart && pOriginalBuffer == processedBuffers.at(i)->pOriginalBuffer)
			{
				return processedBuffers.at(i);
			}
		}
		return nullptr;
	}
	void AudioProcessor::RemoveOldProcessedBuffers(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart)
	{
		for (int64_t i = 0; i < processedBuffers.size(); i++)
		{
			if (processedBuffers.at(i)->fStart < fStart && pOriginalBuffer == processedBuffers.at(i)->pOriginalBuffer)
			{
				delete processedBuffers.at(i);
				processedBuffers.erase(processedBuffers.begin() + i);
				i--;
			}
		}
	}
#pragma endregion
}