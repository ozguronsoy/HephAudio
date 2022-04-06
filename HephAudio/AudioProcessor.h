#pragma once
#include "AudioBuffer.h"
#include "EchoInfo.h"

namespace HephAudio
{
	class AudioProcessor final
	{
	private:
		AudioFormatInfo targetFormat;
	public:
		AudioProcessor(AudioFormatInfo targetFormat);
		// BPS = Bits Per Sample
		void ConvertBPS(AudioBuffer& buffer) const;
		// Mono to stereo, stereo to mono, from two channels to three channels...
		void ConvertChannels(AudioBuffer& buffer) const;
		void ConvertSampleRate(AudioBuffer& buffer) const;
		void ConvertSampleRate(AudioBuffer& buffer, size_t outFrameCount) const;
		void Mix(AudioBuffer& outputBuffer, std::vector<AudioBuffer> inputBuffers) const;
		static void Reverse(AudioBuffer& buffer);
		// Adds echo to the given buffer.
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		// Adds echo to the given subBuffer using the originalBuffer, subBufferFrameIndex and the subBuffers frame count to calculate the echo.
		// Note that this method only adds the echo data to the given subBuffer, thus you should provide the subBuffer data from the originalBuffer.
		static void EchoSubBuffer(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info);
		// All parameters except buffer are in hertz.
		static void LowPassFilter(AudioBuffer& buffer, uint16_t cutoffFreq, uint16_t transitionBandLength);
		// All parameters except buffer are in hertz.
		static void HighPassFilter(AudioBuffer& buffer, uint16_t cutoffFreq, uint16_t transitionBandLength);
		// All parameters except buffer are in hertz.
		static void BandPassFilter(AudioBuffer& buffer, uint16_t lowCutoffFreq, uint16_t highCutoffFreq, uint16_t transitionBandLength);
		// All parameters except buffer are in hertz.
		static void BandCutFilter(AudioBuffer& buffer, uint16_t lowCutoffFreq, uint16_t highCutoffFreq, uint16_t transitionBandLength);
		static void TriangleWindow(AudioBuffer& buffer);
		static void HannWindow(AudioBuffer& buffer);
		static std::vector<AudioBuffer> SplitChannels(const AudioBuffer& buffer);
		static AudioBuffer MergeChannels(const std::vector<AudioBuffer>& channels);
		static void EncodeALAW(AudioBuffer& buffer);
		static void DecodeALAW(AudioBuffer& buffer);
		static void EncodeMULAW(AudioBuffer& buffer);
		static void DecodeMULAW(AudioBuffer& buffer);
	};
}