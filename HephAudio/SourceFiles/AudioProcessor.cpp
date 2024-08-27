#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "HephException.h"
#include "Fourier.h"
#include "File.h"
#include "ConsoleLogger.h"
#include "AudioChannelMixingLookupTables.h"
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
			encodedBufferInfo.size_byte = buffer.SizeAsByte();
			encodedBufferInfo.size_frame = buffer.frameCount;
			buffer = pCodec->Decode(encodedBufferInfo);

			encodedBufferInfo.formatInfo.bitsPerSample = outputBitsPerSample;
			encodedBufferInfo.size_byte = buffer.SizeAsByte();
			pCodec->Encode(buffer, encodedBufferInfo);

			buffer.formatInfo.bitRate = AudioFormatInfo::CalculateBitrate(buffer.formatInfo);
		}
		else
		{
			RAISE_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioProcessor::ChangeBitsPerSample", "Audio buffer is not raw PCM."));
		}
	}
	void AudioProcessor::ChangeChannelLayout(AudioBuffer& buffer, const AudioChannelLayout& outputChannelLayout)
	{
		if (buffer.formatInfo.channelLayout != outputChannelLayout)
		{
			if (buffer.formatInfo.channelLayout.mask == AudioChannelMask::Unknown || outputChannelLayout.mask == AudioChannelMask::Unknown ||
				buffer.formatInfo.channelLayout.count == 0 || outputChannelLayout.count == 0 ||
				buffer.formatInfo.channelLayout.count != AudioChannelLayout::GetChannelCount(buffer.formatInfo.channelLayout) ||
				outputChannelLayout.count != AudioChannelLayout::GetChannelCount(outputChannelLayout))
			{
				HEPHAUDIO_LOG("unsupported channel mapping, returning.", HEPH_CL_WARNING);
				return;
			}

			const std::vector<AudioChannelMask> inputMapping = AudioChannelLayout::GetChannelMapping(buffer.formatInfo.channelLayout);
			const std::vector<AudioChannelMask> outputMapping = AudioChannelLayout::GetChannelMapping(outputChannelLayout);

			if (inputMapping.size() == 0 || outputMapping.size() == 0)
			{
				HEPHAUDIO_LOG("unsupported channel mapping, returning.", HEPH_CL_WARNING);
				return;
			}

			hephaudio_channel_mixing_lookup_table_t* pTable = nullptr;
			switch (outputChannelLayout.count)
			{
			case 1:
				pTable = &AudioChannelMixingLookupTables::_mono_table;
				break;
			case 2:
				pTable = &AudioChannelMixingLookupTables::_stereo_table;
				break;
			case 3:
				pTable = &AudioChannelMixingLookupTables::_3_channels_table;
				break;
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				pTable = &AudioChannelMixingLookupTables::_other_channels_table;
				break;
			default:
				HEPHAUDIO_LOG("unsupported channel mapping, returning.", HEPH_CL_WARNING);
				return;
			}

			AudioBuffer resultBuffer(
				buffer.frameCount,
				AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.bitsPerSample, outputChannelLayout,
					buffer.formatInfo.sampleRate, buffer.formatInfo.bitRate, buffer.formatInfo.endian), 
				BufferFlags::AllocUninitialized
			);

			for (size_t i = 0; i < outputMapping.size(); i++)
			{
				size_t mappedChannelCount = 0;
				for (size_t j = 0; j < inputMapping.size(); j++)
				{
					if ((*pTable)[outputMapping[i]][inputMapping[j]] != 0)
					{
						mappedChannelCount++;
					}
				}

				if (mappedChannelCount > 0)
				{
					for (size_t j = 0; j < inputMapping.size(); j++)
					{
						const float mappingCoeff = (*pTable)[outputMapping[i]][inputMapping[j]];
						if (mappingCoeff != 0)
						{
							for (size_t k = 0; k < buffer.frameCount; k++)
							{
								resultBuffer[k][i] += buffer[k][j] * mappingCoeff / mappedChannelCount;
							}
						}
					}
				}
			}

			buffer = std::move(resultBuffer);
		}
	}
	void AudioProcessor::ChangeSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate)
	{
		const double srRatio = (double)outputSampleRate / (double)buffer.formatInfo.sampleRate;
		if (srRatio != 1.0)
		{
			const size_t targetFrameCount = ceil((double)buffer.frameCount * srRatio);
			AudioBuffer resultBuffer(targetFrameCount, 
				AudioFormatInfo(buffer.formatInfo.formatTag, buffer.formatInfo.bitsPerSample, buffer.formatInfo.channelLayout, 
					outputSampleRate, buffer.formatInfo.bitRate, buffer.formatInfo.endian),
				BufferFlags::AllocUninitialized);

			for (size_t i = 0; i < targetFrameCount; i++)
			{
				const double resampleIndex = i / srRatio;
				const double rho = resampleIndex - floor(resampleIndex);

				for (size_t j = 0; j < buffer.formatInfo.channelLayout.count && (resampleIndex + 1) < buffer.frameCount; j++)
				{
					resultBuffer[i][j] = buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho;
				}
			}

			buffer = std::move(resultBuffer);
		}
	}
	std::vector<DoubleBuffer> AudioProcessor::SplitChannels(const AudioBuffer& buffer)
	{
		std::vector<DoubleBuffer> channels(buffer.formatInfo.channelLayout.count, DoubleBuffer(buffer.frameCount, BufferFlags::AllocUninitialized));
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				channels[j][i] = buffer[i][j];
			}
		}
		return channels;
	}
	AudioBuffer AudioProcessor::MergeChannels(const std::vector<DoubleBuffer>& channels, uint32_t sampleRate)
	{
		if (channels.size() == 0)
		{
			HEPHAUDIO_LOG("no channels to merge, returning empty buffer.", HEPH_CL_WARNING);
			return AudioBuffer();
		}

		AudioBuffer resultBuffer(channels[0].Size(), HEPHAUDIO_INTERNAL_FORMAT(AudioChannelLayout::DefaultChannelLayout(channels.size()), sampleRate));
		for (size_t i = 0; i < resultBuffer.frameCount; i++)
		{
			for (size_t j = 0; j < resultBuffer.formatInfo.channelLayout.count; j++)
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
		encodedBufferInfo.size_byte = buffer.SizeAsByte();
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
				for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
				{
					HephCommon::ChangeEndian((((uint8_t*)buffer.pData) + i * frameSize + j * bytesPerSample), bytesPerSample);
				}
			}
			buffer.formatInfo.endian = !buffer.formatInfo.endian;
		}
	}
#pragma endregion
#pragma region Sound Effects
	void AudioProcessor::Echo(AudioBuffer& buffer, EchoInfo info)
	{
		const size_t delayFrameCount = buffer.formatInfo.sampleRate * info.reflectionDelay_s;
		const size_t echoStartFrame = buffer.frameCount * info.echoStartPosition;
		const AudioBuffer echoBuffer = buffer.SubBuffer(echoStartFrame, buffer.frameCount * info.echoEndPosition - echoStartFrame);
		double factor = info.decayFactor;
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
				for (size_t k = 0; k < buffer.formatInfo.channelLayout.count; k++)
				{
					buffer[j][k] += echoBuffer[j - startFrameIndex][k] * factor;
				}
			}
		}
	}
	void AudioProcessor::LinearPanning(AudioBuffer& buffer, double panningFactor)
	{
		if (buffer.formatInfo.channelLayout.count == 2)
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
		if (buffer.formatInfo.channelLayout.count == 2)
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
		if (buffer.formatInfo.channelLayout.count == 2)
		{
			const double volume = panningFactor * 0.5 + 0.5;
			const double rightVolume = sin(volume * HEPH_MATH_PI * 0.5);
			const double leftVolume = sin((1.0 - volume) * HEPH_MATH_PI * 0.5);

			for (size_t i = 0; i < buffer.frameCount; i++)
			{
				buffer[i][0] *= leftVolume;
				buffer[i][1] *= rightVolume;
			}
		}
	}
	void AudioProcessor::Tremolo(AudioBuffer& buffer, double depth, const Oscillator& lfo)
	{
		const double wetFactor = depth * 0.5;
		const double dryFactor = 1.0 - wetFactor;
		const DoubleBuffer lfoPeriodBuffer = lfo.GenerateBuffer();

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const double& lfoSample = lfoPeriodBuffer[i % lfoPeriodBuffer.Size()];
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				buffer[i][j] *= wetFactor * lfoSample + dryFactor;
			}
		}
	}
	void AudioProcessor::Vibrato(AudioBuffer& buffer, double depth, double extent_semitone, const Oscillator& lfo)
	{
		const double resampleDelta = buffer.formatInfo.sampleRate * (pow(2, extent_semitone / 12.0) - 1.0);
		const double wetFactor = depth * 0.5;
		const double dryFactor = 1.0 - wetFactor;

		DoubleBuffer lfoPeriodBuffer(ceil(lfo.sampleRate / lfo.frequency), BufferFlags::AllocUninitialized);
		for (size_t i = 0; i < lfoPeriodBuffer.Size(); i++)
		{
			lfoPeriodBuffer[i] = lfo[i] * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo, BufferFlags::AllocUninitialized);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			const double resampleIndex = i + lfoPeriodBuffer[i % lfoPeriodBuffer.Size()] * resampleDelta;
			const double rho = resampleIndex - floor(resampleIndex);

			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				heph_audio_sample_t wetSample = 0.0;
				if (resampleIndex + 1.0 < buffer.frameCount)
				{
					wetSample = buffer[resampleIndex][j] * (1.0 - rho) + buffer[resampleIndex + 1.0][j] * rho;
				}
				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::Chorus(AudioBuffer& buffer, double depth, double feedbackGain, double baseDelay_ms, double delay_ms, double extent_semitone, const Oscillator& lfo)
	{
		const double baseDelay_sample = baseDelay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const double delay_sample = delay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const double resampleDelta = buffer.formatInfo.sampleRate * (pow(2, extent_semitone / 12.0) - 1.0);
		const double wetFactor = depth * 0.5;
		const double dryFactor = 1.0 - wetFactor;

		DoubleBuffer feedbackBuffer(buffer.formatInfo.channelLayout.count);
		DoubleBuffer lfoPeriodBuffer(ceil(lfo.sampleRate / lfo.frequency), BufferFlags::AllocUninitialized);
		for (size_t i = 0; i < lfoPeriodBuffer.Size(); i++)
		{
			lfoPeriodBuffer[i] = lfo[i] * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo, BufferFlags::AllocUninitialized);
		memcpy(resultBuffer.pData, buffer.pData, round(delay_sample + baseDelay_sample) * buffer.formatInfo.FrameSize());

		for (size_t i = baseDelay_sample + delay_sample + 1; i < buffer.frameCount; i++)
		{
			const size_t currentDelay_sample = round(lfoPeriodBuffer[i % lfoPeriodBuffer.Size()] * delay_sample + baseDelay_sample);
			const double resampleIndex = (i - currentDelay_sample) + lfoPeriodBuffer[i % lfoPeriodBuffer.Size()] * resampleDelta;
			const double rho = resampleIndex - floor(resampleIndex);

			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				heph_audio_sample_t wetSample;
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
	void AudioProcessor::Flanger(AudioBuffer& buffer, double depth, double feedbackGain, double baseDelay_ms, double delay_ms, const Oscillator& lfo)
	{
		const double baseDelay_sample = baseDelay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const double delay_sample = delay_ms * 1e-3 * buffer.formatInfo.sampleRate;
		const double wetFactor = depth * 0.5;
		const double dryFactor = 1.0 - wetFactor;

		DoubleBuffer feedbackBuffer(buffer.formatInfo.channelLayout.count);
		DoubleBuffer lfoPeriodBuffer(ceil(lfo.sampleRate / lfo.frequency), BufferFlags::AllocUninitialized);
		for (size_t i = 0; i < lfoPeriodBuffer.Size(); i++)
		{
			lfoPeriodBuffer[i] = lfo[i] * 0.5 + 0.5;
		}

		AudioBuffer resultBuffer(buffer.frameCount, buffer.formatInfo, BufferFlags::AllocUninitialized);
		memcpy(resultBuffer.pData, buffer.pData, round(delay_sample + baseDelay_sample) * buffer.formatInfo.FrameSize());

		for (size_t i = baseDelay_sample + delay_sample + 1; i < buffer.frameCount; i++)
		{
			const size_t currentDelay_sample = round(lfoPeriodBuffer[i % lfoPeriodBuffer.Size()] * delay_sample + baseDelay_sample);
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				const heph_audio_sample_t wetSample = (buffer[i - currentDelay_sample][j] * 0.5) + (feedbackBuffer[j] * 0.5);
				resultBuffer[i][j] = wetFactor * wetSample + dryFactor * buffer[i][j];
				feedbackBuffer[j] = feedbackGain * wetSample;
			}
		}

		buffer = std::move(resultBuffer);
	}
	void AudioProcessor::FixOverflow(AudioBuffer& buffer)
	{
		const heph_audio_sample_t maxSample = buffer.AbsMax();
		if (maxSample > HEPH_AUDIO_SAMPLE_MAX)
		{
			buffer /= maxSample;
		}
	}
	void AudioProcessor::Normalize(AudioBuffer& buffer, heph_audio_sample_t peakAmplitude)
	{
		const heph_audio_sample_t maxSample = buffer.AbsMax();
		if (maxSample != 0 && maxSample != peakAmplitude)
		{
			buffer *= (double)peakAmplitude / maxSample;
		}
	}
	void AudioProcessor::RmsNormalize(AudioBuffer& buffer, double rms)
	{
		double sumOfSamplesSquared = 0.0;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				sumOfSamplesSquared += buffer[i][j] * buffer[i][j];
			}
		}

		if (sumOfSamplesSquared != 0.0)
		{
			buffer *= rms * sqrt(buffer.frameCount * buffer.formatInfo.channelLayout.count / sumOfSamplesSquared);
		}
	}
	void AudioProcessor::HardClipDistortion(AudioBuffer& buffer, double clippingLevel_dB)
	{
		const heph_audio_sample_t clippingLevel = HephAudio::DecibelToGain(clippingLevel_dB) * HEPH_AUDIO_SAMPLE_MAX;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				heph_audio_sample_t& sample = buffer[i][j];
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
	void AudioProcessor::ArctanDistortion(AudioBuffer& buffer, double alpha)
	{
		alpha = 5 + alpha * 10;
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				const double fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
				buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(atan(alpha * fltSample) * (2.0 / HEPH_MATH_PI));
			}
		}
	}
	void AudioProcessor::CubicDistortion(AudioBuffer& buffer, double a)
	{
		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				const double fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
				buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(fltSample - a * (1.0 / 3.0) * fltSample * fltSample * fltSample);
			}
		}
	}
	void AudioProcessor::Overdrive(AudioBuffer& buffer, double drive)
	{
		drive *= 0.8;

		const double a = sin(drive * HEPH_MATH_PI / 2.0);
		const double k = 2.0 * a / (1.0 - a);

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				const double fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
				buffer[i][j] = HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT((1.0 + k) * fltSample / (1.0 + k * abs(fltSample)));
			}
		}
	}
	void AudioProcessor::Fuzz(AudioBuffer& buffer, double depth, double alpha)
	{
		const double wetFactor = depth * 0.5;
		const double dryFactor = 1.0 - wetFactor;

		for (size_t i = 0; i < buffer.frameCount; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				const double fltSample = HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(buffer[i][j]);
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
	void AudioProcessor::LinearFadeIn(AudioBuffer& buffer, double duration_s)
	{
		AudioProcessor::LinearFadeIn(buffer, duration_s, 0);
	}
	void AudioProcessor::LinearFadeIn(AudioBuffer& buffer, double duration_s, size_t startIndex)
	{
		const double duration_sample = round(duration_s * buffer.formatInfo.sampleRate);
		const size_t endIndex = HEPH_MATH_MIN(startIndex + (size_t)duration_sample, buffer.frameCount);
		for (size_t i = startIndex; i < endIndex; i++)
		{
			const double factor = (i - startIndex) / duration_sample;
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				buffer[i][j] *= factor;
			}
		}
	}
	void AudioProcessor::LinearFadeOut(AudioBuffer& buffer, double duration_s)
	{
		const size_t duration_sample = round(duration_s * buffer.formatInfo.sampleRate);
		AudioProcessor::LinearFadeOut(buffer, duration_s, buffer.frameCount > duration_sample ? (buffer.frameCount - duration_sample) : 0);
	}
	void AudioProcessor::LinearFadeOut(AudioBuffer& buffer, double duration_s, size_t startIndex)
	{
		const double duration_sample = round(duration_s * buffer.formatInfo.sampleRate);
		const size_t endIndex = HEPH_MATH_MIN(startIndex + (size_t)duration_sample, buffer.frameCount);
		for (size_t i = startIndex; i < endIndex; i++)
		{
			const double factor = (endIndex - i) / duration_sample;
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
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
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<DoubleBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].SubBuffer(i, fftSize) * windowBuffer, fftSize);

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
		auto applyEqualizer = [](AudioBuffer* buffer, const DoubleBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, const std::vector<EqualizerInfo>* const infos
			, double overflowFactor)
			{
				DoubleBuffer channel(frameCount, BufferFlags::AllocUninitialized);
				for (size_t i = 0; i < frameCount && (i + frameIndex) < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.Size(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.SubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

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
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelLayout.count;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = ceil((double)buffer.frameCount / (double)threadCountPerChannel);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelLayout.count * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				threads[i * buffer.formatInfo.channelLayout.count + j] = std::thread(applyEqualizer, &buffer, &windowBuffer
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
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, double speed, Window& window)
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
	void AudioProcessor::ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t windowSize, double speed, Window& window)
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
		const double frameCountRatio = 1.0 / speed;
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		AudioBuffer resultBuffer(buffer.frameCount * frameCountRatio, buffer.formatInfo, BufferFlags::AllocUninitialized);

		if (speed <= 1.0)
		{
			const double overflowFactor = ((double)hopSize) / ((double)windowSize);
			for (size_t i = 0; i < buffer.frameCount; i += hopSize)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
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
			const double overflowFactor = ((double)hopSize) / ((double)windowSize) * frameCountRatio;
			for (size_t i = 0; i < buffer.frameCount; i += hopSize)
			{
				for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
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
	void AudioProcessor::PitchShift(AudioBuffer& buffer, double pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShift(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, pitchChange_semitone, window);
	}
	void AudioProcessor::PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, Window& window)
	{
		constexpr double twopi = 2.0 * HEPH_MATH_PI;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const double shiftFactor = pow(2.0, pitchChange_semitone / 12.0);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<DoubleBuffer> channels = AudioProcessor::SplitChannels(buffer);
		std::vector<DoubleBuffer> lastAnalysisPhases(buffer.formatInfo.channelLayout.count, DoubleBuffer(nyquistFrequency));
		std::vector<DoubleBuffer> lastSynthesisPhases(buffer.formatInfo.channelLayout.count, DoubleBuffer(nyquistFrequency));
		std::vector<DoubleBuffer> synthesisMagnitudes(buffer.formatInfo.channelLayout.count, DoubleBuffer(nyquistFrequency));
		std::vector<DoubleBuffer> synthesisFrequencies(buffer.formatInfo.channelLayout.count, DoubleBuffer(nyquistFrequency));

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				synthesisMagnitudes[j].Reset();
				synthesisFrequencies[j].Reset();
				ComplexBuffer complexBuffer(channels[j].SubBuffer(i, fftSize) * windowBuffer);
				Fourier::FFT(complexBuffer, fftSize);

				for (size_t k = 0; k < nyquistFrequency; k++)
				{
					const double phase = complexBuffer[k].Phase();
					double phaseRemainder = phase - lastAnalysisPhases[j][k] - twopi * k * hopSize / fftSize;
					phaseRemainder = phaseRemainder >= 0 ? (fmod(phaseRemainder + HEPH_MATH_PI, twopi) - HEPH_MATH_PI) : (fmod(phaseRemainder - HEPH_MATH_PI, -twopi) + HEPH_MATH_PI);

					const size_t newBin = floor(k * shiftFactor + 0.5);
					if (newBin < nyquistFrequency)
					{
						synthesisMagnitudes[j][newBin] += complexBuffer[k].Magnitude();
						synthesisFrequencies[j][newBin] = (k + phaseRemainder * fftSize / twopi / hopSize) * shiftFactor;
					}

					lastAnalysisPhases[j][k] = phase;
				}

				for (size_t k = 0; k < nyquistFrequency; k++)
				{
					double synthesisPhase = twopi * hopSize / fftSize * synthesisFrequencies[j][k] + lastSynthesisPhases[j][k];
					synthesisPhase = synthesisPhase >= 0 ? (fmod(synthesisPhase + HEPH_MATH_PI, twopi) - HEPH_MATH_PI) : (fmod(synthesisPhase - HEPH_MATH_PI, -twopi) + HEPH_MATH_PI);

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
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, double pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShiftMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, pitchChange_semitone, 0, window);
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, double pitchChange_semitone, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::PitchShiftMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, pitchChange_semitone, threadCountPerChannel, window);
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, Window& window)
	{
		AudioProcessor::PitchShiftMT(buffer, hopSize, fftSize, pitchChange_semitone, 0, window);
	}
	void AudioProcessor::PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, size_t threadCountPerChannel, Window& window)
	{
		auto applyPitchShift = [](AudioBuffer* buffer, const DoubleBuffer* pWindowBuffer, size_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, double shiftFactor
			, double overflowFactor)
			{
				constexpr double twopi = 2.0 * HEPH_MATH_PI;

				DoubleBuffer lastAnalysisPhases(nyquistFrequency);
				DoubleBuffer lastSynthesisPhases(nyquistFrequency);
				DoubleBuffer synthesisMagnitudes(nyquistFrequency);
				DoubleBuffer synthesisFrequencies(nyquistFrequency);

				DoubleBuffer channel(frameCount, BufferFlags::AllocUninitialized);
				for (size_t i = 0; i < frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.Size(); i += hopSize)
				{
					synthesisMagnitudes.Reset();
					synthesisFrequencies.Reset();

					ComplexBuffer complexBuffer(channel.SubBuffer(i, fftSize) * (*pWindowBuffer));
					Fourier::FFT(complexBuffer, fftSize);

					for (size_t k = 0; k < nyquistFrequency; k++)
					{
						const double phase = complexBuffer[k].Phase();
						double phaseRemainder = phase - lastAnalysisPhases[k] - twopi * k * hopSize / fftSize;
						phaseRemainder = phaseRemainder >= 0 ? (fmod(phaseRemainder + HEPH_MATH_PI, twopi) - HEPH_MATH_PI) : (fmod(phaseRemainder - HEPH_MATH_PI, -twopi) + HEPH_MATH_PI);

						const size_t newBin = floor(k * shiftFactor + 0.5);
						if (newBin < nyquistFrequency)
						{
							synthesisMagnitudes[newBin] += complexBuffer[k].Magnitude();
							synthesisFrequencies[newBin] = (k + phaseRemainder * fftSize / twopi / hopSize) * shiftFactor;
						}

						lastAnalysisPhases[k] = phase;
					}

					for (size_t k = 0; k < nyquistFrequency; k++)
					{
						double synthesisPhase = twopi * hopSize / fftSize * synthesisFrequencies[k] + lastSynthesisPhases[k];
						synthesisPhase = synthesisPhase >= 0 ? (fmod(synthesisPhase + HEPH_MATH_PI, twopi) - HEPH_MATH_PI) : (fmod(synthesisPhase - HEPH_MATH_PI, -twopi) + HEPH_MATH_PI);

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
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelLayout.count;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const double shiftFactor = pow(2.0, pitchChange_semitone / 12.0);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = ceil((double)buffer.frameCount / (double)threadCountPerChannel);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelLayout.count * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				threads[i * buffer.formatInfo.channelLayout.count + j] = std::thread(applyPitchShift, &buffer, &windowBuffer
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
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, double cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<DoubleBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].SubBuffer(i, fftSize) * windowBuffer, fftSize);

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
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, double cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, hopSize, fftSize, cutoffFreq, 0, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, double cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::LowPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const DoubleBuffer* pWindowBuffer, size_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, size_t startIndex
			, double overflowFactor)
			{
				DoubleBuffer channel(frameCount, BufferFlags::AllocUninitialized);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.Size(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.SubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

					for (int64_t j = startIndex; j < nyquistFrequency; j++)
					{
						complexBuffer[j] = Complex();
						complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
					}

					Fourier::IFFT(complexBuffer, false);
					for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->frameCount; j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / (double)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelLayout.count;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = ceil((double)buffer.frameCount / (double)threadCountPerChannel);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelLayout.count * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				threads[i * buffer.formatInfo.channelLayout.count + j] = std::thread(applyFilter, &buffer, &windowBuffer
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
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, double cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<DoubleBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].SubBuffer(i, fftSize) * windowBuffer, fftSize);

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
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, double cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, hopSize, fftSize, cutoffFreq, 0, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, double cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::HighPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, cutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const DoubleBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t stopIndex, double overflowFactor)
			{
				DoubleBuffer channel(frameCount, BufferFlags::AllocUninitialized);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.Size(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.SubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);
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
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelLayout.count;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, cutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = ceil(((double)buffer.frameCount) / ((double)threadCountPerChannel));
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelLayout.count * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				threads[i * buffer.formatInfo.channelLayout.count + j] = std::thread(applyFilter, &buffer, &windowBuffer
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
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<DoubleBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].SubBuffer(i, fftSize) * windowBuffer, fftSize);

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
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, hopSize, fftSize, lowCutoffFreq, highCutoffFreq, 0, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::BandPassFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const DoubleBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, size_t startIndex
			, size_t stopIndex, double overflowFactor)
			{
				DoubleBuffer channel(frameCount, BufferFlags::AllocUninitialized);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.Size(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.SubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

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
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / (double)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelLayout.count;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = ceil((double)buffer.frameCount / (double)threadCountPerChannel);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelLayout.count * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				threads[i * buffer.formatInfo.channelLayout.count + j] = std::thread(applyFilter, &buffer, &windowBuffer
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
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilter(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<DoubleBuffer> channels = AudioProcessor::SplitChannels(buffer);

		buffer.Reset();

		for (size_t i = 0; i < buffer.frameCount; i += hopSize)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				ComplexBuffer complexBuffer = Fourier::FFT(channels[j].SubBuffer(i, fftSize) * windowBuffer, fftSize);
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
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, hopSize, fftSize, lowCutoffFreq, highCutoffFreq, 0, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		AudioProcessor::BandCutFilterMT(buffer, AudioProcessor::DEFAULT_HOP_SIZE, AudioProcessor::DEFAULT_FFT_SIZE, lowCutoffFreq, highCutoffFreq, threadCountPerChannel, window);
	}
	void AudioProcessor::BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window)
	{
		auto applyFilter = [](AudioBuffer* buffer, const DoubleBuffer* pWindowBuffer, uint16_t channelIndex
			, size_t frameIndex, size_t frameCount, size_t hopSize
			, size_t fftSize, size_t nyquistFrequency, size_t startIndex
			, size_t stopIndex, double overflowFactor)
			{
				DoubleBuffer channel(frameCount, BufferFlags::AllocUninitialized);
				for (size_t i = 0; i < frameCount && i + frameIndex < buffer->frameCount; i++)
				{
					channel[i] = (*buffer)[i + frameIndex][channelIndex];
					(*buffer)[i + frameIndex][channelIndex] = 0;
				}

				for (size_t i = 0; i < channel.Size(); i += hopSize)
				{
					ComplexBuffer complexBuffer = Fourier::FFT(channel.SubBuffer(i, fftSize) * (*pWindowBuffer), fftSize);

					for (size_t j = startIndex; j <= stopIndex; j++)
					{
						complexBuffer[j] = Complex();
						complexBuffer[fftSize - j - 1] = complexBuffer[j].Conjugate();
					}

					Fourier::IFFT(complexBuffer, false);
					for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->frameCount; j++, k++)
					{
						(*buffer)[k][channelIndex] += complexBuffer[j].real() * overflowFactor * (*pWindowBuffer)[j] / (double)fftSize;
					}
				}
			};

		if (threadCountPerChannel == 0)
		{
			threadCountPerChannel = std::thread::hardware_concurrency() / buffer.formatInfo.channelLayout.count;
		}

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const size_t nyquistFrequency = fftSize * 0.5;
		const uint64_t startIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, lowCutoffFreq);
		const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.formatInfo.sampleRate, fftSize, highCutoffFreq);
		const double overflowFactor = ((double)hopSize) / ((double)fftSize);
		const size_t frameCountPerThread = ceil((double)buffer.frameCount / (double)threadCountPerChannel);
		const DoubleBuffer windowBuffer = window.GenerateBuffer();
		std::vector<std::thread> threads(buffer.formatInfo.channelLayout.count * threadCountPerChannel);

		for (size_t i = 0; i < threadCountPerChannel; i++)
		{
			for (size_t j = 0; j < buffer.formatInfo.channelLayout.count; j++)
			{
				threads[i * buffer.formatInfo.channelLayout.count + j] = std::thread(applyFilter, &buffer, &windowBuffer
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