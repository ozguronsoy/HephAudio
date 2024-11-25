#include "AudioEffects/ChangeSpeedEffect.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Windows/HannWindow.h"

using namespace Heph;

namespace HephAudio
{
	ChangeSpeedEffect::ChangeSpeedEffect() : ChangeSpeedEffect(1) {}

	ChangeSpeedEffect::ChangeSpeedEffect(double speed) : ChangeSpeedEffect(speed, 1024) {}

	ChangeSpeedEffect::ChangeSpeedEffect(double speed, size_t hopSize) : ChangeSpeedEffect(speed, hopSize, HannWindow(hopSize * 2)) {}

	ChangeSpeedEffect::ChangeSpeedEffect(double speed, size_t hopSize, const Window& wnd) : DoubleBufferedAudioEffect()
	{
		this->SetSpeed(speed);
		this->SetHopSize(hopSize);
		this->SetWindow(wnd);
	}

	std::string ChangeSpeedEffect::Name() const
	{
		return "Change Speed";
	}

	bool ChangeSpeedEffect::HasRTSupport() const
	{
		return false;
	}

	size_t ChangeSpeedEffect::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return outputFrameCount * this->speed;
	}

	size_t ChangeSpeedEffect::CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return inputFrameCount / this->speed;
	}

	double ChangeSpeedEffect::GetSpeed() const
	{
		return this->speed;
	}

	void ChangeSpeedEffect::SetSpeed(double speed)
	{
		if (speed <= 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "speed cannot be negative or zero."));
		}

		this->speed = speed;
	}

	size_t ChangeSpeedEffect::GetHopSize() const
	{
		return this->hopSize;
	}

	void ChangeSpeedEffect::SetHopSize(size_t hopSize)
	{
		if (hopSize == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "hopSize must be greater than zero."));
		}

		this->hopSize = hopSize;
	}

	size_t ChangeSpeedEffect::GetWindowSize() const
	{
		return this->wnd.Size();
	}

	void ChangeSpeedEffect::SetWindow(const Window& wnd)
	{
		if (wnd.GetSize() == 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "window size must be greater than zero."));
		}

		this->wnd = wnd.GenerateBuffer();
	}

	void ChangeSpeedEffect::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		const size_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		
		const double invSpeed = 1.0 / this->speed;
		const size_t wndSize = this->wnd.Size();
		const size_t ifc = inputBuffer.FrameCount();
		const size_t ofc = outputBuffer.FrameCount();

		if (this->speed < 1)
		{
			const double overflowFactor = ((double)this->hopSize) / ((double)this->wnd.Size());
			for (size_t i = startIndex; i < endIndex; i += this->hopSize)
			{
				for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				{
					for (size_t k = 0; k < invSpeed; ++k)
					{
						for (size_t l = 0, m = (i * invSpeed + k * this->hopSize);
							l < wndSize && (i + l) < ifc && m < ofc;
							++l, ++m)
						{
							outputBuffer[m][j] += inputBuffer[i + l][j] * overflowFactor * this->wnd[l];
						}
					}
				}
			}
		}
		else
		{
			const double overflowFactor = ((double)this->hopSize) / ((double)wndSize) * invSpeed;
			for (size_t i = startIndex; i < endIndex; i += this->hopSize)
			{
				for (size_t j = 0; j < formatInfo.channelLayout.count; j++)
				{
					for (size_t k = 0, l = i * invSpeed;
						k < wndSize && (i + k) < ifc && l < ofc;
						k++, l++)
					{
						outputBuffer[l][j] += inputBuffer[i + k][j] * overflowFactor * this->wnd[k];
					}
				}
			}
		}
	}
}