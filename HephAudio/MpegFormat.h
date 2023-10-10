#pragma once
#include "HephAudioFramework.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class MpegFormat final : public IAudioFileFormat
		{
		public:
			struct Granule final
			{
				uint32_t part2_3_length{ 0 }; // 12 bits for each channel
				uint32_t bigValues{ 0 }; // 9 bits for each channel
				uint16_t globalGain{ 0 }; // 8 bits for each channel
				uint8_t scaleFacCompress{ 0 }; // 4 bits for each channel
				uint8_t windowSwitching{ 0 }; // 1 bit for each channel
				uint8_t blockType{ 0 }; // 2 bits for each channel
				uint8_t mixedBlockFlag{ 0 }; // 1 bit for each channel
				uint32_t tableSelect{ 0 }; // 15 (3 * 5 for block type 0, 1, and 3) bits or 10 bits (2 * 5 for block type 2) for each channel
				uint32_t subblockGain{ 0 }; // 10 (2 * 5) bits for mono, 9 (3 * 3) bits for each channel for stereo
				uint8_t region0{ 0 }; // 4 bits for each channel
				uint8_t region1{ 0 }; // 3 bits for each channel
				uint8_t preFlag{ 0 }; // 1 bit for each channel
				uint8_t scaleFacScale{ 0 }; // 1 bit for each channel
				uint8_t count1TableSelect{ 0 }; // 1 bit for each channel
			};
			struct SideInformation final
			{
				uint8_t share{ 0 }; // 4 bits for each channel
				Granule granule0;
				Granule granule1;
			};
		public:
			HephCommon::StringBuffer Extension() const override;
			size_t FileFrameCount(const HephCommon::File* pAudioFile, const AudioFormatInfo& audioFormatInfo) const override;
			AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const override;
			AudioBuffer ReadFile(const HephCommon::File* pAudioFile) const override;
			AudioBuffer ReadFile(const HephCommon::File* pAudioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const override;
			bool SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			void SkipID3Tag(const HephCommon::File* pAudioFile) const;
			void SkipVBRInfoFrame(const HephCommon::File* pAudioFile) const;
			uint32_t GetBitrate(const HephCommon::File* pAudioFile, uint32_t frameHeader) const;
			uint32_t GetSampleRate(const HephCommon::File* pAudioFile, uint32_t frameHeader) const;
			void ReadSideInfo(const HephCommon::File* pAudioFile, SideInformation& sideInfo, const AudioFormatInfo& frameFormatInfo, uint32_t frameHeader) const;
			static void Unsynchsafe(uint32_t& data32) noexcept;
		};
	}
}