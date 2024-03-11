#if defined(HEPHAUDIO_USE_FFMPEG)
#include "FFmpegAudioDecoder.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"

using namespace HephCommon;

namespace HephAudio
{
	FFmpegAudioDecoder::FFmpegAudioDecoder()
		: audioFilePath(nullptr), fileDuration_frame(0), audioStreamIndex(FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND)
		, channelCount(0), sampleRate(0), firstPacketPts(0), avFormatContext(nullptr)
		, avCodecContext(nullptr), swrContext(nullptr), avFrame(nullptr), avPacket(nullptr) {}
	FFmpegAudioDecoder::FFmpegAudioDecoder(const StringBuffer& audioFilePath) : FFmpegAudioDecoder()
	{
		this->OpenFile(audioFilePath);
	}
	FFmpegAudioDecoder::FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept
		: audioFilePath(std::move(rhs.audioFilePath)), fileDuration_frame(rhs.fileDuration_frame), audioStreamIndex(rhs.audioStreamIndex)
		, channelCount(rhs.channelCount), sampleRate(rhs.sampleRate), firstPacketPts(rhs.firstPacketPts), avFormatContext(rhs.avFormatContext)
		, avCodecContext(rhs.avCodecContext), swrContext(nullptr), avFrame(rhs.avFrame), avPacket(rhs.avPacket)
	{
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
		this->CloseFile();

		this->audioFilePath = std::move(rhs.audioFilePath);
		this->fileDuration_frame = rhs.fileDuration_frame;
		this->audioStreamIndex = rhs.audioStreamIndex;
		this->channelCount = rhs.channelCount;
		this->sampleRate = rhs.sampleRate;
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

		return *this;
	}
	void FFmpegAudioDecoder::ChangeFile(const StringBuffer& newAudioFilePath)
	{
		if (!this->audioFilePath.CompareContent(newAudioFilePath))
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

		this->audioFilePath = nullptr;
		this->fileDuration_frame = 0;
		this->audioStreamIndex = FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND;
		this->channelCount = 0;
		this->sampleRate = 0;
		this->firstPacketPts = 0;
	}
	bool FFmpegAudioDecoder::IsFileOpen() const
	{
		return this->audioFilePath != nullptr && this->avFormatContext != nullptr
			&& this->avCodecContext != nullptr && this->swrContext != nullptr
			&& this->avFrame != nullptr && this->avPacket != nullptr;
	}
	AudioFormatInfo FFmpegAudioDecoder::GetOutputFormat() const
	{
		return HEPHAUDIO_INTERNAL_FORMAT(this->channelCount, this->sampleRate);
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
			RAISE_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Seek", "Failed to seek frame."));
			return false;
		}
		return true;
	}
	AudioBuffer FFmpegAudioDecoder::Decode()
	{
		return this->Decode(0, this->fileDuration_frame);
	}
	AudioBuffer FFmpegAudioDecoder::Decode(size_t frameIndex, size_t frameCount)
	{
		if (!this->IsFileOpen())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder::Decode", "No open file to decode."));
			return AudioBuffer(frameCount, this->GetOutputFormat());
		}


		if (frameIndex >= this->fileDuration_frame)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioDecoder::Decode", "frameIndex out of bounds."));
			return AudioBuffer(frameCount, this->GetOutputFormat());
		}

		if (frameIndex + frameCount > this->fileDuration_frame)
		{
			frameCount = this->fileDuration_frame - frameIndex;
		}

		av_frame_unref(this->avFrame);

		int ret = this->SeekFrame(frameIndex);
		if (ret < 0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to seek frame."));
			return AudioBuffer(frameCount, this->GetOutputFormat());
		}

		const AudioFormatInfo outputFormatInfo = this->GetOutputFormat();
		size_t readFrameCount = 0;
		AudioBuffer decodedBuffer(frameCount, outputFormatInfo);
		AVStream* avStream = this->avFormatContext->streams[this->audioStreamIndex];

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
								currentFrameCount = this->avFrame->duration * this->avFrame->sample_rate * avStream->time_base.num / avStream->time_base.den;
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

						// convert the decoded data to inner format
						av_opt_set_chlayout(this->swrContext, "in_chlayout", &this->avFrame->ch_layout, 0);
						av_opt_set_int(this->swrContext, "in_sample_rate", this->avFrame->sample_rate, 0);
						av_opt_set_sample_fmt(this->swrContext, "in_sample_fmt", (AVSampleFormat)this->avFrame->format, 0);

						ret = swr_init(this->swrContext);
						if (ret < 0)
						{
							av_packet_unref(this->avPacket);
							av_frame_unref(this->avFrame);
							RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to initialize SwrContext."));
						}

						AudioBuffer tempBuffer(currentFrameCount, outputFormatInfo);
						uint8_t* targetOutputFrame = (uint8_t*)tempBuffer.Begin();
						ret = swr_convert(this->swrContext, &targetOutputFrame, currentFrameCount, this->avFrame->data, currentFrameCount);
						if (ret < 0)
						{
							av_packet_unref(this->avPacket);
							av_frame_unref(this->avFrame);
							RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder::Decode", "Failed to decode samples."));
						}

						for (size_t i = 0; i < currentFramesToRead; i++)
						{
							for (size_t j = 0; j < outputFormatInfo.channelCount; j++)
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
	void FFmpegAudioDecoder::OpenFile(const StringBuffer& audioFilePath)
	{
		if (!File::FileExists(audioFilePath))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "File not found."));
			return;
		}

		this->audioFilePath = StringBuffer(audioFilePath, StringType::ASCII);

		int ret = 0;

		ret = avformat_open_input(&this->avFormatContext, this->audioFilePath.c_str(), nullptr, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Could not open the file."));
		}

		ret = avformat_find_stream_info(this->avFormatContext, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "An error occurred while finding the stream info, file might be corrupted."));
		}

		// Calculate duration of audio data in frames
		this->audioStreamIndex = FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND;
		for (size_t i = 0; i < this->avFormatContext->nb_streams; i++)
		{
			AVStream* avStream = this->avFormatContext->streams[i];
			if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				this->fileDuration_frame = (double)avStream->codecpar->sample_rate * avStream->duration * avStream->time_base.num / avStream->time_base.den;
				this->channelCount = avStream->codecpar->ch_layout.nb_channels;
				this->sampleRate = avStream->codecpar->sample_rate;
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

		AVChannelLayout avChannelLayout;
		av_channel_layout_default(&avChannelLayout, this->channelCount);
		av_opt_set_chlayout(this->swrContext, "out_chlayout", &avChannelLayout, 0);

		av_opt_set_int(this->swrContext, "out_sample_rate", this->sampleRate, 0);
		av_opt_set_sample_fmt(this->swrContext, "out_sample_fmt", sizeof(heph_audio_sample) == sizeof(double) ? AV_SAMPLE_FMT_DBL : AV_SAMPLE_FMT_FLT, 0);

		// Initialize codec for decoding
		const AVCodec* avCodec = avcodec_find_decoder(this->avFormatContext->streams[audioStreamIndex]->codecpar->codec_id);
		if (avCodec == nullptr)
		{
			const StringBuffer codecName = avcodec_get_name(this->avFormatContext->streams[audioStreamIndex]->codecpar->codec_id);
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Failed to create codec context from the codec parameters."));
		}

		ret = avcodec_open2(this->avCodecContext, avCodec, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Failed to open the av codec context."));
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(ret, "FFmpegAudioDecoder", "Failed to get pts of the first packet."));
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
		const int64_t timestamp = this->firstPacketPts + (((double)frameIndex / this->sampleRate) * avStream->time_base.den / avStream->time_base.num);

		ret = avformat_seek_file(this->avFormatContext, this->audioStreamIndex, INT64_MIN, timestamp, timestamp, seekFlags);
		if (ret < 0)
		{
			return ret;
		}
		avcodec_flush_buffers(this->avCodecContext);

		// calculate frameIndex relative to the current frame pos
		// and how many packets to skip after seek (seek does not always find the packet we are looking for)
		size_t skippedPacketCount = -1;
		int64_t packetDuration = 0;
		int64_t deltaFrameIndex = 0;
		do
		{
			skippedPacketCount++;
			ret = av_read_frame(this->avFormatContext, this->avPacket);
			if (ret < 0)
			{
				return ret;
			}

			if (this->avPacket->stream_index != this->audioStreamIndex)
			{
				continue;
			}

			const int64_t packetFrameIndex = (double)(this->avPacket->pts - this->firstPacketPts) * this->sampleRate * avStream->time_base.num / avStream->time_base.den;
			if (frameIndex < packetFrameIndex)
			{
				HEPHAUDIO_LOG("Failed to find the exact packet that contains the requested frames, picking the closest one...", HEPH_CL_WARNING);
				deltaFrameIndex = 0;
				av_packet_unref(this->avPacket);
				break;
			}

			packetDuration = (double)this->avPacket->duration * this->sampleRate * avStream->time_base.num / avStream->time_base.den;
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
#endif