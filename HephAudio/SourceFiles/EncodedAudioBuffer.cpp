#include "EncodedAudioBuffer.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	EncodedAudioBuffer::EncodedAudioBuffer() : BufferBase<EncodedAudioBuffer, uint8_t>() {}

	EncodedAudioBuffer::EncodedAudioBuffer(const AudioFormatInfo& formatInfo) : BufferBase<EncodedAudioBuffer, uint8_t>(), formatInfo(formatInfo) {}

	EncodedAudioBuffer::EncodedAudioBuffer(size_t size, const AudioFormatInfo& formatInfo)
		: BufferBase<EncodedAudioBuffer, uint8_t>(size, BufferFlags::AllocUninitialized), formatInfo(formatInfo) {}

	EncodedAudioBuffer::EncodedAudioBuffer(const uint8_t* pData, size_t size, const AudioFormatInfo& formatInfo)
		: EncodedAudioBuffer(size, formatInfo)
	{
		if (pData == nullptr)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "pData cannot be null"));
		}
		else if (this->size > 0)
		{
			(void)std::memcpy(this->pData, pData, size);
		}
	}

	EncodedAudioBuffer::EncodedAudioBuffer(EncodedAudioBuffer&& rhs) noexcept
		: BufferBase<EncodedAudioBuffer, uint8_t>(std::move(rhs)), formatInfo(rhs.formatInfo)
	{
		rhs.formatInfo = AudioFormatInfo();
	}

	EncodedAudioBuffer& EncodedAudioBuffer::operator=(EncodedAudioBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release();

			BufferBase::operator=(std::move(rhs));

			this->formatInfo = rhs.formatInfo;

			rhs.formatInfo = AudioFormatInfo();
		}
		return *this;
	}

	void EncodedAudioBuffer::Release()
	{
		BufferBase::Release();
		this->formatInfo = AudioFormatInfo();
	}

	const AudioFormatInfo& EncodedAudioBuffer::GetAudioFormatInfo() const
	{
		return this->formatInfo;
	}

	void EncodedAudioBuffer::SetAudioFormatInfo(const AudioFormatInfo& newFormatInfo)
	{
		this->formatInfo = newFormatInfo;
	}

	void EncodedAudioBuffer::Add(const uint8_t* pData, size_t size)
	{
		if (pData == nullptr)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "pData cannot be null"));
		}
		else if (size > 0)
		{
			const size_t oldSize = this->size;
			this->Resize(this->size + size);
			(void)std::memcpy(this->pData + oldSize, pData, size);
		}
	}
}

namespace Heph
{
	// explicit instantiate for building shared libraries.
	template class HEPH_API BufferBase<HephAudio::EncodedAudioBuffer, uint8_t>;
}