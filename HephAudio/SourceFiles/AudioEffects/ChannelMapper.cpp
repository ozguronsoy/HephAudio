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

		};
	}
}