#include "FlacFormat.h"
#include "AudioException.h"

namespace HephAudio
{
	namespace Formats
	{
#pragma region Enums
		enum class FlacBlockType : uint8_t
		{
			STREAMINFO = 0,
			PADDING = 1,
			APPLICATION = 2,
			SEEKTABLE = 3,
			VORBIS_COMMENT = 4,
			CUESHEET = 5,
			PICTURE = 6,
			Unknown = 0xFF
		};
		FlacBlockType ToFlacBlockType(uint8_t v)
		{
			if (v > 6)
			{
				return FlacBlockType::Unknown;
			}
			return (FlacBlockType)v;
		}
		enum class SubframeType
		{
			Constant = 0,
			Verbatim = 1,
			Fixed = 2,
			LPC = 3,
			Unknown = 0xFF
		};
		SubframeType ToSubFrameType(uint8_t v)
		{
			if (v == 0)
			{
				return SubframeType::Constant;
			}
			else if (v == 1)
			{
				return SubframeType::Verbatim;
			}
			else if (v >= 8 && v < 16)
			{
				return SubframeType::Fixed;
			}
			else if (v >= 32)
			{
				return SubframeType::LPC;
			}
			return SubframeType::Unknown;
		}
#pragma endregion
		std::wstring FlacFormat::Extension() const noexcept
		{
			return L".flac";
		}
		WAVEFORMATEX FlacFormat::ReadFormatInfo(AudioFile& file) const
		{
			void* audioFileBuffer = file.GetInnerBufferAddress();
			WAVEFORMATEX wfx = WAVEFORMATEX();
			wfx.wFormatTag = 1;
			if (Read<uint32_t>(audioFileBuffer, 0, GetSystemEndian()) == *(uint32_t*)"fLaC")
			{
				size_t cursor = 4;
				const size_t metaDataHeaderSize = 4;
				bool lastMetaData = false;
				while (!lastMetaData)
				{
					lastMetaData = Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x80;
					FlacBlockType blockType = ToFlacBlockType(Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x7F);
					uint32_t metaDataSize = Read<uint32_t>(audioFileBuffer, cursor + 1, Endian::Big) >> 8;
					if (blockType == FlacBlockType::STREAMINFO)
					{
						wfx.nSamplesPerSec = Read<uint32_t>(audioFileBuffer, cursor + 14, Endian::Big) >> 12;
						wfx.nChannels = ((Read<uint8_t>(audioFileBuffer, cursor + 16, Endian::Big) & 0x0E) >> 1) + 1;
						wfx.wBitsPerSample = ((Read<uint16_t>(audioFileBuffer, cursor + 16, Endian::Big) & 0x01F0) >> 4) + 1;
					}
					cursor += metaDataSize + metaDataHeaderSize;
				}
				wfx.cbSize = cursor; // Meta data ends.
			}
			else
			{
				throw AudioException(E_FAIL, L"FlacFormat", L"Failed to read the flac file. File might be corrupted.");
			}
			wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
			wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
			return wfx;
		}
		AudioBuffer FlacFormat::ReadFile(AudioFile& file) const
		{
			WAVEFORMATEX wfx = ReadFormatInfo(file);
			void* audioFileBuffer = file.GetInnerBufferAddress();
			ReadHeader(audioFileBuffer, wfx.cbSize, wfx);
			throw AudioException(E_NOTIMPL, L"FlacFormat::ReadFile", L"Not implemented.");
		}
		bool FlacFormat::SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const
		{
			throw AudioException(E_NOTIMPL, L"FlacFormat::SaveToFile", L"Not implemented.");
		}
		void FlacFormat::ReadHeader(void* audioFileBuffer, uint32_t startOfHeader, WAVEFORMATEX& wfx) const
		{
			uint32_t cursor = startOfHeader + 1;
			const bool variableBlocking = Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x01; // false = fixed, true = variable.
			cursor += 1;
			uint16_t blockSize = GetBlockSize(audioFileBuffer, startOfHeader, Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) >> 4, variableBlocking);
			uint8_t srBits = Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x0F;
			cursor += 1;
			uint8_t channelAssignment = Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) >> 4;
			uint16_t bps = GetBPS((Read<uint8_t>(audioFileBuffer, cursor, Endian::Big) & 0x0F) >> 1, wfx);
			cursor += 1;
			uint32_t sfNumber = Read<uint32_t>(audioFileBuffer, cursor, Endian::Big) >> 1;
			cursor += 3;
			SubframeType subFrameType = ToSubFrameType(Read<uint8_t>(audioFileBuffer, wfx.cbSize + 11, Endian::Big) >> 2);
			uint8_t order = 0;
			if (subFrameType == SubframeType::Fixed)
			{
				order = (Read<uint8_t>(audioFileBuffer, wfx.cbSize + 11, Endian::Big) >> 2) & 0x07;
				if (order > 4) { order = 0; } // reserved.
			}
			// TODO...
		}
		uint16_t FlacFormat::GetBlockSize(void* audioFileBuffer, uint32_t startOfHeader, uint8_t bsbits, bool variableBlocking) const
		{
			if (bsbits == 1)
			{
				return 192;
			}
			else if (bsbits >= 2 && bsbits <= 5)
			{
				return 576 * pow(2, bsbits - 2);
			}
			else if (bsbits == 6 || bsbits == 7)
			{
				uint32_t cursor = startOfHeader + 4;
				if (variableBlocking)
				{
					cursor += 4;
					if (bsbits == 6)
					{
						return (Read<uint16_t>(audioFileBuffer, cursor, Endian::Big) & 0x0FF0) >> 4;
					}
					else
					{
						return (Read<uint32_t>(audioFileBuffer, cursor, Endian::Big) & 0x0FFFF000) >> 12;
					}
				}
				else
				{
					cursor += 3;
					if (bsbits == 6)
					{
						return (Read<uint16_t>(audioFileBuffer, cursor, Endian::Big) & 0x01FE);
					}
					else
					{
						return (Read<uint32_t>(audioFileBuffer, cursor, Endian::Big) & 0x01FFFE00) >> 8;
					}
				}
			}
			else if (bsbits >= 8)
			{
				return 256 * pow(2, bsbits - 8);
			}
			throw AudioException(E_INVALIDARG, L"FlacFormat", L"Invalid block size.");
		}
		uint16_t FlacFormat::GetBPS(uint8_t bpsbits, WAVEFORMATEX& wfx) const
		{
			if (bpsbits == 0)
			{
				return wfx.wBitsPerSample;
			}
			else if (bpsbits == 1)
			{
				return 8;
			}
			else if (bpsbits == 2)
			{
				return 12;
			}
			else if (bpsbits == 4)
			{
				return 16;
			}
			else if (bpsbits == 5)
			{
				return 20;
			}
			else if (bpsbits == 6)
			{
				return 24;
			}
			throw AudioException(E_INVALIDARG, L"FlacFormat", L"Invalid bps.");
		}
	}
}