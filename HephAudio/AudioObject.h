#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "StringBuffer.h"
#include "AudioEvent.h"
#include "AudioRenderEventArgs.h"
#include "AudioRenderEventResult.h"
#include "AudioFinishedPlayingEventArgs.h"
#include <vector>

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information for playing audio.
	/// </summary>
	struct AudioObject
	{
		/// <summary>
		/// The path where the file is located.
		/// </summary>
		StringBuffer filePath;
		/// <summary>
		/// Name of the file with its extension.
		/// </summary>
		StringBuffer name;
		/// <summary>
		/// if true, stops playing the object.
		/// </summary>
		bool pause;
		/// <summary>
		/// The number of times the audio object will be played. Set this to "0" for infinite loop.
		/// </summary>
		uint32_t loopCount;
		/// <summary>
		/// The loudness of the audio.
		/// </summary>
		hephaudio_float volume;
		/// <summary>
		/// The audio data.
		/// </summary>
		AudioBuffer buffer;
		/// <summary>
		/// The current position of the buffer.
		/// </summary>
		size_t frameIndex;
		/// <summary>
		/// The name of the queue in which the audio object is. If empty, the audio object is not in a queue.
		/// </summary>
		StringBuffer queueName;
		/// <summary>
		/// The position of the audio object in the queue. If equal to “0”, the object is currently playing.
		/// </summary>
		uint32_t queueIndex;
		/// <summary>
		/// The time, in milliseconds, between the end of the current object and the start of the next object in the queue.
		/// </summary>
		hephaudio_float queueDelay_ms;
		/// <summary>
		/// Called each time before mixing the audio data to the output buffer.
		/// </summary>
		AudioEvent OnRender;
		/// <summary>
		/// Called when the audio object is finished playing.
		/// </summary>
		AudioEvent OnFinishedPlaying;
		/// <summary>
		/// Creates and initializes an AudioObject instance with default values.
		/// </summary>
		AudioObject();
	private:
		static void OnRenderHandler(AudioEventArgs* pArgs, AudioEventResult* pResult);
	};
}