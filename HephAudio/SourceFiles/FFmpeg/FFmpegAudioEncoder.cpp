#if defined(HEPHAUDIO_USE_FFMPEG)
#include "FFmpeg/FFmpegAudioEncoder.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/HephMath.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"

using namespace HephCommon;

namespace HephAudio
{
	FFmpegAudioEncoder::FFmpegAudioEncoder()
		: audioFilePath(nullptr), avFormatContext(nullptr), avIoContext(nullptr)
		, avStream(nullptr), avCodecContext(nullptr), swrContext(nullptr)
		, avPacket(nullptr), avFrame(nullptr) {}
	FFmpegAudioEncoder::FFmpegAudioEncoder(const StringBuffer& audioFilePath, AudioFormatInfo outputFormatInfo, bool overwrite) : FFmpegAudioEncoder()
	{
		this->outputFormatInfo = outputFormatInfo;
		this->ChangeFile(audioFilePath, overwrite);
	}
	FFmpegAudioEncoder::FFmpegAudioEncoder(FFmpegAudioEncoder&& rhs) noexcept
		: audioFilePath(std::move(rhs.audioFilePath)), outputFormatInfo(rhs.outputFormatInfo), avFormatContext(rhs.avFormatContext)
		, avIoContext(rhs.avIoContext), avStream(rhs.avStream), avCodecContext(rhs.avCodecContext)
		, swrContext(rhs.swrContext), avPacket(rhs.avPacket), avFrame(rhs.avFrame)
	{
		rhs.avFormatContext = nullptr;
		rhs.avIoContext = nullptr;
		rhs.avStream = nullptr;
		rhs.avCodecContext = nullptr;
		rhs.swrContext = nullptr;
		rhs.avPacket = nullptr;
		rhs.avFrame = nullptr;
	}
	FFmpegAudioEncoder::~FFmpegAudioEncoder()
	{
		this->CloseFile();
	}
	FFmpegAudioEncoder& FFmpegAudioEncoder::operator=(FFmpegAudioEncoder&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->CloseFile();

			this->audioFilePath = std::move(rhs.audioFilePath);
			this->outputFormatInfo = rhs.outputFormatInfo;
			this->avFormatContext = rhs.avFormatContext;
			this->avIoContext = rhs.avIoContext;
			this->avStream = rhs.avStream;
			this->avCodecContext = rhs.avCodecContext;
			this->swrContext = rhs.swrContext;
			this->avPacket = rhs.avPacket;
			this->avFrame = rhs.avFrame;

			rhs.avFormatContext = nullptr;
			rhs.avIoContext = nullptr;
			rhs.avStream = nullptr;
			rhs.avCodecContext = nullptr;
			rhs.swrContext = nullptr;
			rhs.avPacket = nullptr;
			rhs.avFrame = nullptr;
		}

		return *this;
	}
	void FFmpegAudioEncoder::ChangeFile(const StringBuffer& newAudioFilePath, bool overwrite)
	{
		if (!this->audioFilePath.CompareContent(newAudioFilePath))
		{
			this->CloseFile();
			this->OpenFile(newAudioFilePath, overwrite);
		}
	}
	void FFmpegAudioEncoder::CloseFile()
	{
		if (this->IsFileOpen())
		{
			(void)avcodec_send_frame(this->avCodecContext, nullptr); // mark the stream as EOF
			while (avcodec_receive_packet(this->avCodecContext, this->avPacket) >= 0)
			{
				this->avPacket->pts = av_rescale_q(this->avPacket->pts, this->avCodecContext->time_base, this->avStream->time_base);
				this->avPacket->dts = av_rescale_q(this->avPacket->dts, this->avCodecContext->time_base, this->avStream->time_base);
				this->avPacket->duration = av_rescale_q(this->avPacket->duration, this->avCodecContext->time_base, this->avStream->time_base);

				if (av_interleaved_write_frame(this->avFormatContext, this->avPacket) >= 0)
				{
					this->avFrame->pts += this->avFrame->duration;
				}
			}
			(void)av_interleaved_write_frame(this->avFormatContext, nullptr);

			int ret = av_write_trailer(this->avFormatContext);
			if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder::CloseFile", "Failed to write the file trailer.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}
		}

		if (this->avFrame != nullptr)
		{
			av_frame_free(&this->avFrame);
			this->avFrame = nullptr;
		}

		if (this->avPacket != nullptr)
		{
			av_packet_free(&this->avPacket);
			this->avPacket = nullptr;
		}

		if (this->swrContext != nullptr)
		{
			swr_free(&this->swrContext);
			this->swrContext = nullptr;
		}

		if (this->avCodecContext != nullptr)
		{
			avcodec_free_context(&this->avCodecContext);
			this->avCodecContext = nullptr;
		}

		this->avStream = nullptr;

		if (this->avIoContext != nullptr)
		{
			avio_close(this->avIoContext);
			this->avIoContext = nullptr;
		}

		if (this->avFormatContext != nullptr)
		{
			avformat_free_context(this->avFormatContext);
			this->avFormatContext = nullptr;
		}

		this->audioFilePath = nullptr;
	}
	bool FFmpegAudioEncoder::IsFileOpen() const
	{
		return this->audioFilePath != nullptr && this->avFormatContext != nullptr
			&& this->avIoContext != nullptr && this->avStream != nullptr
			&& this->avCodecContext != nullptr && this->swrContext != nullptr
			&& this->avFrame != nullptr && this->avPacket != nullptr;
	}
	void FFmpegAudioEncoder::Encode(const AudioBuffer& bufferToEncode)
	{
		if (!this->IsFileOpen())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder::Encode", "No open file to encode."));
			return;
		}

		if (bufferToEncode == nullptr || bufferToEncode.FrameCount() == 0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioEncoder::Encode", "Trying to encode empty buffer."));
			return;
		}

		const AudioFormatInfo inputFormatInfo = bufferToEncode.FormatInfo();

		AVChannelLayout inputChannelLayout;
		av_channel_layout_default(&inputChannelLayout, inputFormatInfo.channelCount);

		av_opt_set_chlayout(this->swrContext, "in_chlayout", &inputChannelLayout, 0);
		av_opt_set_int(this->swrContext, "in_sample_rate", inputFormatInfo.sampleRate, 0);
		av_opt_set_sample_fmt(this->swrContext, "in_sample_fmt", FFmpegAudioEncoder::AFI2AVSF(inputFormatInfo), 0);

		int ret = swr_init(this->swrContext);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder::Encode", "Failed to initialize SwrContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		size_t i = 0;
		while (i < bufferToEncode.FrameCount())
		{
			uint8_t* pCurrentInputFrame = (uint8_t*)bufferToEncode.Begin() + i * inputFormatInfo.FrameSize();

			size_t inputFrameCountToRead = av_rescale(this->avFrame->nb_samples, inputFormatInfo.sampleRate, this->outputFormatInfo.sampleRate);
			if (i + inputFrameCountToRead > bufferToEncode.FrameCount())
			{
				inputFrameCountToRead = bufferToEncode.FrameCount() - i;
			}

			ret = swr_convert(this->swrContext, this->avFrame->data, this->avFrame->nb_samples, &pCurrentInputFrame, inputFrameCountToRead);
			if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder::Encode", "Failed to convert.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}
			else if (ret < this->avFrame->nb_samples)
			{
				(void)swr_convert(this->swrContext, this->avFrame->data, this->avFrame->nb_samples, nullptr, 0); // flush the buffered samples
			}

			ret = avcodec_send_frame(this->avCodecContext, this->avFrame);
			if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder::Encode", "Failed to send AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}

			ret = avcodec_receive_packet(this->avCodecContext, this->avPacket);
			if (ret == AVERROR(EAGAIN))
			{
				// not enough samples to encode, send more frames to the encoder
				this->avFrame->pts += this->avFrame->duration;
				i += inputFrameCountToRead;
				continue;
			}
			else if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder::Encode", "Failed to recieve AVPacket.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}

			this->avPacket->pts = av_rescale_q(this->avPacket->pts, this->avCodecContext->time_base, this->avStream->time_base);
			this->avPacket->dts = av_rescale_q(this->avPacket->dts, this->avCodecContext->time_base, this->avStream->time_base);
			this->avPacket->duration = av_rescale_q(this->avPacket->duration, this->avCodecContext->time_base, this->avStream->time_base);

			ret = av_interleaved_write_frame(this->avFormatContext, this->avPacket);
			if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder::Encode", "Failed to write AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}

			this->avFrame->pts += this->avFrame->duration;
			i += inputFrameCountToRead;

			av_packet_unref(this->avPacket);
		}
	}
	void FFmpegAudioEncoder::Transcode(const StringBuffer& inputFilePath, const StringBuffer& outputFilePath, bool overwrite)
	{
		FFmpegAudioDecoder decoder(inputFilePath);
		if (decoder.GetFrameCount() > 0)
		{
			FFmpegAudioEncoder encoder(outputFilePath, decoder.GetOutputFormat(), overwrite);
			FFmpegAudioEncoder::Transcode(&decoder, encoder);
		}
	}
	void FFmpegAudioEncoder::Transcode(const StringBuffer& inputFilePath, const StringBuffer& outputFilePath, AudioFormatInfo outputFormatInfo, bool overwrite)
	{
		FFmpegAudioDecoder decoder(inputFilePath);
		if (decoder.GetFrameCount() > 0)
		{
			FFmpegAudioEncoder encoder(outputFilePath, outputFormatInfo, overwrite);
			FFmpegAudioEncoder::Transcode(&decoder, encoder);
		}
	}
	void FFmpegAudioEncoder::OpenFile(const StringBuffer& audioFilePath, bool overwrite)
	{
		if (!overwrite && File::FileExists(audioFilePath))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "File already exists."));
			return;
		}

		this->audioFilePath = StringBuffer(audioFilePath, StringType::ASCII);

		int ret = avformat_alloc_output_context2(&this->avFormatContext, nullptr, nullptr, this->audioFilePath.c_str());
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder", "Failed to allocate output format context.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		ret = avio_open(&this->avIoContext, this->audioFilePath.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder", "Failed to open the output file.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}
		this->avFormatContext->pb = this->avIoContext;

		const AVCodec* avCodec = avcodec_find_encoder(this->avFormatContext->oformat->audio_codec);
		if (avCodec == nullptr)
		{
			const StringBuffer codecName = avcodec_get_name(this->avFormatContext->oformat->audio_codec);
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "No encoder found for the " + codecName + " codec."));
		}

		this->avCodecContext = avcodec_alloc_context3(avCodec);
		if (this->avCodecContext == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "Failed to create AVCodecContext."));
		}

		this->avCodecContext->sample_fmt = FFmpegAudioEncoder::GetClosestSupportedSampleFormat(this, avCodec, this->outputFormatInfo.formatTag, this->outputFormatInfo.bitsPerSample);

		this->outputFormatInfo.sampleRate = FFmpegAudioEncoder::GetClosestSupportedSampleRate(avCodec, this->outputFormatInfo.sampleRate);
		this->avCodecContext->sample_rate = this->outputFormatInfo.sampleRate;
		av_channel_layout_default(&this->avCodecContext->ch_layout, this->outputFormatInfo.channelCount);

		this->avCodecContext->codec_id = this->avFormatContext->oformat->audio_codec;
		this->avCodecContext->codec_tag = av_codec_get_tag(this->avFormatContext->oformat->codec_tag, this->avFormatContext->oformat->audio_codec);
		this->avCodecContext->codec_type = AVMEDIA_TYPE_AUDIO;

		if (this->avCodecContext->codec_id == AV_CODEC_ID_MP3) // otherwise encoder calculates the duration wrong
		{
			this->avCodecContext->bit_rate = 384000;
		}

		ret = avcodec_open2(this->avCodecContext, avCodec, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder", "Failed to open the AVCodecContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->avStream = avformat_new_stream(this->avFormatContext, nullptr);
		if (this->avStream == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "Failed to create AVStream."));
		}

		ret = avcodec_parameters_from_context(this->avStream->codecpar, this->avCodecContext);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "Failed to copy the codec parameters to the AVStream."));
		}

		if ((this->avFormatContext->oformat->flags & AVFMT_GLOBALHEADER) == AVFMT_GLOBALHEADER)
		{
			this->avCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}

		this->avFrame = av_frame_alloc();
		if (this->avFrame == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "Failed to allocate AVFrame."));
		}

		this->avFrame->sample_rate = this->avCodecContext->sample_rate;

		if (this->avCodecContext->frame_size == 0)
		{
			this->avFrame->nb_samples = this->avFrame->sample_rate / 100;
			this->avCodecContext->frame_size = this->avFrame->nb_samples;
			this->avStream->codecpar->frame_size = this->avFrame->nb_samples;
		}
		else
		{
			this->avFrame->nb_samples = this->avCodecContext->frame_size;
		}

		this->avFrame->format = this->avCodecContext->sample_fmt;
		ret = av_channel_layout_copy(&this->avFrame->ch_layout, &this->avCodecContext->ch_layout);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder", "Failed to copy the ch_layout info to the AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->avFrame->pts = this->avCodecContext->initial_padding;
		this->avFrame->duration = this->avFrame->nb_samples;

		ret = av_frame_get_buffer(this->avFrame, 0);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder", "Failed to allocate buffer(s) for AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->swrContext = swr_alloc();
		if (this->swrContext == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "Failed to create SwrContext."));
		}

		av_opt_set_chlayout(this->swrContext, "out_chlayout", &this->avStream->codecpar->ch_layout, 0);
		av_opt_set_int(this->swrContext, "out_sample_rate", this->avStream->codecpar->sample_rate, 0);
		av_opt_set_sample_fmt(this->swrContext, "out_sample_fmt", (AVSampleFormat)this->avStream->codecpar->format, 0);

		ret = avformat_init_output(this->avFormatContext, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder", "Failed to initialize the output file.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		ret = avformat_write_header(this->avFormatContext, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioEncoder", "Failed to write the file header.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->avPacket = av_packet_alloc();
		if (this->avPacket == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioEncoder", "Failed to allocate AVPacket."));
		}
	}
	AVSampleFormat FFmpegAudioEncoder::AFI2AVSF(const AudioFormatInfo& afi) const
	{
		switch (afi.formatTag)
		{
		case HEPHAUDIO_FORMAT_TAG_ALAW:
		case HEPHAUDIO_FORMAT_TAG_MULAW:
			return AV_SAMPLE_FMT_U8;
		case HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT:
			return HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT;
		case HEPHAUDIO_FORMAT_TAG_PCM:
		{
			switch (afi.bitsPerSample)
			{
			case 8:
				return AV_SAMPLE_FMT_U8;
			case 16:
				return AV_SAMPLE_FMT_S16;
			case 24:
			case 32:
				return AV_SAMPLE_FMT_S32;
			default:
				goto ERROR;
			}
		}
		default:
			goto ERROR;
		}

	ERROR:
		RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioEncoder", "Unsupported format."));
	}
	uint32_t FFmpegAudioEncoder::GetClosestSupportedSampleRate(const AVCodec* avCodec, uint32_t targetSampleRate)
	{
		if (avCodec->supported_samplerates != nullptr)
		{
			int32_t closestSampleRate = avCodec->supported_samplerates[0];
			int32_t closestAbsDeltaSampleRate = Math::Abs((int)targetSampleRate - avCodec->supported_samplerates[0]);
			for (size_t i = 1; avCodec->supported_samplerates[i] != 0; i++)
			{
				if (avCodec->supported_samplerates[i] == targetSampleRate)
				{
					return targetSampleRate;
				}

				const int32_t currentAbsDeltaSampleRate = Math::Abs((int)targetSampleRate - avCodec->supported_samplerates[i]);
				if (currentAbsDeltaSampleRate < closestAbsDeltaSampleRate)
				{
					closestAbsDeltaSampleRate = currentAbsDeltaSampleRate;
					closestSampleRate = avCodec->supported_samplerates[i];
				}
				else if (currentAbsDeltaSampleRate == closestAbsDeltaSampleRate && avCodec->supported_samplerates[i] > closestSampleRate) // choose the greater sample rate
				{
					closestSampleRate = avCodec->supported_samplerates[i];
				}
			}
			return closestSampleRate;
		}

		return targetSampleRate;
	}
	AVSampleFormat FFmpegAudioEncoder::GetClosestSupportedSampleFormat(FFmpegAudioEncoder* pEncoder, const AVCodec* avCodec, uint32_t targetFormatTag, uint16_t targetBitsPerSample)
	{
		if (avCodec->sample_fmts == nullptr)
		{
			return AV_SAMPLE_FMT_FLTP;
		}

		auto getFirstSupportedSampleFormat = [avCodec](std::vector<AVSampleFormat> possibleFormats) -> AVSampleFormat
			{
				for (size_t i = 0; i < possibleFormats.size(); i++)
				{
					for (size_t j = 0; avCodec->sample_fmts[j] != AV_SAMPLE_FMT_NONE; j++)
					{
						if (possibleFormats[i] == avCodec->sample_fmts[j])
						{
							return possibleFormats[i];
						}
					}
				}
				return AV_SAMPLE_FMT_NONE;
			};

		if (targetFormatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT)
		{
			AVSampleFormat closestFormat;
			if (targetBitsPerSample == sizeof(double))
			{
				closestFormat = getFirstSupportedSampleFormat(
					{
						AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
						AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
						AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
						AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P
					});
			}
			else
			{
				closestFormat = getFirstSupportedSampleFormat(
					{
						AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
						AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
						AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
						AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P
					});
			}
			if (closestFormat == AV_SAMPLE_FMT_NONE)
			{
				goto ERROR;
			}
			return closestFormat;
		}

		switch (targetBitsPerSample)
		{
		case 8:
		{
			const AVSampleFormat closestFormat = getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP
				});
			if (closestFormat == AV_SAMPLE_FMT_NONE)
			{
				goto ERROR;
			}
			return closestFormat;
		}
		case 16:
		{
			const AVSampleFormat closestFormat = getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P
				});
			if (closestFormat == AV_SAMPLE_FMT_NONE)
			{
				goto ERROR;
			}
			return closestFormat;
		}
		case 24:
		case 32:
		{
			const AVSampleFormat closestFormat = getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P
				});
			if (closestFormat == AV_SAMPLE_FMT_NONE)
			{
				goto ERROR;
			}
			return closestFormat;
		}
		default:
		{
			const AVSampleFormat closestFormat = getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P
				});
			if (closestFormat == AV_SAMPLE_FMT_NONE)
			{
				goto ERROR;
			}
			return closestFormat;
		}
		}

	ERROR:
		RAISE_AND_THROW_HEPH_EXCEPTION(pEncoder, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioEncoder", "Unsupported format."));
	}
	void FFmpegAudioEncoder::Transcode(void* pDecoder, FFmpegAudioEncoder& encoder)
	{
		FFmpegAudioDecoder& decoder = *(FFmpegAudioDecoder*)pDecoder;
		const uint32_t inputSampleRate = decoder.GetOutputFormat().sampleRate;
		const size_t readSize = av_rescale(encoder.avFrame->nb_samples, inputSampleRate, encoder.outputFormatInfo.sampleRate);
		const size_t minRequiredFrameCount = FFMAX(readSize, encoder.avFrame->nb_samples);
		AudioBuffer decodedBuffer;

		size_t i = 0;
		while (i < decoder.GetFrameCount())
		{
			const size_t decodedBufferFrameCount = decodedBuffer.FrameCount();
			if (decodedBufferFrameCount < minRequiredFrameCount)
			{
				const AudioBuffer tempDecodedBuffer = decoder.DecodeWholePackets(minRequiredFrameCount - decodedBufferFrameCount);
				if (decodedBufferFrameCount == 0)
				{
					decodedBuffer = tempDecodedBuffer;
				}
				else
				{
					decodedBuffer.Append(tempDecodedBuffer);
				}
			}

			if (inputSampleRate != encoder.outputFormatInfo.sampleRate)
			{
				AudioBuffer convertedBuffer(encoder.avFrame->nb_samples, encoder.outputFormatInfo);
				AudioProcessor::ChangeSampleRate(decodedBuffer, convertedBuffer, 0, encoder.outputFormatInfo.sampleRate, encoder.avFrame->nb_samples);

				encoder.Encode(convertedBuffer);
				decodedBuffer.Cut(0, readSize);

				i += readSize;
			}
			else
			{
				encoder.Encode(decodedBuffer.GetSubBuffer(0, encoder.avFrame->nb_samples));
				decodedBuffer.Cut(0, encoder.avFrame->nb_samples);
				i += encoder.avFrame->nb_samples;
			}
		}
	}
}

#endif