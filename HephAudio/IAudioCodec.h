#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "EncodedBufferInfo.h"

namespace HephAudio
{
	namespace AudioCodecs
	{
		/// <summary>
		/// Provides methods for decoding and encoding audio data.
		/// </summary>
		class IAudioCodec
		{
		public:
			/// <summary>
			/// Frees the resources.
			/// </summary>
			virtual ~IAudioCodec() = default;
			/// <summary>
			/// The tag of the audio codec.
			/// </summary>
			virtual uint32_t Tag() const noexcept = 0;
			/// <summary>
			/// Decodes the encoded buffer to an HephAudio buffer.
			/// </summary>
			/// <param name="encodedBufferInfo">Holds the encoded data and the necessary information about how to decode it.</param>
			/// <returns>Decoded audio buffer.</returns>
			virtual AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) const = 0;
			/// <summary>
			/// Encodes the HephAudio buffer to the current codec.
			/// </summary>
			/// <param name="bufferToEncode">The hephaudio buffer to encode.</param>
			/// <param name="encodedBufferInfo">Holds information about where and how to encode the audio data.</param>
			virtual void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const = 0;
		};
	}
}