#pragma once
#include "HephAudioFramework.h"
#include "EventResult.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	/// <summary>
	/// Holds the audio data that will be rendered and whether the audio object finished playing.
	/// </summary>
	struct AudioRenderEventResult : public HephCommon::EventResult
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