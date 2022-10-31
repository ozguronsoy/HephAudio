#include "AudioProcessor.h"
#include "AudioException.h"
#include "Fourier.h"

namespace HephAudio
{
#pragma region Converts, Mix, Split/Merge Channels
	void AudioProcessor::ConvertBPS(AudioBuffer& buffer, AudioFormatInfo outputFormat)
	{
		if (buffer.formatInfo.bitsPerSample == outputFormat.bitsPerSample) { return; }
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, outputFormat.bitsPerSample, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (uint8_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				double sample = buffer.Get(i, j);
				if (outputFormat.bitsPerSample == 8)
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
	void AudioProcessor::ConvertChannels(AudioBuffer& buffer, AudioFormatInfo outputFormat)
	{
		if (buffer.formatInfo.channelCount == outputFormat.channelCount) { return; }
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, outputFormat.channelCount, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate);
		AudioBuffer resultBuffer(buffer.frameCount, resultFormat);
		for (size_t i = 0; i < buffer.frameCount; i++) // For each frame, find the average value and then set all the result channels to it.
		{
			double averageValue = 0.0f;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				averageValue += buffer.Get(i, j);
			}
			averageValue /= buffer.formatInfo.channelCount;
			for (size_t j = 0; j < outputFormat.channelCount; j++)
			{
				resultBuffer.Set(averageValue, i, j);
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, AudioFormatInfo outputFormat)
	{
		ConvertSampleRate(buffer, outputFormat, 0u);
	}
	void AudioProcessor::ConvertSampleRate(AudioBuffer& buffer, AudioFormatInfo outputFormat, size_t outFrameCount)
	{
		if (buffer.formatInfo.sampleRate == outputFormat.sampleRate) { return; }
		const double srRatio = (double)outputFormat.sampleRate / (double)buffer.formatInfo.sampleRate;
		const size_t currentFrameCount = buffer.frameCount;
		size_t targetFrameCount = outFrameCount;
		if (targetFrameCount == 0)
		{
			targetFrameCount = ceil((double)currentFrameCount * srRatio);
		}
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, buffer.formatInfo.bitsPerSample, outputFormat.sampleRate);
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
	void AudioProcessor::Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers)
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
		switch (buffer.formatInfo.bitsPerSample)
		{
		case 8:
			std::reverse((int8_t*)buffer.pAudioData, (int8_t*)buffer.pAudioData + buffer.Size());
			break;
		case 16:
			std::reverse((int16_t*)buffer.pAudioData, (int16_t*)buffer.pAudioData + buffer.Size() / 2);
			break;
		case 24:
			std::reverse((int24*)buffer.pAudioData, (int24*)buffer.pAudioData + buffer.Size() / 3);
			break;
		case 32:
			std::reverse((int32_t*)buffer.pAudioData, (int32_t*)buffer.pAudioData + buffer.Size() / 4);
			break;
		default:
			return;
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
						buffer.Set(buffer.Get(i, j) + echoBuffer.Get(i - keyPoints.at(k).startFrameIndex, j) * keyPoints.at(k).factor, i, j);
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
		for (int i = 0; i < keyPoints.size(); i++) // Find echo key points.
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
							subBuffer.Set(subBuffer.Get(i, j) + echoBuffer.Get(cursor - keyPoints.at(k).startFrameIndex, j) * keyPoints.at(k).factor, i, j);
						}
					}
				}
				cursor++;
			}
		}
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, double f1, double f2, double volume)
	{
		Equalizer(buffer, defaultHopSize, defaultFFTSize, f1, f2, volume);
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double f1, double f2, double volume)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
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
		const size_t frameCount = buffer.frameCount;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.~AudioBuffer();
		std::vector<AudioBuffer> filteredChannels = std::vector<AudioBuffer>(channels.size(), AudioBuffer(frameCount, channels.at(0).formatInfo));
		for (size_t i = 0; i < frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				AudioBuffer subBuffer = channels.at(j).GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(subBuffer, fftSize);
				for (size_t k = lowerFrequencyIndex; k <= upperBound; k++)
				{
					complexBuffer.at(k) *= volume;
					complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
					complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
				}
				Fourier::FFT_Inverse(subBuffer, complexBuffer);
				HannWindow(subBuffer);
				for (size_t k = 0; k < fftSize && (i + k) < frameCount; k++)
				{
					filteredChannels.at(j).Set(filteredChannels.at(j).Get(i + k, 0) + subBuffer.Get(k, 0), i + k, 0);
				}
			}
		}
		channels.clear();
		buffer = MergeChannels(filteredChannels);
	}
#pragma endregion
#pragma region Filters
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength)
	{
		LowPassFilter(buffer, defaultHopSize, defaultFFTSize, cutoffFreq, transitionBandLength);
	}
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t passBand = cutoffFreq > transitionBandLength ? ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq - transitionBandLength)) : ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t stopBand = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t transitionLength = stopBand - passBand;
		const size_t frameCount = buffer.frameCount;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.~AudioBuffer();
		std::vector<AudioBuffer> filteredChannels = std::vector<AudioBuffer>(channels.size(), AudioBuffer(frameCount, channels.at(0).formatInfo));
		for (size_t i = 0; i < frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				AudioBuffer subBuffer = channels.at(j).GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(subBuffer, fftSize);
				for (size_t k = passBand; k < nyquistFrequency; k++)
				{
					if (k >= stopBand)
					{
						complexBuffer.at(k) = Complex();
						complexBuffer.at(fftSize - k - 1) = Complex();
					}
					else if (k > passBand && k < stopBand) // Transition band.
					{
						complexBuffer.at(k) *= (double)(stopBand - k) / (double)transitionLength;
						complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
						complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
					}
				}
				Fourier::FFT_Inverse(subBuffer, complexBuffer);
				HannWindow(subBuffer);
				for (size_t k = 0; k < fftSize && (i + k) < frameCount; k++)
				{
					filteredChannels.at(j).Set(filteredChannels.at(j).Get(i + k, 0) + subBuffer.Get(k, 0), i + k, 0);
				}
			}
		}
		channels.clear();
		buffer = MergeChannels(filteredChannels);
	}
	void AudioProcessor::LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength)
	{
		static std::vector<FilteredBuffer*> filteredBuffers;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t passBand = cutoffFreq > transitionBandLength ? ceil(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq - transitionBandLength)) : ceil(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t stopBand = ceil(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t transitionLength = stopBand - passBand;
		const size_t frameCount = subBuffer.frameCount;
		const auto applyFilter = [&hopSize, &fftSize, &nyquistFrequency, &passBand, &stopBand, &transitionLength, &frameCount](AudioBuffer& subBuffer) -> void
		{
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < frameCount; i += hopSize)
			{
				for (size_t j = 0; j < channels.size(); j++)
				{
					AudioBuffer& channel = channels.at(j);
					ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel, fftSize);
					for (size_t k = passBand; k < nyquistFrequency; k++)
					{
						if (k >= stopBand)
						{
							complexBuffer.at(k) = Complex();
							complexBuffer.at(fftSize - k - 1) = Complex();
						}
						else if (k > passBand && k < stopBand) // Transition band.
						{
							complexBuffer.at(k) *= (double)(stopBand - k) / (double)transitionLength;
							complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
							complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
						}
					}
					Fourier::FFT_Inverse(channel, complexBuffer);
					HannWindow(channel);
				}
			}
			subBuffer = MergeChannels(channels);
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
		RemoveOldFilteredBuffers(filteredBuffers, &originalBuffer, fStart);
		size_t isb, itb;
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			FilteredBuffer* filteredBuffer = GetFilteredBuffer(filteredBuffers, &originalBuffer, i);
			if (filteredBuffer == nullptr)
			{
				filteredBuffer = new FilteredBuffer();
				filteredBuffer->fStart = i;
				filteredBuffer->audioBuffer = originalBuffer.GetSubBuffer(i, fftSize);
				filteredBuffer->pOriginalBuffer = &originalBuffer;
				applyFilter(filteredBuffer->audioBuffer);
				filteredBuffers.push_back(filteredBuffer);
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
			for (size_t j = 0; isb < frameCount && itb < fftSize; j++, isb++, itb++)
			{
				for (size_t k = 0; k < subBuffer.formatInfo.channelCount; k++)
				{
					subBuffer.Set(subBuffer.Get(isb, k) + filteredBuffer->audioBuffer.Get(itb, k), isb, k);
				}
			}
		}
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength)
	{
		HighPassFilter(buffer, defaultHopSize, defaultFFTSize, cutoffFreq, transitionBandLength);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t stopBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t passBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq + transitionBandLength));
		const uint64_t transitionLength = passBand - stopBand;
		const size_t upperBound = passBand < nyquistFrequency ? passBand : nyquistFrequency;
		const size_t frameCount = buffer.frameCount;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.~AudioBuffer();
		std::vector<AudioBuffer> filteredChannels = std::vector<AudioBuffer>(channels.size(), AudioBuffer(frameCount, channels.at(0).formatInfo));
		for (size_t i = 0; i < frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				AudioBuffer subBuffer = channels.at(j).GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(subBuffer, fftSize);
				for (size_t k = 0; k < upperBound; k++)
				{
					if (k <= stopBand)
					{
						complexBuffer.at(k) = Complex();
						complexBuffer.at(fftSize - k - 1) = Complex();
					}
					else if (k > stopBand && k < passBand) // Transition band.
					{
						complexBuffer.at(k) *= (double)(k - stopBand) / (double)transitionLength;
						complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
						complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
					}
				}
				Fourier::FFT_Inverse(subBuffer, complexBuffer);
				HannWindow(subBuffer);
				for (size_t k = 0; k < fftSize && (i + k) < frameCount; k++)
				{
					filteredChannels.at(j).Set(filteredChannels.at(j).Get(i + k, 0) + subBuffer.Get(k, 0), i + k, 0);
				}
			}
		}
		channels.clear();
		buffer = MergeChannels(filteredChannels);
	}
	void AudioProcessor::HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength)
	{
		static std::vector<FilteredBuffer*> filteredBuffers;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t stopBand = floor(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq));
		const uint64_t passBand = floor(Fourier::FrequencyToIndex(subBuffer.formatInfo.sampleRate, fftSize, cutoffFreq + transitionBandLength));
		const uint64_t transitionLength = passBand - stopBand;
		const size_t upperBound = passBand < nyquistFrequency ? passBand : nyquistFrequency;
		const size_t frameCount = subBuffer.frameCount;
		const auto applyFilter = [&hopSize, &fftSize, &nyquistFrequency, &passBand, &stopBand, &transitionLength, &upperBound, &frameCount](AudioBuffer& subBuffer) -> void
		{
			std::vector<AudioBuffer> channels = SplitChannels(subBuffer);
			for (size_t i = 0; i < frameCount; i += hopSize)
			{
				for (size_t j = 0; j < channels.size(); j++)
				{
					AudioBuffer& channel = channels.at(j);
					ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel, fftSize);
					for (size_t k = 0; k < upperBound; k++)
					{
						if (k <= stopBand)
						{
							complexBuffer.at(k) = Complex();
							complexBuffer.at(fftSize - k - 1) = Complex();
						}
						else if (k > stopBand && k < passBand) // Transition band.
						{
							complexBuffer.at(k) *= (double)(k - stopBand) / (double)transitionLength;
							complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
							complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
						}
					}
					Fourier::FFT_Inverse(channel, complexBuffer);
					HannWindow(channel);
				}
			}
			subBuffer = MergeChannels(channels);
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
		RemoveOldFilteredBuffers(filteredBuffers, &originalBuffer, fStart);
		AudioBuffer tempBuffer;
		size_t isb, itb;
		for (size_t i = fStart; i < fEnd; i += hopSize)
		{
			FilteredBuffer* filteredBuffer = GetFilteredBuffer(filteredBuffers, &originalBuffer, i);
			if (filteredBuffer == nullptr)
			{
				filteredBuffer = new FilteredBuffer();
				filteredBuffer->fStart = i;
				filteredBuffer->audioBuffer = originalBuffer.GetSubBuffer(i, fftSize);
				filteredBuffer->pOriginalBuffer = &originalBuffer;
				applyFilter(filteredBuffer->audioBuffer);
				filteredBuffers.push_back(filteredBuffer);
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
			for (size_t j = 0; isb < frameCount && itb < fftSize; j++, isb++, itb++)
			{
				for (size_t k = 0; k < subBuffer.formatInfo.channelCount; k++)
				{
					subBuffer.Set(subBuffer.Get(isb, k) + filteredBuffer->audioBuffer.Get(itb, k), isb, k);
				}
			}
		}
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength)
	{
		BandPassFilter(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq, transitionBandLength);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t lowStopBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq));
		const uint64_t lowPassBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq + transitionBandLength));
		const uint64_t highPassBand = highCutoffFreq > transitionBandLength ? ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq - transitionBandLength)) : ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t highStopBand = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t transitionLength = highStopBand - highPassBand;
		const size_t upperBound = lowPassBand < nyquistFrequency ? lowPassBand : nyquistFrequency;
		const size_t frameCount = buffer.frameCount;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.~AudioBuffer();
		std::vector<AudioBuffer> filteredChannels = std::vector<AudioBuffer>(channels.size(), AudioBuffer(frameCount, channels.at(0).formatInfo));
		for (size_t i = 0; i < frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				AudioBuffer subBuffer = channels.at(j).GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(subBuffer, fftSize);
				for (size_t k = 0; k < upperBound; k++)
				{
					if (k > lowStopBand)
					{
						complexBuffer.at(k) *= (double)(k - lowStopBand) / (double)transitionLength;
						complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
						complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
					}
					else
					{
						complexBuffer.at(j) = Complex();
						complexBuffer.at(fftSize - k - 1) = Complex();
					}
				}
				for (size_t k = highPassBand; k < nyquistFrequency; k++)
				{
					if (k < highStopBand)
					{
						complexBuffer.at(k) *= (double)(highStopBand - k) / (double)transitionLength;
						complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
						complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
					}
					else
					{
						complexBuffer.at(k) = Complex();
						complexBuffer.at(fftSize - k - 1) = Complex();
					}
				}
				Fourier::FFT_Inverse(subBuffer, complexBuffer);
				HannWindow(subBuffer);
				for (size_t k = 0; k < fftSize && (i + k) < frameCount; k++)
				{
					filteredChannels.at(j).Set(filteredChannels.at(j).Get(i + k, 0) + subBuffer.Get(k, 0), i + k, 0);
				}
			}
		}
		channels.clear();
		buffer = MergeChannels(filteredChannels);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength)
	{
		BandCutFilter(buffer, defaultHopSize, defaultFFTSize, lowCutoffFreq, highCutoffFreq, transitionBandLength);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t lowStopBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq));
		const uint64_t lowPassBand = floor(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq + transitionBandLength));
		const uint64_t highPassBand = highCutoffFreq > transitionBandLength ? ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq - transitionBandLength)) : ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t highStopBand = ceil(Fourier::FrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq));
		const uint64_t transitionLength = highStopBand - highPassBand;
		const size_t upperBound = highStopBand < nyquistFrequency ? highStopBand : nyquistFrequency;
		const size_t frameCount = buffer.frameCount;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		buffer.~AudioBuffer();
		std::vector<AudioBuffer> filteredChannels = std::vector<AudioBuffer>(channels.size(), AudioBuffer(frameCount, channels.at(0).formatInfo));
		for (size_t i = 0; i < frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				AudioBuffer subBuffer = channels.at(j).GetSubBuffer(i, fftSize);
				ComplexBuffer complexBuffer = Fourier::FFT_Forward(subBuffer, fftSize);
				for (size_t k = lowStopBand; k < upperBound; k++)
				{
					if (k >= lowPassBand && k <= highPassBand)
					{
						complexBuffer.at(k) = Complex();
						complexBuffer.at(fftSize - k - 1) = Complex();
					}
					else if (k > lowStopBand && k < lowPassBand)
					{
						complexBuffer.at(k) *= (double)(transitionLength - (k - lowStopBand)) / (double)transitionLength;
						complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
						complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
					}
					else if (k > highPassBand && k < highStopBand)
					{
						complexBuffer.at(k) *= (double)(transitionLength - (highStopBand - k)) / (double)transitionLength;
						complexBuffer.at(fftSize - k - 1).real = complexBuffer.at(k).real;
						complexBuffer.at(fftSize - k - 1).imaginary = -complexBuffer.at(k).imaginary;
					}
				}
				Fourier::FFT_Inverse(subBuffer, complexBuffer);
				HannWindow(subBuffer);
				for (size_t k = 0; k < fftSize && (i + k) < frameCount; k++)
				{
					filteredChannels.at(j).Set(filteredChannels.at(j).Get(i + k, 0) + subBuffer.Get(k, 0), i + k, 0);
				}
			}
		}
		channels.clear();
		buffer = MergeChannels(filteredChannels);
	}
	AudioProcessor::FilteredBuffer* AudioProcessor::GetFilteredBuffer(std::vector<FilteredBuffer*>& filteredBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart)
	{
		for (size_t i = 0; i < filteredBuffers.size(); i++)
		{
			if (fStart == filteredBuffers.at(i)->fStart && pOriginalBuffer == filteredBuffers.at(i)->pOriginalBuffer)
			{
				return filteredBuffers.at(i);
			}
		}
		return nullptr;
	}
	void AudioProcessor::RemoveOldFilteredBuffers(std::vector<FilteredBuffer*>& filteredBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart)
	{
		for (int64_t i = 0; i < filteredBuffers.size(); i++)
		{
			if (filteredBuffers.at(i)->fStart < fStart && pOriginalBuffer == filteredBuffers.at(i)->pOriginalBuffer)
			{
				delete filteredBuffers.at(i);
				filteredBuffers.erase(filteredBuffers.begin() + i);
				i--;
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
#pragma region Maximize Volume
	double AudioProcessor::FindMaxVolume(const AudioBuffer& buffer)
	{
		const size_t channelCount = buffer.GetFormat().channelCount;
		double maxSample = 0.0;
		double currentSample = 0.0;
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < channelCount; j++)
			{
				currentSample = abs(buffer.Get(i, j));
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
		const size_t channelCount = buffer.GetFormat().channelCount;
		double maxSample = 0.0;
		double currentSample = 0.0;
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < channelCount; j++)
			{
				currentSample = abs(buffer.Get(i, j));
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
}