#include "FFmpeg/FFmpegAudioEncoder.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "AudioProcessor.h"
#include "HephException.h"
#include "HephMath.h"
#include "ConsoleLogger.h"

using namespace HephCommon;

namespace HephAudio
{
	FFmpegAudioEncoder::FFmpegAudioEncoder()
		: avFormatContext(nullptr), avIoContext(nullptr)
		, avStream(nullptr), avCodecContext(nullptr), swrContext(nullptr)
		, avPacket(nullptr), avFrame(nullptr) {}

	FFmpegAudioEncoder::FFmpegAudioEncoder(const std::filesystem::path& filePath, AudioFormatInfo outputFormatInfo, bool overwrite) : FFmpegAudioEncoder()
	{
		this->outputFormatInfo = outputFormatInfo;
		this->ChangeFile(filePath, overwrite);
	}

	FFmpegAudioEncoder::FFmpegAudioEncoder(FFmpegAudioEncoder&& rhs) noexcept
		: outputFormatInfo(rhs.outputFormatInfo), avFormatContext(rhs.avFormatContext)
		, avIoContext(rhs.avIoContext), avStream(rhs.avStream), avCodecContext(rhs.avCodecContext)
		, swrContext(rhs.swrContext), avPacket(rhs.avPacket), avFrame(rhs.avFrame)
	{
		this->filePath = std::move(rhs.filePath);

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

			this->filePath = std::move(rhs.filePath);
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

	void FFmpegAudioEncoder::ChangeFile(const std::filesystem::path& newAudioFilePath, bool overwrite)
	{
		if (this->filePath != newAudioFilePath)
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to write the file trailer.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
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

		this->filePath = "";
	}

	bool FFmpegAudioEncoder::IsFileOpen() const
	{
		return this->filePath != "" && this->avFormatContext != nullptr
			&& this->avIoContext != nullptr && this->avStream != nullptr
			&& this->avCodecContext != nullptr && this->swrContext != nullptr
			&& this->avFrame != nullptr && this->avPacket != nullptr;
	}

	void FFmpegAudioEncoder::Encode(const AudioBuffer& bufferToEncode)
	{
		if (!this->IsFileOpen())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "No open file to encode."));
			return;
		}

		if (bufferToEncode.IsEmpty())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "Trying to encode empty buffer."));
			return;
		}

		const AudioFormatInfo inputFormatInfo = bufferToEncode.FormatInfo();
		const AVChannelLayout inputChannelLayout = ToAVChannelLayout(inputFormatInfo.channelLayout);
		const AVSampleFormat inputSampleFormat = HephAudio::ToAVSampleFormat(this->avCodecContext->codec, inputFormatInfo);

		if (inputSampleFormat == AV_SAMPLE_FMT_NONE)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Unsupported format"));
		}

		(void)av_opt_set_chlayout(this->swrContext, "in_chlayout", &inputChannelLayout, 0);
		(void)av_opt_set_int(this->swrContext, "in_sample_rate", inputFormatInfo.sampleRate, 0);
		(void)av_opt_set_sample_fmt(this->swrContext, "in_sample_fmt", inputSampleFormat, 0);

		int ret = swr_init(this->swrContext);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to initialize SwrContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		size_t i = 0;
		while (i < bufferToEncode.FrameCount())
		{
			uint8_t* pCurrentInputFrame = (uint8_t*)bufferToEncode.begin() + i * inputFormatInfo.FrameSize();

			size_t inputFrameCountToRead = av_rescale(this->avFrame->nb_samples, inputFormatInfo.sampleRate, this->outputFormatInfo.sampleRate);
			if (i + inputFrameCountToRead > bufferToEncode.FrameCount())
			{
				inputFrameCountToRead = bufferToEncode.FrameCount() - i;
			}

			ret = swr_convert(this->swrContext, this->avFrame->data, this->avFrame->nb_samples, (const uint8_t**)&pCurrentInputFrame, inputFrameCountToRead);
			if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to convert.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}
			else if (ret < this->avFrame->nb_samples)
			{
				(void)swr_convert(this->swrContext, this->avFrame->data, this->avFrame->nb_samples, nullptr, 0); // flush the buffered samples
			}

			ret = avcodec_send_frame(this->avCodecContext, this->avFrame);
			if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to send AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to recieve AVPacket.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}

			this->avPacket->pts = av_rescale_q(this->avPacket->pts, this->avCodecContext->time_base, this->avStream->time_base);
			this->avPacket->dts = av_rescale_q(this->avPacket->dts, this->avCodecContext->time_base, this->avStream->time_base);
			this->avPacket->duration = av_rescale_q(this->avPacket->duration, this->avCodecContext->time_base, this->avStream->time_base);

			ret = av_interleaved_write_frame(this->avFormatContext, this->avPacket);
			if (ret < 0)
			{
				this->CloseFile();
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to write AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}

			this->avFrame->pts += this->avFrame->duration;
			i += inputFrameCountToRead;

			av_packet_unref(this->avPacket);
		}
	}

	void FFmpegAudioEncoder::Encode(const AudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer)
	{
		int ret;
		const AudioFormatInfo& inputFormatInfo = inputBuffer.FormatInfo();
		const AudioFormatInfo& outputFormatInfo = outputBuffer.GetAudioFormatInfo();

		SwrContext* swrContext = swr_alloc();
		if (swrContext == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to create SwrContext."));
		}

		const AVChannelLayout inputAVChLayout = HephAudio::ToAVChannelLayout(inputFormatInfo.channelLayout);
		const AVChannelLayout outputAVChLayout = HephAudio::ToAVChannelLayout(outputFormatInfo.channelLayout);
		const AVSampleFormat outputSampleFormat = HephAudio::ToAVSampleFormat(outputFormatInfo);
		const uint32_t outputSampleRate = HephAudio::GetClosestSupportedSampleRate(outputFormatInfo);

		if (outputSampleFormat == AV_SAMPLE_FMT_NONE)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "Unsupported format"));
		}

		(void)av_opt_set_chlayout(swrContext, "in_chlayout", &inputAVChLayout, 0);
		(void)av_opt_set_int(swrContext, "in_sample_rate", inputFormatInfo.sampleRate, 0);
		(void)av_opt_set_sample_fmt(swrContext, "in_sample_fmt", HephAudio::ToAVSampleFormat(inputFormatInfo), 0);

		(void)av_opt_set_chlayout(swrContext, "out_chlayout", &outputAVChLayout, 0);
		(void)av_opt_set_int(swrContext, "out_sample_rate", outputSampleRate, 0);
		(void)av_opt_set_sample_fmt(swrContext, "out_sample_fmt", outputSampleFormat, 0);

		ret = swr_init(swrContext);
		if (ret < 0)
		{
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to initialize SwrContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		FFmpegEncodedAudioBuffer* pFFmpegBuffer = dynamic_cast<FFmpegEncodedAudioBuffer*>(&outputBuffer);
		if (pFFmpegBuffer == nullptr)
		{
			if (outputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_PCM &&
				outputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT &&
				outputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_ALAW &&
				outputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_MULAW)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "output format must be raw PCM or a-law or mu-law"));
			}

			const size_t frameCount = inputBuffer.FrameCount();

			outputBuffer.Resize(frameCount * outputFormatInfo.FrameSize());

			const uint8_t* const inputBegin = (const uint8_t* const)inputBuffer.begin();
			uint8_t* const outputBegin = (uint8_t* const)outputBuffer.begin();

			ret = swr_convert(swrContext, &outputBegin, frameCount, &inputBegin, frameCount);
			if (ret < 0)
			{
				swr_free(&swrContext);
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to encode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}
			else if (ret < frameCount)
			{
				ret = swr_convert(swrContext, &outputBegin, frameCount, nullptr, 0);
				if (ret < 0)
				{
					swr_free(&swrContext);
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to encode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
				}
			}

			swr_free(&swrContext);
			return;
		}

		const AVCodecID codecID = HephAudio::CodecIdFromAudioFormatInfo(outputFormatInfo);
		const AVCodec* avCodec = avcodec_find_encoder(codecID);

		AVCodecContext* avCodecContext = avcodec_alloc_context3(avCodec);
		if (avCodecContext == nullptr)
		{
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to allocate AVCodecContext"));
		}
		avCodecContext->sample_fmt = outputSampleFormat;
		avCodecContext->sample_rate = outputSampleRate;
		avCodecContext->ch_layout = outputAVChLayout;

		if (avCodecContext->codec_id == AV_CODEC_ID_MP3 && outputFormatInfo.bitRate == 0) // otherwise encoder calculates the duration wrong
		{
			AudioFormatInfo tempFormatInfo = outputFormatInfo;
			tempFormatInfo.bitRate = 128000;
			outputBuffer.SetAudioFormatInfo(tempFormatInfo);
		}
		avCodecContext->bit_rate = outputFormatInfo.bitRate;

		ret = avcodec_open2(avCodecContext, avCodec, nullptr);
		if (ret < 0)
		{
			avcodec_free_context(&avCodecContext);
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to allocate output format context.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		pFFmpegBuffer->SetBlockAlign(avCodecContext->block_align);
		pFFmpegBuffer->SetExtraData(avCodecContext->extradata, avCodecContext->extradata_size);

		if (avCodecContext->frame_size == 0)
		{
			avCodecContext->frame_size = avCodecContext->sample_rate / 100; // 10 ms
		}

		AVFrame* avFrame = av_frame_alloc();
		if (avFrame == nullptr)
		{
			avcodec_free_context(&avCodecContext);
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to allocate AVFrame"));
		}
		avFrame->nb_samples = avCodecContext->frame_size;
		avFrame->ch_layout = avCodecContext->ch_layout;
		avFrame->format = (int)avCodecContext->sample_fmt;
		avFrame->sample_rate = avCodecContext->sample_rate;
		avFrame->pts = avCodecContext->initial_padding;
		avFrame->duration = avCodecContext->frame_size;

		ret = av_frame_get_buffer(avFrame, 0);
		if (ret < 0)
		{
			avcodec_free_context(&avCodecContext);
			av_frame_free(&avFrame);
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to allocate buffer(s) for AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		const size_t inputBufferFrameCount = inputBuffer.FrameCount();
		size_t inputFrameCountToRead = av_rescale(avFrame->nb_samples, inputFormatInfo.sampleRate, outputFormatInfo.sampleRate);
		size_t packetFrameCount = 0;
		for (size_t i = 0; i < inputBufferFrameCount; i += inputFrameCountToRead)
		{
			const uint8_t* pInputFrame = (const uint8_t*)inputBuffer[i];
			if (i + inputFrameCountToRead > inputBufferFrameCount)
			{
				inputFrameCountToRead = inputBufferFrameCount - i;
			}

			packetFrameCount += avFrame->nb_samples;
			avFrame->pts += avFrame->duration;

			ret = swr_convert(swrContext, avFrame->data, avFrame->nb_samples, &pInputFrame, inputFrameCountToRead);
			if (ret < 0)
			{
				avcodec_free_context(&avCodecContext);
				av_frame_free(&avFrame);
				swr_free(&swrContext);
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to swr convert.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}
			else if (ret < avFrame->nb_samples)
			{
				(void)swr_convert(swrContext, avFrame->data, avFrame->nb_samples, nullptr, 0); // flush the buffered samples
			}

			ret = avcodec_send_frame(avCodecContext, avFrame);
			if (ret < 0)
			{
				HEPHAUDIO_LOG("Failed to send frame, skipping the packet...", HEPH_CL_WARNING);
				continue;
			}

			AVPacket* avPacket = av_packet_alloc();
			if (avPacket == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to allocate AVPacket"));
			}

			ret = avcodec_receive_packet(avCodecContext, avPacket);
			if (ret == AVERROR(EAGAIN))
			{
				// not enough samples to encode, send more frames to the encoder
				continue;
			}
			else if (ret < 0)
			{
				avcodec_free_context(&avCodecContext);
				av_frame_free(&avFrame);
				swr_free(&swrContext);
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to recieve AVPacket", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}

			pFFmpegBuffer->Add(avPacket, packetFrameCount);
			packetFrameCount = 0;
		}

		avcodec_free_context(&avCodecContext);
		av_frame_free(&avFrame);
		swr_free(&swrContext);
	}

	void FFmpegAudioEncoder::Transcode(const EncodedAudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer)
	{
		FFmpegAudioDecoder decoder;
		const AudioBuffer decodedBuffer = decoder.Decode(inputBuffer);
		this->Encode(decodedBuffer, outputBuffer);
	}

	void FFmpegAudioEncoder::Transcode(const std::filesystem::path& inputFilePath, const std::filesystem::path& outputFilePath, bool overwrite)
	{
		FFmpegAudioDecoder decoder(inputFilePath);
		if (decoder.GetFrameCount() > 0)
		{
			FFmpegAudioEncoder encoder(outputFilePath, decoder.GetOutputFormatInfo(), overwrite);
			FFmpegAudioEncoder::Transcode(&decoder, encoder);
		}
	}

	void FFmpegAudioEncoder::Transcode(const std::filesystem::path& inputFilePath, const std::filesystem::path& outputFilePath, AudioFormatInfo outputFormatInfo, bool overwrite)
	{
		FFmpegAudioDecoder decoder(inputFilePath);
		if (decoder.GetFrameCount() > 0)
		{
			FFmpegAudioEncoder encoder(outputFilePath, outputFormatInfo, overwrite);
			FFmpegAudioEncoder::Transcode(&decoder, encoder);
		}
	}

	void FFmpegAudioEncoder::OpenFile(const std::filesystem::path& filePath, bool overwrite)
	{
		if (!overwrite && std::filesystem::exists(filePath))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "File already exists."));
			return;
		}

		this->filePath = filePath;

		int ret = avformat_alloc_output_context2(&this->avFormatContext, nullptr, nullptr, this->filePath.string().c_str());
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to allocate output format context.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		ret = avio_open(&this->avIoContext, this->filePath.string().c_str(), AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to open the output file.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}
		this->avFormatContext->pb = this->avIoContext;

		AVCodecID codecID = HephAudio::CodecIdFromAudioFormatInfo(this->outputFormatInfo);
		ret = avformat_query_codec(this->avFormatContext->oformat, codecID, FF_COMPLIANCE_NORMAL);
		if (ret < 0)
		{
			HEPHAUDIO_LOG("Requested file format cannot store the requested codec. Encoding with the default codec.", HEPH_CL_WARNING);
			codecID = this->avFormatContext->oformat->audio_codec;
		}

		const AVCodec* avCodec = avcodec_find_encoder(codecID);
		if (avCodec == nullptr)
		{
			const std::filesystem::path codecName = avcodec_get_name(codecID);
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "No encoder found for the " + codecName.string() + " codec."));
		}

		this->avCodecContext = avcodec_alloc_context3(avCodec);
		if (this->avCodecContext == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to create AVCodecContext."));
		}

		this->avCodecContext->sample_fmt = HephAudio::ToAVSampleFormat(avCodec, this->outputFormatInfo);
		if (this->avCodecContext->sample_fmt == AV_SAMPLE_FMT_NONE)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Unsupported format"));
		}

		this->outputFormatInfo.sampleRate = HephAudio::GetClosestSupportedSampleRate(avCodec, this->outputFormatInfo.sampleRate);
		this->avCodecContext->sample_rate = this->outputFormatInfo.sampleRate;
		this->avCodecContext->ch_layout = ToAVChannelLayout(this->outputFormatInfo.channelLayout);

		this->avCodecContext->codec_id = codecID;
		this->avCodecContext->codec_tag = av_codec_get_tag(this->avFormatContext->oformat->codec_tag, codecID);
		this->avCodecContext->codec_type = AVMEDIA_TYPE_AUDIO;

		if (this->avCodecContext->codec_id == AV_CODEC_ID_MP3 && this->outputFormatInfo.bitRate == 0) // otherwise encoder calculates the duration wrong
		{
			this->outputFormatInfo.bitRate = 128000;
		}
		this->avCodecContext->bit_rate = this->outputFormatInfo.bitRate;

		ret = avcodec_open2(this->avCodecContext, avCodec, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to open the AVCodecContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->avStream = avformat_new_stream(this->avFormatContext, nullptr);
		if (this->avStream == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to create AVStream."));
		}

		ret = avcodec_parameters_from_context(this->avStream->codecpar, this->avCodecContext);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to copy the codec parameters to the AVStream."));
		}

		if ((this->avFormatContext->oformat->flags & AVFMT_GLOBALHEADER) == AVFMT_GLOBALHEADER)
		{
			this->avCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}

		this->avFrame = av_frame_alloc();
		if (this->avFrame == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to allocate AVFrame."));
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

		this->avFrame->format = (int)this->avCodecContext->sample_fmt;
		ret = av_channel_layout_copy(&this->avFrame->ch_layout, &this->avCodecContext->ch_layout);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to copy the ch_layout info to the AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->avFrame->pts = this->avCodecContext->initial_padding;
		this->avFrame->duration = this->avFrame->nb_samples;

		ret = av_frame_get_buffer(this->avFrame, 0);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to allocate buffer(s) for AVFrame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->swrContext = swr_alloc();
		if (this->swrContext == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to create SwrContext."));
		}

		(void)av_opt_set_chlayout(this->swrContext, "out_chlayout", &this->avStream->codecpar->ch_layout, 0);
		(void)av_opt_set_int(this->swrContext, "out_sample_rate", this->avStream->codecpar->sample_rate, 0);
		(void)av_opt_set_sample_fmt(this->swrContext, "out_sample_fmt", (AVSampleFormat)this->avStream->codecpar->format, 0);

		ret = avformat_init_output(this->avFormatContext, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to initialize the output file.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		ret = avformat_write_header(this->avFormatContext, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, HEPH_FUNC, "Failed to write the file header.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		this->avPacket = av_packet_alloc();
		if (this->avPacket == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, HEPH_FUNC, "Failed to allocate AVPacket."));
		}
	}

	void FFmpegAudioEncoder::Transcode(void* pDecoder, FFmpegAudioEncoder& encoder)
	{
		FFmpegAudioDecoder& decoder = *(FFmpegAudioDecoder*)pDecoder;
		const uint32_t inputSampleRate = decoder.GetOutputFormatInfo().sampleRate;
		const size_t readSize = av_rescale(encoder.avFrame->nb_samples, inputSampleRate, encoder.outputFormatInfo.sampleRate);
		const size_t minRequiredFrameCount = FFMAX(readSize, encoder.avFrame->nb_samples);
		AudioBuffer decodedBuffer;

		size_t i = 0;
		while (i < decoder.GetFrameCount())
		{
			const size_t decodedBufferFrameCount = decodedBuffer.FrameCount();
			if (decodedBufferFrameCount < minRequiredFrameCount)
			{
				const AudioBuffer tempDecodedBuffer = decoder.Decode(minRequiredFrameCount - decodedBufferFrameCount);
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
				AudioBuffer convertedBuffer = decodedBuffer.SubBuffer(0, readSize + 1);
				AudioProcessor::ChangeSampleRate(convertedBuffer, encoder.outputFormatInfo.sampleRate);

				if (convertedBuffer.FrameCount() != encoder.avFrame->nb_samples)
				{
					convertedBuffer.Resize(encoder.avFrame->nb_samples);
				}

				encoder.Encode(convertedBuffer);
				decodedBuffer.Cut(0, readSize);

				i += readSize;
			}
			else
			{
				encoder.Encode(decodedBuffer.SubBuffer(0, encoder.avFrame->nb_samples));
				decodedBuffer.Cut(0, encoder.avFrame->nb_samples);
				i += encoder.avFrame->nb_samples;
			}
		}
	}
}