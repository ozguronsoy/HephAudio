#pragma once
#include "framework.h"
#include "AudioEventResult.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	/// <summary>
	/// Holds the audio data that will be rendered and whether the audio object finished playing.
	/// </summary>
	struct AudioRenderEventResult : public AudioEventResult
	{
		/// <summary>
		/// The audio data that will be rendered.
		/// </summary>
		AudioBuffer renderBuffer{ AudioBuffer() };
		/// <summary>
		/// Tells whether the audio object finished playing.
		/// </summary>
		bool isFinishedPlaying{ false };
	};
}