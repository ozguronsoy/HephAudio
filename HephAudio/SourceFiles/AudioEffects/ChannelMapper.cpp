#include "AudioEffects/ChannelMapper.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	ChannelMapper::ChannelMapperMap ChannelMapper::map = ChannelMapper::CreateMap();

	size_t ChannelMapper::AudioChannelLayoutHasher::operator()(const AudioChannelLayout& chLayout) const
	{
		size_t seed = 0;

		seed = std::hash<uint16_t>()(chLayout.count);

		seed ^= std::hash<AudioChannelMask>()(chLayout.mask) << 1;
		seed >>= 1;

		return seed;
	}

	ChannelMapper::ChannelMapper() : ChannelMapper(HEPHAUDIO_CH_LAYOUT_STEREO) {}

	ChannelMapper::ChannelMapper(const AudioChannelLayout& targetLayout) : DoubleBufferedAudioEffect()
	{
		this->SetTargetLayout(targetLayout);
	}

	std::string ChannelMapper::Name() const
	{
		return "Channel Mapper";
	}

	const AudioChannelLayout& ChannelMapper::GetTargetLayout() const
	{
		return this->targetLayout;
	}

	void ChannelMapper::SetTargetLayout(const AudioChannelLayout& targetLayout)
	{
		if (targetLayout.count == 0 || targetLayout.mask == Unknown)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "invalid channel layout."));
		}

		if (ChannelMapper::map.find(targetLayout) == ChannelMapper::map.end())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "no mapping found for the targetLayout."));
		}

		this->targetLayout = targetLayout;
	}

	void ChannelMapper::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		auto& m = ChannelMapper::map[this->targetLayout];
		const AudioChannelLayout& iLayout = inputBuffer.FormatInfo().channelLayout;

		if (m.find(iLayout) == m.end())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "no mapping found between input layout and the target layout."));
		}

		const size_t endIndex = startIndex + frameCount;
		const auto& f = m[iLayout];

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			f(inputBuffer[i], outputBuffer[i]);
		}
	}

	AudioBuffer ChannelMapper::CreateOutputBuffer(const AudioBuffer& inputBuffer, size_t startIndex, size_t frameCount) const
	{
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		return AudioBuffer(
			(inputBuffer.FrameCount() - frameCount) + this->CalculateOutputFrameCount(frameCount, formatInfo),
			this->targetLayout,
			formatInfo.sampleRate, BufferFlags::AllocUninitialized);
	}

	void ChannelMapper::InitializeOutputBuffer(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) const
	{
		if (startIndex != 0 || frameCount != inputBuffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "this effect must be applied to the entire buffer."));
		}

		outputBuffer.Reset();
	}

	void ChannelMapper::SetMapping(const AudioChannelLayout& targetLayout, const AudioChannelLayout& inputLayout, const std::function<void(heph_audio_sample_t*, heph_audio_sample_t*)>& f)
	{
		ChannelMapper::map[targetLayout][inputLayout] = f;
	}

	ChannelMapper::ChannelMapperMap ChannelMapper::CreateMap()
	{
		return
		{
			// MONO
			{
				HEPHAUDIO_CH_LAYOUT_MONO,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] / 2) + (in[1] / 2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] / 2) + (in[1] / 2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.4f) + (in[1] * 0.4f) + (in[2] * 0.2f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.25f) + (in[1] * 0.25f) + (in[2] * 0.5f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.25f) + (in[1] * 0.25f) + (in[2] * 0.5f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.225f) + (in[1] * 0.225f) + (in[2] * 0.45f) + (in[3] * 0.1f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.375f) + (in[1] * 0.375f) + (in[2] * 0.125f) + (in[3] * 0.125f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.375f) + (in[1] * 0.375f) + (in[2] * 0.125f) + (in[3] * 0.125f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.225f) + (in[1] * 0.225f) + (in[2] * 0.45f) + (in[4] * 0.1f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.2f) + (in[1] * 0.2f) + (in[2] * 0.4f) + (in[3] * 0.1f) + (in[4] * 0.1f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.2f) + (in[1] * 0.2f) + (in[2] * 0.4f) + (in[3] * 0.1f) + (in[4] * 0.1f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.2f) + (in[1] * 0.2f) + (in[2] * 0.4f) + (in[4] * 0.1f) + (in[5] * 0.1f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.2f) + (in[1] * 0.2f) + (in[2] * 0.4f) + (in[4] * 0.1f) + (in[5] * 0.1f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.214f) + (in[1] * 0.214f) + (in[2] * 0.428f) + (in[3] * 0.072f) + (in[4] * 0.036f) + (in[5] * 0.036f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.25f) + (in[1] * 0.25f) + (in[2] * 0.166f) + (in[3] * 0.166f) + (in[4] * 0.083f) + (in[5] * 0.083f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.214f) + (in[1] * 0.214f) + (in[2] * 0.428f) + (in[3] * 0.036f) + (in[4] * 0.036f) + (in[5] * 0.072f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.214f) + (in[1] * 0.214f) + (in[2] * 0.428f) + (in[4] * 0.072f) + (in[5] * 0.036f) + (in[6] * 0.036f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.214f) + (in[1] * 0.214f) + (in[2] * 0.428f) + (in[4] * 0.036f) + (in[5] * 0.036f) + (in[6] * 0.072f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.25f) + (in[1] * 0.25f) + (in[3] * 0.166f) + (in[4] * 0.166f) + (in[5] * 0.083f) + (in[6] * 0.083f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.166f) + (in[1] * 0.166f) + (in[2] * 0.25f) + (in[3] * 0.083f) + (in[4] * 0.083f) + (in[5] * 0.083f) + (in[6] * 0.083f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.17f) + (in[1] * 0.17f) + (in[2] * 0.35f) + (in[3] * 0.10f) + (in[4] * 0.10f) + (in[5] * 0.055f) + (in[6] * 0.055f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.18f) + (in[1] * 0.18f) + (in[2] * 0.26f) + (in[4] * 0.095f) + (in[5] * 0.095f) + (in[6] * 0.095f) + (in[7] * 0.095f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.17f) + (in[1] * 0.17f) + (in[2] * 0.35f) + (in[4] * 0.10f) + (in[5] * 0.10f) + (in[6] * 0.055f) + (in[7] * 0.055f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.17f) + (in[1] * 0.17f) + (in[2] * 0.35f) + (in[4] * 0.055f) + (in[5] * 0.055f) + (in[6] * 0.10f) + (in[7] * 0.10f);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.17f) + (in[1] * 0.17f) + (in[2] * 0.35f) + (in[3] * 0.05f) + (in[4] * 0.05f) + (in[5] * 0.11f) + (in[6] * 0.05f) + (in[7] * 0.05f);
						}
					},
				},
			},

			// STEREO
			{
				HEPHAUDIO_CH_LAYOUT_STEREO,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
							out[1] = (in[1] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[3] * 0.050) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[3] * 0.050) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[3] * 0.125) + (in[5] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[5] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[5] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[6] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[6] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[5] * 0.125) + (in[6] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.250) + (in[3] * 0.125) + (in[5] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.250) + (in[4] * 0.125) + (in[6] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[3] * 0.250) + (in[5] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.250) + (in[4] * 0.125) + (in[6] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.250) + (in[5] * 0.125) + (in[7] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.250) + (in[7] * 0.075);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.075) + (in[6] * 0.250);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.075) + (in[7] * 0.250);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[3] * 0.125) + (in[5] * 0.075) + (in[6] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[4] * 0.125) + (in[5] * 0.075) + (in[7] * 0.125);
						}
					},
				},
			},

			// 2.1
			{
				HEPHAUDIO_CH_LAYOUT_2_POINT_1,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
							out[1] = (in[1] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[3] * 0.050) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[3] * 0.050) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[3] * 0.125) + (in[5] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[5] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[5] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[6] * 0.050);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[6] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[5] * 0.125) + (in[6] * 0.050);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.250) + (in[3] * 0.125) + (in[5] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.250) + (in[4] * 0.125) + (in[6] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[3] * 0.250) + (in[5] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.250) + (in[4] * 0.125) + (in[6] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.250) + (in[5] * 0.125) + (in[7] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.250) + (in[7] * 0.075);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.075) + (in[6] * 0.250);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.075) + (in[7] * 0.250);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[3] * 0.125) + (in[5] * 0.075) + (in[6] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[4] * 0.125) + (in[5] * 0.075) + (in[7] * 0.125);
						}
					},
				},
			},

			// 2_1
			 {
				HEPHAUDIO_CH_LAYOUT_2_1,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.70) + (in[2] * 0.30);
							out[1] = (in[1] * 0.70) + (in[2] * 0.30);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
							out[2] = (in[2] * 0.50) + (in[3] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.375);
							out[1] = (in[1] * 0.625) + (in[2] * 0.375);
							out[2] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[2] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[2] = (in[3] * 0.125) + (in[4] * 0.125) + (in[5] * 0.75);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.275) + (in[5] * 0.10);
							out[1] = (in[1] * 0.625) + (in[2] * 0.275) + (in[6] * 0.10);
							out[2] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = (in[4] * 0.125) + (in[5] * 0.125) + (in[6] * 0.75);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.250) + (in[3] * 0.125) + (in[5] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.250) + (in[4] * 0.125) + (in[6] * 0.125);
							out[2] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[3] * 0.250) + (in[5] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.250) + (in[4] * 0.125) + (in[6] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.250) + (in[5] * 0.125) + (in[7] * 0.125);
							out[2] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.250) + (in[7] * 0.075);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.075) + (in[6] * 0.250);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.075) + (in[7] * 0.250);
							out[2] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[3] * 0.125) + (in[5] * 0.075) + (in[6] * 0.125);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[4] * 0.125) + (in[5] * 0.075) + (in[7] * 0.125);
							out[2] = (in[3] * 0.125) + (in[4] * 0.125) + (in[5] * 0.75);
						}
					},
				},
			 },

			// SURROUND (3.0)
			{
				HEPHAUDIO_CH_LAYOUT_SURROUND,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[2] * 0.1);
							out[1] = (in[1] * 0.9) + (in[2] * 0.1);
							out[2] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[3] * 0.100) + (in[4] * 0.175);
							out[1] = (in[1] * 0.725) + (in[3] * 0.100) + (in[5] * 0.175);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[3] * 0.175) + (in[5] * 0.100);
							out[1] = (in[1] * 0.725) + (in[4] * 0.175) + (in[5] * 0.100);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[4] * 0.100) + (in[5] * 0.175);
							out[1] = (in[1] * 0.725) + (in[4] * 0.100) + (in[6] * 0.175);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[4] * 0.175) + (in[6] * 0.100);
							out[1] = (in[1] * 0.725) + (in[5] * 0.175) + (in[6] * 0.100);
							out[2] = (in[2] * 0.8) + (in[6] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.750) + (in[3] * 0.125) + (in[5] * 0.125);
							out[1] = (in[1] * 0.750) + (in[4] * 0.125) + (in[6] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.750) + (in[4] * 0.125) + (in[6] * 0.125);
							out[1] = (in[1] * 0.750) + (in[5] * 0.125) + (in[7] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.125) + (in[6] * 0.250);
							out[1] = (in[1] * 0.625) + (in[5] * 0.125) + (in[7] * 0.250);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.600) + (in[3] * 0.150) + (in[5] * 0.100) + (in[6] * 0.150);
							out[1] = (in[1] * 0.600) + (in[4] * 0.150) + (in[5] * 0.100) + (in[7] * 0.150);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
						}
					},
				},
			},

			// 3.1
			{
				HEPHAUDIO_CH_LAYOUT_3_POINT_1,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[2] * 0.1);
							out[1] = (in[1] * 0.9) + (in[2] * 0.1);
							out[2] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[3] * 0.100) + (in[4] * 0.175);
							out[1] = (in[1] * 0.725) + (in[3] * 0.100) + (in[5] * 0.175);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[3] * 0.175) + (in[5] * 0.100);
							out[1] = (in[1] * 0.725) + (in[4] * 0.175) + (in[5] * 0.100);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[4] * 0.100) + (in[5] * 0.175);
							out[1] = (in[1] * 0.725) + (in[4] * 0.100) + (in[6] * 0.175);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[4] * 0.175) + (in[6] * 0.100);
							out[1] = (in[1] * 0.725) + (in[5] * 0.175) + (in[6] * 0.100);
							out[2] = (in[2] * 0.8) + (in[6] * 0.2);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.750) + (in[3] * 0.125) + (in[5] * 0.125);
							out[1] = (in[1] * 0.750) + (in[4] * 0.125) + (in[6] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.750) + (in[4] * 0.125) + (in[6] * 0.125);
							out[1] = (in[1] * 0.750) + (in[5] * 0.125) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.125) + (in[6] * 0.250);
							out[1] = (in[1] * 0.625) + (in[5] * 0.125) + (in[7] * 0.250);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.600) + (in[3] * 0.150) + (in[5] * 0.100) + (in[6] * 0.150);
							out[1] = (in[1] * 0.600) + (in[4] * 0.150) + (in[5] * 0.100) + (in[7] * 0.150);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
						}
					},
				},
			},

			// 4.0
			{
				HEPHAUDIO_CH_LAYOUT_4_POINT_0,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = (in[2] * 0.50) + (in[3] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[6] * 0.666) + (in[4] * 0.166) + (in[5] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[6] * 0.4);
							out[1] = (in[1] * 0.6) + (in[7] * 0.4);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
						}
					},
				},
			},

			// 2_2
			{
				HEPHAUDIO_CH_LAYOUT_2_2,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
							out[1] = (in[1] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[2] = in[3];
							out[3] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[2] = in[4];
							out[3] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.2) + (in[3] * 0.1);
							out[1] = (in[1] * 0.7) + (in[2] * 0.2) + (in[3] * 0.1);
							out[2] = in[4];
							out[3] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[3] * 0.25);
							out[2] = in[4];
							out[3] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[3] * 0.125) + (in[5] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[5] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[2] = in[5];
							out[3] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[6] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[5] * 0.125) + (in[6] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[3] * 0.25);
							out[1] = (in[1] * 0.75) + (in[4] * 0.25);
							out[2] = in[5];
							out[3] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[2] = in[5];
							out[3] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.575) + (in[2] * 0.175) + (in[3] * 0.250);
							out[1] = (in[1] * 0.575) + (in[2] * 0.175) + (in[4] * 0.250);
							out[2] = in[5];
							out[3] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = in[6];
							out[3] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.575) + (in[2] * 0.175) + (in[4] * 0.250);
							out[1] = (in[1] * 0.575) + (in[2] * 0.175) + (in[5] * 0.250);
							out[2] = in[6];
							out[3] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.075) + (in[6] * 0.250);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.075) + (in[7] * 0.250);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.50) + (in[2] * 0.25) + (in[3] * 0.15) + (in[5] * 0.10);
							out[1] = (in[1] * 0.50) + (in[2] * 0.25) + (in[4] * 0.15) + (in[5] * 0.10);
							out[2] = in[6];
							out[3] = in[7];
						}
					},
				},
			},

			// QUAD
			{
				HEPHAUDIO_CH_LAYOUT_QUAD,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = (in[2] * 0.5);
							out[3] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = (in[3] * 0.5);
							out[3] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = (in[4] * 0.5);
							out[3] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = in[3];
							out[3] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = in[5];
							out[3] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = (in[3] * 0.5);
							out[3] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = (in[3] * 0.66) + (in[5] * 0.33);
							out[3] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.275) + (in[5] * 0.1);
							out[1] = (in[1] * 0.625) + (in[2] * 0.275) + (in[6] * 0.1);
							out[2] = (in[4] * 0.5);
							out[3] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = (in[4] * 0.66) + (in[6] * 0.33);
							out[3] = (in[5] * 0.66) + (in[6] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[2] * 0.3) + (in[5] * 0.1);
							out[1] = (in[1] * 0.6) + (in[2] * 0.3) + (in[6] * 0.1);
							out[2] = in[3];
							out[3] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[3] * 0.250) + (in[5] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[2] * 0.3) + (in[6] * 0.1);
							out[1] = (in[1] * 0.6) + (in[2] * 0.3) + (in[7] * 0.1);
							out[2] = in[4];
							out[3] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.500) + (in[2] * 0.175) + (in[4] * 0.250) + (in[6] * 0.075);
							out[1] = (in[1] * 0.500) + (in[2] * 0.175) + (in[5] * 0.250) + (in[7] * 0.075);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.5) + (in[2] * 0.2) + (in[6] * 0.3);
							out[1] = (in[1] * 0.5) + (in[2] * 0.2) + (in[7] * 0.3);
							out[2] = in[4];
							out[3] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[2] * 0.3) + (in[6] * 0.1);
							out[1] = (in[1] * 0.6) + (in[2] * 0.3) + (in[7] * 0.1);
							out[2] = (in[3] * 0.66) + (in[5] * 0.33);
							out[3] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},
				},
			},

			// 4.1
			{
				HEPHAUDIO_CH_LAYOUT_4_POINT_1,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = (in[2] * 0.50) + (in[3] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[6] * 0.666) + (in[4] * 0.166) + (in[5] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[4] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[6] * 0.4);
							out[1] = (in[1] * 0.6) + (in[7] * 0.4);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[3];
							out[4] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[4] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
						}
					},
				},
			},

			// 5.0
			{
				HEPHAUDIO_CH_LAYOUT_5_POINT_0,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[2] * 0.1);
							out[1] = (in[1] * 0.9) + (in[2] * 0.1);
							out[2] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[3] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.3);
							out[1] = (in[1] * 0.7) + (in[3] * 0.3);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[3] * 0.175) + (in[5] * 0.100);
							out[1] = (in[1] * 0.725) + (in[4] * 0.175) + (in[5] * 0.100);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[4] * 0.1);
							out[1] = (in[1] * 0.9) + (in[4] * 0.1);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
							out[3] = in[5];
							out[4] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[4] * 0.175) + (in[6] * 0.100);
							out[1] = (in[1] * 0.725) + (in[5] * 0.175) + (in[6] * 0.100);
							out[2] = (in[2] * 0.8) + (in[6] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[5];
							out[4] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
							out[3] = in[5];
							out[4] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
							out[3] = in[5];
							out[4] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[6];
							out[4] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[4] * 0.3);
							out[1] = (in[1] * 0.7) + (in[5] * 0.3);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[6];
							out[4] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.125) + (in[6] * 0.250);
							out[1] = (in[1] * 0.625) + (in[5] * 0.125) + (in[7] * 0.250);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[3] * 0.15) + (in[5] * 0.10);
							out[1] = (in[1] * 0.75) + (in[4] * 0.15) + (in[5] * 0.10);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
							out[3] = in[6];
							out[4] = in[7];
						}
					},
				},
			},

			// 5.0_BACK
			{
				HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = (in[2] * 0.5);
							out[4] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.5);
							out[4] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.5);
							out[4] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.175);
							out[1] = (in[1] * 0.875) + (in[5] * 0.175);
							out[2] = in[2];
							out[3] = (in[3] * 0.5);
							out[4] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.66) + (in[5] * 0.33);
							out[4] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = (in[4] * 0.5);
							out[4] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.66) + (in[6] * 0.33);
							out[4] = (in[5] * 0.66) + (in[6] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[6] * 0.3);
							out[1] = (in[1] * 0.7) + (in[7] * 0.3);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = (in[3] * 0.66) + (in[5] * 0.33);
							out[4] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},
				},
			},

			// 5.1
			{
				HEPHAUDIO_CH_LAYOUT_5_POINT_1,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[2] * 0.1);
							out[1] = (in[1] * 0.9) + (in[2] * 0.1);
							out[2] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
							out[3] = in[3];
					}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[3] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
							out[2] = (in[2] * 0.8) + (in[3] * 0.2);
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.3);
							out[1] = (in[1] * 0.7) + (in[3] * 0.3);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[3] * 0.175) + (in[5] * 0.100);
							out[1] = (in[1] * 0.725) + (in[4] * 0.175) + (in[5] * 0.100);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[4] * 0.1);
							out[1] = (in[1] * 0.9) + (in[4] * 0.1);
							out[2] = (in[2] * 0.8) + (in[4] * 0.2);
							out[3] = in[3];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.725) + (in[4] * 0.175) + (in[6] * 0.100);
							out[1] = (in[1] * 0.725) + (in[5] * 0.175) + (in[6] * 0.100);
							out[2] = (in[2] * 0.8) + (in[6] * 0.2);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[2];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[6];
							out[5] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[4] * 0.3);
							out[1] = (in[1] * 0.7) + (in[5] * 0.3);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[3];
							out[4] = in[6];
							out[5] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.125) + (in[6] * 0.250);
							out[1] = (in[1] * 0.625) + (in[5] * 0.125) + (in[7] * 0.250);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[3] * 0.15) + (in[5] * 0.10);
							out[1] = (in[1] * 0.75) + (in[4] * 0.15) + (in[5] * 0.10);
							out[2] = (in[2] * 0.8) + (in[5] * 0.2);
							out[4] = in[6];
							out[5] = in[7];
						}
					},
				},
			},

			// 5.1_BACK
			{
				HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = (in[2] * 0.5);
							out[5] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.5);
							out[5] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.5);
							out[5] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.175);
							out[1] = (in[1] * 0.875) + (in[5] * 0.175);
							out[2] = in[2];
							out[4] = (in[3] * 0.5);
							out[5] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.66) + (in[5] * 0.33);
							out[5] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.5);
							out[5] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.66) + (in[6] * 0.33);
							out[5] = (in[5] * 0.66) + (in[6] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[6] * 0.3);
							out[1] = (in[1] * 0.7) + (in[7] * 0.3);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[4] = (in[3] * 0.66) + (in[5] * 0.33);
							out[5] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},
				},
			},

			// 6.0
			{
				HEPHAUDIO_CH_LAYOUT_6_POINT_0,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = (in[2] * 0.50) + (in[3] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.3);
							out[1] = (in[1] * 0.7) + (in[3] * 0.3);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[6] * 0.666) + (in[4] * 0.166) + (in[5] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.50) + (in[4] * 0.50);
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.50) + (in[5] * 0.50);
							out[4] = in[6];
							out[5] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[4] * 0.3);
							out[1] = (in[1] * 0.7) + (in[5] * 0.3);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[4] = in[6];
							out[5] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[6] * 0.4);
							out[1] = (in[1] * 0.6) + (in[7] * 0.4);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
							out[4] = in[6];
							out[5] = in[7];
						}
					},
				},
			},

			// 6.0_FRONT
			{
				HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
							out[1] = (in[1] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.2) + (in[3] * 0.1);
							out[1] = (in[1] * 0.7) + (in[2] * 0.2) + (in[3] * 0.1);
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[3] * 0.125) + (in[5] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[5] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[6] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[5] * 0.125) + (in[6] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[3];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = in[3];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[4] = in[6];
							out[5] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = in[4];
							out[3] = in[5];
							out[4] = in[6];
							out[5] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.65) + (in[2] * 0.275) + (in[4] * 0.075);
							out[1] = (in[1] * 0.65) + (in[2] * 0.275) + (in[5] * 0.075);
							out[2] = in[6];
							out[3] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.50) + (in[2] * 0.25) + (in[3] * 0.15) + (in[5] * 0.10);
							out[1] = (in[1] * 0.50) + (in[2] * 0.25) + (in[4] * 0.15) + (in[5] * 0.10);
							out[4] = in[6];
							out[5] = in[7];
						}
					},
				},
			},

			// HEXAGONAL
			{
				HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[5] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[6] * 0.4);
							out[1] = (in[1] * 0.6) + (in[7] * 0.4);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},
				},
			},

			// 6.1
			{
				HEPHAUDIO_CH_LAYOUT_6_POINT_1,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[5] = in[2];
							out[6] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = (in[2] * 0.50) + (in[3] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[3];
							out[6] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.50) + (in[4] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.3);
							out[1] = (in[1] * 0.7) + (in[3] * 0.3);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[6] * 0.666) + (in[4] * 0.166) + (in[5] * 0.166);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[2];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.50) + (in[4] * 0.50);
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.50) + (in[5] * 0.50);
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[4] * 0.3);
							out[1] = (in[1] * 0.7) + (in[5] * 0.3);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[3];
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[6] * 0.4);
							out[1] = (in[1] * 0.6) + (in[7] * 0.4);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[3];
							out[4] = (in[4] * 0.50) + (in[5] * 0.50);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[5] * 0.666) + (in[3] * 0.166) + (in[4] * 0.166);
							out[5] = in[6];
							out[6] = in[7];
						}
					},
				},
			},

			// 6.1_BACK
			{
				HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[6] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[6] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[6] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[6] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[6] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[3] * 0.250) + (in[5] * 0.125);
							out[1] = (in[1] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[4] * 0.250) + (in[6] * 0.125);
							out[1] = (in[1] * 0.625) + (in[5] * 0.250) + (in[7] * 0.125);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[6] * 0.4);
							out[1] = (in[1] * 0.6) + (in[7] * 0.4);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
						}
					},
				},
			},

			// 6.1_FRONT
			{
				HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
							out[1] = (in[1] * 0.60) + (in[2] * 0.275) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[5] = in[2];
							out[6] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[5] = in[3];
							out[6] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[2] = in[3];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.2) + (in[3] * 0.1);
							out[1] = (in[1] * 0.7) + (in[2] * 0.2) + (in[3] * 0.1);
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[3] * 0.125) + (in[5] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[5] * 0.050);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[2] = in[3];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.200) + (in[4] * 0.125) + (in[6] * 0.050);
							out[1] = (in[1] * 0.625) + (in[2] * 0.200) + (in[5] * 0.125) + (in[6] * 0.050);
							out[2] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[3] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.625) + (in[2] * 0.250) + (in[4] * 0.125);
							out[1] = (in[1] * 0.625) + (in[2] * 0.250) + (in[5] * 0.125);
							out[2] = in[3];
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.66) + (in[2] * 0.33);
							out[1] = (in[1] * 0.66) + (in[2] * 0.33);
							out[2] = in[3];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.65) + (in[2] * 0.275) + (in[4] * 0.075);
							out[1] = (in[1] * 0.65) + (in[2] * 0.275) + (in[5] * 0.075);
							out[2] = in[3];
							out[3] = in[6];
							out[4] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.50) + (in[2] * 0.25) + (in[3] * 0.15) + (in[5] * 0.10);
							out[1] = (in[1] * 0.50) + (in[2] * 0.25) + (in[4] * 0.15) + (in[5] * 0.10);
							out[5] = in[6];
							out[6] = in[7];
						}
					},
				},
			},

			// 7.0
			{
				HEPHAUDIO_CH_LAYOUT_7_POINT_0,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = (in[2] * 0.5);
							out[4] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.5);
							out[4] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[5] = in[2];
							out[6] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.5);
							out[4] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[3];
							out[6] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.5);
							out[4] = (in[3] * 0.5);
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.3);
							out[1] = (in[1] * 0.7) + (in[3] * 0.3);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.66) + (in[5] * 0.33);
							out[4] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.5);
							out[4] = (in[4] * 0.5);
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[4] * 0.66) + (in[6] * 0.33);
							out[4] = (in[5] * 0.66) + (in[6] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[4] * 0.3);
							out[1] = (in[1] * 0.7) + (in[5] * 0.3);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[6] * 0.3);
							out[1] = (in[1] * 0.7) + (in[7] * 0.3);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = (in[3] * 0.66) + (in[5] * 0.33);
							out[4] = (in[4] * 0.66) + (in[5] * 0.33);
							out[5] = in[6];
							out[6] = in[7];
						}
					},
				},
			},

			// 7.0_FRONT
			{
				HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[3] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[5] = in[2];
							out[6] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[2] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[4] * 0.1);
							out[1] = (in[1] * 0.9) + (in[4] * 0.1);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[3];
							out[6] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[3] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
							out[2] = in[2];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.8) + (in[3] * 0.125) + (in[5] * 0.075);
							out[1] = (in[1] * 0.8) + (in[4] * 0.125) + (in[5] * 0.075);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[4] * 0.1);
							out[1] = (in[1] * 0.9) + (in[4] * 0.1);
							out[2] = in[2];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.8) + (in[4] * 0.125) + (in[6] * 0.075);
							out[1] = (in[1] * 0.8) + (in[5] * 0.125) + (in[6] * 0.075);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
							out[6] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[6];
							out[4] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[3] * 0.15) + (in[5] * 0.10);
							out[1] = (in[1] * 0.75) + (in[4] * 0.15) + (in[5] * 0.10);
							out[2] = in[2];
							out[5] = in[6];
							out[6] = in[7];
						}
					},
				},
			},

			// 7.1
			{
				HEPHAUDIO_CH_LAYOUT_7_POINT_1,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = (in[2] * 0.5);
							out[5] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.5);
							out[5] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[6] = in[2];
							out[7] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.5);
							out[5] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[6] = in[3];
							out[7] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.5);
							out[5] = (in[3] * 0.5);
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.3);
							out[1] = (in[1] * 0.7) + (in[3] * 0.3);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.66) + (in[5] * 0.33);
							out[5] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.5);
							out[5] = (in[4] * 0.5);
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.66) + (in[6] * 0.33);
							out[5] = (in[5] * 0.66) + (in[6] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[3] = in[2];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
							out[7] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[4] * 0.3);
							out[1] = (in[1] * 0.7) + (in[5] * 0.3);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[3] = in[3];
							out[6] = in[6];
							out[7] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[6] * 0.3);
							out[1] = (in[1] * 0.7) + (in[7] * 0.3);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.66) + (in[5] * 0.33);
							out[5] = (in[4] * 0.66) + (in[5] * 0.33);
							out[6] = in[6];
							out[7] = in[7];
						}
					},
				},
			},

			// 7.1_WIDE
			{
				HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[3] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[6] = in[2];
							out[7] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[2] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[4] * 0.1);
							out[1] = (in[1] * 0.9) + (in[4] * 0.1);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[6] = in[3];
							out[7] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[2] * 0.25);
							out[1] = (in[1] * 0.75) + (in[2] * 0.25);
							out[3] = in[3];
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[3] * 0.1);
							out[1] = (in[1] * 0.9) + (in[3] * 0.1);
							out[2] = in[2];
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.8) + (in[3] * 0.125) + (in[5] * 0.075);
							out[1] = (in[1] * 0.8) + (in[4] * 0.125) + (in[5] * 0.075);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.9) + (in[4] * 0.1);
							out[1] = (in[1] * 0.9) + (in[4] * 0.1);
							out[2] = in[2];
							out[3] = in[3];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.8) + (in[4] * 0.125) + (in[6] * 0.075);
							out[1] = (in[1] * 0.8) + (in[5] * 0.125) + (in[6] * 0.075);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[6] = in[6];
							out[7] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
							out[7] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[6];
							out[5] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.75) + (in[3] * 0.15) + (in[5] * 0.10);
							out[1] = (in[1] * 0.75) + (in[4] * 0.15) + (in[5] * 0.10);
							out[2] = in[2];
							out[6] = in[6];
							out[7] = in[7];
						}
					},
				},
			},

			// 7.1_WIDE_BACK
			{
				HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = (in[2] * 0.5);
							out[5] = (in[2] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.5);
							out[5] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[2] * 0.125);
							out[1] = (in[1] * 0.875) + (in[3] * 0.125);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[4] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.5);
							out[5] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[3] * 0.125);
							out[1] = (in[1] * 0.875) + (in[4] * 0.125);
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.175);
							out[1] = (in[1] * 0.875) + (in[5] * 0.175);
							out[2] = in[2];
							out[4] = (in[3] * 0.5);
							out[5] = (in[3] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[4] * 0.125);
							out[1] = (in[1] * 0.875) + (in[5] * 0.125);
							out[6] = in[2];
							out[7] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[4] = (in[3] * 0.66) + (in[5] * 0.33);
							out[5] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.5);
							out[5] = (in[4] * 0.5);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = (in[4] * 0.66) + (in[6] * 0.33);
							out[5] = (in[5] * 0.66) + (in[6] * 0.33);
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[3] = in[2];
							out[6] = in[3];
							out[7] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[4] = in[3];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[5] * 0.125);
							out[1] = (in[1] * 0.875) + (in[6] * 0.125);
							out[2] = in[2];
							out[6] = in[3];
							out[7] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[3] = in[3];
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
							out[7] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.875) + (in[6] * 0.125);
							out[1] = (in[1] * 0.875) + (in[7] * 0.125);
							out[2] = in[2];
							out[4] = (in[3] * 0.66) + (in[5] * 0.33);
							out[5] = (in[4] * 0.66) + (in[5] * 0.33);
						}
					},
				},
			},

			// OCTAGONAL
			{
				HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
				{
					{
						HEPHAUDIO_CH_LAYOUT_MONO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[2] = in[0];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_STEREO,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[5] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_SURROUND,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_3_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_2_2,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[6] = in[2];
							out[7] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_QUAD,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[3] = in[2];
							out[4] = in[3];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_4_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[6] = in[3];
							out[7] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[3];
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[2] * 0.3);
							out[1] = (in[1] * 0.7) + (in[3] * 0.3);
							out[2] = (in[2] * 0.333) + (in[3] * 0.333);
							out[6] = in[4];
							out[7] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_HEXAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[5] = in[4];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
							out[5] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[3] * 0.333) + (in[4] * 0.333);
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[3] * 0.3);
							out[1] = (in[1] * 0.7) + (in[4] * 0.3);
							out[2] = (in[2] * 0.750) + (in[3] * 0.125) + (in[4] * 0.125);
							out[6] = in[5];
							out[7] = in[6];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[4];
							out[4] = in[5];
							out[6] = in[6];
							out[7] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.7) + (in[4] * 0.3);
							out[1] = (in[1] * 0.7) + (in[5] * 0.3);
							out[2] = (in[2] * 0.750) + (in[4] * 0.125) + (in[5] * 0.125);
							out[6] = in[6];
							out[7] = in[7];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = (in[0] * 0.6) + (in[6] * 0.4);
							out[1] = (in[1] * 0.6) + (in[7] * 0.4);
							out[2] = (in[2] * 0.750) + (in[6] * 0.125) + (in[7] * 0.125);
							out[3] = in[4];
							out[4] = in[5];
						}
					},

					{
						HEPHAUDIO_CH_LAYOUT_OCTAGONAL,
						[](heph_audio_sample_t* in, heph_audio_sample_t* out) -> void
						{
							out[0] = in[0];
							out[1] = in[1];
							out[2] = in[2];
							out[3] = in[3];
							out[4] = in[4];
							out[5] = in[5];
							out[6] = in[6];
							out[7] = in[7];
						}
					},
				},
			},

		};
	}
}