#pragma once
#include "HephAudioShared.h"
#include "Audio.h"
#include "AudioBuffer.h"
#include <filesystem>

/** @file */

/**
 * The key to find the \link HephAudio::AudioStream AudioStream \endlink instance when handling events.
 * 
 */
#define HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY "audio_stream"

namespace HephAudio
{
	/**
	 * @brief Class for playing audio files without loading them into memory. 
	 * Reads the portion of audio data from the file just before rendering.
	 * 
	 */
	class HEPH_API AudioStream final
	{
	private:
		Native::NativeAudio* pNativeAudio;
		AudioFormatInfo formatInfo;
		size_t frameCount;
		AudioObject* pAudioObject;
		AudioBuffer decodedBuffer;

	public:
		/** 
		 * @copydoc constructor 
		 * 
		 * @param pNativeAudio pointer to the native audio instance that will be used for playing the files.
		 */
		AudioStream(Native::NativeAudio* pNativeAudio);
		
		/** 
		 * @copydoc constructor 
		 * 
		 * @param audio the audio instance that will be used for playing the files.
		 */
		AudioStream(Audio& audio);
		
		/** 
		 * @copydoc constructor 
		 * 
		 * @param pNativeAudio pointer to the native audio instance that will be used for playing the files.
		 * @param filePath file that will be played.
		 */
		AudioStream(Native::NativeAudio* pNativeAudio, const std::filesystem::path& filePath);
		
		/** 
		 * @copydoc constructor 
		 * 
		 * @param audio the audio instance that will be used for playing the files.
		 * @param filePath file that will be played.
		 */
		AudioStream(Audio& audio, const std::filesystem::path& filePath);
		
		AudioStream(const AudioStream&) = delete;

		/** @copydoc move_constructor */
		AudioStream(AudioStream&& rhs) noexcept;
		
		/** @copydoc destructor */
		~AudioStream();
		
		AudioStream& operator=(const AudioStream&) = delete;
		AudioStream& operator=(AudioStream&& rhs) noexcept;

		/**
		 * gets the pointer to the \link HephAudio::Native::NativeAudio NativeAudio \endlink instance that's used for rendering audio data.
		 * 
		 */
		Native::NativeAudio* GetNativeAudio() const;

		/**
		 * gets the pointer to the \link HephAudio::AudioObject AudioObject \endlink instance that's created to play audio data.
		 * 
		 */
		AudioObject* GetAudioObject() const;

		/**
		 * gets the format info of the audio data.
		 * 
		 */
		const AudioFormatInfo& GetAudioFormatInfo() const;

		/**
		 * gets the number of audio frames the file contains.
		 * 
		 */
		size_t GetFrameCount() const;

		/**
		 * changes the file that's currently playing with the provided one.
		 * 
		 */
		void ChangeFile(const std::filesystem::path& newFilePath);

		/**
		 * starts (resumes) playing the file.
		 * 
		 */
		void Start();

		/**
		 * stops (pauses) playing the file.
		 * 
		 */
		void Stop();

		/**
		 * gets the position of the frame that will be rendered next between 0 and 1.
		 * 
		 */
		double GetPosition() const;

		/**
		 * sets the position of the frame that will be rendered next between 0 and 1.
		 * 
		 */
		void SetPosition(double position);

		/**
		 * releases the resources.
		 * 
		 */
		void Release();

	private:
		void Release(bool destroyAO);
		static void OnRender(const Heph::EventParams& eventParams);
		static void OnFinishedPlaying(const Heph::EventParams& eventParams);
	};
}