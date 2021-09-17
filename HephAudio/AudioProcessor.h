#pragma once
#include "AudioBuffer.h"

namespace HephAudio
{
	class HephAudioAPI AudioProcessor final
	{
	private:
		WAVEFORMATEX targetFormat;
	public:
		AudioProcessor(WAVEFORMATEX targetFormat);
		AudioProcessor(const AudioProcessor&) = delete;
		AudioProcessor& operator=(const AudioProcessor&) = delete;
		// BPS = Bits Per Sample
		void ConvertBPS(AudioBuffer& buffer) const;
		// Mono to stereo, stereo to mono, from two channels to three channels...
		void ConvertChannels(AudioBuffer& buffer) const;
		void ConvertSampleRate(AudioBuffer& buffer, size_t outFrameCount = 0) const;
		static void Reverse(AudioBuffer& buffer);
		static std::vector<AudioBuffer> SplitChannels(AudioBuffer& buffer);
		static AudioBuffer MergeChannels(std::vector<AudioBuffer>& channels);
	private:
		static bool CompareWFX(const WAVEFORMATEX lhs, const WAVEFORMATEX rhs) noexcept;
	};
}