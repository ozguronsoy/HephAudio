#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "HephException.h"
#include "Fourier.h"
#include "File.h"
#include "ConsoleLogger.h"
#include <thread>

using namespace HephCommon;
using namespace HephAudio::Codecs;

namespace HephAudio
{
#pragma region Converts, Mix, Split/Merge Channels
	void AudioProcessor::ChangeBitsPerSample(AudioBuffer& buffer, uint16_t outputBitsPerSample)
	{
		if (buffer.formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_PCM ||
			buffer.formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT)
		{
			IAudioCodec* pCodec = AudioCodecManager::FindCodec(buffer.formatInfo.formatTag);
			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.formatInfo = buffer.formatInfo;
			encodedBufferInfo.pBuffer = buffer.pData;
			encodedBufferInfo.size_byte = buffer.Size();
			encodedBufferInfo.size_frame = buffer.frameCount;
			buffer = pCodec->Decode(encodedBufferInfo);

			encodedBufferInfo.formatInfo.bitsPerSample = outputBitsPerSample;
			encodedBufferInfo.size_byte = buffer.Size();
			pCodec->Encode(buffer, encodedBufferInfo);

			buffer.formatInfo.bitRate = AudioFormatInfo::CalculateBitrate(buffer.formatInfo);
		}
		else
		{
			RAISE_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioProcessor::ChangeBitsPerSample", "Audio buffer is not raw PCM."));
		}
	}
	void AudioProcessor::ChangeNumberOfChannels(AudioBuffer& buffer, uint16_t outputChannelCount)
	{
		if (buffer.formatInfo.channelCount != outputChannelCount)
		{
			AudioBuffer resultBuffer(buffer.frameCount, AudioFormatInfo(buffer.formatInfo.formatTag, outputChannelCount, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate, buffer.formatInfo.bitRate, buffer.formatInfo.endian));

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				heph_audio_sample averageValue = 0.0;
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					averageValue += buffer[i][j] / buffer.formatInfo.channelCount;
				}

				for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
				{
					resultBuffer[i][j] = averageValue;
				}
			}

			buffer = std::move(resultBuffer);
		}
	}
	void AudioProcessor::ChangeSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate)
	{
		const heph_float srRatio = (heph_float)outputSampleRate / (heph_float)buffer.formatInfo.sampleRate;
		if (srRatio != 1.0)
		{
			const size_t targetFrameCount = Math::Ceil((heph_float)buffer.frameCount * srRatio);
			AudioBuffer resultBuffer(targetFrameCount, AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, buffer.formatInfo.bitsPerSample, outputSampleRate, buffer.formatInfo.bitRate, buffer.formatInfo.endian));

			for (size_t i = 0; i < targetFrameCount; i++)
			{
				const heph_float resampleIndex = i / srRatio;
				const heph_float rho = resampleIndex - Math::Floor(resampleIndex);

				for (size_t j = 0; j < buffer.formatInfo.channelCount && (resampleIndex + 1) < buffer.frameCount; j++)
				{
					resultBuffer[i][j] = buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho;
				}
			}

			buffer = std::move(resultBuffer);
		}
	}
	std::vector<FloatBuffer> AudioProcessor::SplitChannels(const AudioBuffer& buffer)
	{
		std::vector<FloatBuffer> channels(buffer.formatInfo.channelCount, FloatBuffer(buffer.frameCount));
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
		if (channels.size() == 0)
		{
			HEPHAUDIO_LOG("no channels to merge, returning empty buffer.", HEPH_CL_WARNING);
			return AudioBuffer();
		}

		AudioBuffer resultBuffer(channels[0].FrameCount(), HEPHAUDIO_INTERNAL_FORMAT(channels.size(), sampleRate));
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = channels[j][i];
			}
		}

		return resultBuffer;
	}
	void AudioProcessor::ConvertToInnerFormat(AudioBuffer& buffer)
	{
		IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(buffer.formatInfo.formatTag);
		if (pAudioCodec == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_FAIL, "AudioProcessor::ConvertToInnerFormat", "Unsupported audio codec."));
		}

		EncodedBufferInfo encodedBufferInfo;
		encodedBufferInfo.pBuffer = buffer.pData;
		encodedBufferInfo.size_byte = buffer.Size();
		encodedBufferInfo.size_frame = buffer.frameCount;
		encodedBufferInfo.formatInfo = buffer.formatInfo;

		buffer = pAudioCodec->Decode(encodedBufferInfo);
	}
	void AudioProcessor::ConvertToTargetFormat(AudioBuffer& buffer, AudioFormatInfo targetFormat)
	{
		AudioProcessor::ConvertToInnerFormat(buffer);

		IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(targetFormat.formatTag);
		if (pAudioCodec == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_FAIL, "AudioProcessor::ConvertToTargetFormat", "Unsupported audio codec."));
		}

		EncodedBufferInfo encodedBufferInfo;
		encodedBufferInfo.size_frame = buffer.frameCount;
		encodedBufferInfo.formatInfo = targetFormat;

		pAudioCodec->Encode(buffer, encodedBufferInfo);
	}
	void AudioProcessor::ChangeEndian(AudioBuffer& buffer)
	{
		if (buffer.formatInfo.bitsPerSample > 8)
		{
			const uint16_t frameSize = buffer.formatInfo.FrameSize();
			const uint16_t bytesPerSample = buffer.formatInfo.bitsPerSample / 8;
			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					HephCommon::ChangeEndian((((uint8_t*)buffer.pData) + i * frameSize + j * bytesPerSample), bytesPerSample);
				}
			}
			buffer.formatInfo.endian = !buffer.formatInfo.endian;
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
				const heph_audio_sample temp = buffer[i][j];
				buffer[i][j] = buffer[buffer.frameCount - i - 1][j];
				buffer[buffer.frameCount - i - 1][j] = temp;
			}
		}
	}
	void AudioProcessor::Echo(AudioBuffer& buffer, EchoInfo info)
	{
		const size_t delayFrameCount = buffer.formatInfo.sampleRate * info.reflectionDelay_s;
		const size_t echoStartFrame = buffer.frameCount * info.echoStartPosition;
		const AudioBuffer echoBuffer = buffer.GetSubBuffer(echoStartFrame, buffer.frameCount * info.echoEndPosition - echoStartFrame);
		heph_float factor = info.decayFactor;
		size_t startFrameIndex = echoStartFrame + delayFrameCount;

		const size_t echoEndFrame = echoStartFrame + delayFrameCount * info.reflectionCount + echoBuffer.frameCount;
		if (echoEndFrame > buffer.frameCount)
		{
			buffer.Resize(echoEndFrame);
		}

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
	void AudioProcessor::LinearPanning(AudioBuffer& buffer, heph_float panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const heph_float rightVolume = panningFactor * 0.5 + 0.5;
			const heph_float leftVolume = 1.0 - rightVolume;

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::SquareLawPanning(AudioBuffer& buffer, heph_float panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const heph_float volume = panningFactor * 0.5 + 0.5;
			const heph_float rightVolume = sqrt(volume);
			const heph_float leftVolume = sqrt(1.0 - volume);

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::SineLawPanning(AudioBuffer& buffer, heph_float panningFactor)
	{
		if (buffer.formatInfo.channelCount == 2)
		{
			const heph_float volume = panningFactor * 0.5 + 0.5;
			const heph_float rightVolume = sin(volume * Math::pi * 0.5);
			const heph_float leftVolume = sin((1.0 - volume) * Math::pi * 0.5);

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::Tremolo(AudioBuffer& buffer, heph_float depth, const Oscillator& lfo)
	{
		const heph_float wetFactor = depth * 0.5;
		const heph_float dryFactor = 1.0 - wetFactor;
		const FloatBuffer lfoPeriodBuffer = lfo.GenerateBuffer();

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const heph_float& lfoSample = lfoPeriodBuffer[i % lfoPeriodBuffer.FrameCount()];
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= wetFactor * lfoSample + dryFactor;
			}
		}
	}
	void AudioProcessor::Vibrato(AudioBuffer& buffer, heph_float depth, heph_float extent_semitone, const Oscillator& lfo)
	{
		const heph_float resampleDelta = buffer.formatInfo.sampleRate * (pow(2, extent_semitone / 12.0) - 1.0);
		const heph_float wetFactor = depth * 0.5;
		const heph_float dryFactor = 1.0 - wetFactor;

		FloatBuffer lfoPeriodBuffer(Math::Ceil(lfo.sampleRate / lfo.frequency));
		for (size_t i = 0; i < lfoPeriodBuffer.FrameCount(); i++)
		{
			lfoPeriodBuffer[i] = lfo[i] * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const heph_float resampleIndex = i + lfoPeriodBuffer[i % lfoPeriodBuffer.FrameCount()] * resampleDelta;
			const heph_float rho = resampleIndex - Math::Floor(resampleIndex);

			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				heph_audio_sample wetSample = 0.0;
				if (resampleIndex + 1.0 < buffer.frameCount)
				{
					wetSample = buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho;
				}
				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::Chorus(AudioBuffer& buffer, heph_float depth, heph_float feedbackGain, heph_float baseDelay_ms, heph_float delay_ms, heph_float extent_semitone, const Oscillator& lfo)
	{
		const heph_float baseDelay_sample = baseDelay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const heph_float delay_sample = delay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const heph_float resampleDelta = buffer.formatInfo.sampleRate * (pow(2, extent_semitone / 12.0) - 1.0);
		const heph_float wetFactor = depth * 0.5;
		const heph_float dryFactor = 1.0 - wetFactor;

		FloatBuffer lfoPeriodBuffer(Math::Ceil(lfo.sampleRate / lfo.frequency));
		FloatBuffer feedbackBuffer(buffer.formatInfo.channelCount);
		for (size_t i = 0; i < lfoPeriodBuffer.FrameCount(); i++)
		{
			lfoPeriodBuffer[i] = lfo[i] * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo);
		memcpy(resultBuffer.pData, buffer.pData, Math::Round(delay_sample + baseDelay_sample) * buffer.formatInfo.FrameSize());

		for (size_t i = baseDelay_sample + delay_sample + 1; i < buffer.frameCount; i++)
		{
			const size_t currentDelay_sample = Math::Round(lfoPeriodBuffer[i % lfoPeriodBuffer.FrameCount()] * delay_sample + baseDelay_sample);
			const heph_float resampleIndex = (i - currentDelay_sample) + lfoPeriodBuffer[i % lfoPeriodBuffer.FrameCount()] * resampleDelta;
			const heph_float rho = resampleIndex - Math::Floor(resampleIndex);

			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				heph_audio_sample wetSample;
				if (resampleIndex + 1.0 < buffer.frameCount)
				{
					wetSample = ((buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho) * 0.5) + (feedbackBuffer[j] * 0.5);
				}
				else
				{
					wetSample = (buffer[i - currentDelay_sample][j] * 0.5) + (feedbackBuffer[j] * 0.5);
				}

				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
				feedbackBuffer[j] = feedbackGain * wetSample;
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::Flanger(AudioBuffer& buffer, heph_float depth, heph_float feedbackGain, heph_float baseDelay_ms, heph_float delay_ms, const Oscillator& lfo)
	{
		const heph_float baseDelay_sample = baseDelay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const heph_float delay_sample = delay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const heph_float wetFactor = depth * 0.5;
		const heph_float dryFactor = 1.0 - wetFactor;
		FloatBuffer feedbackBuffer(buffer.formatInfo.channelCount);
		FloatBuffer lfoPeriodBuffer(Math::Ceil(lfo.sampleRate / lfo.frequency));
		for (size_t i = 0; i < lfoPeriodBuffer.FrameCount(); i++)
		{
			lfoPeriodBuffer[i] = lfo[i] * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo);
		memcpy(resultBuffer.pData, buffer.pData, Math::Round(delay_sample + baseDelay_sample) * buffer.formatInfo.FrameSize());

		for (size_t i = baseDelay_sample + delay_sample + 1; i < buffer.frameCount; i++)
		{
			const size_t currentDelay_sample = Math::Round(lfoPeriodBuffer[i % lfoPeriodBuffer.FrameCount()] * delay_sample + baseDelay_sample);
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const heph_audio_sample wetSample = (buffer[i - currentDelay_sample][j] * 0.5) + (feedbackBuffer[j] * 0.5);
				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
				feedbackBuffer[j] = feedbackGain * wetSample;
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::FixOverflow(AudioBuffer& buffer)
	{
		const heph_audio_sample maxSample = buffer.AbsMax();
		if (maxSample > HEPH_AUDIO_SAMPLE_MAX)
		{
			buffer /= maxSample;
		}
	}
	void AudioProcessor::Normalize(AudioBuffer& buffer, heph_audio_sample peakAmplitude)
	{
		const heph_audio_sample maxSample = buffer.AbsMax();
		if (maxSample != 0 && maxSample != peakAmplitude)
		{
			buffer *= (heph_float)peakAmplitude / maxSample;
		}
	}
	void AudioProcessor::RmsNormalize(AudioBuffer& buffer, heph_float rms)
	{
		heph_float sumOfSamplesSquared = 0.0;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				sumOfSamplesSquared += buffer[i][j] * buffer[i][j];
			}
		}

		if (sumOfSamplesSquared != 0.0)
		{
			buffer *= rms * sqrt(buffer.frameCount * buffer.formatInfo.channelCount / sumOfSamplesSquared);
		}
	}
	void AudioProcessor::HardClipDistortion(AudioBuffer& buffer, heph_float clippingLevel_dB)
	{
		const heph_audio_sample clippingLevel = HephAudio::DecibelToGain(clippingLevel_dB) * HEPH_AUDIO_SAMPLE_MAX;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				heph_audio_sample& sample = buffer[i][j];
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
	void AudioProcessor::ArctanDistortion(AudioBuffer& buffer, heph_float alpha)
	{
		alpha = 5 + alpha * 10;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const heph_float fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
				buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(atan(alpha * fltSample) * (2.0 / Math::pi));
			}
		}
	}
	void AudioProcessor::CubicDistortion(AudioBuffer& buffer, heph_float a)
	{
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const heph_float fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
				buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(fltSample - a * (1.0 / 3.0) * fltSample * fltSample * fltSample);
			}
		}
	}
	void AudioProcessor::Overdrive(AudioBuffer& buffer, heph_float drive)
	{
		drive *= 0.8;

		const heph_float a = sin(drive * Math::pi / 2.0);
		const heph_float k = 2.0 * a / (1.0 - a);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const heph_float fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
				buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT((1.0 + k) * fltSample / (1.0 + k * Math::Abs(fltSample)));
			}
		}
	}
	void AudioProcessor::Fuzz(AudioBuffer& buffer, heph_float depth, heph_float alpha)
	{
		const heph_float wetFactor = depth * 0.5;
		const heph_float dryFactor = 1.0 - wetFactor;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				const heph_float fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
				if (fltSample > 0)
				{
					buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(wetFactor * (1.0 - exp(alpha * fltSample)) + dryFactor * fltSample);
				}
				else
				{
					buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(wetFactor * (-1.0 + exp(alpha * fltSample)) + dryFactor * fltSample);
				}
			}
		}
	}
	void AudioProcessor::LinearFadeIn(AudioBuffer& buffer, heph_float duration_s)
	{
		AudioProcessor::LinearFadeIn(buffer, duration_s, 0);
	}
	void AudioProcessor::LinearFadeIn(AudioBuffer& buffer, heph_float duration_s, size_t startIndex)
	{
		const heph_float duration_sample = Math::Round(duration_s * buffer.formatInfo.sampleRate);
		const size_t endIndex = Math::Min(startIndex + (size_t)duration_sample, buffer.frameCount);
		for (size_t i = startIndex; i < endIndex; i++)
		{
			const heph_float factor = (i - startIndex) / duration_sample;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::LinearFadeOut(AudioBuffer& buffer, heph_float duration_s)
	{
		const size_t duration_sample = Math::Round(duration_s * buffer.formatInfo.sampleRate);
		AudioProcessor::LinearFadeOut(buffer, duration_s, buffer.frameCount > duration_sample ? (buffer.frameCount - duration_sample) : 0);
	}
	void AudioProcessor::LinearFadeOut(AudioBuffer& buffer, heph_float duration_s, size_t startIndex)
	{
		const heph_float duration_sample = Math::Round(duration_s * buffer.formatInfo.sampleRate);
		const size_t endIndex = Math::Min(startIndex + (size_t)duration_sample, buffer.frameCount);
		for (size_t i = startIndex; i < endIndex; i++)
		{
			const heph_float factor = (endIndex - i) / duration_sample;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		AudioProcessor::Equalizer(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, window, infos);
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].GetSubBuffer(i, fftSize) * windowBuffer, fftSize);

				for (size_t k = 0; k < infos.size(); k++)
				{
					const EqualizerInfo& info = infos[k];
					uint64_t lowerFrequencyIndex, higherFrequencyIndex;
					if (info.f1 > info.f2)
					{
						higherFrequencyIndex = Math::Ceil(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f1));
						lowerFrequencyIndex = Math::Floor(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f2));
					}
					else
					{
						higherFrequencyIndex = Math::Ceil(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f2));
						lowerFrequencyIndex = Math::Floor(Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, info.f1));
					}
					const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;

					for (size_t l = lowerFrequencyIndex; l <= upperBound; l++)
					{
						complexBuffer[l] *= info.amplitude;
						complexBuffer[fftSize - l - 1] = complexBuffer[l].Conjugate();
					}
				}

				Fourier::IFFT(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real() * overflowFactor * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		AudioProcessor::EqualizerMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, 0, window, infos);
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, size_t threadCountPerChannel, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		AudioProcessor::EqualizerMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, threadCountPerChannel, window, infos);
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		AudioProcessor::EqualizerMT(buffer, hopSize, fftSize, 0, window, infos);
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, size_t threadCountPerChannel, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		auto applyEqualizer = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, const std::vector<EqualizerInfo>* const infos
			, heph_float overflowFactor)
			{
				FloatBuffer channel(frameCount);
				for (size_t i = 0; i < frameCount && (i + frameIndex) < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.FrameCount(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.GetSubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

					for (size_t j = 0; j < infos->size(); j++)
					{
						const EqualizerInfo& info = infos->at(j);
						uint64_t lowerFrequencyIndex, higherFrequencyIndex;
						if (info.f1 > info.f2)
						{
							higherFrequencyIndex = Math::Ceil(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f1));
							lowerFrequencyIndex = Math::Floor(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f2));
						}
						else
						{
							higherFrequencyIndex = Math::Ceil(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f2));
							lowerFrequencyIndex = Math::Floor(Fourier::BinFrequencyToIndex(buffer->formatInfo.sampleRate, fftSize, info.f1));
						}
						const size_t upperBound = higherFrequencyIndex < nyquistFrequency ? higherFrequencyIndex : nyquistFrequency - 1;

						for (size_t l = lowerFrequencyIndex; l <= upperBound; l++)
						{
							complexBuffer[l] *= info.amplitude;
							complexBuffer[fftSize - l - 1] = complexBuffer[l].Conjugate();
						}
					}

					Fourier::IFFT(complexBuffer, false);
					for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->frameCount; j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelCount;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyEqualizer, &buffer, &windowBuffer
					, j, i * frameCountPerThread, frameCountPerThread
					, hopSize, fftSize, nyquistFrequency
					, &infos, overflowFactor); // 
			}
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, heph_float speed, Window& window)
	{
		if (speed <= 1)
		{
			AudioProcessor::ChangeSpeed(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, speed, window);
		}
		else
		{
			AudioProcessor::ChangeSpeed(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE / 2, speed, window);
		}
	}
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t windowSize, heph_float speed, Window& window)
	{
		if (speed == 0)
		{
			HEPHAUDIO_LOG("speed cannot be zero, canceling the operation.", HEPH_CL_WARNING);
			return;
		}

		if (speed < 0)
		{
			HEPHAUDIO_LOG("speed cannot be negative, using the positive value instead.", HEPH_CL_WARNING);
			speed = -speed;
		}

		if (hopSize > windowSize)
		{
			HEPHAUDIO_LOG("the hopSize is less than the windowSize, using the default values instead.", HEPH_CL_WARNING);
			hopSize = AudioProcessor::DEFAULT_HOP_SIZE;
			windowSize = AudioProcessor::DEFAULT_FFT_SIZE;
		}

		window.SetSize(windowSize);
		const heph_float frameCountRatio = 1.0 / speed;
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		AudioBuffer resultBuffer(buffer.frameCount * frameCountRatio, buffer.formatInfo);

		if (speed <= 1.0)
		{
			const heph_float overflowFactor = ((heph_float)hopSize) / ((heph_float)windowSize);
			for (size_t i = 0; i < buffer.frameCount; i += hopSize)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					for (size_t k = 0; k < frameCountRatio; k++)
					{
						for (size_t l = 0, m = i * frameCountRatio + k * hopSize;
							l < windowSize && (i + l) < buffer.frameCount && m < resultBuffer.frameCount;
							l++, m++)
						{
							resultBuffer[m][j] += buffer[i + l][j] * overflowFactor * windowBuffer[l];
						}
					}
				}
			}
		}
		else
		{
			const heph_float overflowFactor = ((heph_float)hopSize) / ((heph_float)windowSize) * frameCountRatio;
			for (size_t i = 0; i < buffer.frameCount; i += hopSize)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					for (size_t k = 0, l = i * frameCountRatio;
						k < windowSize && (i + k) < buffer.frameCount && l < resultBuffer.frameCount;
						k++, l++)
					{
						resultBuffer[l][j] += buffer[i + k][j] * overflowFactor * windowBuffer[k];
					}
				}
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::PitchShift(AudioBuffer& buffer, heph_float pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShift(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, pitchChange_semitone, window);
	}
	void AudioProcessor::PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone, Window& window)
	{
		constexpr heph_float twopi = 2.0 * Math::pi;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const heph_float shiftFactor = pow(2.0, pitchChange_semitone / 12.0);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);
		std::vector<FloatBuffer> lastAnalysisPhases(buffer.formatInfo.channelCount, FloatBuffer(nyquistFrequency));
		std::vector<FloatBuffer> lastSynthesisPhases(buffer.formatInfo.channelCount, FloatBuffer(nyquistFrequency));
		std::vector<FloatBuffer> synthesisMagnitudes(buffer.formatInfo.channelCount, FloatBuffer(nyquistFrequency));
		std::vector<FloatBuffer> synthesisFrequencies(buffer.formatInfo.channelCount, FloatBuffer(nyquistFrequency));

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				synthesisMagnitudes[j].Reset();
				synthesisFrequencies[j].Reset();
				ComplexBuffer complexBuffer(channels[j].GetSubBuffer(i, fftSize) * windowBuffer);
				Fourier::FFT(complexBuffer, fftSize);

				for (size_t k = 0; k < nyquistFrequency; k++)
				{
					const heph_float phase = complexBuffer[k].Phase();
					heph_float phaseRemainder = phase - lastAnalysisPhases[j][k] - twopi * k * hopSize / fftSize;
					phaseRemainder = phaseRemainder >= 0 ? (fmod(phaseRemainder + Math::pi, twopi) - Math::pi) : (fmod(phaseRemainder - Math::pi, -twopi) + Math::pi);

					const size_t newBin = Math::Floor(k * shiftFactor + 0.5);
					if (newBin < nyquistFrequency)
					{
						synthesisMagnitudes[j][newBin] += complexBuffer[k].Magnitude();
						synthesisFrequencies[j][newBin] = (k + phaseRemainder * fftSize / twopi / hopSize) * shiftFactor;
					}

					lastAnalysisPhases[j][k] = phase;
				}

				for (size_t k = 0; k < nyquistFrequency; k++)
				{
					heph_float synthesisPhase = twopi * hopSize / fftSize * synthesisFrequencies[j][k] + lastSynthesisPhases[j][k];
					synthesisPhase = synthesisPhase >= 0 ? (fmod(synthesisPhase + Math::pi, twopi) - Math::pi) : (fmod(synthesisPhase - Math::pi, -twopi) + Math::pi);

					complexBuffer[k] = Complex(synthesisMagnitudes[j][k] * cos(synthesisPhase), synthesisMagnitudes[j][k] * sin(synthesisPhase));
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();

					lastSynthesisPhases[j][k] = synthesisPhase;
				}

				Fourier::IFFT(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real() * overflowFactor * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, heph_float pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShiftMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, pitchChange_semitone, 0, window);
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, heph_float pitchChange_semitone, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::PitchShiftMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, pitchChange_semitone, threadCountPerChannel, window);
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShiftMT(buffer, hopSize, fftSize, pitchChange_semitone, 0, window);
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone, size_t threadCountPerChannel, Window& window)
	{
		auto applyPitchShift = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, size_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, heph_float shiftFactor
			, heph_float overflowFactor)
			{
				constexpr heph_float twopi = 2.0 * Math::pi;

				FloatBuffer lastAnalysisPhases(nyquistFrequency);
				FloatBuffer lastSynthesisPhases(nyquistFrequency);
				FloatBuffer synthesisMagnitudes(nyquistFrequency);
				FloatBuffer synthesisFrequencies(nyquistFrequency);

				FloatBuffer channel(frameCount);
				for (size_t i = 0; i < frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.FrameCount(); i += hopSize)
				{
					synthesisMagnitudes.Reset();
					synthesisFrequencies.Reset();

					ComplexBuffer complexBuffer(channel.GetSubBuffer(i, fftSize) * (*pWindowBuffer));
					Fourier::FFT(complexBuffer, fftSize);

					for (size_t k = 0; k < nyquistFrequency; k++)
					{
						const heph_float phase = complexBuffer[k].Phase();
						heph_float phaseRemainder = phase - lastAnalysisPhases[k] - twopi * k * hopSize / fftSize;
						phaseRemainder = phaseRemainder >= 0 ? (fmod(phaseRemainder + Math::pi, twopi) - Math::pi) : (fmod(phaseRemainder - Math::pi, -twopi) + Math::pi);

						const size_t newBin = Math::Floor(k * shiftFactor + 0.5);
						if (newBin < nyquistFrequency)
						{
							synthesisMagnitudes[newBin] += complexBuffer[k].Magnitude();
							synthesisFrequencies[newBin] = (k + phaseRemainder * fftSize / twopi / hopSize) * shiftFactor;
						}

						lastAnalysisPhases[k] = phase;
					}

					for (size_t k = 0; k < nyquistFrequency; k++)
					{
						heph_float synthesisPhase = twopi * hopSize / fftSize * synthesisFrequencies[k] + lastSynthesisPhases[k];
						synthesisPhase = synthesisPhase >= 0 ? (fmod(synthesisPhase + Math::pi, twopi) - Math::pi) : (fmod(synthesisPhase - Math::pi, -twopi) + Math::pi);

						complexBuffer[k] = Complex(synthesisMagnitudes[k] * cos(synthesisPhase), synthesisMagnitudes[k] * sin(synthesisPhase));
						complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();

						lastSynthesisPhases[k] = synthesisPhase;
					}

					Fourier::IFFT(complexBuffer, false);
					for (size_t k = 0, l = i + frameIndex; k < fftSize && l < buffer->frameCount; k++, l++)
					{
						(*buffer)[l][channelIndex] += complexBuffer[k].real() * overflowFactor * (*pWindowBuffer)[k] / fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelCount;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const heph_float shiftFactor = pow(2.0, pitchChange_semitone / 12.0);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyPitchShift, &buffer, &windowBuffer
					, j, i * frameCountPerThread, frameCountPerThread
					, hopSize, fftSize, nyquistFrequency
					, shiftFactor, overflowFactor);
			}
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
#pragma endregion
#pragma region Filters
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].GetSubBuffer(i, fftSize) * windowBuffer, fftSize);

				for (size_t k = startIndex; k < nyquistFrequency; k++)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}

				Fourier::IFFT(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real() * overflowFactor * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, hopSize, fftSize, cutoffFreq, 0, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, size_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, size_t startIndex
			, heph_float overflowFactor)
			{
				FloatBuffer channel(frameCount);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.FrameCount(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.GetSubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

					for (int64_t j = startIndex; j < nyquistFrequency; j++)
					{
						complexBuffer[j] = Complex();
						complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
					}

					Fourier::IFFT(complexBuffer, false);
					for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->frameCount; j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / (heph_float)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelCount;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer
					, j, i * frameCountPerThread, frameCountPerThread
					, hopSize, fftSize, nyquistFrequency
					, startIndex, overflowFactor);
			}
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].GetSubBuffer(i, fftSize) * windowBuffer, fftSize);

				for (int64_t k = stopIndex; k >= 0; k--)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}

				Fourier::IFFT(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real() * overflowFactor * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, hopSize, fftSize, cutoffFreq, 0, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t stopIndex, heph_float overflowFactor)
			{
				FloatBuffer channel(frameCount);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.FrameCount(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.GetSubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);
					for (int64_t j = stopIndex; j >= 0; j--)
					{
						complexBuffer[j] = Complex();
						complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
					}

					Fourier::IFFT(complexBuffer, false);
					for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->frameCount; j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelCount;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = Math::Ceil(((heph_float)buffer.frameCount) / ((heph_float)threadCountPerChannel));
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer
					, j, i * frameCountPerThread, frameCountPerThread
					, hopSize, fftSize, stopIndex
					, overflowFactor);
			}
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].GetSubBuffer(i, fftSize) * windowBuffer, fftSize);

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

				Fourier::IFFT(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real() * overflowFactor * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, hopSize, fftSize, lowCutoffFreq, highCutoffFreq, 0, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, size_t startIndex
			, size_t stopIndex, heph_float overflowFactor)
			{
				FloatBuffer channel(frameCount);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.FrameCount(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.GetSubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

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

					Fourier::IFFT(complexBuffer, false);
					for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->frameCount; j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / (heph_float)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelCount;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer
					, j, i * frameCountPerThread, frameCountPerThread
					, hopSize, fftSize, nyquistFrequency
					, startIndex, stopIndex, overflowFactor);
			}
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].GetSubBuffer(i, fftSize) * windowBuffer, fftSize);
				for (size_t k = startIndex; k <= stopIndex; k++)
				{
					complexBuffer[k] = Complex();
					complexBuffer[fftSize - k - 1] = complexBuffer[k].Conjugate();
				}
				Fourier::IFFT(complexBuffer, false);
				for (size_t k = 0, l = i; k < fftSize && l < buffer.frameCount; k++, l++)
				{
					buffer[l][j] += complexBuffer[k].real() * overflowFactor * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, hopSize, fftSize, lowCutoffFreq, highCutoffFreq, 0, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, size_t startIndex
			, size_t stopIndex, heph_float overflowFactor)
			{
				FloatBuffer channel(frameCount);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.FrameCount(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.GetSubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

					for (size_t j = startIndex; j <= stopIndex; j++)
					{
						complexBuffer[j] = Complex();
						complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
					}

					Fourier::IFFT(complexBuffer, false);
					for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->frameCount; j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / (heph_float)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelCount;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const heph_float overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer
					, j, i * frameCountPerThread, frameCountPerThread
					, hopSize, fftSize, nyquistFrequency
					, startIndex, stopIndex, overflowFactor);
			}
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
#pragma endregion
}