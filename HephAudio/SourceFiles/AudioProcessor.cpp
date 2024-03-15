#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "int24.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "../HephCommon/HeaderFiles/Fourier.h"
#include "../HephCommon/HeaderFiles/File.h"
#include <thread>

using namespace HephCommon;
using namespace HephAudio::Codecs;

namespace HephAudio
{
#pragma region Converts, Mix, Split/Merge Channels
	void AudioProcessor::ChangeBitsPerSample(AudioBuffer& buffer, uint16_t outputBitsPerSample)
	{
		if (buffer.formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_PCM)
		{
			IAudioCodec* pCodec = AudioCodecManager::FindCodec(HEPHAUDIO_FORMAT_TAG_PCM);
			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.formatInfo = buffer.formatInfo;
			encodedBufferInfo.pBuffer = buffer.pData;
			encodedBufferInfo.size_byte = buffer.Size();
			encodedBufferInfo.size_frame = buffer.frameCount;
			encodedBufferInfo.endian = HEPH_SYSTEM_ENDIAN;
			buffer = pCodec->Decode(encodedBufferInfo);

			encodedBufferInfo.formatInfo.bitsPerSample = outputBitsPerSample;
			encodedBufferInfo.size_byte = buffer.Size();
			pCodec->Encode(buffer, encodedBufferInfo);
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
			AudioBuffer resultBuffer(buffer.frameCount, AudioFormatInfo(buffer.formatInfo.formatTag, outputChannelCount, buffer.formatInfo.bitsPerSample, buffer.formatInfo.sampleRate));

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				heph_audio_sample averageValue = 0.0;
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
	void AudioProcessor::ChangeSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate)
	{
		const heph_float srRatio = (heph_float)outputSampleRate / (heph_float)buffer.formatInfo.sampleRate;
		if (srRatio != 1.0)
		{
			const size_t targetFrameCount = Math::Ceil((heph_float)buffer.frameCount * srRatio);
			AudioBuffer resultBuffer(targetFrameCount, AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.channelCount, buffer.formatInfo.bitsPerSample, outputSampleRate));

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
	void AudioProcessor::ChangeSampleRate(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, uint32_t outputSampleRate, size_t outFrameCount)
	{
		const heph_float srRatio = (heph_float)outputSampleRate / (heph_float)originalBuffer.formatInfo.sampleRate;
		if (srRatio != 1.0)
		{
			subBuffer.Resize(outFrameCount);
			subBuffer.formatInfo.sampleRate = outputSampleRate;

			for (size_t i = 0; i < outFrameCount; i++)
			{
				const heph_float resampleIndex = i / srRatio + subBufferFrameIndex;
				const heph_float rho = resampleIndex - Math::Floor(resampleIndex);

				for (size_t j = 0; j < subBuffer.formatInfo.channelCount && (resampleIndex + 1) < originalBuffer.frameCount; j++)
				{
					subBuffer[i][j] = originalBuffer[resampleIndex][j] * (1.0 - rho) + originalBuffer[resampleIndex + 1.0][j] * rho;
				}
			}
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
		if (channels.size() == 0) { return AudioBuffer(0, AudioFormatInfo()); }
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
		encodedBufferInfo.endian = buffer.formatInfo.endian;

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
		encodedBufferInfo.endian = targetFormat.endian;

		pAudioCodec->Encode(buffer, encodedBufferInfo);
	}
	void AudioProcessor::ChangeEndian(AudioBuffer& buffer)
	{
		if (buffer.formatInfo.bitsPerSample > 8)
		{
			switch (buffer.formatInfo.bitsPerSample)
			{
			case 16:
			{
				for (size_t i = 0; i < buffer.frameCount; i++)
				{
					for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
					{
						HephCommon::ChangeEndian((uint8_t*)((int16_t*)buffer.pData + i * buffer.formatInfo.channelCount + j), 2);
					}
				}
			}
			break;
			case 24:
			{
				for (size_t i = 0; i < buffer.frameCount; i++)
				{
					for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
					{
						HephCommon::ChangeEndian((uint8_t*)((int24*)buffer.pData + i * buffer.formatInfo.channelCount + j), 3);
					}
				}
			}
			break;
			case 32:
			{
				for (size_t i = 0; i < buffer.frameCount; i++)
				{
					for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
					{
						HephCommon::ChangeEndian((uint8_t*)((int32_t*)buffer.pData + i * buffer.formatInfo.channelCount + j), 4);
					}
				}
			}
			break;
			case 64:
			{
				for (size_t i = 0; i < buffer.frameCount; i++)
				{
					for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
					{
						HephCommon::ChangeEndian((uint8_t*)((int64_t*)buffer.pData + i * buffer.formatInfo.channelCount + j), 8);
					}
				}
			}
			break;
			default:
				break;
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
		const heph_float wetFactor = depth;
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
					wetSample = buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho;
				}
				else
				{
					wetSample = buffer[i - currentDelay_sample][j] + feedbackBuffer[j];
				}

				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
				feedbackBuffer[j] = feedbackGain * wetSample;
			}
		}

		buffer = std::move(resultBuffer);

		AudioProcessor::FixOverflow(buffer);
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
				const heph_audio_sample wetSample = buffer[i - currentDelay_sample][j] + feedbackBuffer[j];
				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
				feedbackBuffer[j] = feedbackGain * wetSample;
			}
		}

		buffer = std::move(resultBuffer);

		AudioProcessor::FixOverflow(buffer);
	}
	void AudioProcessor::Wah(AudioBuffer& buffer, heph_float depth, heph_float damping, heph_float fcmin, heph_float fcmax, const Oscillator& lfo)
	{
		if (buffer.frameCount > 0)
		{
			const heph_float wetFactor = depth;
			const heph_float dryFactor = 1.0 - wetFactor;
			const heph_float fcdelta = fcmax - fcmin;
			const heph_float Q = 2.0 * damping;

			FloatBuffer lfoPeriodBuffer(Math::Ceil(lfo.sampleRate / lfo.frequency));
			for (size_t i = 0; i < lfoPeriodBuffer.FrameCount(); i++)
			{
				lfoPeriodBuffer[i] = lfo[i] * 0.5 + 0.5;
			}

			heph_float fc = fcdelta * lfoPeriodBuffer[0] + fcmin;
			heph_float alpha = 2.0 * sin(Math::pi * fc / lfo.sampleRate);
			AudioBuffer lowPassBuffer(buffer.frameCount, buffer.formatInfo);
			AudioBuffer bandPassBuffer(buffer.frameCount, buffer.formatInfo);
			AudioBuffer highPassBuffer(buffer.frameCount, buffer.formatInfo);

			for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
			{
				highPassBuffer[0][i] = buffer[0][i];
				bandPassBuffer[0][i] = alpha * buffer[0][i];
				lowPassBuffer[0][i] = alpha * bandPassBuffer[0][i];
			}

			for (size_t i = 1; i < buffer.frameCount; i++)
			{
				fc = fcdelta * lfoPeriodBuffer[i % lfoPeriodBuffer.FrameCount()] + fcmin;
				alpha = 2.0 * sin(Math::pi * fc / lfo.sampleRate);

				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					highPassBuffer[i][j] = buffer[i][j] - lowPassBuffer[i - 1][j] - Q * bandPassBuffer[i - 1][j];
					bandPassBuffer[i][j] = alpha * highPassBuffer[i][j] + bandPassBuffer[i - 1][j];
					lowPassBuffer[i][j] = alpha * bandPassBuffer[i][j] + lowPassBuffer[i - 1][j];

					buffer[i][j] = wetFactor * bandPassBuffer[i][j] + dryFactor * buffer[i][j];
				}
			}

			AudioProcessor::FixOverflow(buffer);
		}
	}
	void AudioProcessor::FixOverflow(AudioBuffer& buffer)
	{
		const heph_audio_sample maxSample = buffer.AbsMax();
		if (maxSample > 1)
		{
			buffer *= 1.0 / maxSample;
		}
	}
	void AudioProcessor::Normalize(AudioBuffer& buffer, heph_float peakAmplitude)
	{
		const heph_audio_sample maxSample = buffer.AbsMax();
		if (maxSample != 0 && maxSample != peakAmplitude)
		{
			buffer *= peakAmplitude / maxSample;
		}
	}
	void AudioProcessor::RmsNormalize(AudioBuffer& buffer, heph_float desiredRms)
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
			buffer *= desiredRms * sqrt(buffer.frameCount * buffer.formatInfo.channelCount / sumOfSamplesSquared);
		}
	}
	void AudioProcessor::HardClipDistortion(AudioBuffer& buffer, heph_float clippingLevel_dB)
	{
		const heph_float clippingLevel = DecibelToGain(clippingLevel_dB);
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
	void AudioProcessor::SoftClipDistortion(AudioBuffer& buffer, heph_float alpha)
	{
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				heph_audio_sample& sample = buffer[i][j];
				sample = 2.0 * atan(alpha * sample) / Math::pi;
			}
		}
	}
	void AudioProcessor::Overdrive(AudioBuffer& buffer, heph_float drive)
	{
		const heph_float a = sin(drive * Math::pi / 2.0);
		const heph_float k = 2.0 * a / (1.0 - a);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				heph_audio_sample& sample = buffer[i][j];
				sample = (1.0 + k) * sample / (1.0 + k * Math::Abs(sample));
			}
		}
	}
	void AudioProcessor::Fuzz(AudioBuffer& buffer, heph_float depth, heph_float alpha)
	{
		const heph_float& wetFactor = depth;
		const heph_float dryFactor = 1.0 - wetFactor;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				heph_audio_sample& sample = buffer[i][j];
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
	void AudioProcessor::FastSpatialize(AudioBuffer& buffer, const HephCommon::Vector3& source, heph_float speedOfSound, heph_float maxDistance)
	{
		constexpr heph_float distanceBetweenEars = 0.0215;
		constexpr Vector3 reciever = Vector3(0, 0, 0);
		constexpr Vector3 leftEar = Vector3(-distanceBetweenEars / 2, 0, 0);
		constexpr Vector3 rightEar = Vector3(distanceBetweenEars / 2, 0, 0);

		const heph_float leftDistance = source.Distance(leftEar);
		const heph_float rightDistance = source.Distance(rightEar);

		heph_float maxDelay = 0;
		size_t delays[2]{ 0 };
		if (leftDistance > rightDistance)
		{
			maxDelay = (leftDistance - rightDistance) * buffer.formatInfo.sampleRate / speedOfSound;
			delays[0] = maxDelay;
			delays[1] = 0;
		}
		else
		{
			maxDelay = (rightDistance - leftDistance) * buffer.formatInfo.sampleRate / speedOfSound;
			delays[0] = 0;
			delays[1] = maxDelay;
		}

		const heph_float azimuth = atan2(source.x, source.z);
		const heph_float volumes[2] =
		{
			(heph_float)((leftDistance < maxDistance ? ((maxDistance - leftDistance) / maxDistance) : 0_hf) * (0.5 - azimuth / Math::pi)),
			(heph_float)((rightDistance < maxDistance ? ((maxDistance - rightDistance) / maxDistance) : 0_hf) * (0.5 + azimuth / Math::pi))
		};

		AudioFormatInfo resultBufferFormatInfo = buffer.formatInfo;
		resultBufferFormatInfo.channelCount = 2;

		AudioBuffer resultBuffer(buffer.frameCount + maxDelay, resultBufferFormatInfo);
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			heph_audio_sample monoSample = 0.0;
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				monoSample += buffer[i][j];
			}
			monoSample /= buffer.formatInfo.channelCount;

			for (size_t j = 0; j < 2; j++)
			{
				resultBuffer[i + delays[j]][j] = monoSample * volumes[j];
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::FastSpatialize(AudioBuffer& buffer, const HephCommon::Vector3& source, const HephCommon::Vector3& reciever, heph_float speedOfSound, heph_float maxDistance)
	{
		AudioProcessor::FastSpatialize(buffer, source - reciever, speedOfSound, maxDistance);
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		AudioProcessor::Equalizer(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, window, infos);
	}
	void AudioProcessor::Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

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
					buffer[l][j] += complexBuffer[k].real * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		AudioProcessor::EqualizerMT(buffer, defaultHopSize, defaultFFTSize, window, infos);
	}
	void AudioProcessor::EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos)
	{
		auto applyEqualizer = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, const std::vector<EqualizerInfo>* const infos)
			{
				FloatBuffer channel(buffer->frameCount);
				for (size_t i = 0; i < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i][channelIndex];
					(*buffer)[i][channelIndex] = 0;
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
					for (size_t j = 0, k = i; j < fftSize && k < channel.FrameCount(); j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real * (*pWindowBuffer)[j] / fftSize;
					}
				}
			};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads[i] = std::thread(applyEqualizer, &buffer, &windowBuffer, i, hopSize, fftSize, nyquistFrequency, &infos);
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, heph_float speed, Window& window)
	{
		AudioProcessor::ChangeSpeed(buffer, buffer.formatInfo.sampleRate * 0.03, buffer.formatInfo.sampleRate * 0.06, speed, window);
	}
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t windowSize, heph_float speed, Window& window)
	{
		if (speed == 0) { return; }
		if (speed < 0) { speed = -speed; }

		window.SetSize(windowSize);
		const heph_float frameCountRatio = 1.0 / speed;
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		AudioBuffer resultBuffer(buffer.frameCount * frameCountRatio, buffer.formatInfo);

		if (speed <= 1.0)
		{
			for (size_t i = 0; i < buffer.frameCount; i += hopSize)
			{
				AudioBuffer subBuffer = buffer.GetSubBuffer(i, windowSize);
				for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
				{
					for (size_t k = 0; k < frameCountRatio; k++)
					{
						for (size_t l = 0, m = i * frameCountRatio + k * hopSize; l < windowSize && m < resultBuffer.frameCount; l++, m++)
						{
							resultBuffer[m][j] += subBuffer[l][j] * windowBuffer[l];
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
					for (size_t k = 0, l = i * frameCountRatio; k < windowSize && l < resultBuffer.frameCount; k++, l++)
					{
						resultBuffer[l][j] += subBuffer[k][j] * windowBuffer[k];
					}
				}
			}
		}

		buffer = std::move(resultBuffer);

		AudioProcessor::FixOverflow(buffer);
	}
	void AudioProcessor::PitchShift(AudioBuffer& buffer, heph_float pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShift(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, pitchChange_semitone, window);
	}
	void AudioProcessor::PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone, Window& window)
	{
		constexpr heph_float twopi = 2.0 * Math::pi;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		const heph_float shiftFactor = pow(2.0, pitchChange_semitone / 12.0);
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
				ComplexBuffer complexBuffer = ComplexBuffer(channels[j].GetSubBuffer(i, fftSize) * windowBuffer);
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
					buffer[l][j] += complexBuffer[k].real * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, heph_float pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShiftMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, pitchChange_semitone, window);
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone, Window& window)
	{
		auto applyPitchShift = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, size_t channelIndex, size_t hopSize, size_t fftSize, size_t nyquistFrequency, heph_float shiftFactor)
			{
				constexpr heph_float twopi = 2.0 * Math::pi;

				FloatBuffer lastAnalysisPhases(nyquistFrequency);
				FloatBuffer lastSynthesisPhases(nyquistFrequency);
				FloatBuffer synthesisMagnitudes(nyquistFrequency);
				FloatBuffer synthesisFrequencies(nyquistFrequency);

				FloatBuffer channel(buffer->frameCount);
				for (size_t i = 0; i < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i][channelIndex];
					(*buffer)[i][channelIndex] = 0;
				}

				for (size_t i = 0; i < buffer->frameCount; i += hopSize)
				{
					synthesisMagnitudes.Reset();
					synthesisFrequencies.Reset();

					ComplexBuffer complexBuffer = ComplexBuffer(channel.GetSubBuffer(i, fftSize) * (*pWindowBuffer));
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
					for (size_t k = 0, l = i; k < fftSize && l < buffer->frameCount; k++, l++)
					{
						(*buffer)[l][channelIndex] += complexBuffer[k].real * (*pWindowBuffer)[k] / fftSize;
					}
				}
			};

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const heph_float shiftFactor = pow(2.0, pitchChange_semitone / 12.0);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount);

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
		{
			threads[i] = std::thread(applyPitchShift, &buffer, &windowBuffer, i, hopSize, fftSize, nyquistFrequency, shiftFactor);
		}

		for (size_t i = 0; i < buffer.formatInfo.channelCount; i++)
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
		AudioProcessor::LowPassFilter(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, cutoffFreq, window);
	}
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

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
					buffer[l][j] += complexBuffer[k].real * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, cutoffFreq, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, hopSize, fftSize, cutoffFreq, std::thread::hardware_concurrency() / buffer.formatInfo.channelCount, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, cutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, size_t channelIndex, size_t frameIndex, size_t frameCount, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex)
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
						(*buffer)[k][channelIndex] += complexBuffer[j].real * (*pWindowBuffer)[j] / (heph_float)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = 1;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer, j, i * frameCountPerThread, frameCountPerThread, hopSize, fftSize, nyquistFrequency, startIndex);
			}
		}

		for (size_t i = 0; i < threadCountPerChannel * buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilter(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, cutoffFreq, window);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

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
					buffer[l][j] += complexBuffer[k].real * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, cutoffFreq, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, hopSize, fftSize, cutoffFreq, std::thread::hardware_concurrency() / buffer.formatInfo.channelCount, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, cutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex, size_t frameIndex, size_t frameCount, size_t hopSize, size_t fftSize, size_t stopIndex)
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
						(*buffer)[k][channelIndex] += complexBuffer[j].real * (*pWindowBuffer)[j] / (heph_float)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = 1;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer, j, i * frameCountPerThread, frameCountPerThread, hopSize, fftSize, stopIndex);
			}
		}

		for (size_t i = 0; i < threadCountPerChannel * buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilter(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

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
					buffer[l][j] += complexBuffer[k].real * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, hopSize, fftSize, lowCutoffFreq, highCutoffFreq, std::thread::hardware_concurrency() / buffer.formatInfo.channelCount, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, lowCutoffFreq, highCutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex, size_t frameIndex, size_t frameCount, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex, size_t stopIndex)
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
						(*buffer)[k][channelIndex] += complexBuffer[j].real * (*pWindowBuffer)[j] / (heph_float)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = 1;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer, j, i * frameCountPerThread, frameCountPerThread, hopSize, fftSize, nyquistFrequency, startIndex, stopIndex);
			}
		}

		for (size_t i = 0; i < threadCountPerChannel * buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilter(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<FloatBuffer> channels = SplitChannels(buffer);

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
					buffer[l][j] += complexBuffer[k].real * windowBuffer[k] / fftSize;
				}
			}
		}
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, hopSize, fftSize, lowCutoffFreq, highCutoffFreq, std::thread::hardware_concurrency() / buffer.formatInfo.channelCount, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, AudioProcessor::defaultHopSize, AudioProcessor::defaultFFTSize, lowCutoffFreq, highCutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* pWindowBuffer, uint16_t channelIndex, size_t frameIndex, size_t frameCount, size_t hopSize, size_t fftSize, size_t nyquistFrequency, size_t startIndex, size_t stopIndex)
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
						(*buffer)[k][channelIndex] += complexBuffer[j].real * (*pWindowBuffer)[j] / (heph_float)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = 1;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const size_t frameCountPerThread = Math::Ceil((heph_float)buffer.frameCount / (heph_float)threadCountPerChannel);
		const FloatBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelCount * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				threads[i * buffer.formatInfo.channelCount + j] = std::thread(applyFilter, &buffer, &windowBuffer, j, i * frameCountPerThread, frameCountPerThread, hopSize, fftSize, nyquistFrequency, startIndex, stopIndex);
			}
		}

		for (size_t i = 0; i < threadCountPerChannel * buffer.formatInfo.channelCount; i++)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}
	void AudioProcessor::SchroederAllpassFilter(AudioBuffer& buffer, heph_float delay_ms, heph_float gain)
	{
		const size_t delay_sample = delay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo);
		memcpy(resultBuffer.pData, buffer.pData, delay_sample * buffer.formatInfo.FrameSize());

		for (size_t i = delay_sample; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = buffer[i - delay_sample][j] - gain * buffer[i][j] + gain * resultBuffer[i - delay_sample][j];
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::FeedbackCombFilter(AudioBuffer& buffer, heph_float delay_ms, heph_float gain)
	{
		const size_t delay_sample = delay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo);
		memcpy(resultBuffer.pData, buffer.pData, delay_sample * buffer.formatInfo.FrameSize());

		for (size_t i = delay_sample; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelCount; j++)
			{
				resultBuffer[i][j] = buffer[i][j] + gain * resultBuffer[i - delay_sample][j];
			}
		}

		buffer = std::move(resultBuffer);
	}
#pragma endregion
}