#pragma once
#include "framework.h"
#include <cstdint>

namespace HephAudio
{
	namespace Structs
	{
		struct EchoInfo 
		{
			// This is used in INativeAudio, ignore this if you are using AudioProcessor::Echo method.
			bool echo;
			uint32_t reflectionCount;
			// The delay (in seconds) between each reflection.
			double reflectionDelay;
			// Volume will be multiplied by pow(volumeFactor, n) on each reflection.
			// For example volumeFactor = 0.5 and reflectionCount = 3
			// First reflection = sample * 0.5, second reflection = sample * 0.25, third reflection = sample * 0.125
			double volumeFactor;
			// Must be between 0 and 1.
			double echoStartPosition;
			// Must be greater than echoStartPosition and must be between 0 and 1.
			double echoEndPosition;
			EchoInfo()
			{
				echo = false;
				reflectionCount = 0;
				reflectionDelay = 0.0;
				volumeFactor = 0.0;
				echoStartPosition = 0.0;
				echoEndPosition = 1.0;
			}
		};
	}
}