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
		AudioBuffer resultBuffer(
			(buffer.FrameCount() - frameCount) + this->CalculateOutputFrameCount(frameCount, formatInfo),
			formatInfo.channelLayout,
			formatInfo.sampleRate);

		if (startIndex > 0)
		{
			resultBuffer.Replace(buffer, 0, startIndex);
		}

		if (endIndex < buffer.FrameCount())
		{
			const size_t resultPadding = startIndex + this->CalculateOutputFrameCount(frameCount, formatInfo);
			const AudioBuffer temp = buffer.SubBuffer(endIndex, buffer.FrameCount() - endIndex);
			resultBuffer.Replace(temp, resultPadding, temp.FrameCount());
		}

		if (this->threadCount == 1)
			this->ProcessST(buffer, resultBuffer, startIndex, frameCount);
		else
			this->ProcessMT(buffer, resultBuffer, startIndex, frameCount);

		buffer = std::move(resultBuffer);
	}
}