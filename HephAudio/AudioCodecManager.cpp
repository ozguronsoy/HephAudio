#include "AudioCodecManager.h"
#include "PcmCodec.h"
#include "IEEE_FloatCodec.h"
#include <vector>

namespace HephAudio
{
	namespace AudioCodecs
	{
		std::vector<IAudioCodec*> audioCodecs = {
			new PcmCodec(),
			new IEEE_FloatCodec()
		};

		IAudioCodec* AudioCodecManager::FindCodec(uint32_t formatTag)
		{
			for (size_t i = 0; i < audioCodecs.size(); i++)
			{
				if (audioCodecs[i]->Tag() == formatTag)
				{
					return audioCodecs[i];
				}
			}
			return nullptr;
		}
		void AudioCodecManager::AddCodec(IAudioCodec* pNewCodec)
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