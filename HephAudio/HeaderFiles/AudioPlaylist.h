#pragma once
#include "HephAudioShared.h"
#include "AudioStream.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "EventArgs.h"
#include "EventResult.h"
#include <string>
#include <vector>

/** @file */

/**
 * the key to find the \link HephAudio::AudioPlaylist AudioPlaylist \endlink instance when handling events.
 *
 */
#define HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY "audio_playlist"

namespace HephAudio
{
	/**
	 * @brief class for creating playlists.
	 * Uses \link HephAudio::AudioStream AudioStream \endlink internally to play the files.
	 *
	 */
	class HEPH_API AudioPlaylist final
	{
	private:
		AudioStream stream;
		std::vector<std::filesystem::path> files;

	public:
		/**
		 * @copydoc constructor
		 *
		 * @param pNativeAudio pointer to the native audio instance that will be used for playing the files.
		 *
		 */
		AudioPlaylist(Native::NativeAudio* pNativeAudio);
		/**
		 * @copydoc constructor
		 *
		 * @param audio the audio instance that will be used for playing the files.
		 *
		 */
		AudioPlaylist(Audio& audio);

		/**
		 * @copydoc constructor
		 *
		 * @param pNativeAudio pointer to the native audio instance that will be used for playing the files.
		 * @param files file paths.
		 *
		 */
		AudioPlaylist(Native::NativeAudio* pNativeAudio, const std::vector<std::filesystem::path>& files);

		/**
		 * @copydoc constructor
		 *
		 * @param audio the audio instance that will be used for playing the files.
		 * @param files file paths.
		 *
		 */
		AudioPlaylist(Audio& audio, const std::vector<std::filesystem::path>& files);

		AudioPlaylist(const AudioPlaylist&) = delete;

		/** @copydoc move_constructor */
		AudioPlaylist(AudioPlaylist&& rhs) noexcept;

		AudioPlaylist& operator=(const AudioPlaylist&) = delete;
		AudioPlaylist& operator=(const std::filesystem::path& rhs);
		AudioPlaylist& operator=(const std::vector<std::filesystem::path>& rhs);
		AudioPlaylist& operator=(AudioPlaylist&& rhs) noexcept;

		/**
		 * gets the number of files present in the playlist.
		 *
		 */
		size_t Size() const;

		/**
		 * gets the pointer to the \link HephAudio::Native::NativeAudio NativeAudio \endlink instance that's used for playing the files.
		 *
		 */
		Native::NativeAudio* GetNativeAudio() const;

		/**
		 * starts (resumes) playing the files.
		 *
		 */
		void Start();

		/**
		 * stops (pauses) playing the files.
		 *
		 */
		void Stop();

		/**
		 * adds a file to the end of the playlist.
		 *
		 * @param filePath path of the file that will be added to the playlist.
		 */
		void Add(const std::filesystem::path& filePath);

		/**
		 * adds multiple files to the end of the playlist.
		 *
		 * @param files file paths.
		 */
		void Add(const std::vector<std::filesystem::path>& files);

		/**
		 * inserts the provided file to the playlist.
		 *
		 * @param filePath path of the file that will be added to the playlist.
		 * @param index position of the new file within the playlist.
		 */
		void Insert(const std::filesystem::path& filePath, size_t index);

		/**
		 * inserts the provided files to the playlist.
		 *
		 * @param files file paths.
		 * @param index position of the new files within the playlist.
		 */
		void Insert(const std::vector<std::filesystem::path>& files, size_t index);

		/**
		 * removes the file at the provided index.
		 *
		 * @param index index of the file to be removed.
		 */
		void Remove(size_t index);

		/**
		 * removes the files at the provided index.
		 *
		 * @param index index of the first file to be removed.
		 * @param count number of files to remove.
		 */
		void Remove(size_t index, size_t count);

		/**
		 * removes the file with the provided path.
		 *
		 */
		void Remove(const std::filesystem::path& filePath);

		/**
		 * skips to the next file.
		 *
		 */
		void Skip();

		/**
		 * skips the first <b>n</b> files.
		 *
		 * @param n number of files to skip.
		 */
		void Skip(size_t n);

		/**
		 * removes all files from the playlist.
		 *
		 */
		void Clear();

	private:
		void ChangeFile();
		static void OnFinishedPlaying(const Heph::EventParams& eventParams);
	};
}