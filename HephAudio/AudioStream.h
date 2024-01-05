#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "AudioFileFormatManager.h"
#include "AudioObject.h"
#include "NativeAudio.h"
#include "IAudioCodec.h"
#include "Audio.h"

#define HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY "audio_stream"_sb

namespace HephAudio
{
	class AudioStream final
	{
	private:
		Native::NativeAudio* pNativeAudio;
		HephCommon::File file;
		FileFormats::IAudioFileFormat* pFileFormat;
		Codecs::IAudioCodec* pAudioCodec;
		AudioFormatInfo formatInfo;
		AudioObject* pAudioObject;
	public:
		AudioStream(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath);
		AudioStream(Audio& audio, HephCommon::StringBuffer filePath);
		AudioStream(const AudioStream&) = delete;
		AudioStream(AudioStream&& rhs) noexcept;
		AudioStream& operator=(const AudioStream&) = delete;
		AudioStream& operator=(AudioStream&& rhs) noexcept;
		~AudioStream();
		Native::NativeAudio* GetNativeAudio() const;
		HephCommon::File* GetFile();
		FileFormats::IAudioFileFormat* GetFileFormat() const;
		Codecs::IAudioCodec* GetAudioCodec() const;
		AudioObject* GetAudioObject() const;
		const AudioFormatInfo& GetAudioFormatInfo() const;
		void Release();
	private:
		void Release(bool destroyAO);
		static void OnRender(const HephCommon::EventParams& eventParams);
		static void OnFinishedPlaying(const HephCommon::EventParams& eventParams);
	};
}