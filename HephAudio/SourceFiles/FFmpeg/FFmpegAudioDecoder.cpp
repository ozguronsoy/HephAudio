#include "FFmpeg/FFmpegAudioDecoder.h"
#include "HephException.h"
#include "File.h"
#include "ConsoleLogger.h"

using namespace HephCommon;

namespace HephAudio
{
	FFmpegAudioDecoder::FFmpegAudioDecoder()
		: fileDuration_frame(0), audioStreamIndex(FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND)
		, firstPacketPts(0), avFormatContext(nullptr), avCodecContext(nullptr)
		, swrContext(nullptr), avFrame(nullptr), avPacket(nullptr) {}

	FFmpegAudioDecoder::FFmpegAudioDecoder(const std::string& filePath) : FFmpegAudioDecoder()
	{
		this->OpenFile(filePath);
	}

	FFmpegAudioDecoder::FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept
		: fileDuration_frame(rhs.fileDuration_frame), audioStreamIndex(rhs.audioStreamIndex)
		, firstPacketPts(rhs.firstPacketPts), avFormatContext(rhs.avFormatContext), avCodecContext(rhs.avCodecContext)
		, swrContext(rhs.swrContext), avFrame(rhs.avFrame), avPacket(rhs.avPacket)
	{
		this->filePath = std::move(rhs.filePath);

		rhs.avFormatContext = nullptr;
		rhs.avCodecContext = nullptr;
		rhs.swrContext = nullptr;
		rhs.avFrame = nullptr;
		rhs.avPacket = nullptr;
	}

	FFmpegAudioDecoder::~FFmpegAudioDecoder()
	{
		this->CloseFile();
	}

	FFmpegAudioDecoder& FFmpegAudioDecoder::operator=(FFmpegAudioDecoder&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->CloseFile();

			this->filePath = std::move(rhs.filePath);
			this->fileDuration_frame = rhs.fileDuration_frame;
			this->audioStreamIndex = rhs.audioStreamIndex;
			this->firstPacketPts = rhs.firstPacketPts;
			this->avFormatContext = rhs.avFormatContext;
			this->avCodecContext = rhs.avCodecContext;
			this->swrContext = rhs.swrContext;
			this->avFrame = rhs.avFrame;
			this->avPacket = rhs.avPacket;

			rhs.avFormatContext = nullptr;
			rhs.avCodecContext = nullptr;
			rhs.swrContext = nullptr;
			rhs.avFrame = nullptr;
			rhs.avPacket = nullptr;
		}

		return *this;
	}

	void FFmpegAudioDecoder::ChangeFile(const std::string& newAudioFilePath)
	{
		if (this->filePath != newAudioFilePath)
		{
			this->CloseFile();
			this->OpenFile(newAudioFilePath);
		}
	}

	void FFmpegAudioDecoder::CloseFile()
	{
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

		if (this->avFormatContext != nullptr)
		{
			avformat_close_input(&this->avFormatContext);
			this->avFormatContext = nullptr;
		}

		this->filePath = "";
		this->fileDuration_frame = 0;
		this->audioStreamIndex = FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND;
		this->firstPacketPts = 0;
	}

	bool FFmpegAudioDecoder::IsFileOpen() const
	{
		return this->filePath != "" && this->avFormatContext != nullptr
			&& this->avCodecContext != nullptr && this->swrContext != nullptr
			&& this->avFrame != nullptr && this->avPacket != nullptr;
	}

	AudioFormatInfo FFmpegAudioDecoder::GetOutputFormatInfo() const
	{
		AVStream* avStream = this->avFormatContext->streams[this->audioStreamIndex];
		return HEPHAUDIO_INTERNAL_FORMAT(HephAudio::FromAVChannelLayout(avStream->codecpar->ch_layout), avStream->codecpar->sample_rate);
	}

	size_t FFmpegAudioDecoder::GetFrameCount() const
	{
		return this->fileDuration_frame;
	}

	bool FFmpegAudioDecoder::Seek(size_t frameIndex)
	{
		if (frameIndex >= this->fileDuration_frame)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioDecoder::Seek", "frameIndex out of bounds."));
			return false;
		}

		const int ret = this->SeekFrame(frameIndex);
		if (ret < 0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Seek", "Failed to seek frame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			return false;
		}
		return true;
	}

	AudioBuffer FFmpegAudioDecoder::Decode()
	{
		return this->Decode(0, this->fileDuration_frame);
	}

	AudioBuffer FFmpegAudioDecoder::Decode(size_t frameCount)
	{
		if (!this->IsFileOpen())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder::Decode", "No open file to decode."));
			return AudioBuffer(frameCount, this->GetOutputFormatInfo().channelLayout, this->GetOutputFormatInfo().sampleRate);
		}

		int ret = 0;
		AudioFormatInfo outputFormatInfo = this->GetOutputFormatInfo();
		size_t readFrameCount = 0;
		AudioBuffer decodedBuffer(frameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate);
		AVStream* avStream = this->avFormatContext->streams[this->audioStreamIndex];

		// convert the decoded data to inner format
		(void)av_opt_set_chlayout(this->swrContext, "in_chlayout", &avStream->codecpar->ch_layout, 0);
		(void)av_opt_set_int(this->swrContext, "in_sample_rate", avStream->codecpar->sample_rate, 0);
		(void)av_opt_set_sample_fmt(this->swrContext, "in_sample_fmt", (AVSampleFormat)avStream->codecpar->format, 0);

		ret = swr_init(this->swrContext);
		if (ret < 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to initialize SwrContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		while (readFrameCount < frameCount)
		{
			// get the next frame from file
			ret = av_read_frame(this->avFormatContext, this->avPacket);
			if (ret == AVERROR_EOF)
			{
				HEPHAUDIO_LOG("EOF, no more frames to read.", HEPH_CL_INFO);
				return decodedBuffer;
			}
			else if (ret < 0)
			{
				HEPHAUDIO_LOG("Failed to read the frame, skipping it...", HEPH_CL_WARNING);
				continue;
			}

			if (this->avPacket->stream_index == this->audioStreamIndex)
			{
				// send the packet to the decoder
				ret = avcodec_send_packet(this->avCodecContext, this->avPacket);
				if (ret == AVERROR_EOF)
				{
					av_packet_unref(this->avPacket);
					HEPHAUDIO_LOG("EOF, no more frames to read.", HEPH_CL_INFO);
					return decodedBuffer;
				}
				else if (ret < 0)
				{
					av_packet_unref(this->avPacket);
					HEPHAUDIO_LOG("Failed to decode the packet, skipping it...", HEPH_CL_WARNING);
					continue;
				}

				while (avcodec_receive_frame(this->avCodecContext, this->avFrame) >= 0)
				{
					const size_t currentFrameCount = this->avFrame->nb_samples;
					if (currentFrameCount > 0)
					{
						if (readFrameCount + currentFrameCount > frameCount)
						{
							decodedBuffer.Resize(readFrameCount + currentFrameCount);
						}

						AudioBuffer tempBuffer(currentFrameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate, BufferFlags::AllocUninitialized);
						uint8_t* targetOutputFrame = (uint8_t*)tempBuffer.begin();
						ret = swr_convert(this->swrContext, &targetOutputFrame, currentFrameCount, (const uint8_t**)this->avFrame->data, currentFrameCount);
						if (ret < 0)
						{
							av_packet_unref(this->avPacket);
							av_frame_unref(this->avFrame);
							RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
						}
						else if (ret < currentFrameCount)
						{
							ret = swr_convert(this->swrContext, &targetOutputFrame, currentFrameCount, nullptr, 0);
							if (ret < 0)
							{
								av_packet_unref(this->avPacket);
								av_frame_unref(this->avFrame);
								RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
							}
						}

						for (size_t i = 0; i < currentFrameCount; i++)
						{
							for (size_t j = 0; j < outputFormatInfo.channelLayout.count; j++)
							{
								decodedBuffer[i + readFrameCount][j] = tempBuffer[i][j];
							}
						}

						readFrameCount += currentFrameCount;
					}
					av_frame_unref(this->avFrame);
				}
			}
			av_packet_unref(this->avPacket);
		}

		return decodedBuffer;
	}

	AudioBuffer FFmpegAudioDecoder::Decode(size_t frameIndex, size_t frameCount)
	{
		const AudioFormatInfo outputFormatInfo = this->GetOutputFormatInfo();

		if (!this->IsFileOpen())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder::Decode", "No open file to decode."));
			return AudioBuffer(frameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate);
		}

		if (frameIndex >= this->fileDuration_frame)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioDecoder::Decode", "frameIndex out of bounds."));
			return AudioBuffer(frameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate);
		}

		if (frameIndex + frameCount > this->fileDuration_frame)
		{
			frameCount = this->fileDuration_frame - frameIndex;
		}

		int ret = this->SeekFrame(frameIndex);
		if (ret < 0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to seek frame.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			return AudioBuffer(frameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate);
		}

		size_t readFrameCount = 0;
		AudioBuffer decodedBuffer(frameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate);
		AVStream* avStream = this->avFormatContext->streams[this->audioStreamIndex];

		// convert the decoded data to inner format
		(void)av_opt_set_chlayout(this->swrContext, "in_chlayout", &avStream->codecpar->ch_layout, 0);
		(void)av_opt_set_int(this->swrContext, "in_sample_rate", avStream->codecpar->sample_rate, 0);
		(void)av_opt_set_sample_fmt(this->swrContext, "in_sample_fmt", (AVSampleFormat)avStream->codecpar->format, 0);

		ret = swr_init(this->swrContext);
		if (ret < 0)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to initialize SwrContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		while (readFrameCount < frameCount)
		{
			// get the next frame from file
			ret = av_read_frame(this->avFormatContext, this->avPacket);
			if (ret == AVERROR_EOF)
			{
				HEPHAUDIO_LOG("EOF, no more frames to read.", HEPH_CL_INFO);
				return decodedBuffer;
			}
			else if (ret < 0)
			{
				HEPHAUDIO_LOG("Failed to read the frame, skipping it...", HEPH_CL_WARNING);
				continue;
			}

			if (this->avPacket->stream_index == this->audioStreamIndex)
			{
				// send the packet to the decoder
				ret = avcodec_send_packet(this->avCodecContext, this->avPacket);
				if (ret == AVERROR_EOF)
				{
					av_packet_unref(this->avPacket);
					HEPHAUDIO_LOG("EOF, no more frames to read.", HEPH_CL_INFO);
					return decodedBuffer;
				}
				else if (ret < 0)
				{
					av_packet_unref(this->avPacket);
					HEPHAUDIO_LOG("Failed to decode the packet, skipping it...", HEPH_CL_WARNING);
					continue;
				}

				while (readFrameCount < frameCount && avcodec_receive_frame(this->avCodecContext, this->avFrame) >= 0)
				{
					size_t currentFrameCount = this->avFrame->nb_samples;
					size_t currentFramesToRead = currentFrameCount;
					if (currentFrameCount > 0)
					{
						// prevent overflow
						if (frameIndex > 0)
						{
							if (frameIndex >= currentFrameCount)
							{
								currentFrameCount = av_rescale(this->avFrame->duration * this->avFrame->sample_rate, avStream->time_base.num, avStream->time_base.den);
								if (frameIndex >= currentFrameCount)
								{
									frameIndex -= currentFrameCount;
									av_frame_unref(this->avFrame);
									continue;
								}
							}

							if (frameIndex + frameCount > currentFrameCount)
							{
								currentFramesToRead = currentFrameCount - frameIndex;
							}
						}

						if (readFrameCount + currentFramesToRead > frameCount)
						{
							currentFramesToRead = frameCount - readFrameCount;
						}

						AudioBuffer tempBuffer(currentFrameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate, BufferFlags::AllocUninitialized);
						uint8_t* targetOutputFrame = (uint8_t*)tempBuffer.begin();
						ret = swr_convert(this->swrContext, &targetOutputFrame, currentFrameCount, (const uint8_t**)this->avFrame->data, currentFrameCount);
						if (ret < 0)
						{
							av_packet_unref(this->avPacket);
							av_frame_unref(this->avFrame);
							RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
						}
						else if (ret < currentFrameCount)
						{
							ret = swr_convert(this->swrContext, &targetOutputFrame, currentFrameCount, nullptr, 0);
							if (ret < 0)
							{
								av_packet_unref(this->avPacket);
								av_frame_unref(this->avFrame);
								RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
							}
						}

						for (size_t i = 0; i < currentFramesToRead; i++)
						{
							for (size_t j = 0; j < outputFormatInfo.channelLayout.count; j++)
							{
								decodedBuffer[i + readFrameCount][j] = tempBuffer[i + frameIndex][j];
							}
						}

						readFrameCount += currentFramesToRead;
						frameIndex = 0;
					}
					av_frame_unref(this->avFrame);
				}
			}
			av_packet_unref(this->avPacket);
		}

		return decodedBuffer;
	}

	AudioBuffer FFmpegAudioDecoder::Decode(const EncodedAudioBuffer& encodedBuffer)
	{
		int ret = 0;
		const AudioFormatInfo& inputFormatInfo = encodedBuffer.GetAudioFormatInfo();
		AudioFormatInfo outputFormatInfo = HEPHAUDIO_INTERNAL_FORMAT(inputFormatInfo.channelLayout, inputFormatInfo.sampleRate);

		SwrContext* swrContext = swr_alloc();
		if (swrContext == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder::Decode", "Failed to create SwrContext."));
		}

		const AVChannelLayout avChLayout = HephAudio::ToAVChannelLayout(inputFormatInfo.channelLayout);
		const AVSampleFormat avSampleFormat = HephAudio::ToAVSampleFormat(inputFormatInfo);

		const FFmpegEncodedAudioBuffer* pFFmpegBuffer = dynamic_cast<const FFmpegEncodedAudioBuffer*>(&encodedBuffer);
		if (pFFmpegBuffer == nullptr)
		{
			if (inputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_PCM &&
				inputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT &&
				inputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_ALAW &&
				inputFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_MULAW)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioDecoder::Decode", "EncodedAudioBuffer must contain raw PCM or a-law or mu-law data"));
			}

			av_opt_set_chlayout(swrContext, "in_chlayout", &avChLayout, 0);
			av_opt_set_int(swrContext, "in_sample_rate", inputFormatInfo.sampleRate, 0);
			av_opt_set_sample_fmt(swrContext, "in_sample_fmt", avSampleFormat, 0);

			av_opt_set_chlayout(swrContext, "out_chlayout", &avChLayout, 0);
			av_opt_set_int(swrContext, "out_sample_rate", inputFormatInfo.sampleRate, 0);
			av_opt_set_sample_fmt(swrContext, "out_sample_fmt", HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT, 0);

			ret = swr_init(swrContext);
			if (ret < 0)
			{
				swr_free(&swrContext);
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to initialize SwrContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}

			const size_t frameCount = encodedBuffer.Size() / inputFormatInfo.FrameSize();
			AudioBuffer resultBuffer(frameCount, outputFormatInfo.channelLayout, outputFormatInfo.sampleRate, BufferFlags::AllocUninitialized);
			const uint8_t* const inputBegin = (const uint8_t* const)encodedBuffer.begin();
			uint8_t* const outputBegin = (uint8_t* const)resultBuffer.begin();

			ret = swr_convert(swrContext, &outputBegin, frameCount, &inputBegin, frameCount);
			if (ret < 0)
			{
				swr_free(&swrContext);
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
			}
			else if (ret < frameCount)
			{
				ret = swr_convert(swrContext, &outputBegin, frameCount, nullptr, 0);
				if (ret < 0)
				{
					swr_free(&swrContext);
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
				}
			}

			swr_free(&swrContext);

			return resultBuffer;
		}

		const AVCodecID codecID = HephAudio::CodecIdFromAudioFormatInfo(inputFormatInfo);
		const AVCodec* avCodec = avcodec_find_decoder(codecID);

		AVCodecContext* avCodecContext = avcodec_alloc_context3(avCodec);
		if (avCodecContext == nullptr)
		{
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder::Decode", "Failed to allocate AVCodecContext"));
		}
		avCodecContext->sample_fmt = avSampleFormat;
		avCodecContext->sample_rate = inputFormatInfo.sampleRate;
		avCodecContext->ch_layout = avChLayout;
		avCodecContext->block_align = pFFmpegBuffer->GetBlockAlign();
		avCodecContext->extradata_size = pFFmpegBuffer->GetExtraDataSize();
		avCodecContext->extradata = (uint8_t*)pFFmpegBuffer->GetExtraData();

		ret = avcodec_open2(avCodecContext, avCodec, nullptr);
		if (ret < 0)
		{
			avCodecContext->extradata = nullptr;
			avcodec_free_context(&avCodecContext);
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to allocate output format context.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		// avcodec_open2 may change the sample_rate and sample_fmt
		(void)av_opt_set_chlayout(swrContext, "in_chlayout", &avCodecContext->ch_layout, 0);
		(void)av_opt_set_int(swrContext, "in_sample_rate", avCodecContext->sample_rate, 0);
		(void)av_opt_set_sample_fmt(swrContext, "in_sample_fmt", avCodecContext->sample_fmt, 0);

		(void)av_opt_set_chlayout(swrContext, "out_chlayout", &avCodecContext->ch_layout, 0);
		(void)av_opt_set_int(swrContext, "out_sample_rate", avCodecContext->sample_rate, 0);
		(void)av_opt_set_sample_fmt(swrContext, "out_sample_fmt", HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT, 0);

		outputFormatInfo.channelLayout = HephAudio::FromAVChannelLayout(avCodecContext->ch_layout);
		outputFormatInfo.sampleRate = avCodecContext->sample_rate;

		ret = swr_init(swrContext);
		if (ret < 0)
		{
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to initialize SwrContext.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		AVFrame* avFrame = av_frame_alloc();
		if (avFrame == nullptr)
		{
			avCodecContext->extradata = nullptr;
			avcodec_free_context(&avCodecContext);
			swr_free(&swrContext);
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder::Decode", "Failed to allocate AVFrame"));
		}

		size_t frameIndex = 0;
		AudioBuffer resultBuffer(pFFmpegBuffer->GetFrameCount(), outputFormatInfo.channelLayout, outputFormatInfo.sampleRate);

		for (AVPacket*& packet : (*pFFmpegBuffer))
		{
			ret = avcodec_send_packet(avCodecContext, packet);
			if (ret < 0)
			{
				HEPHAUDIO_LOG("Failed to decode the packet, skipping it...", HEPH_CL_WARNING);
				continue;
			}

			while (avcodec_receive_frame(avCodecContext, avFrame) >= 0)
			{
				size_t currentFrameCount = avFrame->nb_samples;
				if (currentFrameCount > 0)
				{
					if (frameIndex + currentFrameCount > resultBuffer.FrameCount())
					{
						currentFrameCount = resultBuffer.FrameCount() - frameIndex;
					}

					uint8_t* pOutputFrame = (uint8_t*)resultBuffer[frameIndex];
					ret = swr_convert(swrContext, &pOutputFrame, currentFrameCount, (const uint8_t**)avFrame->data, currentFrameCount);
					if (ret < 0)
					{
						av_frame_unref(avFrame);
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
					}
					else if (ret < currentFrameCount)
					{
						ret = swr_convert(swrContext, &pOutputFrame, currentFrameCount, nullptr, 0);
						if (ret < 0)
						{
							av_frame_unref(avFrame);
							RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
						}
					}

					frameIndex += currentFrameCount;
				}
				av_frame_unref(avFrame);
			}
		}

		avCodecContext->extradata = nullptr;
		avcodec_free_context(&avCodecContext);
		av_frame_free(&avFrame);
		swr_free(&swrContext);

		return resultBuffer;
	}

	void FFmpegAudioDecoder::OpenFile(const std::string& filePath)
	{
		if (!File::FileExists(filePath))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "File not found."));
			return;
		}

		this->filePath = filePath;

		int ret = avformat_open_input(&this->avFormatContext, this->filePath.c_str(), nullptr, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Could not open the file.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		ret = avformat_find_stream_info(this->avFormatContext, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "An error occurred while finding the stream info, file might be corrupted.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		// Calculate duration of audio data in frames
		this->audioStreamIndex = FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND;
		AVStream* avStream = nullptr;
		for (size_t i = 0; i < this->avFormatContext->nb_streams; i++)
		{
			avStream = this->avFormatContext->streams[i];
			if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				this->fileDuration_frame = av_rescale(avStream->duration * avStream->codecpar->sample_rate, avStream->time_base.num, avStream->time_base.den);
				this->audioStreamIndex = i;
				break;
			}
		}

		if (this->audioStreamIndex == FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "No audio stream found."));
		}

		// Initialize swr context
		this->swrContext = swr_alloc();
		if (this->swrContext == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Failed to create SwrContext."));
		}

		(void)av_opt_set_chlayout(this->swrContext, "out_chlayout", &avStream->codecpar->ch_layout, 0);
		(void)av_opt_set_int(this->swrContext, "out_sample_rate", avStream->codecpar->sample_rate, 0);
		(void)av_opt_set_sample_fmt(this->swrContext, "out_sample_fmt", HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT, 0);

		// Initialize codec for decoding
		const AVCodec* avCodec = avcodec_find_decoder(this->avFormatContext->streams[audioStreamIndex]->codecpar->codec_id);
		if (avCodec == nullptr)
		{
			const std::string codecName = avcodec_get_name(this->avFormatContext->streams[audioStreamIndex]->codecpar->codec_id);
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "No decoder found for the " + codecName + " codec."));
		}

		this->avCodecContext = avcodec_alloc_context3(avCodec);
		if (this->avCodecContext == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Failed to allocate codec context."));
		}

		ret = avcodec_parameters_to_context(this->avCodecContext, this->avFormatContext->streams[audioStreamIndex]->codecpar);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Failed to create codec context from the codec parameters.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		ret = avcodec_open2(this->avCodecContext, avCodec, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Failed to open the av codec context.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}

		// allocate frame and packet
		this->avFrame = av_frame_alloc();
		if (this->avFrame == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Failed to allocate av frame."));
		}

		this->avPacket = av_packet_alloc();
		if (this->avPacket == nullptr)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Failed to allocate av packet."));
		}

		ret = av_read_frame(this->avFormatContext, this->avPacket);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Failed to get pts of the first packet.", "FFmpeg", HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(ret)));
		}
		this->firstPacketPts = this->avPacket->pts;
		av_packet_unref(this->avPacket);
	}

	int FFmpegAudioDecoder::SeekFrame(size_t& frameIndex)
	{
		constexpr int seekFlags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME;

		int ret = 0;
		bool endSeek = false;
		AVStream* avStream = this->avFormatContext->streams[this->audioStreamIndex];
		const int64_t timestamp = this->firstPacketPts + av_rescale(frameIndex, avStream->time_base.den, (int64_t)avStream->time_base.num * avStream->codecpar->sample_rate);

		ret = avformat_seek_file(this->avFormatContext, this->audioStreamIndex, INT64_MIN, timestamp, timestamp, seekFlags);
		if (ret < 0)
		{
			return ret;
		}
		avcodec_flush_buffers(this->avCodecContext);

		// calculate frameIndex relative to the packet pos
		// and how many packets to skip after seek (seek does not always find the packet we are looking for)
		int64_t skippedPacketCount = -1;
		int64_t packetDuration = 0;
		int64_t deltaFrameIndex = 0;
		do
		{
			ret = av_read_frame(this->avFormatContext, this->avPacket);
			if (ret < 0)
			{
				return ret;
			}

			if (this->avPacket->stream_index != this->audioStreamIndex)
			{
				av_packet_unref(this->avPacket);
				continue;
			}

			skippedPacketCount++;

			const int64_t packetFrameIndex = av_rescale((this->avPacket->pts - this->firstPacketPts) * avStream->codecpar->sample_rate, avStream->time_base.num, avStream->time_base.den);
			if (frameIndex < packetFrameIndex)
			{
				HEPHAUDIO_LOG("Failed to find the exact packet that contains the requested frames, picking the closest one...", HEPH_CL_WARNING);
				deltaFrameIndex = 0;
				av_packet_unref(this->avPacket);
				break;
			}

			packetDuration = av_rescale(this->avPacket->duration * avStream->codecpar->sample_rate, avStream->time_base.num, avStream->time_base.den);
			deltaFrameIndex = frameIndex - packetFrameIndex;
			av_packet_unref(this->avPacket);
		} while (deltaFrameIndex >= packetDuration);

		frameIndex = deltaFrameIndex;

		// seek again and skip the unnecessary packets
		ret = avformat_seek_file(this->avFormatContext, this->audioStreamIndex, INT64_MIN, timestamp, timestamp, seekFlags);
		if (ret < 0)
		{
			return ret;
		}
		avcodec_flush_buffers(this->avCodecContext);

		for (size_t i = 0; i < skippedPacketCount; i++)
		{
			ret = av_read_frame(this->avFormatContext, this->avPacket);
			if (ret < 0)
			{
				return ret;
			}
			av_packet_unref(this->avPacket);
		}

		return 0;
	}
}