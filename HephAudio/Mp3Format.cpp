#include "Mp3Format.h"
#include "AudioException.h"

// https://checkmate.gissen.nl/mp3header.png
// Every audio frame contains 1152 samples.

namespace HephAudio
{
	namespace Formats
	{
		StringBuffer Mp3Format::Extension() const noexcept
		{
			return L".mp3";
		}
		AudioBuffer Mp3Format::ReadFile(const AudioFile& file) const
		{
			void* audioFileBuffer = file.GetInnerBufferAddress();
			uint32_t cursor = LocateAudioData(audioFileBuffer);
			uint32_t frameLength = 0;
			std::vector<uint8_t> mp3Data;
			while (cursor < file.Size())
			{
				std::vector<uint8_t> frameData = ReadAudioFrame(audioFileBuffer, cursor, &frameLength);
				if (frameLength == UINT32_MAX) { break; } // End of the audio data.
				if (frameData.size() > 0)
				{
					const size_t oldDataSize = mp3Data.size();
					mp3Data.reserve(oldDataSize + frameData.size());
					mp3Data.resize(mp3Data.capacity());
					memcpy(&mp3Data.at(oldDataSize), &frameData.at(0), frameData.size());
				}
				cursor += frameLength;
			}
			throw AudioException(E_NOTIMPL, L"Mp3Format::ReadFile", L"Not implemented.");
		}
		bool Mp3Format::SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const
		{
			throw AudioException(E_NOTIMPL, L"Mp3Format::SaveToFile", L"Not implemented.");
		}
		uint32_t Mp3Format::LocateAudioData(void* audioFileBuffer) const
		{
			uint32_t id3 = Read<uint32_t>(audioFileBuffer, 0, GetSystemEndian());
			if (GetSystemEndian() == Endian::Little)
			{
				id3 &= 0x00FFFFFF;
			}
			else
			{
				id3 &= 0xFFFFFF00;
			}
			if (id3 == *(uint32_t*)"ID3")
			{
				uint32_t cursor = Read<uint32_t>(audioFileBuffer, 6, Endian::Big) + 10;
				const bool hasExtendedHeader = Read<uint8_t>(audioFileBuffer, 5, Endian::Big) & 0x01;
				if (hasExtendedHeader)
				{
					cursor += Read<uint32_t>(audioFileBuffer, 10, Endian::Big);
				}
				return cursor;
			}
			throw AudioException(E_FAIL, L"WavFormat", L"Failed to read the mp3 file. File might be corrupted.");
		}
		std::vector<uint8_t> Mp3Format::ReadAudioFrame(void* audioFileBuffer, uint32_t frameStart, uint32_t* frameLength) const
		{
			if ((Read<uint16_t>(audioFileBuffer, frameStart, Endian::Big) & 0xFFE0) != 0xFFE0) // Every audio frame has a sync code 0xFFE at the begining of the frame, thus we know this is an audio frame.
			{
				(*frameLength) = UINT32_MAX;
				return std::vector<uint8_t>(0);
			}
			uint32_t cursor = frameStart + 1;
			uint8_t mpegVID = (Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x18) >> 3; // 0 = v2.5, 1 = reserved, 2 = v2, 3 = v1
			if (mpegVID == 1)
			{
				throw AudioException(E_FAIL, L"Mp3Format", L"Invalid version.");
			}
			uint8_t mpegLayer = (Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x06) >> 1; // 0 = reserved, 1 = layer 3, 2 = layer 2, 1 = layer 1 (this value should be 1)
			if (mpegLayer != 1)
			{
				throw AudioException(E_FAIL, L"Mp3Format", L"MPEG Layer not supported.");
			}
			cursor += 1;
			uint16_t bitRate = GetBitrate(Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) >> 4, mpegVID);
			if (bitRate == UINT16_MAX)
			{
				throw AudioException(E_FAIL, L"Mp3Format", L"Invalid bit rate.");
			}
			uint32_t sampleRate = GetSampleRate((Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x0C) >> 2, mpegVID);
			if (sampleRate == UINT32_MAX)
			{
				throw AudioException(E_FAIL, L"Mp3Format", L"Invalid sample rate.");
			}
			uint8_t padding = (Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x02) >> 1;
			cursor += 1;
			uint8_t channelMode = (Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0xC0) >> 6; // 0 = Stereo, 1 = Joint Stereo (mid/side), 2 = Dual (two seperate channels), 3 = Mono
			uint8_t jsMode = (Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x30) >> 4; // Joint stereo mode.
			(*frameLength) = floor((144000.0 * (double)bitRate / (double)sampleRate) + (double)padding); // In bytes.
			uint32_t dataSize = (*frameLength) - 4; // In bytes.
			cursor += 2;
			std::vector<uint8_t> frameData = std::vector<uint8_t>(dataSize, 0u);
			memcpy(&frameData.at(0), (uint8_t*)audioFileBuffer + frameStart + 4, dataSize);
			return frameData;
		}
		uint16_t Mp3Format::GetBitrate(uint8_t bitrateBits, uint8_t mpegVID) const
		{
			if (mpegVID == 3) // MPEG 1
			{
				switch (bitrateBits)
				{
				case 1:
					return 32;
				case 2:
					return 40;
				case 3:
					return 48;
				case 4:
					return 56;
				case 5:
					return 64;
				case 6:
					return 80;
				case 7:
					return 96;
				case 8:
					return 112;
				case 9:
					return 128;
				case 10:
					return 160;
				case 11:
					return 192;
				case 12:
					return 224;
				case 13:
					return 256;
				case 14:
					return 320;
				default:
					return UINT16_MAX;
				}
			}
			else // MPEG 2 or MPEG 2.5
			{
				switch (bitrateBits)
				{
				case 1:
					return 8;
				case 2:
					return 16;
				case 3:
					return 24;
				case 4:
					return 32;
				case 5:
					return 40;
				case 6:
					return 48;
				case 7:
					return 56;
				case 8:
					return 64;
				case 9:
					return 80;
				case 10:
					return 96;
				case 11:
					return 112;
				case 12:
					return 128;
				case 13:
					return 144;
				case 14:
					return 160;
				default:
					return UINT16_MAX;
				}
			}
		}
		uint32_t Mp3Format::GetSampleRate(uint8_t srBits, uint8_t mpegVID) const
		{
			if (mpegVID == 3) // MPEG 1
			{
				switch (srBits)
				{
				case 0:
					return 44100;
				case 1:
					return 48000;
				case 2:
					return 32000;
				default:
					return UINT32_MAX;
				}
			}
			else if (mpegVID == 2) // MPEG 2
			{
				switch (srBits)
				{
				case 0:
					return 22050;
				case 1:
					return 24000;
				case 2:
					return 16000;
				default:
					return UINT32_MAX;
				}
			}
			else // MPEG 2.5
			{
				switch (srBits)
				{
				case 0:
					return 11025;
				case 1:
					return 12000;
				case 2:
					return 8000;
				default:
					return UINT32_MAX;
				}
			}
		}
	}
}