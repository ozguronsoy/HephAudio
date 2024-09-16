#pragma once
#include "HephAudioShared.h"
#include "AudioChannelLayout.h"
#include <unordered_map>

/** @file */

namespace HephAudio
{
	typedef std::unordered_map<HephAudio::AudioChannelMask, std::unordered_map<HephAudio::AudioChannelMask, float>> hephaudio_channel_mixing_lookup_table_t;

	/**
	 * provides the lookup tables for channel mixing.
	 * 
	 */
	class AudioChannelMixingLookupTables
	{
	public:
		AudioChannelMixingLookupTables() = delete;
		AudioChannelMixingLookupTables(const AudioChannelMixingLookupTables&) = delete;
		AudioChannelMixingLookupTables& operator=(const AudioChannelMixingLookupTables&) = delete;
	public:
		static hephaudio_channel_mixing_lookup_table_t _mono_table;
		static hephaudio_channel_mixing_lookup_table_t _stereo_table;
		static hephaudio_channel_mixing_lookup_table_t _3_channels_table;
		static hephaudio_channel_mixing_lookup_table_t _other_channels_table;
	};
}