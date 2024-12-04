#include "AudioEffects/DoubleBufferedAudioEffect.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	DoubleBufferedAudioEffect::DoubleBufferedAudioEffect() : AudioEffect() {}

	void DoubleBufferedAudioEffect::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		if (startIndex > buffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "startIndex out of bounds."));
		}

		if ((startIndex + frameCount) > buffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "endIndex exceeds the buffer's frame count."));
		}

		const AudioFormatInfo& formatInfo = buffer.FormatInfo();
		AudioBuffer outputBuffer = this->CreateOutputBuffer(buffer, startIndex, frameCount);
		this->InitializeOutputBuffer(buffer, outputBuffer, startIndex, frameCount);

		if (this->threadCount == 1)
			this->ProcessST(buffer, outputBuffer, startIndex, frameCount);
		else
			this->ProcessMT(buffer, outputBuffer, startIndex, frameCount);

		buffer = std::move(outputBuffer);
	}

	AudioBuffer DoubleBufferedAudioEffect::CreateOutputBuffer(const AudioBuffer& inputBuffer, size_t startIndex, size_t frameCount) const
	{
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		return AudioBuffer(
			(inputBuffer.FrameCount() - frameCount) + this->CalculateOutputFrameCount(frameCount, formatInfo),
			formatInfo.channelLayout,
			formatInfo.sampleRate, BufferFlags::AllocUninitialized);
	}

	void DoubleBufferedAudioEffect::InitializeOutputBuffer(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) const
	{
		const size_t iFrameCount = inputBuffer.FrameCount();
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& iFormatInfo = inputBuffer.FormatInfo();

		outputBuffer.Reset();

		if (startIndex > 0)
		{
			outputBuffer.Replace(inputBuffer, 0, startIndex);
		}

		if (endIndex < iFrameCount)
		{
			const size_t resultPadding = startIndex + this->CalculateOutputFrameCount(frameCount, iFormatInfo);
			const AudioBuffer temp = inputBuffer.SubBuffer(endIndex, iFrameCount - endIndex);
			outputBuffer.Replace(temp, resultPadding, temp.FrameCount());
		}
	}
}