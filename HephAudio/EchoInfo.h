#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include <cstdint>

namespace HephAudio
{
	namespace Structs
	{
		struct EchoInfo 
		{
			uint32_t reflectionCount;
			/// <summary>
			/// The delay (in seconds) between each reflection.
			/// </summary>
			double reflectionDelay;
			/// <summary>
			/// Volume will be multiplied by pow(volumeFactor, n) on each reflection.
			/// For example volumeFactor = 0.5 and reflectionCount = 3
			/// First reflection = sample * 0.5, second reflection = sample * 0.25, third reflection = sample * 0.125
			/// </summary>
			double volumeFactor;
			/// <summary>
			/// The begining of the audio data which will be used in echo.
			/// Must be between 0 and 1.
			/// </summary>
			double echoStartPosition;
			/// <summary>
			/// The end of the audio data which will be used in echo.
			/// Must be greater than echoStartPosition and must be between 0 and 1.
			/// </summary>
			double echoEndPosition;
			EchoInfo();
			/// <summary>
			/// Calculates the given buffers frame count after the echo is applied.
			/// </summary>
			/// <returns>The buffers frame count after the echo is applied</returns>
			virtual size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
		};
	}
}