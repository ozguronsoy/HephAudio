#if defined(HEPHAUDIO_USE_FFMPEG)
#include "FFmpegAudioDecoder.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"

using namespace HephCommon;

namespace HephAudio
{
	FFmpegAudioDecoder::FFmpegAudioDecoder()
		: audioFilePath(nullptr), fileDuration_frame(0), audioStreamIndex(FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND)
		, channelCount(0), sampleRate(0), avFormatContext(nullptr), avCodecContext(nullptr)
		, avFrame(nullptr), avPacket(nullptr) {}
	FFmpegAudioDecoder::FFmpegAudioDecoder(const StringBuffer& audioFilePath) : FFmpegAudioDecoder()
	{
		this->OpenFile(audioFilePath);
	}
	FFmpegAudioDecoder::FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept
		: audioFilePath(std::move(rhs.audioFilePath)), fileDuration_frame(rhs.fileDuration_frame), audioStreamIndex(rhs.audioStreamIndex)
		, channelCount(rhs.channelCount), sampleRate(rhs.sampleRate), avFormatContext(rhs.avFormatContext), avCodecContext(rhs.avCodecContext)
		, avFrame(rhs.avFrame), avPacket(rhs.avPacket)
	{
		rhs.avFormatContext = nullptr;
		rhs.avCodecContext = nullptr;
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
		this->avFormatContext = rhs.avFormatContext;
		this->avCodecContext = rhs.avCodecContext;
		this->avFrame = rhs.avFrame;
		this->avPacket = rhs.avPacket;

		rhs.avFormatContext = nullptr;
		rhs.avCodecContext = nullptr;
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
	}
	bool FFmpegAudioDecoder::IsFileOpen() const
	{
		return this->audioFilePath != nullptr && this->avFormatContext != nullptr
			&& this->avCodecContext != nullptr && this->avFrame != nullptr
			&& this->avPacket != nullptr;
	}
	AudioFormatInfo FFmpegAudioDecoder::GetOutputFormat() const
	{
		return HEPHAUDIO_INTERNAL_FORMAT(this->channelCount, this->sampleRate);
	}
	size_t FFmpegAudioDecoder::GetFrameCount() const
	{
		return this->fileDuration_frame;
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
			return AudioBuffer();
		}

		if (frameIndex >= this->fileDuration_frame)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FFmpegAudioDecoder::Decode", "frameIndex out of bounds."));
			return AudioBuffer();
		}

		if (frameIndex + frameCount > this->fileDuration_frame)
		{
			frameCount = this->fileDuration_frame - frameIndex;
		}

		int ret = 0;
		size_t decodedFrameCount = 0;
		AudioBuffer decodedBuffer(frameCount, this->GetOutputFormat());
		AVStream* avStream = this->avFormatContext->streams[audioStreamIndex];

		// seek the frame
		const int64_t timeStamp = av_rescale(frameIndex / this->sampleRate, avStream->time_base.den, avStream->time_base.num);
		ret = av_seek_frame(this->avFormatContext, this->audioStreamIndex, timeStamp, AVSEEK_FLAG_BACKWARD);
		if (ret < 0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder::Decode", "Failed to seek frame."));
			return AudioBuffer();
		}
		avcodec_flush_buffers(this->avCodecContext);

		while (decodedFrameCount < frameCount)
		{
			// get the next frame from file
			ret = av_read_frame(this->avFormatContext, this->avPacket);
			if (ret == AVERROR_EOF)
			{
				av_packet_unref(this->avPacket);
				HEPHAUDIO_LOG("EOF, no more frames to read.", HEPH_CL_INFO);
				break;
			}
			else if (ret < 0)
			{
				av_packet_unref(this->avPacket);
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
					break;
				}
				else if (ret < 0)
				{
					av_packet_unref(this->avPacket);
					HEPHAUDIO_LOG("Failed to send the packet, skipping it...", HEPH_CL_WARNING);
					continue;
				}

				const size_t packetFrameCount = (heph_float)avStream->codecpar->sample_rate * this->avPacket->duration * avStream->time_base.num / avStream->time_base.den;
				size_t framesReadForCurrentPacket = 0;
				while (framesReadForCurrentPacket < packetFrameCount)
				{
					// get the decoded data
					ret = avcodec_receive_frame(this->avCodecContext, this->avFrame);
					if (ret < 0)
					{
						HEPHAUDIO_LOG("Failed to recieve frame, skipping the packet...", HEPH_CL_WARNING);
						break;
					}

					size_t currentFrameCount = this->avFrame->nb_samples;
					if (currentFrameCount > 0)
					{
						// prevent overflow
						if (decodedFrameCount + framesReadForCurrentPacket + currentFrameCount > frameCount)
						{
							currentFrameCount = frameCount - decodedFrameCount - framesReadForCurrentPacket;
						}

						// read the decoded data into a temporary buffer
						bool isPlanar = false;
						const AudioFormatInfo currentFrameBufferFormatInfo = this->SF2AFI(this->avFrame->ch_layout.nb_channels, this->avFrame->sample_rate, (AVSampleFormat)this->avFrame->format, isPlanar);
						AudioBuffer currentFrameBuffer(currentFrameCount, currentFrameBufferFormatInfo);
						const uint16_t sampleSize_byte = currentFrameBufferFormatInfo.bitsPerSample / 8;
						const uint16_t bufferFrameSize = currentFrameBufferFormatInfo.FrameSize();

						if (isPlanar)
						{
							for (size_t i = 0; i < currentFrameCount; i++)
							{
								for (size_t j = 0; j < currentFrameBufferFormatInfo.channelCount; j++)
								{
									memcpy((uint8_t*)currentFrameBuffer.Begin() + (framesReadForCurrentPacket + i) * bufferFrameSize + j * sampleSize_byte,
										this->avFrame->data[j] + i * sampleSize_byte, sampleSize_byte);
								}
							}
						}
						else
						{
							memcpy((uint8_t*)currentFrameBuffer.Begin() + framesReadForCurrentPacket * bufferFrameSize,
								this->avFrame->data[0], currentFrameCount * bufferFrameSize);
						}

						// convert the temporary buffer to the output format and fill the output buffer
						AudioProcessor::ChangeNumberOfChannels(currentFrameBuffer, this->channelCount);
						AudioProcessor::ChangeSampleRate(currentFrameBuffer, this->sampleRate);
						if (currentFrameBufferFormatInfo.formatTag != HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL)
						{
							AudioProcessor::ConvertToInnerFormat(currentFrameBuffer);
						}
						memcpy(decodedBuffer[decodedFrameCount + framesReadForCurrentPacket], currentFrameBuffer.Begin(), currentFrameBuffer.Size());

						framesReadForCurrentPacket += currentFrameCount;
					}
				}
				decodedFrameCount += framesReadForCurrentPacket;
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Could not open the file."));
		}

		ret = avformat_find_stream_info(this->avFormatContext, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "An error occurred while finding the stream info, file might be corrupted."));
		}

		// Calculate duration of audio data in frames
		this->audioStreamIndex = FFmpegAudioDecoder::AUDIO_STREAM_INDEX_NOT_FOUND;
		for (size_t i = 0; i < this->avFormatContext->nb_streams; i++)
		{
			AVStream* avStream = this->avFormatContext->streams[i];
			if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				this->fileDuration_frame = (heph_float)avStream->codecpar->sample_rate * avStream->duration * avStream->time_base.num / avStream->time_base.den;
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Failed to create codec context from the codec parameters."));
		}

		ret = avcodec_open2(this->avCodecContext, avCodec, nullptr);
		if (ret < 0)
		{
			this->CloseFile();
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Failed to open the av codec context."));
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
	}
	AudioFormatInfo FFmpegAudioDecoder::SF2AFI(uint16_t channelCount, uint32_t sampleRate, AVSampleFormat sampleFormat, bool& outIsPlanar) const
	{
		outIsPlanar = false;
		switch (sampleFormat)
		{
		case AV_SAMPLE_FMT_U8P:
			outIsPlanar = true;
		case AV_SAMPLE_FMT_U8:
			return AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, channelCount, 8, sampleRate);
			break;
		case AV_SAMPLE_FMT_S16P:
			outIsPlanar = true;
		case AV_SAMPLE_FMT_S16:
			return AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, channelCount, 16, sampleRate);
			break;
		case AV_SAMPLE_FMT_S32P:
			outIsPlanar = true;
		case AV_SAMPLE_FMT_S32:
			return AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, channelCount, 32, sampleRate);
			break;
		case AV_SAMPLE_FMT_FLTP:
			outIsPlanar = true;
		case AV_SAMPLE_FMT_FLT:
			return AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT, channelCount, sizeof(float) * 8, sampleRate);
			break;
		case AV_SAMPLE_FMT_DBLP:
			outIsPlanar = true;
		case AV_SAMPLE_FMT_DBL:
			return AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT, channelCount, sizeof(double) * 8, sampleRate);
			break;
		default:
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FFmpegAudioDecoder", "Unsupported sample format [" + StringBuffer::ToString(sampleFormat) + "]."));
		}
	}
}
#endif