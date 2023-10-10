#include "MpegFormat.h"
#include "HephException.h"
#include "AudioCodecManager.h"
#include "AudioProcessor.h"
#include "HephMath.h"
#include "BitStream.h"

#define SWITCH_DEFAULT_ERROR(methodName) RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, methodName, "Failed to read the file. File might be corrupted.")); break

#define MPEG_SYNC_FRAME(x) ((x & syncValue) == syncValue)

#define MPEG_VERSION(x) ((x >> 19) & 0b11)
#define MPEG_VERSION_2_5 0b00
#define MPEG_VERSION_RESERVED 0b01
#define MPEG_VERSION_2 0b10
#define MPEG_VERSION_1 0b11

#define MPEG_LAYER(x) ((x >> 17) & 0b11)
#define MPEG_LAYER_RESERVED 0b00
#define MPEG_LAYER_3 0b01
#define MPEG_LAYER_2 0b10
#define MPEG_LAYER_1 0b11

#define MPEG_PROTECTION_BIT(x) ((x >> 16) & 1)
#define MPEG_BITRATE_BITS(x) ((x >> 12) & 0x0F)
#define MPEG_SAMPLE_RATE_BITS(x) ((x >> 10) & 0b11)
#define MPEG_PADDING_BIT(x) ((x >> 9) & 1)

#define MPEG_CHANNEL_MODE(x) ((x >> 6) & 0b11)
#define MPEG_CHANNEL_MODE_STEREO 0b00
#define MPEG_CHANNEL_MODE_JOINT_STEREO 0b01
#define MPEG_CHANNEL_MODE_DUAL_CHANNEL 0b10
#define MPEG_CHANNEL_MODE_SINGLE_CHANNEL 0b11

#define MPEG_CHANNEL_MODE_EXTENSION(x) ((x >> 4) & 0b11)
#define MPEG_CHANNEL_MODE_EXTENSION_NONE 0b00
#define MPEG_CHANNEL_MODE_EXTENSION_INTENSITY 0b01
#define MPEG_CHANNEL_MODE_EXTENSION_MS 0b10
#define MPEG_CHANNEL_MODE_EXTENSION_BOTH 0b11

#define MPEG_EMPHASIS(x) (x & 0b11)
#define MPEG_EMPHASIS_NONE 0b00
#define MPEG_EMPHASIS_50_15 0b01 // 50 / 15
#define MPEG_EMPHASIS_RESERVED 0b10
#define MPEG_EMPHASIS_CCIT 0b11 // CCIT J.17

#define MPEG_GRANULE_COUNT(x) ((MPEG_VERSION(x) == MPEG_VERSION_1) ? 2 : 1)

/* 
	MPEG-1:
		-Layer-1: 384
		-Layer-2: 1152
		-Layer-3: 1152
	MPEG-2 & MPEG-2.5:
		-Layer-1: 384
		-Layer-2: 1152
		-Layer-3: 576
*/
#define MPEG_FRAME_SIZE(x) (((MPEG_VERSION(x) != MPEG_VERSION_1) && (MPEG_LAYER(x) == MPEG_LAYER_3)) ? (576) : ((MPEG_LAYER(x) == MPEG_LAYER_1) ? 384 : 1152))
#define MPEG_FRAME_LENGTH(frameSize, bitrate, sampleRate, padding) (((frameSize / 8) * bitrate) / (sampleRate + padding))
#define MPEG_SIDE_INFO_SIZE(x) ((MPEG_VERSION(x) == MPEG_VERSION_1) ? ((MPEG_CHANNEL_MODE(x) == MPEG_CHANNEL_MODE_SINGLE_CHANNEL) ? 17 : 32) : ((MPEG_CHANNEL_MODE(x) == MPEG_CHANNEL_MODE_SINGLE_CHANNEL) ? 9 : 17))

#define MPEG_SIDE_INFO_MAIN_DATA_BEGIN(x) (x & 0x01FF)

using namespace HephCommon;
using namespace HephAudio::Codecs;

constexpr uint32_t ID3 = 0x00494433;
constexpr uint32_t syncValue = 0xFFE00000;
constexpr uint32_t xing = 0x58696E67;

namespace HephAudio
{
	namespace FileFormats
	{
		StringBuffer MpegFormat::Extension() const
		{
			return ".mpa .mp2 .mp3";
		}
		size_t MpegFormat::FileFrameCount(const HephCommon::File* pAudioFile, const AudioFormatInfo& audioFormatInfo) const
		{
			size_t frameCount = 0;
			uint32_t frameHeader;

			pAudioFile->SetOffset(0);
			SkipID3Tag(pAudioFile);
			SkipVBRInfoFrame(pAudioFile);

			pAudioFile->Read(&frameHeader, 4, Endian::Big);
			while (MPEG_SYNC_FRAME(frameHeader))
			{

				const uint64_t frameSize = MPEG_FRAME_LENGTH(MPEG_FRAME_SIZE(frameHeader), GetBitrate(pAudioFile, frameHeader), GetSampleRate(pAudioFile, frameHeader), MPEG_PADDING_BIT(frameHeader));
				const uint64_t frameEnd = pAudioFile->GetOffset() + frameSize - 4ull;

				frameCount += MPEG_FRAME_SIZE(frameHeader);

				pAudioFile->SetOffset(frameEnd);
				pAudioFile->Read(&frameHeader, 4, Endian::Big);
			}

			return frameCount;
		}
		AudioFormatInfo MpegFormat::ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const
		{
			AudioFormatInfo formatInfo;
			uint32_t data32;

			SkipID3Tag(pAudioFile);

			pAudioFile->Read(&data32, 4, Endian::Big);
			if (!MPEG_SYNC_FRAME(data32))
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}

			formatInfo.formatTag = WAVE_FORMAT_MPEG;
			formatInfo.sampleRate = GetSampleRate(pAudioFile, data32);
			formatInfo.channelCount = MPEG_CHANNEL_MODE(data32) == MPEG_CHANNEL_MODE_SINGLE_CHANNEL ? 1 : 2;

			return formatInfo;
		}
		AudioBuffer MpegFormat::ReadFile(const HephCommon::File* pAudioFile) const
		{
			uint32_t data32, frameHeader;
			MpegFormat::SideInformation sideInfo;
			const AudioFormatInfo mpegFormatInfo = ReadAudioFormatInfo(pAudioFile);
			AudioFormatInfo frameFormatInfo;
			frameFormatInfo.formatTag = WAVE_FORMAT_MPEG;
			
			pAudioFile->SetOffset(0);
			SkipID3Tag(pAudioFile);
			SkipVBRInfoFrame(pAudioFile);

			pAudioFile->Read(&frameHeader, 4, Endian::Big);
			while (MPEG_SYNC_FRAME(frameHeader))
			{
				const uint32_t bitrate = GetBitrate(pAudioFile, frameHeader);
				frameFormatInfo.sampleRate = GetSampleRate(pAudioFile, frameHeader);
				frameFormatInfo.channelCount = MPEG_CHANNEL_MODE(frameHeader) == MPEG_CHANNEL_MODE_SINGLE_CHANNEL ? 1 : 2;
				const uint64_t frameSize = MPEG_FRAME_LENGTH(MPEG_FRAME_SIZE(frameHeader), bitrate, frameFormatInfo.sampleRate, MPEG_PADDING_BIT(frameHeader));
				const uint64_t frameEnd = pAudioFile->GetOffset() + frameSize - 4ull;

				if (!MPEG_PROTECTION_BIT(frameHeader)) // skip the CRC bits
				{
					pAudioFile->IncreaseOffset(2);
				}

				const uint32_t sideInfoSize = MPEG_SIDE_INFO_SIZE(frameHeader);
				const uint32_t mainDataSize = frameSize - sideInfoSize - 4ull - ((!MPEG_PROTECTION_BIT(frameHeader)) * 2ull);

				pAudioFile->Read(&data32, 4, Endian::Big);
				const uint64_t mainDataBegin = pAudioFile->GetOffset() + MPEG_SIDE_INFO_MAIN_DATA_BEGIN(data32);
				pAudioFile->DecreaseOffset(4);

				memset(&sideInfo, 0, sizeof(MpegFormat::SideInformation));
				ReadSideInfo(pAudioFile, sideInfo, frameFormatInfo, frameHeader);
				pAudioFile->SetOffset(mainDataBegin);

				// TODO: decode main data

				pAudioFile->SetOffset(frameEnd);
				pAudioFile->Read(&frameHeader, 4, Endian::Big);
			}

			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "MpegFormat::ReadFile", "Not implemented"));
		}
		AudioBuffer MpegFormat::ReadFile(const HephCommon::File* pAudioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "MpegFormat::ReadFile", "Not implemented"));
		}
		bool MpegFormat::SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_not_implemented, "MpegFormat::SaveToFile", "Not implemented"));
		}
		void MpegFormat::SkipID3Tag(const HephCommon::File* pAudioFile) const
		{
			uint8_t id3Flags;
			uint32_t data32 = 0, tagSize_byte;

			pAudioFile->SetOffset(0);

			pAudioFile->Read(&data32, 3, Endian::Big);
			if (data32 != ID3)
			{
				pAudioFile->SetOffset(0);
				pAudioFile->Read(&data32, 4, Endian::Big);
				if (!MPEG_SYNC_FRAME(data32))
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}
				pAudioFile->SetOffset(0);
				return;
			}
			pAudioFile->IncreaseOffset(2);
			pAudioFile->Read(&id3Flags, 1, Endian::Big);

			pAudioFile->Read(&tagSize_byte, 4, Endian::Big);
			Unsynchsafe(tagSize_byte);

			if ((id3Flags & 0x10) == 0x10) // footer size is excluded in the tag size.
			{
				tagSize_byte += 10;
			}

			pAudioFile->SetOffset(tagSize_byte + 10); // tag size + the ID3 header size
		}
		void MpegFormat::SkipVBRInfoFrame(const HephCommon::File* pAudioFile) const
		{
			const uint64_t initialOffset = pAudioFile->GetOffset();
			uint32_t frameHeader, data32;

			pAudioFile->Read(&frameHeader, 4, Endian::Big);
			if (!MPEG_SYNC_FRAME(frameHeader))
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. File might be corrupted."));
			}

			if (MPEG_VERSION(frameHeader) == MPEG_VERSION_1)
			{
				if (MPEG_CHANNEL_MODE(frameHeader) == MPEG_CHANNEL_MODE_SINGLE_CHANNEL)
				{
					pAudioFile->SetOffset(initialOffset + 21);
				}
				else
				{
					pAudioFile->SetOffset(initialOffset + 36);
				}
			}
			else
			{
				if (MPEG_CHANNEL_MODE(frameHeader) == MPEG_CHANNEL_MODE_SINGLE_CHANNEL)
				{
					pAudioFile->SetOffset(initialOffset + 13);
				}
				else
				{
					pAudioFile->SetOffset(initialOffset + 21);
				}
			}

			pAudioFile->Read(&data32, 4, Endian::Big);
			pAudioFile->SetOffset((data32 != xing) ? (initialOffset) : (initialOffset + MPEG_FRAME_LENGTH(MPEG_FRAME_SIZE(frameHeader), GetBitrate(pAudioFile, frameHeader), GetSampleRate(pAudioFile, frameHeader), MPEG_PADDING_BIT(frameHeader))));
		}
		uint32_t MpegFormat::GetBitrate(const HephCommon::File* pAudioFile, uint32_t frameHeader) const
		{
			switch (MPEG_VERSION(frameHeader))
			{
			case MPEG_VERSION_1:
			{
				switch (MPEG_LAYER(frameHeader))
				{
				case MPEG_LAYER_1:
				{
					switch (MPEG_BITRATE_BITS(frameHeader))
					{
					case 0x01:
						return 32000;
					case 0x02:
						return 64000;
					case 0x03:
						return 96000;
					case 0x04:
						return 128000;
					case 0x05:
						return 160000;
					case 0x06:
						return 192000;
					case 0x07:
						return 224000;
					case 0x08:
						return 256000;
					case 0x09:
						return 288000;
					case 0x0A:
						return 320000;
					case 0x0B:
						return 352000;
					case 0x0C:
						return 384000;
					case 0x0D:
						return 416000;
					case 0x0E:
						return 448000;
					case 0x00:
					case 0x0F:
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Invalid bitrate."));
						break;
					default:
						SWITCH_DEFAULT_ERROR("MpegFormat");
					}
				}
				break;
				case MPEG_LAYER_2:
				{
					switch (MPEG_BITRATE_BITS(frameHeader))
					{
					case 0x01:
						return 32000;
					case 0x02:
						return 48000;
					case 0x03:
						return 56000;
					case 0x04:
						return 64000;
					case 0x05:
						return 80000;
					case 0x06:
						return 96000;
					case 0x07:
						return 112000;
					case 0x08:
						return 128000;
					case 0x09:
						return 160000;
					case 0x0A:
						return 192000;
					case 0x0B:
						return 224000;
					case 0x0C:
						return 256000;
					case 0x0D:
						return 320000;
					case 0x0E:
						return 384000;
					case 0x00:
					case 0x0F:
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Invalid bitrate."));
						break;
					default:
						SWITCH_DEFAULT_ERROR("MpegFormat");
					}
				}
				break;
				case MPEG_LAYER_3:
				{
					switch (MPEG_BITRATE_BITS(frameHeader))
					{
					case 0x01:
						return 32000;
					case 0x02:
						return 40000;
					case 0x03:
						return 48000;
					case 0x04:
						return 56000;
					case 0x05:
						return 64000;
					case 0x06:
						return 80000;
					case 0x07:
						return 96000;
					case 0x08:
						return 112000;
					case 0x09:
						return 128000;
					case 0x0A:
						return 160000;
					case 0x0B:
						return 192000;
					case 0x0C:
						return 224000;
					case 0x0D:
						return 256000;
					case 0x0E:
						return 320000;
					case 0x00:
					case 0x0F:
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Invalid bitrate."));
						break;
					default:
						SWITCH_DEFAULT_ERROR("MpegFormat");
					}
				}
				break;
				case MPEG_LAYER_RESERVED:
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Reserved mpeg layer."));
					break;
				default:
					SWITCH_DEFAULT_ERROR("MpegFormat");
				}
			}
			break;
			case MPEG_VERSION_2:
			case MPEG_VERSION_2_5:
			{
				switch (MPEG_LAYER(frameHeader))
				{
				case MPEG_LAYER_1:
				{
					switch (MPEG_BITRATE_BITS(frameHeader))
					{
					case 0x01:
						return 32000;
					case 0x02:
						return 48000;
					case 0x03:
						return 56000;
					case 0x04:
						return 64000;
					case 0x05:
						return 80000;
					case 0x06:
						return 96000;
					case 0x07:
						return 112000;
					case 0x08:
						return 128000;
					case 0x09:
						return 144000;
					case 0x0A:
						return 160000;
					case 0x0B:
						return 176000;
					case 0x0C:
						return 192000;
					case 0x0D:
						return 224000;
					case 0x0E:
						return 256000;
					case 0x00:
					case 0x0F:
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Invalid bitrate."));
						break;
					default:
						SWITCH_DEFAULT_ERROR("MpegFormat");
					}
				}
				break;
				case MPEG_LAYER_2:
				case MPEG_LAYER_3:
				{
					switch (MPEG_BITRATE_BITS(frameHeader))
					{
					case 0x01:
						return 8000;
					case 0x02:
						return 16000;
					case 0x03:
						return 24000;
					case 0x04:
						return 32000;
					case 0x05:
						return 40000;
					case 0x06:
						return 48000;
					case 0x07:
						return 56000;
					case 0x08:
						return 64000;
					case 0x09:
						return 80000;
					case 0x0A:
						return 96000;
					case 0x0B:
						return 112000;
					case 0x0C:
						return 128000;
					case 0x0D:
						return 144000;
					case 0x0E:
						return 160000;
					case 0x00:
					case 0x0F:
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Invalid bitrate."));
						break;
					default:
						SWITCH_DEFAULT_ERROR("MpegFormat");
					}
				}
				break;
				case MPEG_LAYER_RESERVED:
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Reserved mpeg layer."));
					break;
				default:
					SWITCH_DEFAULT_ERROR("MpegFormat");
				}
			}
			break;
			case MPEG_VERSION_RESERVED:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "MpegFormat", "Failed to read the file. Reserved mpeg version."));
				break;
			default:
				SWITCH_DEFAULT_ERROR("MpegFormat");
			}
		}
		uint32_t MpegFormat::GetSampleRate(const HephCommon::File* pAudioFile, uint32_t frameHeader) const
		{
			switch (MPEG_VERSION(frameHeader))
			{
			case MPEG_VERSION_1:
			{
				switch (MPEG_SAMPLE_RATE_BITS(frameHeader))
				{
				case 0:
					return 44100;
				case 1:
					return 48000;
				case 2:
					return 32000;
				default:
					SWITCH_DEFAULT_ERROR("MpegFormat");
				}
			}
			break;
			case MPEG_VERSION_2:
			{
				switch (MPEG_SAMPLE_RATE_BITS(frameHeader))
				{
				case 0:
					return 22050;
				case 1:
					return 24000;
				case 2:
					return 16000;
				default:
					SWITCH_DEFAULT_ERROR("MpegFormat");
				}
			}
			break;
			case MPEG_VERSION_2_5:
			{
				switch (MPEG_SAMPLE_RATE_BITS(frameHeader))
				{
				case 0:
					return 11025;
				case 1:
					return 12000;
				case 2:
					return 8000;
				default:
					SWITCH_DEFAULT_ERROR("MpegFormat");
				}
			}
			break;
			default:
				SWITCH_DEFAULT_ERROR("MpegFormat");
			}
		}
		void MpegFormat::ReadSideInfo(const HephCommon::File* pAudioFile, MpegFormat::SideInformation& sideInfo, const AudioFormatInfo& frameFormatInfo, uint32_t frameHeader) const
		{
			uint8_t* sideInfoBuffer = (uint8_t*)malloc(MPEG_SIDE_INFO_SIZE(frameHeader));
			if (sideInfoBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "MpegFormat", "Insufficient memory."));
			}

			BitStream bitStream(sideInfoBuffer, MPEG_SIDE_INFO_SIZE(frameHeader) * 8);
			const bool isV1 = MPEG_VERSION(frameHeader) == MPEG_VERSION_1;

			// TODO: read side info

			free(sideInfoBuffer);
		}
		void MpegFormat::Unsynchsafe(uint32_t& data32) noexcept
		{
			data32 = (
				(data32 & 0x0000007F) |
				((data32 & 0x00007F00) >> 1) |
				((data32 & 0x007F0000) >> 2) |
				((data32 & 0x7F000000) >> 3)
				);
		}
	}
}