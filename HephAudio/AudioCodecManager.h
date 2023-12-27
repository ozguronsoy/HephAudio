#pragma once
#include "HephAudioFramework.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	namespace Codecs
	{
		class AudioCodecManager final
		{
		public:
			AudioCodecManager() = delete;
			AudioCodecManager(const AudioCodecManager&) = delete;
			AudioCodecManager& operator=(const AudioCodecManager&) = delete;
			static IAudioCodec* FindCodec(uint32_t codecTag);
			static void RegisterCodec(IAudioCodec* pNewCodec);
		};
	}
}