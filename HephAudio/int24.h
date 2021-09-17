#pragma once
#include "framework.h"

#define UINT24_MAX 16777215
#define INT24_MAX 8388607
#define INT24_MIN -8388608

#pragma pack(push, 1)
namespace HephAudio
{
	namespace Structs
	{
		struct HephAudioAPI int24
		{
			int value : 24;
		};
		struct HephAudioAPI uint24
		{
			unsigned int value : 24;
		};
	}
}
#pragma pack(pop)