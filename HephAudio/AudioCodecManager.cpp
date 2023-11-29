#include "AudioCodecManager.h"
#include "PcmCodec.h"
#include "IEEE_FloatCodec.h"
#include "MuLawCodec.h"
#include "ALawCodec.h"
#include <vector>

namespace HephAudio
{
	namespace Codecs
	{
		std::vector<IAudioCodec*> audioCodecs = 
		{
			new PcmCodec(),
			new IEEE_FloatCodec(),
			new MuLawCodec(),
			new ALawCodec()
		};

		IAudioCodec* AudioCodecManager::FindCodec(uint32_t codecTag)
		{
			for (size_t i = 0; i < audioCodecs.size(); i++)
			{
				if (audioCodecs[i]->Tag() == codecTag)
				{
					return audioCodecs[i];
				}
			}
			return nullptr;
		}
		void AudioCodecManager::RegisterCodec(IAudioCodec* pNewCodec)
		{
			for (size_t i = 0; i < audioCodecs.size(); i++)
			{
				if (audioCodecs[i]->Tag() == pNewCodec->Tag())
				{
					delete audioCodecs[i];
					audioCodecs.erase(audioCodecs.begin() + i);
					break;
				}
			}
			audioCodecs.push_back(pNewCodec);
		}
	}
}