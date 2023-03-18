#pragma once
#include "framework.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	namespace AudioCodecs
	{
		/// <summary>
		/// Class for managing the IAudioCodec instances.
		/// </summary>
		class AudioCodecManager final
		{
		public:
			AudioCodecManager() = delete;
			AudioCodecManager(const AudioCodecManager&) = delete;
			AudioCodecManager& operator=(const AudioCodecManager&) = delete;
			/// <summary>
			/// Finds the audio codec with the provided tag.
			/// </summary>
			/// <param name="formatTag">The audio codec tag.</param>
			/// <returns>If found, the audio codec instance; otherwise, nullptr.</returns>
			static IAudioCodec* FindCodec(uint32_t formatTag);
			/// <summary>
			/// Adds a new codec or replaces the existing one with the same tag.
			/// </summary>
			/// <param name="pNewCodec"></param>
			static void RegisterCodec(IAudioCodec* pNewCodec);
		};
	}
}