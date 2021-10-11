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
		static void Reverse(AudioBuffer& buffer);
		// Adds echo to the given buffer.
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		// Adds echo to the given subBuffer using the originalBuffer, subBufferFrameIndex and the subBuffers frame count to calculate the echo.
		// Note that this method only adds the echo data to the given subBuffer, thus you should provide the subBuffer data from the originalBuffer.
		static void EchoSubBuffer(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info, bool isReversed);
		static std::vector<AudioBuffer> SplitChannels(AudioBuffer& buffer);
		static AudioBuffer MergeChannels(std::vector<AudioBuffer>& channels);
		static void EncodeALAW(AudioBuffer& buffer);
		static void DecodeALAW(AudioBuffer& buffer);
		static void EncodeMULAW(AudioBuffer& buffer);
		static void DecodeMULAW(AudioBuffer& buffer);
	};
}