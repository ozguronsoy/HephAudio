#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "DistortionInfo.h"
#include <string>
#include <vector>

namespace HephAudio
{
	namespace Structs
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
		struct IAudioObject;
		typedef AudioBuffer(*AudioGetSubBufferEventHandler)(IAudioObject* sender, size_t nFramesToRender, size_t* outFrameIndex);
		typedef bool (*AudioIsFinishedPlayingEventHandler)(IAudioObject* sender);
		typedef void (*AudioRenderEventHandler)(IAudioObject* sender, AudioBuffer& renderBuffer, size_t frameIndex);
		struct IAudioObject
		{
			std::wstring filePath;
			std::wstring name;
			bool paused;
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
			std::vector<std::wstring> categories;
			DistortionInfo distortionInfo;
			AudioBuffer buffer;
			/// <summary>
			/// Starting frame to get sub buffer before the next render.
			/// </summary>
			size_t frameIndex;
			/// <summary>
			/// There can be more than one queue, if empty the audio object is not in queue.
			/// </summary>
			std::wstring queueName;
			/// <summary>
			/// Position of the audio object on the queue, if equals to 0 its currently playing.
			/// </summary>
			uint32_t queueIndex;
			/// <summary>
			/// In milliseconds.
			/// </summary>
			uint32_t queueDelay;
			AudioWindowType windowType;
			/// <summary>
			/// Called each time before mixing to get the desired audio data for rendering.
			/// </summary>
			AudioGetSubBufferEventHandler GetSubBuffer;
			/// <summary>
			/// Called each time after mixing to check whether the audio object finished rendering all its data.
			/// </summary>
			AudioIsFinishedPlayingEventHandler IsFinishedPlaying;
			/// <summary>
			/// Called each time before mixing the audio data.
			/// </summary>
			AudioRenderEventHandler OnRender;
			IAudioObject();
			virtual ~IAudioObject() = default;
			virtual bool IsPlaying() const;
			virtual bool IsInQueue() const;
		private:
			static AudioBuffer OnGetSubBuffer(IAudioObject* sender, size_t nFramesToRender, size_t* outFrameIndex);
			static bool OnIsFinishedPlaying(IAudioObject* sender);
		};
	}
}