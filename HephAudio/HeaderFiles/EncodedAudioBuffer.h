#pragma once
#include "HephAudioShared.h"
#include "Buffers/BufferBase.h"
#include "AudioFormatInfo.h"

namespace HephAudio
{
	class EncodedAudioBuffer : protected HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>
	{
	public:
		using HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>::Size;
		using HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>::begin;
		using HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>::end;

	protected:
		AudioFormatInfo formatInfo;

	public:
		EncodedAudioBuffer();
		explicit EncodedAudioBuffer(const AudioFormatInfo& formatInfo);
		EncodedAudioBuffer(const uint8_t* pData, size_t size, const AudioFormatInfo& formatInfo);
		EncodedAudioBuffer(const EncodedAudioBuffer&) = delete;
		EncodedAudioBuffer(EncodedAudioBuffer&& rhs) noexcept;
		virtual ~EncodedAudioBuffer() = default;
		EncodedAudioBuffer& operator=(const EncodedAudioBuffer&) = delete;
		EncodedAudioBuffer& operator=(EncodedAudioBuffer&& rhs) noexcept;
		virtual void Release() override;
		const AudioFormatInfo& GetAudioFormatInfo() const;
		void SetAudioFormatInfo(const AudioFormatInfo& newFormatInfo);
		void Add(const uint8_t* pData, size_t size);
	};
}