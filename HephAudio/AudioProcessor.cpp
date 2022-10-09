#include "AudioProcessor.h"
#include "AudioException.h"
#include "Fourier.h"

namespace HephAudio
{
	AudioProcessor::AudioProcessor(AudioFormatInfo targetFormat)
	{
		this->targetFormat = targetFormat;
	}
#pragma region Converts, Mix, Split/Merge Channels
	void AudioProcessor::ConvertBPS(AudioBuffer& buffer) const
	{
		if (buffer.formatInfo.bitsPerSample == targetFormat.bitsPerSample) { return; }
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, targetFormat.bitsPerSample, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (uint8_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				double sample = buffer.Get(i, j);
				if (targetFormat.bitsPerSample == 8)
				{
					sample += 1.0;
					sample /= 2.0;
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
	void AudioProcessor::ConvertChannels(AudioBuffer& buffer) const
	{
		if (buffer.formatInfo.channelCount == targetFormat.channelCount) { return; }
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, targetFormat.channelCount, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
		for (size_t i = 0; i < buffer.frameCount; i++) // For each frame, find the average value and then set all the result channels to it.
		{
			double averageValue = 0.0f;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				averageValue += buffer.Get(i, j);
			}
			averageValue /= buffer.formatInfo.channelCount;
			for (size_t j = 0; j < targetFormat.channelCount; j++)
			{
				resultBuffer.Set(averageValue, i, j);
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer) const
	{
		ConvertSampleRate(buffer, 0);
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, size_t outFrameCount) const
	{
		if (buffer.formatInfo.sampleRate == targetFormat.sampleRate) { return; }
		const double srRatio = (double)targetFormat.sampleRate / (double)buffer.formatInfo.sampleRate;
		const size_t currentFrameCount = buffer.frameCount;
		size_t targetFrameCount = outFrameCount;
		if (targetFrameCount == 0)
		{
			targetFrameCount = ceil((double)currentFrameCount * srRatio);
		}
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, buffer.formatInfo.bitsPerSample, targetFormat.sampleRate);
		AudioBuffer resultBuffer(targetFrameCount, resultFormat);
		const double cursorRatio = (1.0 / (targetFrameCount - 1)) * (currentFrameCount - 1);
		double cursor = 0.0;
		double A = 0.0;
		double B = 0.0;
		for (size_t i = 0; i < targetFrameCount; i++)
		{
			const double fc = floor(cursor);
			const double cursorFactor = cursor - fc;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				A = buffer.Get(fc, j);
				if (fc + 1 < currentFrameCount)
				{
					B = buffer.Get(fc + 1, j);
				}
				else
				{
					B = buffer.Get(currentFrameCount - 1, j);
				}
				resultBuffer.Set(A * (1.0 - cursorFactor) + B * cursorFactor, i, j);
			}
			cursor += cursorRatio;
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::Mix(AudioBuffer& outputBuffer, std::vector<AudioBuffer> inputBuffers) const
	{
		size_t outputBufferFrameCount = 0;
		for (int i = 0; i < inputBuffers.size(); i++)
		{
			AudioBuffer& buffer = inputBuffers.at(i);
			if (buffer.frameCount == 0)
			{
				inputBuffers.erase(inputBuffers.begin() + i);
				i--;
				continue;
			}
			this->ConvertSampleRate(buffer);
			this->ConvertBPS(buffer);
			this->ConvertChannels(buffer);
			if (buffer.frameCount > outputBufferFrameCount)
			{
				outputBufferFrameCount = buffer.frameCount;
			}
		}
		outputBuffer = AudioBuffer(outputBufferFrameCount, targetFormat);
		for (size_t i = 0; i < inputBuffers.size(); i++)
		{
			AudioBuffer& buffer = inputBuffers.at(i);
			for (size_t j = 0; j < outputBuffer.frameCount; j++)
			{
				if (j >= buffer.frameCount) { break; }
				for (size_t k = 0; k < outputBuffer.formatInfo.channelCount; k++)
				{
					const double outputSample = outputBuffer.Get(j, k);
					double inputSample = buffer.Get(j, k);
					outputBuffer.Set(outputSample + inputSample / (double)inputBuffers.size(), j, k);
				}
			}
		}
	}
	void AudioProcessor::Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers)
	{
		size_t outputBufferFrameCount = 0;
		AudioProcessor audioProcessor = AudioProcessor(outputFormat);
		for (int i = 0; i < inputBuffers.size(); i++)
		{
			AudioBuffer& buffer = inputBuffers.at(i);
			if (buffer.frameCount == 0)
			{
				inputBuffers.erase(inputBuffers.begin() + i);
				i--;
				continue;
			}
			audioProcessor.ConvertSampleRate(buffer);
			audioProcessor.ConvertBPS(buffer);
			audioProcessor.ConvertChannels(buffer);
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
					const double outputSample = outputBuffer.Get(j, k);
					double inputSample = buffer.Get(j, k);
					outputBuffer.Set(outputSample + inputSample / (double)inputBuffers.size(), j, k);
				}
			}
		}
	}
	std::vector<AudioBuffer> AudioProcessor::SplitChannels(const AudioBuffer& buffer)
	{
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, 1, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate);
		std::vector<AudioBuffer> channels(buffer.formatInfo.channelCount, AudioBuffer(buffer.frameCount, resultFormat));
		if (buffer.formatInfo.channelCount == 1)
		{
			channels.at(0) = buffer;
			return channels;
		}
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				channels.at(j).Set(buffer.Get(i, j), i, 0);
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
				throw AudioException(E_FAIL, L"AudioProcessor::MergeChannels", L"All channels must have the same wave format.");
			}
		}
		AudioBuffer resultBuffer(channels.at(0).frameCount, AudioFormatInfo(channels.at(0).formatInfo.formatTag, channels.size(), channels.at(0).formatInfo.bitsPerSample, channels.at(0).formatInfo.sampleRate));
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				resultBuffer.Set(channels.at(j).Get(i, 0), i, j);
			}
		}
		return resultBuffer;
	}
#pragma endregion
#pragma region Sound Effects
	void AudioProcessor::Reverse(AudioBuffer& buffer)
	{
		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				resultBuffer.Set(buffer.Get(buffer.frameCount - i - 1, j), i, j);
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::Echo(AudioBuffer& buffer, EchoInfo info)
	{
		if (info.reflectionCount == 0 || info.volumeFactor == 0.0 || info.echoStartPosition < 0 || info.echoStartPosition >= 1.0 || info.reflectionDelay < 0) { return; }
		struct EchoKeyPoints
		{
			size_t startFrameIndex;
			size_t endFrameIndex;
		};
		const size_t delayFrameCount = buffer.formatInfo.sampleRate * info.reflectionDelay;
		const size_t echoStartFrame = buffer.frameCount * info.echoStartPosition;
		const double echoEndPosition = info.echoEndPosition > info.echoStartPosition ? info.echoEndPosition : 1.0;
		const AudioBuffer echoBuffer = buffer.GetSubBuffer(echoStartFrame, buffer.frameCount * echoEndPosition - echoStartFrame);
		size_t resultBufferFrameCount = buffer.frameCount;
		std::vector<EchoKeyPoints> keyPoints(info.reflectionCount + 1);
		for (size_t i = 0; i < keyPoints.size(); i++) // Find echo key points.
		{
			keyPoints.at(i).startFrameIndex = echoStartFrame + delayFrameCount * (i + 1);
			keyPoints.at(i).endFrameIndex = keyPoints.at(i).startFrameIndex + echoBuffer.frameCount - 1;
			if (keyPoints.at(i).endFrameIndex >= resultBufferFrameCount)
			{
				resultBufferFrameCount = keyPoints.at(i).endFrameIndex + 1;
			}
		}
		AudioBuffer resultBuffer(resultBufferFrameCount, buffer.formatInfo);
		memcpy(resultBuffer.pAudioData, buffer.pAudioData, buffer.Size());
		for (size_t i = keyPoints.at(0).startFrameIndex; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				for (size_t k = 0; k < keyPoints.size(); k++)
				{
					if (i >= keyPoints.at(k).startFrameIndex && i <= keyPoints.at(k).endFrameIndex)
					{
						resultBuffer.Set(resultBuffer.Get(i, j) + echoBuffer.Get(i - keyPoints.at(k).startFrameIndex, j) * pow(info.volumeFactor, k + 1.0), i, j);
					}
				}
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info)
	{
		if (subBuffer.frameCount == 0 || info.reflectionCount == 0 || info.volumeFactor == 0.0 || info.echoStartPosition < 0 || info.echoStartPosition >= 1.0 || info.reflectionDelay < 0) { return; }
		struct EchoKeyPoints
		{
			size_t startFrameIndex;
			size_t endFrameIndex;
			size_t n;
		};
		const size_t delayFrameCount = originalBuffer.formatInfo.sampleRate * info.reflectionDelay;
		const size_t echoStartFrame = originalBuffer.frameCount * info.echoStartPosition;
		const double echoEndPosition = info.echoEndPosition > info.echoStartPosition ? info.echoEndPosition : 1.0;
		const size_t echoFrameCount = originalBuffer.frameCount * echoEndPosition - echoStartFrame;
		std::vector<EchoKeyPoints> keyPoints(info.reflectionCount);
		size_t erasedKeyPointsCount = 0;
		for (int i = 0; i < keyPoints.size(); i++) // Find echo key points.
		{
			keyPoints.at(i).startFrameIndex = echoStartFrame + delayFrameCount * (i + erasedKeyPointsCount + 1);
			keyPoints.at(i).endFrameIndex = keyPoints.at(i).startFrameIndex + echoFrameCount - 1;
			keyPoints.at(i).n = i + erasedKeyPointsCount + 1;
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
							subBuffer.Set(subBuffer.Get(i, j) + echoBuffer.Get(cursor - keyPoints.at(k).startFrameIndex, j) * pow(info.volumeFactor, keyPoints.at(k).n), i, j);
						}
					}
				}
				cursor++;
			}
		}
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, double f1, double f2, double volume)
	{
		const size_t fftSize = Fourier::CalculateFFTSize(buffer.frameCount);
		const size_t nyquistFrequency = fftSize * 0.5;
		uint64_t lowerFrequencyIndex, higherFrequencyIndex;
		if (f1 > f2)
		{
			higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, f1));
			lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, f2));
		}
		else
		{
			higherFrequencyIndex = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, f2));
			lowerFrequencyIndex = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, f1));
		}
		const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = lowerFrequencyIndex; j <= upperBound; j++)
			{
				fourier.complexBuffer.at(j) *= volume;
				fourier.complexBuffer.at(fftSize - j - 1).real = fourier.complexBuffer.at(j).real;
				fourier.complexBuffer.at(fftSize - j - 1).imaginary = -fourier.complexBuffer.at(j).imaginary;
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
#pragma endregion
#pragma region Filters
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength)
	{
		const size_t fftSize = Fourier::CalculateFFTSize(buffer.frameCount);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t passBand = cutoffFreq > transitionBandLength ? ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq - transitionBandLength)) : ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t stopBand = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t transitionLength = stopBand - passBand;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = passBand; j < nyquistFrequency; j++)
			{
				if (j >= stopBand)
				{
					fourier.complexBuffer.at(j) = Complex();
					fourier.complexBuffer.at(fftSize - j - 1) = Complex();
				}
				else if (j > passBand && j < stopBand) // Transition band.
				{
					fourier.complexBuffer.at(j) *= (double)(stopBand - j) / (double)transitionLength;
					fourier.complexBuffer.at(fftSize - j - 1).real = fourier.complexBuffer.at(j).real;
					fourier.complexBuffer.at(fftSize - j - 1).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength)
	{
		const size_t fftSize = Fourier::CalculateFFTSize(buffer.frameCount);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t stopBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t passBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq + transitionBandLength));
		const uint64_t transitionLength = passBand - stopBand;
		const size_t upperBound = passBand < nyquistFrequency ? passBand : nyquistFrequency;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = 0; j < upperBound; j++)
			{
				if (j <= stopBand)
				{
					fourier.complexBuffer.at(j) = Complex();
					fourier.complexBuffer.at(fftSize - j - 1) = Complex();
				}
				else if (j > stopBand && j < passBand) // Transition band.
				{
					fourier.complexBuffer.at(j) *= (double)(j - stopBand) / (double)transitionLength;
					fourier.complexBuffer.at(fftSize - j - 1).real = fourier.complexBuffer.at(j).real;
					fourier.complexBuffer.at(fftSize - j - 1).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength)
	{
		if (lowCutoffFreq > highCutoffFreq)
		{
			const double temp = highCutoffFreq;
			highCutoffFreq = lowCutoffFreq;
			lowCutoffFreq = temp;
		}
		const size_t fftSize = Fourier::CalculateFFTSize(buffer.frameCount);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t lowStopBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq));
		const uint64_t lowPassBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq + transitionBandLength));
		const uint64_t highPassBand = highCutoffFreq > transitionBandLength ? ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq - transitionBandLength)) : ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t highStopBand = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t transitionLength = highStopBand - highPassBand;
		const size_t upperBound = lowPassBand < nyquistFrequency ? lowPassBand : nyquistFrequency;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = 0; j < upperBound; j++)
			{
				if (j > lowStopBand)
				{
					fourier.complexBuffer.at(j) *= (double)(j - lowStopBand) / (double)transitionLength;
					fourier.complexBuffer.at(fftSize - j - 1).real = fourier.complexBuffer.at(j).real;
					fourier.complexBuffer.at(fftSize - j - 1).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
				else
				{
					fourier.complexBuffer.at(j) = Complex();
					fourier.complexBuffer.at(fftSize - j - 1) = Complex();
				}
			}
			for (size_t j = highPassBand; j < nyquistFrequency; j++)
			{
				if (j < highStopBand)
				{
					fourier.complexBuffer.at(j) *= (double)(highStopBand - j) / (double)transitionLength;
					fourier.complexBuffer.at(fftSize - j - 1).real = fourier.complexBuffer.at(j).real;
					fourier.complexBuffer.at(fftSize - j - 1).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
				else
				{
					fourier.complexBuffer.at(j) = Complex();
					fourier.complexBuffer.at(fftSize - j - 1) = Complex();
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength)
	{
		if (lowCutoffFreq > highCutoffFreq)
		{
			const double temp = highCutoffFreq;
			highCutoffFreq = lowCutoffFreq;
			lowCutoffFreq = temp;
		}
		const size_t fftSize = Fourier::CalculateFFTSize(buffer.frameCount);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t lowStopBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq));
		const uint64_t lowPassBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq + transitionBandLength));
		const uint64_t highPassBand = highCutoffFreq > transitionBandLength ? ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq - transitionBandLength)) : ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t highStopBand = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t transitionLength = highStopBand - highPassBand;
		const size_t upperBound = highStopBand < nyquistFrequency ? highStopBand : nyquistFrequency;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = lowStopBand; j < upperBound; j++)
			{
				if (j >= lowPassBand && j <= highPassBand)
				{
					fourier.complexBuffer.at(j) = Complex();
					fourier.complexBuffer.at(fftSize - j - 1) = Complex();
				}
				else if (j > lowStopBand && j < lowPassBand)
				{
					fourier.complexBuffer.at(j) *= (double)(transitionLength - (j - lowStopBand)) / (double)transitionLength;
					fourier.complexBuffer.at(fftSize - j - 1).real = fourier.complexBuffer.at(j).real;
					fourier.complexBuffer.at(fftSize - j - 1).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
				else if (j > highPassBand && j < highStopBand)
				{
					fourier.complexBuffer.at(j) *= (double)(transitionLength - (highStopBand - j)) / (double)transitionLength;
					fourier.complexBuffer.at(fftSize - j - 1).real = fourier.complexBuffer.at(j).real;
					fourier.complexBuffer.at(fftSize - j - 1).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
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
				buffer.Set(buffer.Get(i, j) * factor, i, j);
			}
		}
	}
#pragma endregion
}