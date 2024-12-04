#include "AudioEffects/TimeStretcher.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Windows/HannWindow.h"

using namespace Heph;

namespace HephAudio
{
	TimeStretcher::TimeStretcher() : TimeStretcher(1) {}

	TimeStretcher::TimeStretcher(double speed) : TimeStretcher(speed, 1024) {}

	TimeStretcher::TimeStretcher(double speed, size_t hopSize) : TimeStretcher(speed, hopSize, HannWindow(hopSize * 2)) {}

	TimeStretcher::TimeStretcher(double speed, size_t hopSize, const Window& wnd) : OlaEffect(hopSize, wnd)
	{
		this->SetSpeed(speed);
	}

	std::string TimeStretcher::Name() const
	{
		return "Time Stretcher";
	}

	bool TimeStretcher::HasRTSupport() const
	{
		return false;
	}

	size_t TimeStretcher::CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return outputFrameCount * this->speed;
	}

	size_t TimeStretcher::CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const
	{
		return inputFrameCount / this->speed;
	}

	double TimeStretcher::GetSpeed() const
	{
		return this->speed;
	}

	void TimeStretcher::SetSpeed(double speed)
	{
		if (speed <= 0)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "speed cannot be negative or zero."));
		}

		this->speed = speed;
	}

	void TimeStretcher::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
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