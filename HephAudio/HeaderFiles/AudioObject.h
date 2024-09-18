#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "Event.h"
#include "Guid.h"
#include <vector>

/** @file */

/**
 * indicates to play the audio object infinitely.
 * 
 */
#define HEPHAUDIO_INFINITE_LOOP 				(0)

/**
 * The default handler for the \link HephAudio::AudioObject::OnRender AudioObject::OnRender \endlink event. 
 * Plays the audio data as is.
 * 
 */
#define HEPHAUDIO_RENDER_HANDLER_DEFAULT 		&HephAudio::AudioObject::DefaultRenderHandler

/**
 * An handler for the \link HephAudio::AudioObject::OnRender AudioObject::OnRender \endlink event. 
 * Converts the audio data to the render format before playing.
 * 
 */
#define HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT 	&HephAudio::AudioObject::MatchFormatRenderHandler

namespace HephAudio
{
	/**
	 * @brief stores information that's necessary to play audio.
	 * 
	 */
	struct AudioObject
	{
		/**
		 * unique identifier of the object.
		 * 
		 */
		HephCommon::Guid id;

		/**
		 * path of the file the object is created with, or empty if created via \link HephAudio::Native::NativeAudio::CreateAudioObject NativeAudio::CreateAudioObject.
		 * 
		 */
		std::string filePath;

		/**
		 * name of the object.
		 * 
		 */
		std::string name;

		/**
		 * indicates whether the object is paused. 
		 * If true, the object will not be played until this field is set to false.
		 * 
		 */
		bool isPaused;

		/**
		 * number of times the object will be played.
		 * Set this to #HEPHAUDIO_INFINITE_LOOP in order to play it infinite times.
		 * 
		 */
		uint32_t playCount;

		/**
		 * loudness of the audio between 0 and 1.
		 * 
		 * @important values above 1 may cause distortion.
		 */
		double volume;

		/**
		 * contains the audio data.
		 * 
		 */
		AudioBuffer buffer;

		/**
		 * index of the first audio frame that will be rendered (played) next.
		 * 
		 */
		size_t frameIndex;

		/**
		 * event that will be invoked each time before rendering (playing) audio data.
		 * 
		 */
		HephCommon::Event OnRender;

		/**
		 * event that will be invoked each time when the object finishes playing.
		 * 
		 */
		HephCommon::Event OnFinishedPlaying;
		
		/** @copydoc default_constructor */
		AudioObject();

		/** @copydoc move_constructor */
		AudioObject(AudioObject&& rhs) noexcept;
		
		AudioObject& operator=(AudioObject&& rhs) noexcept;

		/**
		 * calculates the playback position between 0 and 1.
		 * 
		 */
		double GetPosition() const;

		/**
		 * sets the playback position.
		 * 
		 * @param position between 0 and 1.
		 */
		void SetPosition(double position);

		static void DefaultRenderHandler(const HephCommon::EventParams& eventParams);
		static void MatchFormatRenderHandler(const HephCommon::EventParams& eventParams);
	};
}