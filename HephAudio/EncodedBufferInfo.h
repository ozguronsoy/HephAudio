#pragma once
#include "framework.h"
#include "AudioFormatInfo.h"

namespace HephAudio
{
	namespace AudioCodecs
	{
		/// <summary>
		/// Holds information about an encoded buffer.
		/// </summary>
		struct EncodedBufferInfo final
		{
			/// <summary>
			/// Pointer to the beginning of the encoded data buffer.
			/// </summary>
			void* pBuffer{ nullptr };
			/// <summary>
			/// Size of the encoded data buffer in bytes.
			/// </summary>
			size_t size_byte{ 0 };
			/// <summary>
			/// Size of the encoded data buffer in frames.
			/// </summary>
			size_t size_frame{ 0 };
			/// <summary>
			/// Information about the encoded audio data.
			/// </summary>
			AudioFormatInfo formatInfo{};
			/// <summary>
			/// Endianness of the encoded data.
			/// </summary>
			Endian endian{Endian::Little};
		};
	}
}