#pragma once
#include "HephAudioShared.h"
#include "AudioFormatInfo.h"

namespace HephAudio
{
	namespace Codecs
	{
		struct EncodedBufferInfo final
		{
			void* pBuffer{ nullptr };
			size_t size_byte{ 0 };
			size_t size_frame{ 0 };
			AudioFormatInfo formatInfo{};
		};
	}
}