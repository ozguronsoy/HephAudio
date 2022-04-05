#include "AudioProcessor.h"
#include "AudioException.h"
#include "Fourier.h"

using namespace HephAudio::Structs;

namespace HephAudio
{
	AudioProcessor::AudioProcessor(AudioFormatInfo targetFormat)
	{
		this->targetFormat = targetFormat;
	}
	void AudioProcessor::ConvertBPS(AudioBuffer& buffer) const
	{
		if (buffer.wfx.wBitsPerSample == targetFormat.wBitsPerSample) { return; }
		const size_t frameCount = buffer.FrameCount();
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.wfx.wFormatTag, buffer.wfx.nChannels, targetFormat.wBitsPerSample, buffer.wfx.nSamplesPerSec);
		AudioBuffer resultBuffer(frameCount, resultFormat);
		for (size_t i = 0; i < frameCount; i++)
		{
			for (uint8_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				double sample = buffer.Get(i, j);
				if (targetFormat.wBitsPerSample == 8)
				{
					sample += 1.0;
					sample /= 2.0;
				}
				else if (buffer.wfx.wBitsPerSample == 8)
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
		if (buffer.wfx.nChannels == targetFormat.nChannels) { return; }
		const size_t frameCount = buffer.FrameCount();
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.wfx.wFormatTag, targetFormat.nChannels, buffer.wfx.wBitsPerSample, buffer.wfx.nSamplesPerSec);
		AudioBuffer resultBuffer(frameCount, resultFormat);
		for (size_t i = 0; i < frameCount; i++) // For each frame, find the average value and then set all the result channels to it.
		{
			double averageValue = 0.0f;
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				averageValue += buffer.Get(i, j);
			}
			averageValue /= buffer.wfx.nChannels;
			for (size_t j = 0; j < targetFormat.nChannels; j++)
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
		if (buffer.wfx.nSamplesPerSec == targetFormat.nSamplesPerSec) { return; }
		const double srRatio = (double)targetFormat.nSamplesPerSec / (double)buffer.wfx.nSamplesPerSec;
		const size_t currentFrameCount = buffer.FrameCount();
		size_t targetFrameCount = outFrameCount;
		if (targetFrameCount == 0)
		{
			targetFrameCount = ceil((double)currentFrameCount * srRatio);
		}
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.wfx.wFormatTag, buffer.wfx.nChannels, buffer.wfx.wBitsPerSample, targetFormat.nSamplesPerSec);
		AudioBuffer resultBuffer(targetFrameCount, resultFormat);
		const double cursorRatio = (1.0 / (targetFrameCount - 1)) * (currentFrameCount - 1);
		double cursor = 0.0;
		double A = 0.0;
		double B = 0.0;
		for (size_t i = 0; i < targetFrameCount; i++)
		{
			const double fc = floor(cursor);
			const double cursorFactor = cursor - fc;
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
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
			if (buffer.FrameCount() == 0)
			{
				inputBuffers.erase(inputBuffers.begin() + i);
				i--;
				continue;
			}
			this->ConvertSampleRate(buffer);
			this->ConvertBPS(buffer);
			this->ConvertChannels(buffer);
			if (buffer.FrameCount() > outputBufferFrameCount)
			{
				outputBufferFrameCount = buffer.FrameCount();
			}
		}
		outputBuffer = AudioBuffer(outputBufferFrameCount, targetFormat);
		for (size_t i = 0; i < inputBuffers.size(); i++)
		{
			AudioBuffer& buffer = inputBuffers.at(i);
			for (size_t j = 0; j < outputBuffer.FrameCount(); j++)
			{
				if (j >= buffer.FrameCount()) { break; }
				for (size_t k = 0; k < outputBuffer.wfx.nChannels; k++)
				{
					const double outputSample = outputBuffer.Get(j, k);
					double inputSample = buffer.Get(j, k);
					outputBuffer.Set(outputSample + inputSample / (double)inputBuffers.size(), j, k);
				}
			}
		}
	}
	void AudioProcessor::Reverse(AudioBuffer& buffer)
	{
		const size_t frameCount = buffer.FrameCount();
		AudioBuffer resultBuffer(frameCount, buffer.wfx);
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
			{
				resultBuffer.Set(buffer.Get(frameCount - 1 - i, j), i, j);
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
		const size_t delayFrameCount = buffer.wfx.nSamplesPerSec * info.reflectionDelay;
		const size_t echoStartFrame = buffer.FrameCount() * info.echoStartPosition;
		const double echoEndPosition = info.echoEndPosition > info.echoStartPosition ? info.echoEndPosition : 1.0;
		const AudioBuffer echoBuffer = buffer.GetSubBuffer(echoStartFrame, buffer.FrameCount() * echoEndPosition - echoStartFrame);
		size_t resultBufferFrameCount = buffer.FrameCount();
		std::vector<EchoKeyPoints> keyPoints(info.reflectionCount + 1);
		keyPoints.at(0).startFrameIndex = 0; // Original data key points.
		keyPoints.at(0).endFrameIndex = buffer.FrameCount() - 1;
		for (size_t i = 1; i < keyPoints.size(); i++) // Find echo key points.
		{
			keyPoints.at(i).startFrameIndex = echoStartFrame + delayFrameCount * i;
			keyPoints.at(i).endFrameIndex = keyPoints.at(i).startFrameIndex + echoBuffer.FrameCount() - 1;
			if (keyPoints.at(i).endFrameIndex >= resultBufferFrameCount)
			{
				resultBufferFrameCount = keyPoints.at(i).endFrameIndex + 1;
			}
		}
		AudioBuffer resultBuffer(resultBufferFrameCount, buffer.wfx);
		memcpy(resultBuffer.GetInnerBufferAddress(), buffer.GetInnerBufferAddress(), buffer.Size());
		for (size_t i = keyPoints.at(1).startFrameIndex; i < resultBuffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < resultBuffer.wfx.nChannels; j++)
			{
				for (size_t k = 1; k < keyPoints.size(); k++)
				{
					if (i >= keyPoints.at(k).startFrameIndex && i <= keyPoints.at(k).endFrameIndex)
					{
						resultBuffer.Set(resultBuffer.Get(i, j) + echoBuffer.Get(i - keyPoints.at(k).startFrameIndex, j) * pow(info.volumeFactor, k), i, j);
					}
				}
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::EchoSubBuffer(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info, bool isReversed)
	{
		if (subBuffer.FrameCount() == 0 || info.reflectionCount == 0 || info.volumeFactor == 0.0 || info.echoStartPosition < 0 || info.echoStartPosition >= 1.0 || info.reflectionDelay < 0) { return; }
		struct EchoKeyPoints
		{
			size_t startFrameIndex;
			size_t endFrameIndex;
			size_t index;
		};
		const size_t delayFrameCount = originalBuffer.wfx.nSamplesPerSec * info.reflectionDelay;
		const size_t echoStartFrame = originalBuffer.FrameCount() * info.echoStartPosition;
		const double echoEndPosition = info.echoEndPosition > info.echoStartPosition ? info.echoEndPosition : 1.0;
		const AudioBuffer echoBuffer = originalBuffer.GetSubBuffer(echoStartFrame, originalBuffer.FrameCount() * echoEndPosition - echoStartFrame);
		std::vector<EchoKeyPoints> keyPoints(info.reflectionCount);
		size_t erasedKeyPointsCount = 0;
		for (int i = 0; i < keyPoints.size(); i++) // Find echo key points.
		{
			keyPoints.at(i).startFrameIndex = echoStartFrame + delayFrameCount * (i + 1 + erasedKeyPointsCount);
			keyPoints.at(i).endFrameIndex = keyPoints.at(i).startFrameIndex + echoBuffer.FrameCount() - 1;
			if (isReversed)
			{
				keyPoints.at(i).index = info.reflectionCount - (i + erasedKeyPointsCount + 1);
			}
			else
			{
				keyPoints.at(i).index = i + erasedKeyPointsCount + 1;
			}
			if (keyPoints.at(i).startFrameIndex > subBufferFrameIndex + subBuffer.FrameCount() || keyPoints.at(i).endFrameIndex < subBufferFrameIndex) // Remove keyPoints if not needed to reduce the total loop count.
			{
				keyPoints.erase(keyPoints.begin() + i);
				i--;
				erasedKeyPointsCount++;
			}
		}
		if (keyPoints.size() > 0)
		{
			size_t originalFrameIndex = subBufferFrameIndex;
			for (size_t i = 0; i < subBuffer.FrameCount(); i++)
			{
				for (size_t j = 0; j < subBuffer.wfx.nChannels; j++)
				{
					for (size_t k = 0; k < keyPoints.size(); k++)
					{
						if (originalFrameIndex >= keyPoints.at(k).startFrameIndex && originalFrameIndex <= keyPoints.at(k).endFrameIndex)
						{
							subBuffer.Set(subBuffer.Get(i, j) + echoBuffer.Get(originalFrameIndex - keyPoints.at(k).startFrameIndex, j) * pow(info.volumeFactor, keyPoints.at(k).index), i, j);
						}
					}
				}
				originalFrameIndex++;
			}
		}
	}
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, uint16_t cutoffFreq, uint16_t transitionBandLength)
	{
		size_t fftSize = buffer.FrameCount();
		if (!(fftSize > 0 && !(fftSize & (fftSize - 1)))) // if not power of 2
		{
			fftSize = pow(2, floor(log2f(fftSize)) + 1); // smallest power of 2 thats greater than nSample
		}
		const size_t nyquistFrequency = fftSize * 0.5;
		const double frequencyFactor = (double)cutoffFreq / 20000.0;
		const double transitionFactor = (double)transitionBandLength / 20000.0;
		const uint16_t passBand = frequencyFactor > transitionFactor ? nyquistFrequency * (frequencyFactor - transitionFactor) : 0;
		const uint16_t stopBand = nyquistFrequency * frequencyFactor;
		const uint16_t transitionLength = stopBand - passBand;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, 0);
			fourier.Forward();
			for (size_t j = passBand; j < fftSize; j++)
			{
				if (j <= nyquistFrequency)
				{
					if (j >= stopBand)
					{
						fourier.complexBuffer.at(j) = Complex();
					}
					else if (j > passBand && j < stopBand) // Transition band.
					{
						fourier.complexBuffer.at(j) *= (double)(stopBand - j) / (double)transitionLength;
					}
				}
				else
				{
					fourier.complexBuffer.at(j) = fourier.complexBuffer.at(fftSize - j);
					fourier.complexBuffer.at(j).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, uint16_t cutoffFreq, uint16_t transitionBandLength)
	{
		size_t fftSize = buffer.FrameCount();
		if (!(fftSize > 0 && !(fftSize & (fftSize - 1)))) // if not power of 2
		{
			fftSize = pow(2, floor(log2f(fftSize)) + 1); // smallest power of 2 thats greater than nSample
		}
		const size_t nyquistFrequency = fftSize * 0.5;
		const double frequencyFactor = (double)cutoffFreq / 20000.0;
		const double transitionFactor = (double)transitionBandLength / 20000.0;
		const uint16_t stopBand = nyquistFrequency * frequencyFactor;
		const uint16_t passBand = stopBand + nyquistFrequency * transitionFactor;
		const uint16_t transitionLength = passBand - stopBand;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = 0; j < fftSize; j++)
			{
				if (j <= nyquistFrequency)
				{
					if (j <= stopBand)
					{
						fourier.complexBuffer.at(j) = Complex();
					}
					else if (j > stopBand && j < passBand) // Transition band.
					{
						fourier.complexBuffer.at(j) *= (double)(j - stopBand) / (double)transitionLength;
					}
				}
				else
				{
					fourier.complexBuffer.at(j) = fourier.complexBuffer.at(fftSize - j);
					fourier.complexBuffer.at(j).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, uint16_t lowCutoffFreq, uint16_t highCutoffFreq, uint16_t transitionBandLength)
	{
		size_t fftSize = buffer.FrameCount();
		if (!(fftSize > 0 && !(fftSize & (fftSize - 1)))) // if not power of 2
		{
			fftSize = pow(2, floor(log2f(fftSize)) + 1); // smallest power of 2 thats greater than nSample
		}
		const size_t nyquistFrequency = fftSize * 0.5;
		const double lowFrequencyFactor = (double)lowCutoffFreq / 20000.0;
		const double highFrequencyFactor = (double)highCutoffFreq / 20000.0;
		const double transitionFactor = (double)transitionBandLength / 20000.0;
		const uint16_t lowStopBand = nyquistFrequency * lowFrequencyFactor;
		const uint16_t lowPassBand = lowStopBand + nyquistFrequency * transitionFactor;
		const uint16_t highPassBand = highFrequencyFactor > transitionFactor ? nyquistFrequency * (highFrequencyFactor - transitionFactor) : 0;
		const uint16_t highStopBand = nyquistFrequency * highFrequencyFactor;
		const uint16_t transitionLength = highStopBand - highPassBand;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = 0; j < fftSize; j++)
			{
				if (j <= nyquistFrequency)
				{
					if (j <= lowStopBand || j >= highStopBand)
					{
						fourier.complexBuffer.at(j) = Complex();
					}
					else if (j > lowStopBand && j < lowPassBand)
					{
						fourier.complexBuffer.at(j) *= (double)(j - lowStopBand) / (double)transitionLength;
					}
					else if (j > highPassBand && j < highStopBand)
					{
						fourier.complexBuffer.at(j) *= (double)(highStopBand - j) / (double)transitionLength;
					}
				}
				else
				{
					fourier.complexBuffer.at(j) = fourier.complexBuffer.at(fftSize - j);
					fourier.complexBuffer.at(j).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, uint16_t lowCutoffFreq, uint16_t highCutoffFreq, uint16_t transitionBandLength)
	{
		size_t fftSize = buffer.FrameCount();
		if (!(fftSize > 0 && !(fftSize & (fftSize - 1)))) // if not power of 2
		{
			fftSize = pow(2, floor(log2f(fftSize)) + 1); // smallest power of 2 thats greater than nSample
		}
		const size_t nyquistFrequency = fftSize * 0.5;
		const double lowFrequencyFactor = (double)lowCutoffFreq / 20000.0;
		const double highFrequencyFactor = (double)highCutoffFreq / 20000.0;
		const double transitionFactor = (double)transitionBandLength / 20000.0;
		const uint16_t lowStopBand = nyquistFrequency * lowFrequencyFactor;
		const uint16_t lowPassBand = lowStopBand + nyquistFrequency * transitionFactor;
		const uint16_t highPassBand = highFrequencyFactor > transitionFactor ? nyquistFrequency * (highFrequencyFactor - transitionFactor) : 0;
		const uint16_t highStopBand = nyquistFrequency * highFrequencyFactor;
		const uint16_t transitionLength = highStopBand - highPassBand;
		std::vector<AudioBuffer> channels = SplitChannels(buffer);
		for (size_t i = 0; i < channels.size(); i++)
		{
			AudioBuffer& channel = channels.at(i);
			Fourier fourier(channel, fftSize);
			fourier.Forward();
			for (size_t j = 0; j < fftSize; j++)
			{
				if (j <= fftSize * 0.5)
				{
					if (j >= lowPassBand && j <= highPassBand)
					{
						fourier.complexBuffer.at(j) = Complex();
					}
					else if (j > lowStopBand && j < lowPassBand)
					{
						fourier.complexBuffer.at(j) *= (double)(transitionLength - (j - lowStopBand)) / (double)transitionLength;
					}
					else if (j > highPassBand && j < highStopBand)
					{
						fourier.complexBuffer.at(j) *= (double)(transitionLength - (highStopBand - j)) / (double)transitionLength;
					}
				}
				else
				{
					fourier.complexBuffer.at(j) = fourier.complexBuffer.at(fftSize - j);
					fourier.complexBuffer.at(j).imaginary = -fourier.complexBuffer.at(j).imaginary;
				}
			}
			fourier.Inverse();
			fourier.ComplexBufferToAudioBuffer(channel);
		}
		buffer = MergeChannels(channels);
	}
	void AudioProcessor::TriangleWindow(AudioBuffer& buffer)
	{
		const size_t fc = buffer.FrameCount();
		const size_t hfc = buffer.FrameCount() * 0.5;
		for (size_t i = 0; i < fc; i++)
		{
			double factor;
			if (i <= hfc)
			{
				factor = (double)i / (double)hfc;
			}
			else
			{
				factor = (double)(fc - i) / (double)hfc;
			}
			for (size_t j = 0; j < buffer.GetFormat().nChannels; j++)
			{
				buffer.Set(buffer.Get(i, j) * factor, i, j);
			}
		}
	}
	void AudioProcessor::HannWindow(AudioBuffer& buffer)
	{
		const size_t fc = buffer.FrameCount();
		const size_t hfc = buffer.FrameCount() * 0.5;
		for (size_t i = 0; i < fc; i++)
		{
			const double factor = pow(sin(PI * i / fc), 2);
			for (size_t j = 0; j < buffer.GetFormat().nChannels; j++)
			{
				buffer.Set(buffer.Get(i, j) * factor, i, j);
			}
		}
	}
	std::vector<AudioBuffer> AudioProcessor::SplitChannels(const AudioBuffer& buffer)
	{
		const size_t frameCount = buffer.FrameCount();
		AudioFormatInfo resultFormat = AudioFormatInfo(buffer.wfx.wFormatTag, 1, buffer.wfx.wBitsPerSample, buffer.wfx.nSamplesPerSec);
		std::vector<AudioBuffer> channels(buffer.wfx.nChannels, AudioBuffer(frameCount, resultFormat));
		if (buffer.wfx.nChannels == 1)
		{
			channels.at(0) = buffer;
			return channels;
		}
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t j = 0; j < buffer.wfx.nChannels; j++)
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
			if (channels.at(i).wfx != channels.at(i + 1).wfx)
			{
				throw AudioException(E_FAIL, L"AudioProcessor::MergeChannels", L"All channels must have the same wave format.");
			}
		}
		AudioBuffer resultBuffer(channels.at(0).FrameCount(), AudioFormatInfo(channels.at(0).wfx.wFormatTag, channels.size(), channels.at(0).wfx.wBitsPerSample, channels.at(0).wfx.nSamplesPerSec));
		for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < resultBuffer.wfx.nChannels; j++)
			{
				resultBuffer.Set(channels.at(j).Get(i, 0), i, j);
			}
		}
		return resultBuffer;
	}
	void AudioProcessor::EncodeALAW(AudioBuffer& buffer)
	{
		const AudioFormatInfo alawFormat = AudioFormatInfo(6, 2, 8, 8000);
		AudioProcessor(AudioFormatInfo(1, buffer.GetFormat().nChannels, 16, buffer.GetFormat().nSamplesPerSec)).ConvertBPS(buffer); // Convert current buffer to 16 bps.
		AudioProcessor alawProcessor(alawFormat);
		alawProcessor.ConvertSampleRate(buffer);
		alawProcessor.ConvertChannels(buffer);
		AudioBuffer resultBuffer(buffer.FrameCount(), alawFormat);
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < buffer.GetFormat().nChannels; j++)
			{
				int16_t pcm = buffer.GetAsInt32(i, j);
				int16_t sign = (pcm & 0x8000) >> 8;
				if (sign != 0)
				{
					pcm = -pcm;
				}
				if (pcm > 32635)
				{
					pcm = 32635;
				}
				int16_t exponent = 7;
				for (int16_t expMask = 0x4000; (pcm & expMask) == 0 && exponent > 0; exponent--, expMask >>= 1);
				int16_t mantissa = (pcm >> ((exponent == 0) ? 4 : (exponent + 3))) & 0x0f;
				uint8_t alaw = (uint8_t)(sign | exponent << 4 | mantissa);
				alaw = alaw ^ 0xD5;
				resultBuffer.Set((double)alaw / (double)UINT8_MAX, i, j);
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::DecodeALAW(AudioBuffer& buffer)
	{
		if (buffer.GetFormat().wFormatTag != 6) { throw AudioException(E_INVALIDARG, L"AudioProcessor::DecodeALAW", L"Buffer must be an a-law buffer."); }
		AudioBuffer resultBuffer(buffer.FrameCount(), AudioFormatInfo(1, buffer.GetFormat().nChannels, 16, buffer.GetFormat().nSamplesPerSec));
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < buffer.GetFormat().nChannels; j++)
			{
				uint8_t sample = buffer.GetAsInt32(i, j);
				sample ^= 0xD5;
				uint8_t sign = sample & 0x80;
				uint8_t exponent = (sample & 0x70) >> 4;
				int16_t decodedSample = sample & 0x0f;
				decodedSample <<= 4;
				decodedSample += 8;
				if (exponent != 0)
				{
					decodedSample += 0x0100;
				}
				if (exponent > 1)
				{
					decodedSample <<= (exponent - 1);
				}
				decodedSample = sign == 0 ? decodedSample : -decodedSample;
				resultBuffer.Set((double)decodedSample / (double)INT16_MAX, i, j);
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::EncodeMULAW(AudioBuffer& buffer)
	{
		const AudioFormatInfo mulawFormat = AudioFormatInfo(7, 2, 8, 8000);
		AudioProcessor(AudioFormatInfo(1, buffer.GetFormat().nChannels, 16, buffer.GetFormat().nSamplesPerSec)).ConvertBPS(buffer); // Convert current buffer to 16 bps.
		AudioProcessor mulawProcessor(mulawFormat);
		mulawProcessor.ConvertSampleRate(buffer);
		mulawProcessor.ConvertChannels(buffer);
		AudioBuffer resultBuffer(buffer.FrameCount(), mulawFormat);
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < buffer.GetFormat().nChannels; j++)
			{
				int16_t pcm = buffer.GetAsInt32(i, j);
				int16_t sign = (pcm & 0x8000) >> 8;
				if (sign != 0)
				{
					pcm = -pcm;
				}
				if (pcm > 32635)
				{
					pcm = 32635;
				}
				pcm += 0x84;
				int16_t exponent = 7;
				for (int16_t expMask = 0x4000; (pcm & expMask) == 0; exponent--, expMask >>= 1);
				int16_t mantissa = (pcm >> (exponent + 3)) & 0x0f;
				uint8_t mulaw = (sign | exponent << 4 | mantissa);
				mulaw = ~mulaw;
				resultBuffer.Set((double)mulaw / (double)UINT8_MAX, i, j);
			}
		}
		buffer = resultBuffer;
	}
	void AudioProcessor::DecodeMULAW(AudioBuffer& buffer)
	{
		if (buffer.GetFormat().wFormatTag != 7) { throw AudioException(E_INVALIDARG, L"AudioProcessor::DecodeALAW", L"Buffer must be an mu-law buffer."); }
		AudioBuffer resultBuffer(buffer.FrameCount(), AudioFormatInfo(1, buffer.GetFormat().nChannels, 16, buffer.GetFormat().nSamplesPerSec));
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			for (size_t j = 0; j < buffer.GetFormat().nChannels; j++)
			{
				uint8_t mulaw = buffer.GetAsInt32(i, j);
				mulaw = ~mulaw;
				uint8_t sign = mulaw & 0x80;
				uint8_t exponent = (mulaw & 0x70) >> 4;
				int16_t data = mulaw & 0x0f;
				data |= 0x0010;
				data <<= 1;
				data += 1;
				data <<= exponent + 2;
				data -= 0x0084;
				data = sign == 0 ? data : -data;
				resultBuffer.Set((double)data / (double)INT16_MAX, i, j);
			}
		}
		buffer = resultBuffer;
	}
}