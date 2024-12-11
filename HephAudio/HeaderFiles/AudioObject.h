#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "Event.h"
#include "Guid.h"
#include <vector>
#include <filesystem>

/** @file */

/**
 * indicates the audio object will be played infinitely.
 *
 */
#define HEPHAUDIO_INFINITE_LOOP (0)

 /** @copydoc HephAudio::AudioObject::DefaultRenderHandler */
#define HEPHAUDIO_RENDER_HANDLER_DEFAULT &HephAudio::AudioObject::DefaultRenderHandler

  /** @copydoc HephAudio::AudioObject::MatchFormatRenderHandler */
#define HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT &HephAudio::AudioObject::MatchFormatRenderHandler

  /** @copydoc HephAudio::AudioObject::DefaultFinishedPlayingHandler */
#define HEPHAUDIO_FINISHED_PLAYING_HANDLER_DEFAULT &HephAudio::AudioObject::DefaultFinishedPlayingHandler

namespace HephAudio
{
	/**
	 * @brief stores information that's necessary to play audio.
	 *
	 */
	struct HEPH_API AudioObject
	{
		/**
		 * unique identifier of the object.
		 *
		 */
		Heph::Guid id;

		/**
		 * path of the file the object is created with, or empty if created via \link HephAudio::Native::NativeAudio::CreateAudioObject NativeAudio::CreateAudioObject.
		 *
		 */
		std::filesystem::path filePath;

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
		Heph::Event OnRender;

		/**
		 * event that will be invoked each time when the object finishes playing.
		 *
		 */
		Heph::Event OnFinishedPlaying;

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

		/**
		 * stops playing the audio object.
		 *
		 */
		void Pause();

		/**
		 * starts playing the audio object.
		 *
		 */
		void Resume();

		/**
		 * the default handler for the \link HephAudio::AudioObject::OnRender AudioObject::OnRender \endlink event.
		 * Plays the audio data as is.
		 *
		 */
		static void DefaultRenderHandler(const Heph::EventParams& eventParams);

		/**
		 * an handler for the \link HephAudio::AudioObject::OnRender AudioObject::OnRender \endlink event.
		 * Converts the audio data to the render format before playing.
		 *
		 */
		static void MatchFormatRenderHandler(const Heph::EventParams& eventParams);

		/**
		 * the default handler for the \link HephAudio::AudioObject::OnFinishedPlaying AudioObject::OnFinishedPlaying \endlink event.
		 * If looping, replays the audio object; otherwise, destroys it.
		 *
		 */
		static void DefaultFinishedPlayingHandler(const Heph::EventParams& eventParams);
	};
}