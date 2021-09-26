#pragma once
#include "AudioBuffer.h"

namespace HephAudio
{
	class AudioProcessor final
	{
	private:
		AudioFormatInfo targetFormat;
	public:
		AudioProcessor(AudioFormatInfo targetFormat);
		AudioProcessor(const AudioProcessor&) = delete;
		AudioProcessor& operator=(const AudioProcessor&) = delete;
		// BPS = Bits Per Sample
		void ConvertBPS(AudioBuffer& buffer) const;
		// Mono to stereo, stereo to mono, from two channels to three channels...
		void ConvertChannels(AudioBuffer& buffer) const;
		void ConvertSampleRate(AudioBuffer& buffer) const;
		void ConvertSampleRate(AudioBuffer& buffer, size_t outFrameCount) const;
		static void Reverse(AudioBuffer& buffer);
		static std::vector<AudioBuffer> SplitChannels(AudioBuffer& buffer);
		static AudioBuffer MergeChannels(std::vector<AudioBuffer>& channels);
		static void EncodeALAW(AudioBuffer& buffer);
		static void DecodeALAW(AudioBuffer& buffer);
		static void EncodeMULAW(AudioBuffer& buffer);
		static void DecodeMULAW(AudioBuffer& buffer);
	};
}