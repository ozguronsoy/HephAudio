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

		const size_t endIndex = startIndex + frameCount;
		if (endIndex > buffer.FrameCount())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "(startIndex + frameCount) exceeds the buffer's frame count."));
		}

		const AudioFormatInfo& formatInfo = buffer.FormatInfo();
		AudioBuffer resultBuffer(this->CalculateOutputFrameCount(buffer), formatInfo.channelLayout, formatInfo.sampleRate, BufferFlags::AllocUninitialized);

		if (startIndex > 0)
		{
			(void)memcpy(resultBuffer.begin(), buffer.begin(), startIndex * formatInfo.FrameSize());
		}

		if (endIndex < buffer.FrameCount())
		{
			(void)memcpy(resultBuffer.begin() + endIndex, buffer.begin() + endIndex, (buffer.FrameCount() - endIndex) * formatInfo.FrameSize());
		}

		if (this->threadCount == 1)
			this->ProcessST(buffer, resultBuffer, startIndex, frameCount);
		else
			this->ProcessMT(buffer, resultBuffer, startIndex, frameCount);

		buffer = std::move(resultBuffer);
	}
}