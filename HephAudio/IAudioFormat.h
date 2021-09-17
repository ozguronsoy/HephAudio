#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "AudioFile.h"
#include <string>
#include <memory>

namespace HephAudio
{
	namespace Formats
	{
		enum class HephAudioAPI Endian : uint8_t
		{
			Little = 0,
			Big = 1
		};
		constexpr Endian operator!(const Endian& lhs) { return lhs == Endian::Big ? Endian::Little : Endian::Big; }
		class HephAudioAPI IAudioFormat
		{
		public:
			virtual ~IAudioFormat() = default;
			// Gets the extension of the current format.
			virtual std::wstring Extension() const noexcept = 0;
			// Reads audio data from the given file and converts it to a pcm buffer.
			// File format must be the current format or this method will return an empty buffer.
			virtual AudioBuffer ReadFile(AudioFile& file) const = 0;
			// Converts pcm buffer to current format and then writes it into a file.
			virtual bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const = 0;
			static Endian GetSystemEndian()
			{
				int32_t n = 1;
				return (*(char*)&n == 1) ? Endian::Little : Endian::Big;
			}
		protected:
			// Reads data from buffer in given type.
			template<class T>
			static T Read(void* audioFileBuffer, uint32_t paddingInBytes, Endian endian)
			{
				Endian systemEndian = GetSystemEndian();
				uint8_t* afb = (uint8_t*)audioFileBuffer;
				T data = *(T*)&afb[paddingInBytes];
				if (systemEndian != endian)
				{
					switch (sizeof(T))
					{
					case 1:
						break;
					case 2:
						return ChangeEndian16(data);
					case 4:
						return ChangeEndian32(data);
					case 8:
						return ChangeEndian64(data);
					default:
						break;
					}
				}
				return data;
			}
			static uint16_t ChangeEndian16(uint16_t value)
			{
				value = (value >> 8) | (value << 8);
				return value;
			}
			static uint16_t ChangeEndian16(uint16_t value, Endian to)
			{
				if (to == GetSystemEndian())
				{
					return value;
				}
				return ChangeEndian16(value);
			}
			static uint32_t ChangeEndian32(uint32_t value)
			{
				value = ((value >> 24) & 0xff) |
					((value << 8) & 0xff0000) |
					((value >> 8) & 0xff00) |
					((value << 24) & 0xff000000);
				return value;
			}
			static uint32_t ChangeEndian32(uint32_t  value, Endian to)
			{
				if (to == GetSystemEndian())
				{
					return value;
				}
				return ChangeEndian32(value);
			}
			static uint64_t ChangeEndian64(uint64_t value)
			{
				value = (value >> 56) |
					((value << 40) & 0x00FF000000000000) |
					((value << 24) & 0x0000FF0000000000) |
					((value << 8) & 0x000000FF00000000) |
					((value >> 8) & 0x00000000FF000000) |
					((value >> 24) & 0x0000000000FF0000) |
					((value >> 40) & 0x000000000000FF00) |
					(value << 56);
				return value;
			}
			static uint64_t ChangeEndian64(uint64_t  value, Endian to)
			{
				if (to == GetSystemEndian())
				{
					return value;
				}
				return ChangeEndian64(value);
			}
		};
	}
}