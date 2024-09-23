#include "AudioBuffer.h"
#include "AudioProcessor.h"
#include "HephMath.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	AudioBuffer::AudioBuffer() : SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>(), frameCount(0) { AudioBuffer::AddEventHandlers(); }

	AudioBuffer::AudioBuffer(size_t frameCount, const AudioChannelLayout& channelLayout, uint32_t sampleRate)
		: SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>(frameCount* channelLayout.count),
		frameCount(frameCount), formatInfo(HEPHAUDIO_INTERNAL_FORMAT(channelLayout, sampleRate))
	{
		AudioBuffer::AddEventHandlers();
	}

	AudioBuffer::AudioBuffer(size_t frameCount, const AudioChannelLayout& channelLayout, uint32_t sampleRate, BufferFlags flags)
		: SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>(frameCount* channelLayout.count, flags),
		frameCount(frameCount), formatInfo(HEPHAUDIO_INTERNAL_FORMAT(channelLayout, sampleRate))
	{
		AudioBuffer::AddEventHandlers();
	}

	AudioBuffer::AudioBuffer(const AudioBuffer& rhs)
		: SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>(rhs.size), frameCount(rhs.frameCount), formatInfo(rhs.formatInfo)
	{
		AudioBuffer::AddEventHandlers();
	}

	AudioBuffer::AudioBuffer(AudioBuffer&& rhs) noexcept
		: SignedArithmeticBuffer<AudioBuffer, heph_audio_sample_t>(std::move(rhs)), frameCount(rhs.frameCount), formatInfo(rhs.formatInfo)
	{
		rhs.frameCount = 0;
		rhs.formatInfo = AudioFormatInfo();

		AudioBuffer::AddEventHandlers();
	}

	AudioBuffer::~AudioBuffer()
	{
		this->frameCount = 0;
		this->formatInfo = AudioFormatInfo();
	}

	AudioBuffer& AudioBuffer::operator=(const AudioBuffer& rhs)
	{
		if (this != &rhs)
		{
			this->Release();
			this->size = rhs.size;
			this->frameCount = rhs.frameCount;
			this->formatInfo = rhs.formatInfo;

			if (rhs.size > 0)
			{
				const size_t size_byte = rhs.SizeAsByte();
				this->pData = AudioBuffer::AllocateUninitialized(size_byte);
				(void)std::memcpy(this->pData, rhs.pData, size_byte);
			}
		}

		return *this;
	}

	AudioBuffer& AudioBuffer::operator=(AudioBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release();

			this->pData = rhs.pData;
			this->size = rhs.size;
			this->frameCount = rhs.frameCount;
			this->formatInfo = rhs.formatInfo;

			rhs.pData = nullptr;
			rhs.size = 0;
			rhs.frameCount = 0;
			rhs.formatInfo = AudioFormatInfo();
		}

		return *this;
	}

	AudioBuffer AudioBuffer::operator<<(size_t rhs) const
	{
		return SignedArithmeticBuffer::operator<<(rhs * this->formatInfo.channelLayout.count);
	}

	AudioBuffer& AudioBuffer::operator<<=(size_t rhs)
	{
		return (AudioBuffer&)SignedArithmeticBuffer::operator<<=(rhs * this->formatInfo.channelLayout.count);
	}

	AudioBuffer AudioBuffer::operator>>(size_t rhs) const
	{
		return SignedArithmeticBuffer::operator>>(rhs * this->formatInfo.channelLayout.count);
	}

	AudioBuffer& AudioBuffer::operator>>=(size_t rhs)
	{
		return (AudioBuffer&)SignedArithmeticBuffer::operator>>=(rhs * this->formatInfo.channelLayout.count);
	}

	bool AudioBuffer::operator==(const AudioBuffer& rhs) const
	{
		return (this->IsEmpty() && rhs.IsEmpty()) ||
			(this->size == rhs.size && this->formatInfo == rhs.formatInfo && std::memcmp(this->pData, rhs.pData, this->SizeAsByte()) == 0);
	}

	heph_audio_sample_t* AudioBuffer::operator[](size_t frameIndex) const
	{
		return (heph_audio_sample_t*)(((uint8_t*)this->pData) + this->formatInfo.FrameSize() * frameIndex);
	}

	void AudioBuffer::Release()
	{
		SignedArithmeticBuffer::Release();
		this->frameCount = 0;
		this->formatInfo = AudioFormatInfo();
	}

	AudioBuffer AudioBuffer::SubBuffer(size_t frameIndex, size_t frameCount) const
	{
		AudioBuffer subBuffer = SignedArithmeticBuffer::SubBuffer(frameIndex * this->formatInfo.channelLayout.count, frameCount * this->formatInfo.channelLayout.count);
		subBuffer.frameCount = frameCount;
		subBuffer.formatInfo = this->formatInfo;
		return subBuffer;
	}

	void AudioBuffer::Prepend(const AudioBuffer& rhs)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Both buffers must have the same audio format"));
		}

		SignedArithmeticBuffer::Prepend(rhs);

		if (this->formatInfo.channelLayout.count > 0)
		{
			this->frameCount = this->size / this->formatInfo.channelLayout.count;
		}
	}

	void AudioBuffer::Append(const AudioBuffer& rhs)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Both buffers must have the same audio format"));
		}

		SignedArithmeticBuffer::Append(rhs);

		if (this->formatInfo.channelLayout.count > 0)
		{
			this->frameCount = this->size / this->formatInfo.channelLayout.count;
		}
	}

	void AudioBuffer::Insert(const AudioBuffer& rhs, size_t frameIndex)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Both buffers must have the same audio format"));
		}

		SignedArithmeticBuffer::Insert(rhs, frameIndex * this->formatInfo.channelLayout.count);

		if (this->formatInfo.channelLayout.count > 0)
		{
			this->frameCount = this->size / this->formatInfo.channelLayout.count;
		}
	}

	void AudioBuffer::Cut(size_t frameIndex, size_t frameCount)
	{
		SignedArithmeticBuffer::Cut(frameIndex * this->formatInfo.channelLayout.count, frameCount * this->formatInfo.channelLayout.count);

		if (this->formatInfo.channelLayout.count > 0)
		{
			this->frameCount = this->size / this->formatInfo.channelLayout.count;
		}
	}

	void AudioBuffer::Replace(const AudioBuffer& rhs, size_t frameIndex, size_t frameCount)
	{
		if (this->formatInfo != rhs.formatInfo)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Both buffers must have the same audio format"));
		}

		SignedArithmeticBuffer::Replace(rhs, frameIndex * this->formatInfo.channelLayout.count, frameCount * this->formatInfo.channelLayout.count);
	}

	void AudioBuffer::Resize(size_t newFrameCount)
	{
		SignedArithmeticBuffer::Resize(newFrameCount * this->formatInfo.channelLayout.count);
		if (this->formatInfo.channelLayout.count > 0)
		{
			this->frameCount = this->size / this->formatInfo.channelLayout.count;
		}
	}

	void AudioBuffer::Reverse()
	{
		const size_t halfFrameCount = this->frameCount / 2;
		for (size_t i = 0; i < halfFrameCount; ++i)
		{
			for (size_t j = 0; j < this->formatInfo.channelLayout.count; ++j)
			{
				std::swap((*this)[i][j], (*this)[this->frameCount - i - 1][j]);
			}
		}
	}

	size_t AudioBuffer::FrameCount() const
	{
		return this->frameCount;
	}

	const AudioFormatInfo& AudioBuffer::FormatInfo() const
	{
		return this->formatInfo;
	}

	void AudioBuffer::SetChannelLayout(const AudioChannelLayout& channelLayout)
	{
		const size_t size = this->size;
		const size_t frameCount = this->frameCount;
		const uint16_t sampleRate = this->formatInfo.sampleRate;

		this->Release();

		this->size = size;
		this->frameCount = frameCount;
		this->formatInfo = HEPHAUDIO_INTERNAL_FORMAT(channelLayout, sampleRate);
		this->pData = SignedArithmeticBuffer::Allocate(this->SizeAsByte());
	}

	void AudioBuffer::SetSampleRate(uint32_t sampleRate)
	{
		this->formatInfo.sampleRate = sampleRate;
	}

	void AudioBuffer::AddEventHandlers()
	{
		if (!AudioBuffer::ADD_EVENT_HANDLERS)
		{
			BufferOperatorEvents<AudioBuffer, heph_audio_sample_t>::OnResultCreated += AudioBuffer::ResultCreatedEventHandler;
			BufferOperatorEvents<AudioBuffer, AudioBuffer>::OnResultCreated += AudioBuffer::ResultCreatedEventHandlerBuffer;
			AudioBuffer::ADD_EVENT_HANDLERS = true;
		}
	}

	void AudioBuffer::ResultCreatedEventHandler(const EventParams& params)
	{
		BufferOperatorResultCreatedEventArgs<AudioBuffer, heph_audio_sample_t>* pArgs = (BufferOperatorResultCreatedEventArgs<AudioBuffer, heph_audio_sample_t>*)params.pArgs;

		pArgs->result.frameCount = pArgs->lhs.frameCount;
		pArgs->result.formatInfo = pArgs->lhs.formatInfo;
	}

	void AudioBuffer::ResultCreatedEventHandlerBuffer(const EventParams& params)
	{
		BufferOperatorResultCreatedEventArgs<AudioBuffer, AudioBuffer>* pArgs = (BufferOperatorResultCreatedEventArgs<AudioBuffer, AudioBuffer>*)params.pArgs;

		if (pArgs->lhs.formatInfo != pArgs->rhs.formatInfo)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(&pArgs->lhs, InvalidArgumentException(HEPH_FUNC, "Operands must have the same audio format"));
		}

		pArgs->result.frameCount = pArgs->lhs.frameCount;
		pArgs->result.formatInfo = pArgs->lhs.formatInfo;
	}
}

namespace Heph
{
	// explicit instantiate for building shared libraries.
	template class HEPH_API BufferBase<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API ArithmeticBuffer<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API SignedArithmeticBuffer<HephAudio::AudioBuffer, heph_audio_sample_t>;

	template class HEPH_API BufferAdditionOperator<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferSubtractionOperator<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferDivisionOperator<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferMultiplicationOperator<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferArithmeticOperators<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<HephAudio::AudioBuffer, heph_audio_sample_t>;
	template struct HEPH_API BufferOperatorEvents<HephAudio::AudioBuffer, heph_audio_sample_t>;

	template class HEPH_API BufferAdditionOperator<HephAudio::AudioBuffer, heph_audio_sample_t, HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferSubtractionOperator<HephAudio::AudioBuffer, heph_audio_sample_t, HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferDivisionOperator<HephAudio::AudioBuffer, heph_audio_sample_t, HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferMultiplicationOperator<HephAudio::AudioBuffer, heph_audio_sample_t, HephAudio::AudioBuffer, heph_audio_sample_t>;
	template class HEPH_API BufferArithmeticOperators<HephAudio::AudioBuffer, heph_audio_sample_t, HephAudio::AudioBuffer, heph_audio_sample_t>;
	template struct HEPH_API BufferOperatorResultCreatedEventArgs<HephAudio::AudioBuffer, HephAudio::AudioBuffer>;
	template struct HEPH_API BufferOperatorEvents<HephAudio::AudioBuffer, HephAudio::AudioBuffer>;

	template class HEPH_API BufferUnaryMinusOperator<HephAudio::AudioBuffer, heph_audio_sample_t>;
}