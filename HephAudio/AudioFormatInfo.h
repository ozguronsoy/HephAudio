#pragma once
#include "framework.h"
#include <cstdint>
#ifdef _WIN32
#include <Mmreg.h>
#include <mmeapi.h>
#endif

namespace HephAudio
{
	namespace Structs
	{
		struct AudioFormatInfo
		{
			uint16_t formatTag;        
			uint16_t channelCount;
			uint32_t sampleRate;
			uint16_t bitsPerSample;
			uint16_t headerSize;
			AudioFormatInfo();
			AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate);
			virtual ~AudioFormatInfo() = default;
			bool operator==(const AudioFormatInfo& rhs) const;
			bool operator!=(const AudioFormatInfo& rhs) const;
			uint16_t FrameSize() const noexcept;
			uint32_t BitRate() const noexcept;
			uint32_t ByteRate() const noexcept;
#ifdef _WIN32
			AudioFormatInfo(const WAVEFORMATEX& wfx);
			operator WAVEFORMATEX() const;
#endif
		};
	}
}
#pragma region Exports
#if defined(_WIN32)
using namespace HephAudio::Structs;
extern "C" __declspec(dllexport) AudioFormatInfo * _stdcall CreateFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate);
extern "C" __declspec(dllexport) uint16_t _stdcall FormatInfoGetFormatTag(AudioFormatInfo * pFormatInfo);
extern "C" __declspec(dllexport) void _stdcall FormatInfoSetFormatTag(AudioFormatInfo * pFormatInfo, uint16_t formatTag);
extern "C" __declspec(dllexport) uint16_t _stdcall FormatInfoGetChannelCount(AudioFormatInfo * pFormatInfo);
extern "C" __declspec(dllexport) void _stdcall FormatInfoSetChannelCount(AudioFormatInfo * pFormatInfo, uint16_t channelCount);
extern "C" __declspec(dllexport) uint32_t _stdcall FormatInfoGetSampleRate(AudioFormatInfo * pFormatInfo);
extern "C" __declspec(dllexport) void _stdcall FormatInfoSetSampleRate(AudioFormatInfo * pFormatInfo, uint32_t sampleRate);
extern "C" __declspec(dllexport) uint16_t _stdcall FormatInfoGetBitsPerSample(AudioFormatInfo * pFormatInfo);
extern "C" __declspec(dllexport) void _stdcall FormatInfoSetBitsPerSample(AudioFormatInfo * pFormatInfo, uint16_t bitsPerSample);
extern "C" __declspec(dllexport) void _stdcall DestroyFormatInfo(AudioFormatInfo * pFormatInfo);
#endif
#pragma endregion