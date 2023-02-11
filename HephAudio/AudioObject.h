#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "StringBuffer.h"
#include "AudioEvent.h"
#include "AudioRenderEventArgs.h"
#include "AudioRenderEventResult.h"
#include <vector>

namespace HephAudio
{
	enum class AudioWindowType : uint8_t
	{
		RectangleWindow = 0x00,
		TriangleWindow = 0x01,
		ParzenWindow = 0x02,
		WelchWindow = 0x03,
		SineWindow = 0x04,
		HannWindow = 0x05,
		HammingWindow = 0x06,
		BlackmanWindow = 0x07,
		ExactBlackmanWindow = 0x08,
		NuttallWindow = 0x09,
		BlackmanNuttallWindow = 0x0A,
		BlackmanHarrisWindow = 0x0B,
		FlatTopWindow = 0x0C,
		GaussianWindow = 0x0D,
		TukeyWindow = 0x0E,
		BartlettHannWindow = 0x0F,
		HannPoissonWindow = 0x10,
		LanczosWindow = 0x11
	};
	struct AudioObject
	{
		StringBuffer filePath;
		StringBuffer name;
		bool pause;
		bool mute;
		/// <summary>
		/// A constant object does not finish playing until you either destroy it by calling the INativeAudio::DestroyAO method or make this false and wait until it finishes.
		/// </summary>
		bool constant;
		/// <summary>
		/// Set this to 0 for an infinite loop.
		/// </summary>
		uint32_t loopCount;
		/// <summary>
		/// Setting the volume to more than 1 might cause some glitching in audio.
		/// </summary>
		double volume;
		std::vector<StringBuffer> categories;
		AudioBuffer buffer;
		/// <summary>
		/// Starting frame to get sub buffer before the next render.
		/// </summary>
		size_t frameIndex;
		/// <summary>
		/// There can be more than one queue, if empty the audio object is not in queue.
		/// </summary>
		StringBuffer queueName;
		/// <summary>
		/// Position of the audio object on the queue, if equals to 0 its currently playing.
		/// </summary>
		uint32_t queueIndex;
		/// <summary>
		/// In milliseconds.
		/// </summary>
		double queueDelay;
		AudioWindowType windowType;
		/// <summary>
		/// Called each time before mixing the audio data.
		/// </summary>
		AudioEvent OnRender;
		AudioObject();
		virtual ~AudioObject() = default;
		virtual bool IsPlaying() const;
		virtual bool IsInQueue() const;
	private:
		static void OnRenderHandler(AudioEventArgs* pArgs, AudioEventResult* pResult);
	};
}